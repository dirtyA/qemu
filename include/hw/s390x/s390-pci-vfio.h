/*
 * s390 vfio-pci interfaces
 *
 * Copyright 2020 IBM Corp.
 * Author(s): Matthew Rosato <mjrosato@linux.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or (at
 * your option) any later version. See the COPYING file in the top-level
 * directory.
 */

#ifndef HW_S390_PCI_VFIO_H
#define HW_S390_PCI_VFIO_H

#include "hw/s390x/s390-pci-bus.h"

void s390_pci_get_clp_info(S390PCIBusDevice *pbdev);

#endif
