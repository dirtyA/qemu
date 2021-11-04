/*
 * FreeBSD has a common ucontext definition for all architectures.
 *
 * Copyright 2021 Warner Losh <imp@bsdimp.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later OR BSD-3-Clause
 */

/*
 * Defines the common bits for all of FreeBSD's architectures. Has to be
 * included AFTER the MD target_mcontext_t is defined, however, so can't
 * be in the grab-bag that is target_os_signal.h.
 */

/* See FreeBSD's sys/ucontext.h */
#define TARGET_MC_GET_CLEAR_RET 0x0001

/* FreeBSD's sys/_ucontext.h structures */
typedef struct target_ucontext {
    target_sigset_t     uc_sigmask;
    target_mcontext_t   uc_mcontext;
    abi_ulong           uc_link;
    target_stack_t      uc_stack;
    int32_t             uc_flags;
    int32_t             __spare__[4];
} target_ucontext_t;
