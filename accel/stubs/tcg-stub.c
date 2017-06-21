/*
 * QEMU TCG accelerator stub
 *
 * Copyright Red Hat, Inc. 2013
 *
 * Author: Paolo Bonzini     <pbonzini@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#include "qemu/osdep.h"
#include "qemu-common.h"
#include "cpu.h"
#include "tcg/tcg.h"
#include "exec/cpu-common.h"
#include "exec/exec-all.h"
#include "translate-all.h"
#include "exec/cpu-all.h"

void tb_lock(void)
{
}

void tb_unlock(void)
{
}

void tb_flush(CPUState *cpu)
{
}

void tb_invalidate_phys_page_fast(tb_page_addr_t start, int len)
{
}

void tb_invalidate_phys_range(tb_page_addr_t start, tb_page_addr_t end)
{
}

void tb_check_watchpoint(CPUState *cpu)
{
}

TranslationBlock *tb_gen_code(CPUState *cpu,
                              target_ulong pc, target_ulong cs_base,
                              uint32_t flags, int cflags)
{
    return NULL;
}

void tlb_reset_dirty(CPUState *cpu, ram_addr_t start1, ram_addr_t length)
{
}

void tlb_set_dirty(CPUState *cpu, target_ulong vaddr)
{
}

bool cpu_restore_state(CPUState *cpu, uintptr_t searched_pc)
{
    return false;
}

void update_fp_status(CPUX86State *env)
{
}

void dump_exec_info(FILE *f, fprintf_function cpu_fprintf)
{
}

void dump_opcount_info(FILE *f, fprintf_function cpu_fprintf)
{
}

void cpu_loop_exit(CPUState *cpu)
{
    abort();
}

void cpu_loop_exit_noexc(CPUState *cpu)
{
    abort();
}

void raise_exception(CPUX86State *env, int exception_index)
{
    abort();
}

void raise_exception_err_ra(CPUX86State *env, int exception_index,
                            int error_code, uintptr_t retaddr)
{
    abort();
}

void x86_cpu_do_interrupt(CPUState *cs)
{
}

bool x86_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
    return false;
}

int cpu_exec(CPUState *cpu)
{
    abort();
}
