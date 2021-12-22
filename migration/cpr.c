/*
 * Copyright (c) 2021 Oracle and/or its affiliates.
 *
 * This work is licensed under the terms of the GNU GPL, version 2.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "exec/memory.h"
#include "io/channel-buffer.h"
#include "io/channel-file.h"
#include "migration.h"
#include "migration/cpr.h"
#include "migration/global_state.h"
#include "migration/misc.h"
#include "migration/snapshot.h"
#include "qapi/error.h"
#include "qapi/qapi-commands-cpr.h"
#include "qapi/qmp/qerror.h"
#include "qemu-file-channel.h"
#include "qemu-file.h"
#include "savevm.h"
#include "sysemu/cpu-timers.h"
#include "sysemu/replay.h"
#include "sysemu/runstate.h"
#include "sysemu/runstate-action.h"
#include "sysemu/sysemu.h"
#include "sysemu/xen.h"

void qmp_cpr_save(const char *filename, CprMode mode, Error **errp)
{
    int ret;
    QEMUFile *f;
    int flags = O_CREAT | O_WRONLY | O_TRUNC;
    int saved_vm_running = runstate_is_running();

    if (qemu_check_ram_volatile(errp)) {
        return;
    }

    if (migrate_colo_enabled()) {
        error_setg(errp, "cpr-save does not support x-colo");
        return;
    }

    if (replay_mode != REPLAY_MODE_NONE) {
        error_setg(errp, "cpr-save does not support replay");
        return;
    }

    if (global_state_store()) {
        error_setg(errp, "Error saving global state");
        return;
    }

    f = qemu_file_open(filename, flags, 0600, "cpr-save", errp);
    if (!f) {
        return;
    }

    if (runstate_check(RUN_STATE_SUSPENDED)) {
        /* Update timers_state before saving.  Suspend did not so do. */
        cpu_disable_ticks();
    }
    vm_stop(RUN_STATE_SAVE_VM);

    cpr_set_mode(mode);
    ret = qemu_save_device_state(f);
    qemu_fclose(f);
    if (ret < 0) {
        error_setg(errp, "Error %d while saving VM state", ret);
        goto err;
    }

    return;

err:
    if (saved_vm_running) {
        vm_start();
    }
    cpr_set_mode(CPR_MODE_NONE);
}

void qmp_cpr_load(const char *filename, Error **errp)
{
    QEMUFile *f;
    int ret;
    RunState state;

    if (runstate_is_running()) {
        error_setg(errp, "cpr-load called for a running VM");
        return;
    }

    f = qemu_file_open(filename, O_RDONLY, 0, "cpr-load", errp);
    if (!f) {
        return;
    }

    if (qemu_get_be32(f) != QEMU_VM_FILE_MAGIC ||
        qemu_get_be32(f) != QEMU_VM_FILE_VERSION) {
        error_setg(errp, "%s is not a vmstate file", filename);
        qemu_fclose(f);
        return;
    }

    cpr_set_mode(CPR_MODE_REBOOT);
    ret = qemu_load_device_state(f);
    qemu_fclose(f);
    if (ret < 0) {
        error_setg(errp, "Error %d while loading VM state", ret);
        goto out;
    }

    state = global_state_get_runstate();
    if (state == RUN_STATE_RUNNING) {
        vm_start();
    } else {
        runstate_set(state);
        if (runstate_check(RUN_STATE_SUSPENDED)) {
            /* Force vm_start to be called later. */
            qemu_system_start_on_wakeup_request();
        }
    }

out:
    cpr_set_mode(CPR_MODE_NONE);
}
