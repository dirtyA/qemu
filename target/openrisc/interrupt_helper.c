/*
 * OpenRISC interrupt helper routines
 *
 * Copyright (c) 2011-2012 Jia Liu <proljc@gmail.com>
 *                         Feng Gao <gf91597@gmail.com>
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
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "exec/exec-all.h"
#include "exec/helper-proto.h"

void HELPER(rfe)(CPUOpenRISCState *env)
{
    OpenRISCCPU *cpu = openrisc_env_get_cpu(env);
#ifndef CONFIG_USER_ONLY
    int need_flush_tlb = (cpu->env.sr & (SR_SM | SR_IME | SR_DME)) ^
                         (cpu->env.esr & (SR_SM | SR_IME | SR_DME));
    if (need_flush_tlb) {
        CPUState *cs = CPU(cpu);
        tlb_flush(cs);
    }
#endif
    cpu->env.pc = cpu->env.epcr;
    cpu->env.lock_addr = -1;
    cpu_set_sr(&cpu->env, cpu->env.esr);
}
