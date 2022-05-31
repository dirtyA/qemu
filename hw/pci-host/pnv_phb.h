/*
 * QEMU PowerPC PowerNV Proxy PHB model
 *
 * Copyright (c) 2022, IBM Corporation.
 *
 * This code is licensed under the GPL version 2 or later. See the
 * COPYING file in the top-level directory.
 */

#ifndef PCI_HOST_PNV_PHB_H
#define PCI_HOST_PNV_PHB_H

#include "hw/pci/pcie_host.h"
#include "hw/pci/pcie_port.h"
#include "qom/object.h"

typedef struct PnvChip PnvChip;
typedef struct PnvPhb4PecState PnvPhb4PecState;

struct PnvPHB {
    PCIExpressHost parent_obj;

    uint32_t chip_id;
    uint32_t phb_id;
    uint32_t version;
    char bus_path[8];

    PnvChip *chip;

    PnvPhb4PecState *pec;

    /* The PHB backend (PnvPHB3, PnvPHB4 ...) being used */
    Object *backend;
};

#define TYPE_PNV_PHB "pnv-phb"
OBJECT_DECLARE_SIMPLE_TYPE(PnvPHB, PNV_PHB)

#endif /* PCI_HOST_PNV_PHB_H */
