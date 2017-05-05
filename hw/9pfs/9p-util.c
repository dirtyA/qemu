/*
 * 9p utilities
 *
 * Copyright IBM, Corp. 2017
 *
 * Authors:
 *  Greg Kurz <groug@kaod.org>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qemu/xattr.h"
#include "9p-util.h"

int relative_openat_nofollow(int dirfd, const char *path, int flags,
                             mode_t mode)
{
    int fd = dirfd;

    assert(*path);

    while (*path && fd != -1) {
        const char *c;
        int next_fd;
        char *head;

        /* Only relative paths without consecutive slashes */
        assert(path[0] != '/');

        head = g_strdup(path);
        c = strchrnul(path, '/');
        if (*c) {
            /* Intermediate path element */
            head[c - path] = 0;
            path = c + 1;
            next_fd = openat_dir(fd, head);
        } else {
            /* Rightmost path element */
            next_fd = openat_file(fd, head, flags, mode);
            path = c;
        }
        g_free(head);
        if (dirfd != fd) {
            close_preserve_errno(fd);
        }
        fd = next_fd;
    }

    return fd;
}

ssize_t fgetxattrat_nofollow(int dirfd, const char *filename, const char *name,
                             void *value, size_t size)
{
    char *proc_path = g_strdup_printf("/proc/self/fd/%d/%s", dirfd, filename);
    int ret;

    ret = lgetxattr(proc_path, name, value, size);
    g_free(proc_path);
    return ret;
}
