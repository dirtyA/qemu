/*
 * Present a block device as a raw image through FUSE
 *
 * Copyright (c) 2020 Max Reitz <mreitz@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; under version 2 or later of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#define FUSE_USE_VERSION 31

#include "qemu/osdep.h"
#include "block/aio.h"
#include "block/block.h"
#include "block/export.h"
#include "block/fuse.h"
#include "block/qapi.h"
#include "qapi/error.h"
#include "qapi/qapi-commands-block.h"
#include "sysemu/block-backend.h"

#include <fuse.h>
#include <fuse_lowlevel.h>


/* Prevent overly long bounce buffer allocations */
#define FUSE_MAX_BOUNCE_BYTES (MIN(BDRV_REQUEST_MAX_BYTES, 64 * 1024 * 1024))


typedef struct FuseExport {
    BlockExport common;

    struct fuse_session *fuse_session;
    struct fuse_buf fuse_buf;
    bool mounted, fd_handler_set_up;

    char *mountpoint;
    bool writable;
} FuseExport;

static GHashTable *exports;
static const struct fuse_lowlevel_ops fuse_ops;

static void fuse_export_shutdown(BlockExport *exp);
static void fuse_export_delete(BlockExport *exp);

static void init_exports_table(void);

static int setup_fuse_export(FuseExport *exp, const char *mountpoint,
                             Error **errp);
static void read_from_fuse_export(void *opaque);

static bool is_regular_file(const char *path, Error **errp);


static int fuse_export_create(BlockExport *blk_exp,
                              BlockExportOptions *blk_exp_args,
                              Error **errp)
{
    FuseExport *exp = container_of(blk_exp, FuseExport, common);
    BlockExportOptionsFuse *args = &blk_exp_args->u.fuse;
    int ret;

    assert(blk_exp_args->type == BLOCK_EXPORT_TYPE_FUSE);

    init_exports_table();

    /*
     * It is important to do this check before calling is_regular_file() --
     * that function will do a stat(), which we would have to handle if we
     * already exported something on @mountpoint.  But we cannot, because
     * we are currently caught up here.
     * (Note that ideally we would want to resolve relative paths here,
     * but bdrv_make_absolute_filename() might do the wrong thing for
     * paths that contain colons, and realpath() would resolve symlinks,
     * which we do not want: The mount point is not going to be the
     * symlink's destination, but the link itself.)
     * So this will not catch all potential clashes, but hopefully at
     * least the most common one of specifying exactly the same path
     * string twice.
     */
    if (g_hash_table_contains(exports, args->mountpoint)) {
        error_setg(errp, "There already is a FUSE export on '%s'",
                   args->mountpoint);
        ret = -EEXIST;
        goto fail;
    }

    if (!is_regular_file(args->mountpoint, errp)) {
        ret = -EINVAL;
        goto fail;
    }

    exp->mountpoint = g_strdup(args->mountpoint);
    exp->writable = blk_exp_args->writable;

    ret = setup_fuse_export(exp, args->mountpoint, errp);
    if (ret < 0) {
        goto fail;
    }

    return 0;

fail:
    fuse_export_delete(blk_exp);
    return ret;
}

/**
 * Allocates the global @exports hash table.
 */
static void init_exports_table(void)
{
    if (exports) {
        return;
    }

    exports = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
}

/**
 * Create exp->fuse_session and mount it.
 */
static int setup_fuse_export(FuseExport *exp, const char *mountpoint,
                             Error **errp)
{
    const char *fuse_argv[4];
    char *mount_opts;
    struct fuse_args fuse_args;
    int ret;

    /* Needs to match what fuse_init() sets.  Only max_read must be supplied. */
    mount_opts = g_strdup_printf("max_read=%zu", FUSE_MAX_BOUNCE_BYTES);

    fuse_argv[0] = ""; /* Dummy program name */
    fuse_argv[1] = "-o";
    fuse_argv[2] = mount_opts;
    fuse_argv[3] = NULL;
    fuse_args = (struct fuse_args)FUSE_ARGS_INIT(3, (char **)fuse_argv);

    exp->fuse_session = fuse_session_new(&fuse_args, &fuse_ops,
                                         sizeof(fuse_ops), exp);
    g_free(mount_opts);
    if (!exp->fuse_session) {
        error_setg(errp, "Failed to set up FUSE session");
        ret = -EIO;
        goto fail;
    }

    ret = fuse_session_mount(exp->fuse_session, mountpoint);
    if (ret < 0) {
        error_setg(errp, "Failed to mount FUSE session to export");
        ret = -EIO;
        goto fail;
    }
    exp->mounted = true;

    g_hash_table_insert(exports, g_strdup(mountpoint), NULL);

    aio_set_fd_handler(exp->common.ctx,
                       fuse_session_fd(exp->fuse_session), true,
                       read_from_fuse_export, NULL, NULL, exp);
    exp->fd_handler_set_up = true;

    return 0;

fail:
    fuse_export_shutdown(&exp->common);
    return ret;
}

