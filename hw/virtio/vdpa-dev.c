/*
 * Vhost Vdpa Device
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2022. All Rights Reserved.
 *
 * Authors:
 *   Longpeng <longpeng2@huawei.com>
 *
 * Largely based on the "vhost-user-blk-pci.c" and "vhost-user-blk.c" implemented by:
 *   Changpeng Liu <changpeng.liu@intel.com>
 *
 * This work is licensed under the terms of the GNU LGPL, version 2 or later.
 * See the COPYING.LIB file in the top-level directory.
 */
#include "qemu/osdep.h"
#include <sys/ioctl.h>
#include <linux/vhost.h>
#include "qapi/error.h"
#include "qemu/error-report.h"
#include "qemu/cutils.h"
#include "hw/qdev-core.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "hw/virtio/vhost.h"
#include "hw/virtio/virtio.h"
#include "hw/virtio/virtio-bus.h"
#include "hw/virtio/virtio-access.h"
#include "hw/virtio/vdpa-dev.h"
#include "sysemu/sysemu.h"
#include "sysemu/runstate.h"

static void vhost_vdpa_device_class_init(ObjectClass *klass, void *data)
{
    return;
}

static void vhost_vdpa_device_instance_init(Object *obj)
{
    return;
}

static const TypeInfo vhost_vdpa_device_info = {
    .name = TYPE_VHOST_VDPA_DEVICE,
    .parent = TYPE_VIRTIO_DEVICE,
    .instance_size = sizeof(VhostVdpaDevice),
    .class_init = vhost_vdpa_device_class_init,
    .instance_init = vhost_vdpa_device_instance_init,
};

static void register_vhost_vdpa_device_type(void)
{
    type_register_static(&vhost_vdpa_device_info);
}

type_init(register_vhost_vdpa_device_type);
