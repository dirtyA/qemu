/*
 * PCMCIA emulation
 *
 * Copyright 2013 SUSE LINUX Products GmbH
 */

#include "qemu/osdep.h"
#include "qemu/module.h"
#include "hw/pcmcia.h"

static const TypeInfo pcmcia_card_type_info = {
    .name = TYPE_PCMCIA_CARD,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(PCMCIACardState),
    .abstract = true,
    .class_size = sizeof(PCMCIACardClass),
};
TYPE_INFO(pcmcia_card_type_info)


