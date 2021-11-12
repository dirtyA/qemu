/*
 * vmnet-shared.c
 *
 * Copyright(c) 2021 Vladislav Yaroshchuk <yaroshchuk2000@gmail.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#include "qemu/osdep.h"
#include "qapi/qapi-types-net.h"
#include "qapi/error.h"
#include "vmnet_int.h"
#include "clients.h"

#include <vmnet/vmnet.h>

typedef struct VmnetSharedState {
  VmnetCommonState cs;
} VmnetSharedState;


static xpc_object_t create_if_desc(const Netdev *netdev, Error **errp)
{
    const NetdevVmnetSharedOptions *options = &(netdev->u.vmnet_shared);
    xpc_object_t if_desc = xpc_dictionary_create(NULL, NULL, 0);

    xpc_dictionary_set_uint64(
        if_desc,
        vmnet_operation_mode_key,
        VMNET_SHARED_MODE
    );

    xpc_dictionary_set_bool(
        if_desc,
        vmnet_enable_isolation_key,
        options->isolated
    );

    if (options->has_nat66_prefix) {
        xpc_dictionary_set_string(if_desc,
                                  vmnet_nat66_prefix_key,
                                  options->nat66_prefix);
    }

    if (options->has_dhcpstart ||
        options->has_dhcpend ||
        options->has_subnet_mask) {

        if (options->has_dhcpstart &&
            options->has_dhcpend &&
            options->has_subnet_mask) {

            xpc_dictionary_set_string(if_desc,
                                      vmnet_start_address_key,
                                      options->dhcpstart);
            xpc_dictionary_set_string(if_desc,
                                      vmnet_end_address_key,
                                      options->dhcpend);
            xpc_dictionary_set_string(if_desc,
                                      vmnet_subnet_mask_key,
                                      options->subnet_mask);
        } else {
            error_setg(
                errp,
                "'dhcpstart', 'dhcpend', 'subnet_mask' "
                "should be provided together"
            );
        }
    }

    return if_desc;
}

static NetClientInfo net_vmnet_shared_info = {
    .type = NET_CLIENT_DRIVER_VMNET_SHARED,
    .size = sizeof(VmnetSharedState),
    .receive = vmnet_receive_common,
    .cleanup = vmnet_cleanup_common,
};

int net_init_vmnet_shared(const Netdev *netdev, const char *name,
                          NetClientState *peer, Error **errp)
{
    NetClientState *nc = qemu_new_net_client(&net_vmnet_shared_info,
                                             peer, "vmnet-shared", name);
    xpc_object_t if_desc = create_if_desc(netdev, errp);

    return vmnet_if_create(nc, if_desc, errp, NULL);
}

