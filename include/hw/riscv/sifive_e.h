/*
 * SiFive E series machine interface
 *
 * Copyright (c) 2017 SiFive, Inc.
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

#ifndef HW_SIFIVE_E_H
#define HW_SIFIVE_E_H

#include "hw/riscv/riscv_hart.h"
#include "hw/riscv/sifive_cpu.h"
#include "hw/riscv/sifive_gpio.h"
#include "qom/object.h"

#define TYPE_RISCV_E_SOC "riscv.sifive.e.soc"
typedef struct SiFiveESoCState SiFiveESoCState;
DECLARE_INSTANCE_CHECKER(SiFiveESoCState, RISCV_E_SOC,
                         TYPE_RISCV_E_SOC)

struct SiFiveESoCState {
    /*< private >*/
    DeviceState parent_obj;

    /*< public >*/
    RISCVHartArrayState cpus;
    DeviceState *plic;
    SIFIVEGPIOState gpio;
    MemoryRegion xip_mem;
    MemoryRegion mask_rom;
};

struct SiFiveEState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    SiFiveESoCState soc;
    bool revb;
};
typedef struct SiFiveEState SiFiveEState;

#define TYPE_RISCV_E_MACHINE MACHINE_TYPE_NAME("sifive_e")
DECLARE_INSTANCE_CHECKER(SiFiveEState, RISCV_E_MACHINE,
                         TYPE_RISCV_E_MACHINE)

enum {
    SIFIVE_E_DEBUG,
    SIFIVE_E_MROM,
    SIFIVE_E_OTP,
    SIFIVE_E_CLINT,
    SIFIVE_E_PLIC,
    SIFIVE_E_AON,
    SIFIVE_E_PRCI,
    SIFIVE_E_OTP_CTRL,
    SIFIVE_E_GPIO0,
    SIFIVE_E_UART0,
    SIFIVE_E_QSPI0,
    SIFIVE_E_PWM0,
    SIFIVE_E_UART1,
    SIFIVE_E_QSPI1,
    SIFIVE_E_PWM1,
    SIFIVE_E_QSPI2,
    SIFIVE_E_PWM2,
    SIFIVE_E_XIP,
    SIFIVE_E_DTIM
};

enum {
    SIFIVE_E_UART0_IRQ  = 3,
    SIFIVE_E_UART1_IRQ  = 4,
    SIFIVE_E_GPIO0_IRQ0 = 8
};

#define SIFIVE_E_PLIC_HART_CONFIG "M"
#define SIFIVE_E_PLIC_NUM_SOURCES 127
#define SIFIVE_E_PLIC_NUM_PRIORITIES 7
#define SIFIVE_E_PLIC_PRIORITY_BASE 0x04
#define SIFIVE_E_PLIC_PENDING_BASE 0x1000
#define SIFIVE_E_PLIC_ENABLE_BASE 0x2000
#define SIFIVE_E_PLIC_ENABLE_STRIDE 0x80
#define SIFIVE_E_PLIC_CONTEXT_BASE 0x200000
#define SIFIVE_E_PLIC_CONTEXT_STRIDE 0x1000

#endif
