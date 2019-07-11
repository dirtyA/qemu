/*
 * QEMU RISC-V Hart Array interface
 *
 * Copyright (c) 2017 SiFive, Inc.
 *
 * Holds the state of a heterogenous array of RISC-V harts
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* NOTE: May only be included into target-dependent code */
/* FIXME Does not pass make check-headers for user emulation, yet! */

#ifndef HW_RISCV_HART_H
#define HW_RISCV_HART_H

#include "hw/sysbus.h"
#include "target/riscv/cpu.h"

#define TYPE_RISCV_HART_ARRAY "riscv.hart_array"

#define RISCV_HART_ARRAY(obj) \
    OBJECT_CHECK(RISCVHartArrayState, (obj), TYPE_RISCV_HART_ARRAY)

typedef struct RISCVHartArrayState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    uint32_t num_harts;
    char *cpu_type;
    RISCVCPU *harts;
} RISCVHartArrayState;

#endif
