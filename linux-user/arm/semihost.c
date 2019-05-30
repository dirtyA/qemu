/*
 * ARM Semihosting Console Support
 *
 * Copyright (c) 2019 Linaro Ltd
 *
 * Currently ARM is unique in having support for semihosting support
 * in linux-user. So for now we implement the common console API but
 * just for arm linux-user.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "hw/semihosting/console.h"
#include "qemu.h"

int qemu_semihosting_console_outs(CPUArchState *env, target_ulong addr)
{
    int len;
    void *s = lock_user_string(addr);
    if (!s) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: passed inaccessible address " TARGET_FMT_lx,
                      __func__, addr);
        return 0;
    }

    len = write(STDERR_FILENO, s, strlen(s));
    unlock_user(s, addr, 0);
    return len;
}

void qemu_semihosting_console_outc(CPUArchState *env, target_ulong addr)
{
    char c;

    if (get_user_u8(c, addr)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: passed inaccessible address " TARGET_FMT_lx,
                      __func__, addr);
    } else {
        if (write(STDERR_FILENO, &c, 1) != 1) {
            qemu_log_mask(LOG_UNIMP, "%s: unexpected write to stdout failure",
                          __func__);
        }
    }
}
