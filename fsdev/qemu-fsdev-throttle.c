/*
 * Fsdev Throttle
 *
 * Copyright (C) 2016 Huawei Technologies Duesseldorf GmbH
 *
 * Author: Pradeep Jagadeesh <pradeep.jagadeesh@huawei.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * (at your option) any later version.
 *
 * See the COPYING file in the top-level directory for details.
 *
 */

#include "qemu/osdep.h"
#include "qemu/error-report.h"
#include "qapi/error.h"
#include "qemu-fsdev-throttle.h"

static void fsdev_throttle_schedule_next_request(FsThrottle *fst, bool is_write)
{
    if (!qemu_co_queue_empty(&fst->throttled_reqs[is_write])) {
        if (throttle_schedule_timer(&fst->ts, &fst->tt, is_write)) {
            return;
        }
       qemu_co_queue_next(&fst->throttled_reqs[is_write]);
   }
}

static void fsdev_throttle_read_timer_cb(void *opaque)
{
    FsThrottle *fst = opaque;
    qemu_co_enter_next(&fst->throttled_reqs[false]);
}

static void fsdev_throttle_write_timer_cb(void *opaque)
{
    FsThrottle *fst = opaque;
    qemu_co_enter_next(&fst->throttled_reqs[true]);
}

void fsdev_throttle_parse_opts(QemuOpts *opts, FsThrottle *fst)
{
    fst->cfg.buckets[THROTTLE_BPS_TOTAL].avg =
        qemu_opt_get_number(opts, "throttling.bps-total", 0);
    fst->cfg.buckets[THROTTLE_BPS_READ].avg  =
        qemu_opt_get_number(opts, "throttling.bps-read", 0);
    fst->cfg.buckets[THROTTLE_BPS_WRITE].avg =
        qemu_opt_get_number(opts, "throttling.bps-write", 0);
    fst->cfg.buckets[THROTTLE_OPS_TOTAL].avg =
        qemu_opt_get_number(opts, "throttling.iops-total", 0);
    fst->cfg.buckets[THROTTLE_OPS_READ].avg =
        qemu_opt_get_number(opts, "throttling.iops-read", 0);
    fst->cfg.buckets[THROTTLE_OPS_WRITE].avg =
        qemu_opt_get_number(opts, "throttling.iops-write", 0);

    fst->cfg.buckets[THROTTLE_BPS_TOTAL].max =
        qemu_opt_get_number(opts, "throttling.bps-total-max", 0);
    fst->cfg.buckets[THROTTLE_BPS_READ].max  =
        qemu_opt_get_number(opts, "throttling.bps-read-max", 0);
    fst->cfg.buckets[THROTTLE_BPS_WRITE].max =
        qemu_opt_get_number(opts, "throttling.bps-write-max", 0);
    fst->cfg.buckets[THROTTLE_OPS_TOTAL].max =
        qemu_opt_get_number(opts, "throttling.iops-total-max", 0);
    fst->cfg.buckets[THROTTLE_OPS_READ].max =
        qemu_opt_get_number(opts, "throttling.iops-read-max", 0);
    fst->cfg.buckets[THROTTLE_OPS_WRITE].max =
        qemu_opt_get_number(opts, "throttling.iops-write-max", 0);

    fst->cfg.buckets[THROTTLE_BPS_TOTAL].burst_length =
        qemu_opt_get_number(opts, "throttling.bps-total-max-length", 1);
    fst->cfg.buckets[THROTTLE_BPS_READ].burst_length  =
        qemu_opt_get_number(opts, "throttling.bps-read-max-length", 1);
    fst->cfg.buckets[THROTTLE_BPS_WRITE].burst_length =
        qemu_opt_get_number(opts, "throttling.bps-write-max-length", 1);
    fst->cfg.buckets[THROTTLE_OPS_TOTAL].burst_length =
        qemu_opt_get_number(opts, "throttling.iops-total-max-length", 1);
    fst->cfg.buckets[THROTTLE_OPS_READ].burst_length =
        qemu_opt_get_number(opts, "throttling.iops-read-max-length", 1);
    fst->cfg.buckets[THROTTLE_OPS_WRITE].burst_length =
        qemu_opt_get_number(opts, "throttling.iops-write-max-length", 1);
    fst->cfg.op_size =
        qemu_opt_get_number(opts, "throttling.iops-size", 0);
}

int fsdev_throttle_init(FsThrottle *fst)
{
    Error *err = NULL;

    if (!throttle_is_valid(&fst->cfg, &err)) {
        error_reportf_err(err, "Throttle configuration is not valid: ");
        return -1;
    }
    if (throttle_enabled(&fst->cfg)) {
        g_assert((fst->aioctx = qemu_get_aio_context()));
        throttle_init(&fst->ts);
        throttle_timers_init(&fst->tt,
                             fst->aioctx,
                             QEMU_CLOCK_REALTIME,
                             fsdev_throttle_read_timer_cb,
                             fsdev_throttle_write_timer_cb,
                             fst);
        throttle_config(&fst->ts, &fst->tt, &fst->cfg);
        qemu_co_queue_init(&fst->throttled_reqs[0]);
        qemu_co_queue_init(&fst->throttled_reqs[1]);
    }
    return 0;
}

static uint64_t get_num_bytes(struct iovec *iov, int iovcnt)
{
    int i;
    uint64_t bytes = 0;

    for (i = 0; i < iovcnt; i++) {
        bytes += iov[i].iov_len;
    }
    return bytes;
}

void coroutine_fn fsdev_co_throttle_request(FsThrottle *fst, bool is_write,
                                            struct iovec *iov, int iovcnt)
{
    if (throttle_enabled(&fst->cfg)) {
        uint64_t bytes = get_num_bytes(iov, iovcnt);
        bool must_wait = throttle_schedule_timer(&fst->ts, &fst->tt, is_write);
        if (must_wait || !qemu_co_queue_empty(&fst->throttled_reqs[is_write])) {
            qemu_co_queue_wait(&fst->throttled_reqs[is_write]);
        }
        throttle_account(&fst->ts, is_write, bytes);

        fsdev_throttle_schedule_next_request(fst, is_write);
    }
}

void fsdev_throttle_cleanup(FsThrottle *fst)
{
    throttle_timers_destroy(&fst->tt);
}