/**
 * Callback to be invoked when the FUSE session FD can be read from.
 * (This is basically the FUSE event loop.)
 */
static void read_from_fuse_export(void *opaque)
{
    FuseExport *exp = opaque;
    int ret;

    blk_exp_ref(&exp->common);

    do {
        ret = fuse_session_receive_buf(exp->fuse_session, &exp->fuse_buf);
    } while (ret == -EINTR);
    if (ret < 0) {
        goto out;
    }

    fuse_session_process_buf(exp->fuse_session, &exp->fuse_buf);

out:
    blk_exp_unref(&exp->common);
}

static void fuse_export_shutdown(BlockExport *blk_exp)
{
    FuseExport *exp = container_of(blk_exp, FuseExport, common);

    if (exp->fuse_session) {
        fuse_session_exit(exp->fuse_session);

        if (exp->fd_handler_set_up) {
            aio_set_fd_handler(exp->common.ctx,
                               fuse_session_fd(exp->fuse_session), true,
                               NULL, NULL, NULL, NULL);
            exp->fd_handler_set_up = false;
        }
    }

    if (exp->mountpoint) {
        /*
         * Safe to drop now, because we will not handle any requests
         * for this export anymore anyway.
         */
        g_hash_table_remove(exports, exp->mountpoint);
    }
}

static void fuse_export_delete(BlockExport *blk_exp)
{
    FuseExport *exp = container_of(blk_exp, FuseExport, common);

    if (exp->fuse_session) {
        if (exp->mounted) {
            fuse_session_unmount(exp->fuse_session);
        }

        fuse_session_destroy(exp->fuse_session);
    }

    free(exp->fuse_buf.mem);
    g_free(exp->mountpoint);
}

/**
 * Check whether @path points to a regular file.  If not, put an
 * appropriate message into *errp.
 */
static bool is_regular_file(const char *path, Error **errp)
{
    struct stat statbuf;
    int ret;

    ret = stat(path, &statbuf);
    if (ret < 0) {
        error_setg_errno(errp, errno, "Failed to stat '%s'", path);
        return false;
    }

    if (!S_ISREG(statbuf.st_mode)) {
        error_setg(errp, "'%s' is not a regular file", path);
        return false;
    }

    return true;
}

/**
 * A chance to set change some parameters supplied to FUSE_INIT.
 */
static void fuse_init(void *userdata, struct fuse_conn_info *conn)
{
    /*
     * MIN_NON_ZERO() would not be wrong here, but what we set here
     * must equal what has been passed to fuse_session_new().
     * Therefore, as long as max_read must be passed as a mount option
     * (which libfuse claims will be changed at some point), we have
     * to set max_read to a fixed value here.
     */
    conn->max_read = FUSE_MAX_BOUNCE_BYTES;

    conn->max_write = MIN_NON_ZERO(BDRV_REQUEST_MAX_BYTES, conn->max_write);
}

/**
 * Let clients look up files.  Always return ENOENT because we only
 * care about the mountpoint itself.
 */
static void fuse_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    fuse_reply_err(req, ENOENT);
}

/**
 * Let clients get file attributes (i.e., stat() the file).
 */
static void fuse_getattr(fuse_req_t req, fuse_ino_t inode,
                         struct fuse_file_info *fi)
{
    struct stat statbuf;
    int64_t length, allocated_blocks;
    time_t now = time(NULL);
    FuseExport *exp = fuse_req_userdata(req);
    mode_t mode;

    length = blk_getlength(exp->common.blk);
    if (length < 0) {
        fuse_reply_err(req, -length);
        return;
    }

    allocated_blocks = bdrv_get_allocated_file_size(blk_bs(exp->common.blk));
    if (allocated_blocks <= 0) {
        allocated_blocks = DIV_ROUND_UP(length, 512);
    } else {
        allocated_blocks = DIV_ROUND_UP(allocated_blocks, 512);
    }

    mode = S_IFREG | S_IRUSR;
    if (exp->writable) {
        mode |= S_IWUSR;
    }

    statbuf = (struct stat) {
        .st_ino     = inode,
        .st_mode    = mode,
        .st_nlink   = 1,
        .st_uid     = getuid(),
        .st_gid     = getgid(),
        .st_size    = length,
        .st_blksize = blk_bs(exp->common.blk)->bl.request_alignment,
        .st_blocks  = allocated_blocks,
        .st_atime   = now,
        .st_mtime   = now,
        .st_ctime   = now,
    };

    fuse_reply_attr(req, &statbuf, 1.);
}

static int fuse_do_truncate(const FuseExport *exp, int64_t size,
                            bool req_zero_write, PreallocMode prealloc)
{
    uint64_t blk_perm, blk_shared_perm;
    BdrvRequestFlags truncate_flags = 0;
    int ret;

    if (req_zero_write) {
        truncate_flags |= BDRV_REQ_ZERO_WRITE;
    }

    blk_get_perm(exp->common.blk, &blk_perm, &blk_shared_perm);

    ret = blk_set_perm(exp->common.blk, blk_perm | BLK_PERM_RESIZE,
                       blk_shared_perm, NULL);
    if (ret < 0) {
        return ret;
    }

    ret = blk_truncate(exp->common.blk, size, true, prealloc,
                       truncate_flags, NULL);

    /* Must succeed, because we are only giving up the RESIZE permission */
    blk_set_perm(exp->common.blk, blk_perm, blk_shared_perm, &error_abort);

    return ret;
}

