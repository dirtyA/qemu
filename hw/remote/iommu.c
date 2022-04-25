/**
 * IOMMU for remote device
 *
 * Copyright © 2022 Oracle and/or its affiliates.
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#include "qemu/osdep.h"
#include "qemu-common.h"

#include "hw/remote/iommu.h"
#include "hw/pci/pci_bus.h"
#include "hw/pci/pci.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "trace.h"

static AddressSpace *remote_iommu_find_add_as(PCIBus *pci_bus,
                                              void *opaque, int devfn)
{
    RemoteIommu *iommu = opaque;
    RemoteIommuElem *elem = NULL;

    qemu_mutex_lock(&iommu->lock);

    elem = g_hash_table_lookup(iommu->elem_by_devfn, INT2VOIDP(devfn));

    if (!elem) {
        elem = g_malloc0(sizeof(RemoteIommuElem));
        g_hash_table_insert(iommu->elem_by_devfn, INT2VOIDP(devfn), elem);
    }

    if (!elem->mr) {
        elem->mr = MEMORY_REGION(object_new(TYPE_MEMORY_REGION));
        memory_region_set_size(elem->mr, UINT64_MAX);
        address_space_init(&elem->as, elem->mr, NULL);
    }

    qemu_mutex_unlock(&iommu->lock);

    return &elem->as;
}

void remote_iommu_unplug_dev(PCIDevice *pci_dev)
{
    AddressSpace *as = pci_device_iommu_address_space(pci_dev);
    RemoteIommuElem *elem = NULL;

    if (as == &address_space_memory) {
        return;
    }

    elem = container_of(as, RemoteIommuElem, as);

    address_space_destroy(&elem->as);

    object_unref(elem->mr);

    elem->mr = NULL;
}

static void remote_iommu_init(Object *obj)
{
    RemoteIommu *iommu = REMOTE_IOMMU(obj);

    iommu->elem_by_devfn = g_hash_table_new_full(NULL, NULL, NULL, g_free);

    qemu_mutex_init(&iommu->lock);
}

static void remote_iommu_finalize(Object *obj)
{
    RemoteIommu *iommu = REMOTE_IOMMU(obj);

    qemu_mutex_destroy(&iommu->lock);

    if (iommu->elem_by_devfn) {
        g_hash_table_destroy(iommu->elem_by_devfn);
        iommu->elem_by_devfn = NULL;
    }
}

void remote_iommu_setup(PCIBus *pci_bus)
{
    RemoteIommu *iommu = NULL;

    g_assert(pci_bus);

    iommu = REMOTE_IOMMU(object_new(TYPE_REMOTE_IOMMU));

    pci_setup_iommu(pci_bus, remote_iommu_find_add_as, iommu);

    object_property_add_child(OBJECT(pci_bus), "remote-iommu", OBJECT(iommu));

    object_unref(OBJECT(iommu));
}

static const TypeInfo remote_iommu_info = {
    .name = TYPE_REMOTE_IOMMU,
    .parent = TYPE_OBJECT,
    .instance_size = sizeof(RemoteIommu),
    .instance_init = remote_iommu_init,
    .instance_finalize = remote_iommu_finalize,
};

static void remote_iommu_register_types(void)
{
    type_register_static(&remote_iommu_info);
}

type_init(remote_iommu_register_types)
