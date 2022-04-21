/* HPPA cores and system support chips.  */

#ifndef HW_HPPA_SYS_H
#define HW_HPPA_SYS_H

#include "hw/pci/pci.h"
#include "hw/pci/pci_host.h"
#include "hw/boards.h"
#include "hw/intc/i8259.h"

#include "hppa_hardware.h"

DeviceState *lasi_init(MemoryRegion *);
#define enable_lasi_lan()       0

/* hppa_pci.c.  */
extern const MemoryRegionOps hppa_pci_ignore_ops;
extern const MemoryRegionOps hppa_pci_conf1_ops;
extern const MemoryRegionOps hppa_pci_iack_ops;

#endif