/**
 * Let clients set file attributes.  Only resizing is supported.
 */
static void fuse_setattr(fuse_req_t req, fuse_ino_t inode, struct stat *statbuf,
                         int to_set, struct fuse_file_info *fi)
{
    FuseExport *exp = fuse_req_userdata(req);
    int ret;

    if (!exp->writable) {
        fuse_reply_err(req, EACCES);
        return;
    }

    if (to_set & ~FUSE_SET_ATTR_SIZE) {
        fuse_reply_err(req, ENOTSUP);
        return;
    }

    ret = fuse_do_truncate(exp, statbuf->st_size, true, PREALLOC_MODE_OFF);
    if (ret < 0) {
        fuse_reply_err(req, -ret);
        return;
    }

    fuse_getattr(req, inode, fi);
}

/**
 * Let clients open a file (i.e., the exported image).
 */
static void fuse_open(fuse_req_t req, fuse_ino_t inode,
                      struct fuse_file_info *fi)
{
    fuse_reply_open(req, fi);
}

/**
 * Handle client reads from the exported image.
 */
static void fuse_read(fuse_req_t req, fuse_ino_t inode,
                      size_t size, off_t offset, struct fuse_file_info *fi)
{
    FuseExport *exp = fuse_req_userdata(req);
    int64_t length;
    void *buf;
    int ret;

    /* Limited by max_read, should not happen */
    if (size > FUSE_MAX_BOUNCE_BYTES) {
        fuse_reply_err(req, EINVAL);
        return;
    }

    /**
     * Clients will expect short reads at EOF, so we have to limit
     * offset+size to the image length.
     */
    length = blk_getlength(exp->common.blk);
    if (length < 0) {
        fuse_reply_err(req, -length);
        return;
    }

    if (offset + size > length) {
        size = length - offset;
    }

    buf = qemu_try_blockalign(blk_bs(exp->common.blk), size);
    if (!buf) {
        fuse_reply_err(req, ENOMEM);
        return;
    }

    ret = blk_pread(exp->common.blk, offset, buf, size);
    if (ret >= 0) {
        fuse_reply_buf(req, buf, size);
    } else {
        fuse_reply_err(req, -ret);
    }

    qemu_vfree(buf);
}

/**
 * Handle client writes to the exported image.
 */
static void fuse_write(fuse_req_t req, fuse_ino_t inode, const char *buf,
                       size_t size, off_t offset, struct fuse_file_info *fi)
{
    FuseExport *exp = fuse_req_userdata(req);
    int64_t length;
    int ret;

    /* Limited by max_write, should not happen */
    if (size > BDRV_REQUEST_MAX_BYTES) {
        fuse_reply_err(req, EINVAL);
        return;
    }

    if (!exp->writable) {
        fuse_reply_err(req, EACCES);
        return;
    }

    /**
     * Clients will expect short writes at EOF, so we have to limit
     * offset+size to the image length.
     */
    length = blk_getlength(exp->common.blk);
    if (length < 0) {
        fuse_reply_err(req, -length);
        return;
    }

    if (offset + size > length) {
        size = length - offset;
    }

    ret = blk_pwrite(exp->common.blk, offset, buf, size, 0);
    if (ret >= 0) {
        fuse_reply_write(req, size);
    } else {
        fuse_reply_err(req, -ret);
    }
}

/**
 * Let clients fsync the exported image.
 */
static void fuse_fsync(fuse_req_t req, fuse_ino_t inode, int datasync,
                       struct fuse_file_info *fi)
{
    FuseExport *exp = fuse_req_userdata(req);
    int ret;

    ret = blk_flush(exp->common.blk);
    fuse_reply_err(req, ret < 0 ? -ret : 0);
}

/**
 * Called before an FD to the exported image is closed.  (libfuse
 * notes this to be a way to return last-minute errors.)
 */
static void fuse_flush(fuse_req_t req, fuse_ino_t inode,
                        struct fuse_file_info *fi)
{
    fuse_fsync(req, inode, 1, fi);
}

static const struct fuse_lowlevel_ops fuse_ops = {
    .init       = fuse_init,
    .lookup     = fuse_lookup,
    .getattr    = fuse_getattr,
    .setattr    = fuse_setattr,
    .open       = fuse_open,
    .read       = fuse_read,
    .write      = fuse_write,
    .flush      = fuse_flush,
    .fsync      = fuse_fsync,
};

const BlockExportDriver blk_exp_fuse = {
    .type               = BLOCK_EXPORT_TYPE_FUSE,
    .instance_size      = sizeof(FuseExport),
    .create             = fuse_export_create,
    .delete             = fuse_export_delete,
    .request_shutdown   = fuse_export_shutdown,
};
