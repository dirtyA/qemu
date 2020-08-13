/*
 * Adjunct Processor (AP) matrix device
 *
 * Copyright 2018 IBM Corp.
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or (at
 * your option) any later version. See the COPYING file in the top-level
 * directory.
 */
#include "qemu/osdep.h"
#include "qemu/module.h"
#include "qapi/error.h"
#include "hw/s390x/ap-device.h"

static void ap_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->desc = "AP device class";
    dc->hotpluggable = false;
}

static const TypeInfo ap_device_info = {
    .name = AP_DEVICE_TYPE,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(APDevice),
    .class_size = sizeof(DeviceClass),
    .class_init = ap_class_init,
    .abstract = true,
};
TYPE_INFO(ap_device_info)


