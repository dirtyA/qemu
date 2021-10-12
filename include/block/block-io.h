#ifndef BLOCK_IO_H
#define BLOCK_IO_H

#include "block-common.h"

/*
 * I/O API functions. These functions are thread-safe, and therefore
 * can run in any thread as long as the thread has called
 * aio_context_acquire/release().
 */

int bdrv_replace_child_bs(BdrvChild *child, BlockDriverState *new_bs,
                          Error **errp);

int bdrv_pwrite_zeroes(BdrvChild *child, int64_t offset,
                       int64_t bytes, BdrvRequestFlags flags);
int bdrv_pread(BdrvChild *child, int64_t offset, void *buf, int64_t bytes);
int bdrv_pwrite(BdrvChild *child, int64_t offset, const void *buf,
                int64_t bytes);
int bdrv_pwrite_sync(BdrvChild *child, int64_t offset,
                     const void *buf, int64_t bytes);
/*
 * Efficiently zero a region of the disk image.  Note that this is a regular
 * I/O request like read or write and should have a reasonable size.  This
 * function is not suitable for zeroing the entire image in a single request
 * because it may allocate memory for the entire region.
 */
int coroutine_fn bdrv_co_pwrite_zeroes(BdrvChild *child, int64_t offset,
                                       int64_t bytes, BdrvRequestFlags flags);

int coroutine_fn bdrv_co_truncate(BdrvChild *child, int64_t offset, bool exact,
                                  PreallocMode prealloc, BdrvRequestFlags flags,
                                  Error **errp);
int generated_co_wrapper
bdrv_truncate(BdrvChild *child, int64_t offset, bool exact,
              PreallocMode prealloc, BdrvRequestFlags flags, Error **errp);

int64_t bdrv_nb_sectors(BlockDriverState *bs);
int64_t bdrv_getlength(BlockDriverState *bs);
int64_t bdrv_get_allocated_file_size(BlockDriverState *bs);
BlockMeasureInfo *bdrv_measure(BlockDriver *drv, QemuOpts *opts,
                               BlockDriverState *in_bs, Error **errp);
void bdrv_get_geometry(BlockDriverState *bs, uint64_t *nb_sectors_ptr);
int coroutine_fn bdrv_co_delete_file(BlockDriverState *bs, Error **errp);
void coroutine_fn bdrv_co_delete_file_noerr(BlockDriverState *bs);


int generated_co_wrapper bdrv_check(BlockDriverState *bs, BdrvCheckResult *res,
                                    BdrvCheckMode fix);

/* sg packet commands */
int bdrv_co_ioctl(BlockDriverState *bs, int req, void *buf);

/* Invalidate any cached metadata used by image formats */
int generated_co_wrapper bdrv_invalidate_cache(BlockDriverState *bs,
                                               Error **errp);

/* Ensure contents are flushed to disk.  */
int generated_co_wrapper bdrv_flush(BlockDriverState *bs);
int coroutine_fn bdrv_co_flush(BlockDriverState *bs);

int generated_co_wrapper bdrv_pdiscard(BdrvChild *child, int64_t offset,
                                       int64_t bytes);
int bdrv_co_pdiscard(BdrvChild *child, int64_t offset, int64_t bytes);
bool bdrv_can_write_zeroes_with_unmap(BlockDriverState *bs);
int bdrv_block_status(BlockDriverState *bs, int64_t offset,
                      int64_t bytes, int64_t *pnum, int64_t *map,
                      BlockDriverState **file);
int bdrv_block_status_above(BlockDriverState *bs, BlockDriverState *base,
                            int64_t offset, int64_t bytes, int64_t *pnum,
                            int64_t *map, BlockDriverState **file);
int bdrv_is_allocated(BlockDriverState *bs, int64_t offset, int64_t bytes,
                      int64_t *pnum);
int bdrv_is_allocated_above(BlockDriverState *top, BlockDriverState *base,
                            bool include_base, int64_t offset, int64_t bytes,
                            int64_t *pnum);
int coroutine_fn bdrv_co_is_zero_fast(BlockDriverState *bs, int64_t offset,
                                      int64_t bytes);

