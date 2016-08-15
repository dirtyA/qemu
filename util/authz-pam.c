/*
 * QEMU pam authorization driver
 *
 * Copyright (c) 2016 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "qemu/osdep.h"
#include "qemu/authz-pam.h"
#include "qom/object_interfaces.h"
#include "qapi-visit.h"

 #include <security/pam_appl.h>


static bool qauthz_pam_is_allowed(QAuthZ *authz,
                                  const char *identity,
                                  Error **errp)
{
    QAuthZPam *pauthz = QAUTHZ_PAM(authz);
    const struct pam_conv pam_conversation = { 0 };
    pam_handle_t *pamh = NULL;
    int ret;

    ret = pam_start(pauthz->service,
                    identity,
                    &pam_conversation,
                    &pamh);
    if (ret != PAM_SUCCESS) {
        error_setg(errp, "Unable to start PAM transaction: %s",
                   pam_strerror(NULL, ret));
        return false;
    }

    ret = pam_acct_mgmt(pamh, PAM_SILENT);
    if (ret != PAM_SUCCESS) {
        error_setg(errp, "Unable to authorize user '%s': %s",
                   identity, pam_strerror(pamh, ret));
        goto cleanup;
    }

 cleanup:
    pam_end(pamh, ret);
    return ret == PAM_SUCCESS;
}


static void
qauthz_pam_prop_set_service(Object *obj,
                            const char *service,
                            Error **errp G_GNUC_UNUSED)
{
    QAuthZPam *pauthz = QAUTHZ_PAM(obj);

    g_free(pauthz->service);
    pauthz->service = g_strdup(service);
}


static char *
qauthz_pam_prop_get_service(Object *obj,
                            Error **errp G_GNUC_UNUSED)
{
    QAuthZPam *pauthz = QAUTHZ_PAM(obj);

    return g_strdup(pauthz->service);
}


static void
qauthz_pam_complete(UserCreatable *uc, Error **errp)
{
}


static void
qauthz_pam_finalize(Object *obj)
{
    QAuthZPam *pauthz = QAUTHZ_PAM(obj);

    g_free(pauthz->service);
}


static void
qauthz_pam_class_init(ObjectClass *oc, void *data)
{
    UserCreatableClass *ucc = USER_CREATABLE_CLASS(oc);
    QAuthZClass *authz = QAUTHZ_CLASS(oc);

    ucc->complete = qauthz_pam_complete;
    authz->is_allowed = qauthz_pam_is_allowed;

    object_class_property_add_str(oc, "service",
                                  qauthz_pam_prop_get_service,
                                  qauthz_pam_prop_set_service,
                                  NULL);
}


QAuthZPam *qauthz_pam_new(const char *id,
                          const char *service,
                          Error **errp)
{
    return QAUTHZ_PAM(
        object_new_with_props(TYPE_QAUTHZ_PAM,
                              object_get_objects_root(),
                              id, errp,
                              "service", service,
                              NULL));
}


static const TypeInfo qauthz_pam_info = {
    .parent = TYPE_QAUTHZ,
    .name = TYPE_QAUTHZ_PAM,
    .instance_size = sizeof(QAuthZPam),
    .instance_finalize = qauthz_pam_finalize,
    .class_size = sizeof(QAuthZPamClass),
    .class_init = qauthz_pam_class_init,
    .interfaces = (InterfaceInfo[]) {
        { TYPE_USER_CREATABLE },
        { }
    }
};


static void
qauthz_pam_register_types(void)
{
    type_register_static(&qauthz_pam_info);
}


type_init(qauthz_pam_register_types);
