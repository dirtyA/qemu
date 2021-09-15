/*
 * host-signal.h: signal info dependent on the host architecture
 *
 * Copyright (C) 2021 Linaro Limited
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#ifndef ARM_HOST_SIGNAL_H
#define ARM_HOST_SIGNAL_H

static inline uintptr_t host_signal_pc(ucontext_t *uc)
{
    return uc->uc_mcontext.gregs[R15];
}

static inline bool host_sigsegv_write(siginfo_t *info, ucontext_t *uc,
                                      uintptr_t pc)
{
    /*
     * In the FSR, bit 11 is WnR, assuming a v6 or
     * later processor.  On v5 we will always report
     * this as a read, which will fail later.
     */
    uint32_t fsr = uc->uc_mcontext.error_code;
    return extract32(fsr, 11, 1);
}

#endif