bool bdrv_is_read_only(BlockDriverState *bs);
int bdrv_can_set_read_only(BlockDriverState *bs, bool read_only,
                           bool ignore_allow_rdw, Error **errp);
int bdrv_apply_auto_read_only(BlockDriverState *bs, const char *errmsg,
                              Error **errp);
bool bdrv_is_writable(BlockDriverState *bs);
bool bdrv_is_sg(BlockDriverState *bs);
bool bdrv_is_inserted(BlockDriverState *bs);
const char *bdrv_get_format_name(BlockDriverState *bs);

bool bdrv_supports_compressed_writes(BlockDriverState *bs);
const char *bdrv_get_node_name(const BlockDriverState *bs);
int bdrv_get_info(BlockDriverState *bs, BlockDriverInfo *bdi);
ImageInfoSpecific *bdrv_get_specific_info(BlockDriverState *bs,
                                          Error **errp);
BlockStatsSpecific *bdrv_get_specific_stats(BlockDriverState *bs);
void bdrv_round_to_clusters(BlockDriverState *bs,
                            int64_t offset, int64_t bytes,
                            int64_t *cluster_offset,
                            int64_t *cluster_bytes);

void bdrv_get_backing_filename(BlockDriverState *bs,
                               char *filename, int filename_size);

int generated_co_wrapper
bdrv_readv_vmstate(BlockDriverState *bs, QEMUIOVector *qiov, int64_t pos);
int generated_co_wrapper
bdrv_writev_vmstate(BlockDriverState *bs, QEMUIOVector *qiov, int64_t pos);

/*
 * Returns the alignment in bytes that is required so that no bounce buffer
 * is required throughout the stack
 */
size_t bdrv_min_mem_align(BlockDriverState *bs);
/* Returns optimal alignment in bytes for bounce buffer */
size_t bdrv_opt_mem_align(BlockDriverState *bs);
void *qemu_blockalign(BlockDriverState *bs, size_t size);
void *qemu_blockalign0(BlockDriverState *bs, size_t size);
void *qemu_try_blockalign(BlockDriverState *bs, size_t size);
void *qemu_try_blockalign0(BlockDriverState *bs, size_t size);
bool bdrv_qiov_is_aligned(BlockDriverState *bs, QEMUIOVector *qiov);

void bdrv_enable_copy_on_read(BlockDriverState *bs);
void bdrv_disable_copy_on_read(BlockDriverState *bs);

void bdrv_debug_event(BlockDriverState *bs, BlkdebugEvent event);

#define BLKDBG_EVENT(child, evt) \
    do { \
        if (child) { \
            bdrv_debug_event(child->bs, evt); \
        } \
    } while (0)

/**
 * bdrv_get_aio_context:
 *
 * Returns: the currently bound #AioContext
 */
AioContext *bdrv_get_aio_context(BlockDriverState *bs);
/**
 * Move the current coroutine to the AioContext of @bs and return the old
 * AioContext of the coroutine. Increase bs->in_flight so that draining @bs
 * will wait for the operation to proceed until the corresponding
 * bdrv_co_leave().
 *
 * Consequently, you can't call drain inside a bdrv_co_enter/leave() section as
 * this will deadlock.
 */
AioContext *coroutine_fn bdrv_co_enter(BlockDriverState *bs);

/**
 * Ends a section started by bdrv_co_enter(). Move the current coroutine back
 * to old_ctx and decrease bs->in_flight again.
 */
void coroutine_fn bdrv_co_leave(BlockDriverState *bs, AioContext *old_ctx);

/**
 * Transfer control to @co in the aio context of @bs
 */
void bdrv_coroutine_enter(BlockDriverState *bs, Coroutine *co);

AioContext *bdrv_child_get_parent_aio_context(BdrvChild *c);
AioContext *child_of_bds_get_parent_aio_context(BdrvChild *c);

void bdrv_io_plug(BlockDriverState *bs);
void bdrv_io_unplug(BlockDriverState *bs);

/**
 * bdrv_parent_drained_begin_single:
 *
 * Begin a quiesced section for the parent of @c. If @poll is true, wait for
 * any pending activity to cease.
 */
