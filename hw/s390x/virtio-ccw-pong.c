/*
 * virtio ccw PONG device
 *
 * Copyright 2020, IBM Corp.
 * Author(s): Pierre Morel <pmorel@linux.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or (at
 * your option) any later version. See the COPYING file in the top-level
 * directory.
 */

#include "qemu/osdep.h"
#include "hw/qdev-properties.h"
#include "hw/virtio/virtio.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "virtio-ccw.h"

static void virtio_ccw_pong_realize(VirtioCcwDevice *ccw_dev, Error **errp)
{
    VirtIOPONGCcw *dev = VIRTIO_PONG_CCW(ccw_dev);
    DeviceState *vdev = DEVICE(&dev->vdev);

    if (!qdev_realize(vdev, BUS(&ccw_dev->bus), errp)) {
        return;
    }
}

static void virtio_ccw_pong_instance_init(Object *obj)
{
    VirtIOPONGCcw *dev = VIRTIO_PONG_CCW(obj);

    virtio_instance_init_common(obj, &dev->vdev, sizeof(dev->vdev),
                                TYPE_VIRTIO_PONG);
}

static Property virtio_ccw_pong_properties[] = {
    DEFINE_PROP_UINT32("max_revision", VirtioCcwDevice, max_rev,
                       VIRTIO_CCW_MAX_REV),
    DEFINE_PROP_END_OF_LIST(),
};

static void virtio_ccw_pong_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtIOCCWDeviceClass *k = VIRTIO_CCW_DEVICE_CLASS(klass);

    k->realize = virtio_ccw_pong_realize;
    device_class_set_props(dc, virtio_ccw_pong_properties);
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static const TypeInfo virtio_ccw_pong = {
    .name          = TYPE_VIRTIO_PONG_CCW,
    .parent        = TYPE_VIRTIO_CCW_DEVICE,
    .instance_size = sizeof(VirtIOPONGCcw),
    .instance_init = virtio_ccw_pong_instance_init,
    .class_init    = virtio_ccw_pong_class_init,
};

static void virtio_ccw_pong_register(void)
{
    type_register_static(&virtio_ccw_pong);
}

type_init(virtio_ccw_pong_register)
