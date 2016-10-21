/*
 * 9p
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * later.  See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qemu/config-file.h"
#include "qemu/option.h"
#include "qemu/module.h"

static QemuOptsList qemu_fsdev_opts = {
    .name = "fsdev",
    .implied_opt_name = "fsdriver",
    .head = QTAILQ_HEAD_INITIALIZER(qemu_fsdev_opts.head),
    .desc = {
        {
            .name = "fsdriver",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "path",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "security_model",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "writeout",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "readonly",
            .type = QEMU_OPT_BOOL,

        }, {
            .name = "socket",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "sock_fd",
            .type = QEMU_OPT_NUMBER,
        }, {
            .name = "throttling.iops-total",
            .type = QEMU_OPT_NUMBER,
            .help = "limit total I/O operations per second",
        },{
            .name = "throttling.iops-read",
            .type = QEMU_OPT_NUMBER,
            .help = "limit read operations per second",
        },{
            .name = "throttling.iops-write",
            .type = QEMU_OPT_NUMBER,
            .help = "limit write operations per second",
        },{
            .name = "throttling.bps-total",
            .type = QEMU_OPT_NUMBER,
            .help = "limit total bytes per second",
        },{
            .name = "throttling.bps-read",
            .type = QEMU_OPT_NUMBER,
            .help = "limit read bytes per second",
        },{
            .name = "throttling.bps-write",
            .type = QEMU_OPT_NUMBER,
            .help = "limit write bytes per second",
        },{
            .name = "throttling.iops-total-max",
            .type = QEMU_OPT_NUMBER,
            .help = "I/O operations burst",
        },{
            .name = "throttling.iops-read-max",
            .type = QEMU_OPT_NUMBER,
            .help = "I/O operations read burst",
        },{
            .name = "throttling.iops-write-max",
            .type = QEMU_OPT_NUMBER,
            .help = "I/O operations write burst",
        },{
            .name = "throttling.bps-total-max",
            .type = QEMU_OPT_NUMBER,
            .help = "total bytes burst",
        },{
            .name = "throttling.bps-read-max",
            .type = QEMU_OPT_NUMBER,
            .help = "total bytes read burst",
        },{
            .name = "throttling.bps-write-max",
            .type = QEMU_OPT_NUMBER,
            .help = "total bytes write burst",
        },{
            .name = "throttling.iops-total-max-length",
            .type = QEMU_OPT_NUMBER,
            .help = "length of the iops-total-max burst period, in seconds",
        },{
            .name = "throttling.iops-read-max-length",
            .type = QEMU_OPT_NUMBER,
            .help = "length of the iops-read-max burst period, in seconds",
        },{
            .name = "throttling.iops-write-max-length",
            .type = QEMU_OPT_NUMBER,
            .help = "length of the iops-write-max burst period, in seconds",
        },{
            .name = "throttling.bps-total-max-length",
            .type = QEMU_OPT_NUMBER,
            .help = "length of the bps-total-max burst period, in seconds",
        },{
            .name = "throttling.bps-read-max-length",
            .type = QEMU_OPT_NUMBER,
            .help = "length of the bps-read-max burst period, in seconds",
        },{
            .name = "throttling.bps-write-max-length",
            .type = QEMU_OPT_NUMBER,
            .help = "length of the bps-write-max burst period, in seconds",
        },{
            .name = "throttling.iops-size",
            .type = QEMU_OPT_NUMBER,
            .help = "when limiting by iops max size of an I/O in bytes",
        },

        { /*End of list */ }
    },
};

static QemuOptsList qemu_virtfs_opts = {
    .name = "virtfs",
    .implied_opt_name = "fsdriver",
    .head = QTAILQ_HEAD_INITIALIZER(qemu_virtfs_opts.head),
    .desc = {
        {
            .name = "fsdriver",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "path",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "mount_tag",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "security_model",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "writeout",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "readonly",
            .type = QEMU_OPT_BOOL,
        }, {
            .name = "socket",
            .type = QEMU_OPT_STRING,
        }, {
            .name = "sock_fd",
            .type = QEMU_OPT_NUMBER,
        },

        { /*End of list */ }
    },
};

static void fsdev_register_config(void)
{
    qemu_add_opts(&qemu_fsdev_opts);
    qemu_add_opts(&qemu_virtfs_opts);
}
opts_init(fsdev_register_config);