void bdrv_parent_drained_begin_single(BdrvChild *c, bool poll);

/**
 * bdrv_parent_drained_end_single:
 *
 * End a quiesced section for the parent of @c.
 *
 * This polls @bs's AioContext until all scheduled sub-drained_ends
 * have settled, which may result in graph changes.
 */
void bdrv_parent_drained_end_single(BdrvChild *c);

/**
 * bdrv_drain_poll:
 *
 * Poll for pending requests in @bs, its parents (except for @ignore_parent),
 * and if @recursive is true its children as well (used for subtree drain).
 *
 * If @ignore_bds_parents is true, parents that are BlockDriverStates must
 * ignore the drain request because they will be drained separately (used for
 * drain_all).
 *
 * This is part of bdrv_drained_begin.
 */
bool bdrv_drain_poll(BlockDriverState *bs, bool recursive,
                     BdrvChild *ignore_parent, bool ignore_bds_parents);

/**
 * bdrv_drained_begin:
 *
 * Begin a quiesced section for exclusive access to the BDS, by disabling
 * external request sources including NBD server and device model. Note that
 * this doesn't block timers or coroutines from submitting more requests, which
 * means block_job_pause is still necessary.
 *
 * This function can be recursive.
 */
void bdrv_drained_begin(BlockDriverState *bs);

/**
 * bdrv_do_drained_begin_quiesce:
 *
 * Quiesces a BDS like bdrv_drained_begin(), but does not wait for already
 * running requests to complete.
 */
void bdrv_do_drained_begin_quiesce(BlockDriverState *bs,
                                   BdrvChild *parent, bool ignore_bds_parents);

/**
 * Like bdrv_drained_begin, but recursively begins a quiesced section for
 * exclusive access to all child nodes as well.
 */
void bdrv_subtree_drained_begin(BlockDriverState *bs);

/**
 * bdrv_drained_end:
 *
 * End a quiescent section started by bdrv_drained_begin().
 *
 * This polls @bs's AioContext until all scheduled sub-drained_ends
 * have settled.  On one hand, that may result in graph changes.  On
 * the other, this requires that the caller either runs in the main
 * loop; or that all involved nodes (@bs and all of its parents) are
 * in the caller's AioContext.
 */
void bdrv_drained_end(BlockDriverState *bs);

/**
 * End a quiescent section started by bdrv_subtree_drained_begin().
 */
void bdrv_subtree_drained_end(BlockDriverState *bs);

bool bdrv_can_store_new_dirty_bitmap(BlockDriverState *bs, const char *name,
                                     uint32_t granularity, Error **errp);

/**
 *
 * bdrv_co_copy_range:
 *
 * Do offloaded copy between two children. If the operation is not implemented
 * by the driver, or if the backend storage doesn't support it, a negative
 * error code will be returned.
 *
 * Note: block layer doesn't emulate or fallback to a bounce buffer approach
 * because usually the caller shouldn't attempt offloaded copy any more (e.g.
 * calling copy_file_range(2)) after the first error, thus it should fall back
 * to a read+write path in the caller level.
 *
 * @src: Source child to copy data from
 * @src_offset: offset in @src image to read data
 * @dst: Destination child to copy data to
 * @dst_offset: offset in @dst image to write data
 * @bytes: number of bytes to copy
 * @flags: request flags. Supported flags:
 *         BDRV_REQ_ZERO_WRITE - treat the @src range as zero data and do zero
 *                               write on @dst as if bdrv_co_pwrite_zeroes is
 *                               called. Used to simplify caller code, or
 *                               during BlockDriver.bdrv_co_copy_range_from()
 *                               recursion.
 *         BDRV_REQ_NO_SERIALISING - do not serialize with other overlapping
 *                                   requests currently in flight.
 *
 * Returns: 0 if succeeded; negative error code if failed.
 **/
int coroutine_fn bdrv_co_copy_range(BdrvChild *src, int64_t src_offset,
                                    BdrvChild *dst, int64_t dst_offset,
                                    int64_t bytes, BdrvRequestFlags read_flags,
                                    BdrvRequestFlags write_flags);

#endif /* BLOCK_IO_H */
