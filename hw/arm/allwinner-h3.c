/*
 * Allwinner H3 System on Chip emulation
 *
 * Copyright (C) 2019 Niek Linnenbank <nieklinnenbank@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "exec/address-spaces.h"
#include "qapi/error.h"
#include "qemu/error-report.h"
#include "qemu/module.h"
#include "qemu/units.h"
#include "hw/qdev-core.h"
#include "cpu.h"
#include "hw/sysbus.h"
#include "hw/char/serial.h"
#include "hw/misc/unimp.h"
#include "hw/usb/hcd-ehci.h"
#include "hw/loader.h"
#include "sysemu/sysemu.h"
#include "hw/arm/allwinner-h3.h"

/* Memory map */
const hwaddr allwinner_h3_memmap[] = {
    [AW_H3_SRAM_A1]    = 0x00000000,
    [AW_H3_SRAM_A2]    = 0x00044000,
    [AW_H3_SRAM_C]     = 0x00010000,
    [AW_H3_SYSCTRL]    = 0x01c00000,
    [AW_H3_MMC0]       = 0x01c0f000,
    [AW_H3_SID]        = 0x01c14000,
    [AW_H3_EHCI0]      = 0x01c1a000,
    [AW_H3_OHCI0]      = 0x01c1a400,
    [AW_H3_EHCI1]      = 0x01c1b000,
    [AW_H3_OHCI1]      = 0x01c1b400,
    [AW_H3_EHCI2]      = 0x01c1c000,
    [AW_H3_OHCI2]      = 0x01c1c400,
    [AW_H3_EHCI3]      = 0x01c1d000,
    [AW_H3_OHCI3]      = 0x01c1d400,
    [AW_H3_CCU]        = 0x01c20000,
    [AW_H3_PIT]        = 0x01c20c00,
    [AW_H3_UART0]      = 0x01c28000,
    [AW_H3_UART1]      = 0x01c28400,
    [AW_H3_UART2]      = 0x01c28800,
    [AW_H3_UART3]      = 0x01c28c00,
    [AW_H3_EMAC]       = 0x01c30000,
    [AW_H3_GIC_DIST]   = 0x01c81000,
    [AW_H3_GIC_CPU]    = 0x01c82000,
    [AW_H3_GIC_HYP]    = 0x01c84000,
    [AW_H3_GIC_VCPU]   = 0x01c86000,
    [AW_H3_CPUCFG]     = 0x01f01c00,
    [AW_H3_SDRAM]      = 0x40000000
};

/* List of unimplemented devices */
struct AwH3Unimplemented {
    const char *device_name;
    hwaddr base;
    hwaddr size;
} unimplemented[] = {
    { "d-engine",  0x01000000, 4 * MiB },
    { "d-inter",   0x01400000, 128 * KiB },
    { "dma",       0x01c02000, 4 * KiB },
    { "nfdc",      0x01c03000, 4 * KiB },
    { "ts",        0x01c06000, 4 * KiB },
    { "keymem",    0x01c0b000, 4 * KiB },
    { "lcd0",      0x01c0c000, 4 * KiB },
    { "lcd1",      0x01c0d000, 4 * KiB },
    { "ve",        0x01c0e000, 4 * KiB },
    { "mmc1",      0x01c10000, 4 * KiB },
    { "mmc2",      0x01c11000, 4 * KiB },
    { "crypto",    0x01c15000, 4 * KiB },
    { "msgbox",    0x01c17000, 4 * KiB },
    { "spinlock",  0x01c18000, 4 * KiB },
    { "usb0-otg",  0x01c19000, 4 * KiB },
    { "usb0-phy",  0x01c1a000, 4 * KiB },
    { "usb1-phy",  0x01c1b000, 4 * KiB },
    { "usb2-phy",  0x01c1c000, 4 * KiB },
    { "usb3-phy",  0x01c1d000, 4 * KiB },
    { "smc",       0x01c1e000, 4 * KiB },
    { "pio",       0x01c20800, 1 * KiB },
    { "owa",       0x01c21000, 1 * KiB },
    { "pwm",       0x01c21400, 1 * KiB },
    { "keyadc",    0x01c21800, 1 * KiB },
    { "pcm0",      0x01c22000, 1 * KiB },
    { "pcm1",      0x01c22400, 1 * KiB },
    { "pcm2",      0x01c22800, 1 * KiB },
    { "audio",     0x01c22c00, 2 * KiB },
    { "smta",      0x01c23400, 1 * KiB },
    { "ths",       0x01c25000, 1 * KiB },
    { "uart0",     0x01c28000, 1 * KiB },
    { "uart1",     0x01c28400, 1 * KiB },
    { "uart2",     0x01c28800, 1 * KiB },
    { "uart3",     0x01c28c00, 1 * KiB },
    { "twi0",      0x01c2ac00, 1 * KiB },
    { "twi1",      0x01c2b000, 1 * KiB },
    { "twi2",      0x01c2b400, 1 * KiB },
    { "scr",       0x01c2c400, 1 * KiB },
    { "gpu",       0x01c40000, 64 * KiB },
    { "hstmr",     0x01c60000, 4 * KiB },
    { "dramcom",   0x01c62000, 4 * KiB },
    { "dramctl0",  0x01c63000, 4 * KiB },
    { "dramphy0",  0x01c65000, 4 * KiB },
    { "spi0",      0x01c68000, 4 * KiB },
    { "spi1",      0x01c69000, 4 * KiB },
    { "csi",       0x01cb0000, 320 * KiB },
    { "tve",       0x01e00000, 64 * KiB },
    { "hdmi",      0x01ee0000, 128 * KiB },
    { "rtc",       0x01f00000, 1 * KiB },
    { "r_timer",   0x01f00800, 1 * KiB },
    { "r_intc",    0x01f00c00, 1 * KiB },
    { "r_wdog",    0x01f01000, 1 * KiB },
    { "r_prcm",    0x01f01400, 1 * KiB },
    { "r_twd",     0x01f01800, 1 * KiB },
    { "r_cir-rx",  0x01f02000, 1 * KiB },
    { "r_twi",     0x01f02400, 1 * KiB },
    { "r_uart",    0x01f02800, 1 * KiB },
    { "r_pio",     0x01f02c00, 1 * KiB },
    { "r_pwm",     0x01f03800, 1 * KiB },
    { "core-dbg",  0x3f500000, 128 * KiB },
    { "tsgen-ro",  0x3f506000, 4 * KiB },
    { "tsgen-ctl", 0x3f507000, 4 * KiB },
    { "ddr-mem",   0x40000000, 2 * GiB },
    { "n-brom",    0xffff0000, 32 * KiB },
    { "s-brom",    0xffff0000, 64 * KiB }
};

/* Per Processor Interrupts */
enum {
    AW_H3_GIC_PPI_MAINT     =  9,
    AW_H3_GIC_PPI_HYPTIMER  = 10,
    AW_H3_GIC_PPI_VIRTTIMER = 11,
    AW_H3_GIC_PPI_SECTIMER  = 13,
    AW_H3_GIC_PPI_PHYSTIMER = 14
};

/* Shared Processor Interrupts */
enum {
    AW_H3_GIC_SPI_UART0     =  0,
    AW_H3_GIC_SPI_UART1     =  1,
    AW_H3_GIC_SPI_UART2     =  2,
    AW_H3_GIC_SPI_UART3     =  3,
    AW_H3_GIC_SPI_TIMER0    = 18,
    AW_H3_GIC_SPI_TIMER1    = 19,
    AW_H3_GIC_SPI_MMC0      = 60,
    AW_H3_GIC_SPI_EHCI0     = 72,
    AW_H3_GIC_SPI_OHCI0     = 73,
    AW_H3_GIC_SPI_EHCI1     = 74,
    AW_H3_GIC_SPI_OHCI1     = 75,
    AW_H3_GIC_SPI_EHCI2     = 76,
    AW_H3_GIC_SPI_OHCI2     = 77,
    AW_H3_GIC_SPI_EHCI3     = 78,
    AW_H3_GIC_SPI_OHCI3     = 79,
    AW_H3_GIC_SPI_EMAC      = 82
};

/* Allwinner H3 general constants */
enum {
    AW_H3_GIC_NUM_SPI       = 128
};

void allwinner_h3_bootrom_setup(AwH3State *s, BlockBackend *blk)
{
    const int64_t rom_size = 32 * KiB;
    uint8_t *buffer = g_new0(uint8_t, rom_size);

    if (blk_pread(blk, 8 * KiB, buffer, rom_size) < 0) {
        error_setg(&error_fatal, "%s: failed to read BlockBackend data",
                   __func__);
        return;
    }

    rom_add_blob("allwinner-h3.bootrom", buffer, rom_size,
                  rom_size, s->memmap[AW_H3_SRAM_A1],
                  NULL, NULL, NULL, NULL, false);
    g_free(buffer);
}

static void allwinner_h3_init(Object *obj)
{
    AwH3State *s = AW_H3(obj);

    s->memmap = allwinner_h3_memmap;

    for (int i = 0; i < AW_H3_NUM_CPUS; i++) {
        object_initialize_child(obj, "cpu[*]", &s->cpus[i], sizeof(s->cpus[i]),
                                ARM_CPU_TYPE_NAME("cortex-a7"),
                                &error_abort, NULL);
    }

    sysbus_init_child_obj(obj, "gic", &s->gic, sizeof(s->gic),
                          TYPE_ARM_GIC);

    sysbus_init_child_obj(obj, "timer", &s->timer, sizeof(s->timer),
                          TYPE_AW_A10_PIT);
    object_property_add_alias(obj, "clk0-freq", OBJECT(&s->timer),
                              "clk0-freq", &error_abort);
    object_property_add_alias(obj, "clk1-freq", OBJECT(&s->timer),
                              "clk1-freq", &error_abort);

    sysbus_init_child_obj(obj, "ccu", &s->ccu, sizeof(s->ccu),
                          TYPE_AW_H3_CCU);

    sysbus_init_child_obj(obj, "sysctrl", &s->sysctrl, sizeof(s->sysctrl),
                          TYPE_AW_H3_SYSCTRL);

    sysbus_init_child_obj(obj, "cpucfg", &s->cpucfg, sizeof(s->cpucfg),
                          TYPE_AW_CPUCFG);

    sysbus_init_child_obj(obj, "sid", &s->sid, sizeof(s->sid),
                          TYPE_AW_SID);
    object_property_add_alias(obj, "identifier", OBJECT(&s->sid),
                              "identifier", &error_abort);

    sysbus_init_child_obj(obj, "mmc0", &s->mmc0, sizeof(s->mmc0),
                          TYPE_AW_SDHOST_SUN5I);

    sysbus_init_child_obj(obj, "emac", &s->emac, sizeof(s->emac),
                          TYPE_AW_SUN8I_EMAC);
}

static void allwinner_h3_realize(DeviceState *dev, Error **errp)
{
    AwH3State *s = AW_H3(dev);
    unsigned i;

    /* CPUs */
    for (i = 0; i < AW_H3_NUM_CPUS; i++) {

        /* Provide Power State Coordination Interface */
        qdev_prop_set_int32(DEVICE(&s->cpus[i]), "psci-conduit",
                            QEMU_PSCI_CONDUIT_HVC);

        /* Disable secondary CPUs */
        qdev_prop_set_bit(DEVICE(&s->cpus[i]), "start-powered-off",
                          i > 0);

        /* All exception levels required */
        qdev_prop_set_bit(DEVICE(&s->cpus[i]), "has_el3", true);
        qdev_prop_set_bit(DEVICE(&s->cpus[i]), "has_el2", true);

        /* Mark realized */
        qdev_init_nofail(DEVICE(&s->cpus[i]));
    }

    /* Generic Interrupt Controller */
    qdev_prop_set_uint32(DEVICE(&s->gic), "num-irq", AW_H3_GIC_NUM_SPI +
                                                     GIC_INTERNAL);
    qdev_prop_set_uint32(DEVICE(&s->gic), "revision", 2);
    qdev_prop_set_uint32(DEVICE(&s->gic), "num-cpu", AW_H3_NUM_CPUS);
    qdev_prop_set_bit(DEVICE(&s->gic), "has-security-extensions", false);
    qdev_prop_set_bit(DEVICE(&s->gic), "has-virtualization-extensions", true);
    qdev_init_nofail(DEVICE(&s->gic));

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gic), 0, s->memmap[AW_H3_GIC_DIST]);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gic), 1, s->memmap[AW_H3_GIC_CPU]);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gic), 2, s->memmap[AW_H3_GIC_HYP]);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gic), 3, s->memmap[AW_H3_GIC_VCPU]);

    /*
     * Wire the outputs from each CPU's generic timer and the GICv3
     * maintenance interrupt signal to the appropriate GIC PPI inputs,
     * and the GIC's IRQ/FIQ/VIRQ/VFIQ interrupt outputs to the CPU's inputs.
     */
    for (i = 0; i < AW_H3_NUM_CPUS; i++) {
        DeviceState *cpudev = DEVICE(&s->cpus[i]);
        int ppibase = AW_H3_GIC_NUM_SPI + i * GIC_INTERNAL + GIC_NR_SGIS;
        int irq;
        /*
         * Mapping from the output timer irq lines from the CPU to the
         * GIC PPI inputs used for this board.
         */
        const int timer_irq[] = {
            [GTIMER_PHYS] = AW_H3_GIC_PPI_PHYSTIMER,
            [GTIMER_VIRT] = AW_H3_GIC_PPI_VIRTTIMER,
            [GTIMER_HYP]  = AW_H3_GIC_PPI_HYPTIMER,
            [GTIMER_SEC]  = AW_H3_GIC_PPI_SECTIMER,
        };

        /* Connect CPU timer outputs to GIC PPI inputs */
        for (irq = 0; irq < ARRAY_SIZE(timer_irq); irq++) {
            qdev_connect_gpio_out(cpudev, irq,
                                  qdev_get_gpio_in(DEVICE(&s->gic),
                                                   ppibase + timer_irq[irq]));
        }

        /* Connect GIC outputs to CPU interrupt inputs */
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gic), i,
                           qdev_get_gpio_in(cpudev, ARM_CPU_IRQ));
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gic), i + AW_H3_NUM_CPUS,
                           qdev_get_gpio_in(cpudev, ARM_CPU_FIQ));
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gic), i + (2 * AW_H3_NUM_CPUS),
                           qdev_get_gpio_in(cpudev, ARM_CPU_VIRQ));
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gic), i + (3 * AW_H3_NUM_CPUS),
                           qdev_get_gpio_in(cpudev, ARM_CPU_VFIQ));

        /* GIC maintenance signal */
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gic), i + (4 * AW_H3_NUM_CPUS),
                           qdev_get_gpio_in(DEVICE(&s->gic),
                                            ppibase + AW_H3_GIC_PPI_MAINT));
    }

    /* Timer */
    qdev_init_nofail(DEVICE(&s->timer));
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->timer), 0, s->memmap[AW_H3_PIT]);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->timer), 0,
                       qdev_get_gpio_in(DEVICE(&s->gic), AW_H3_GIC_SPI_TIMER0));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->timer), 1,
                       qdev_get_gpio_in(DEVICE(&s->gic), AW_H3_GIC_SPI_TIMER1));

    /* SRAM */
    memory_region_init_ram(&s->sram_a1, OBJECT(dev), "sram A1",
                            64 * KiB, &error_abort);
    memory_region_init_ram(&s->sram_a2, OBJECT(dev), "sram A2",
                            32 * KiB, &error_abort);
    memory_region_init_ram(&s->sram_c, OBJECT(dev), "sram C",
                            44 * KiB, &error_abort);
    memory_region_add_subregion(get_system_memory(), s->memmap[AW_H3_SRAM_A1],
                                &s->sram_a1);
    memory_region_add_subregion(get_system_memory(), s->memmap[AW_H3_SRAM_A2],
                                &s->sram_a2);
    memory_region_add_subregion(get_system_memory(), s->memmap[AW_H3_SRAM_C],
                                &s->sram_c);

    /* Clock Control Unit */
    qdev_init_nofail(DEVICE(&s->ccu));
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ccu), 0, s->memmap[AW_H3_CCU]);

    /* System Control */
    qdev_init_nofail(DEVICE(&s->sysctrl));
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sysctrl), 0, s->memmap[AW_H3_SYSCTRL]);

    /* CPU Configuration */
    qdev_init_nofail(DEVICE(&s->cpucfg));
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->cpucfg), 0, s->memmap[AW_H3_CPUCFG]);

    /* Security Identifier */
    qdev_init_nofail(DEVICE(&s->sid));
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sid), 0, s->memmap[AW_H3_SID]);

    /* SD/MMC */
    qdev_init_nofail(DEVICE(&s->mmc0));
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->mmc0), 0, s->memmap[AW_H3_MMC0]);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->mmc0), 0,
                       qdev_get_gpio_in(DEVICE(&s->gic), AW_H3_GIC_SPI_MMC0));

    object_property_add_alias(OBJECT(s), "sd-bus", OBJECT(&s->mmc0),
                              "sd-bus", &error_abort);

    /* EMAC */
    if (nd_table[0].used) {
        qemu_check_nic_model(&nd_table[0], TYPE_AW_SUN8I_EMAC);
        qdev_set_nic_properties(DEVICE(&s->emac), &nd_table[0]);
    }
    qdev_init_nofail(DEVICE(&s->emac));
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->emac), 0, s->memmap[AW_H3_EMAC]);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->emac), 0,
                       qdev_get_gpio_in(DEVICE(&s->gic), AW_H3_GIC_SPI_EMAC));

    /* Universal Serial Bus */
    sysbus_create_simple(TYPE_AW_H3_EHCI, s->memmap[AW_H3_EHCI0],
                         qdev_get_gpio_in(DEVICE(&s->gic),
                                          AW_H3_GIC_SPI_EHCI0));
    sysbus_create_simple(TYPE_AW_H3_EHCI, s->memmap[AW_H3_EHCI1],
                         qdev_get_gpio_in(DEVICE(&s->gic),
                                          AW_H3_GIC_SPI_EHCI1));
    sysbus_create_simple(TYPE_AW_H3_EHCI, s->memmap[AW_H3_EHCI2],
                         qdev_get_gpio_in(DEVICE(&s->gic),
                                          AW_H3_GIC_SPI_EHCI2));
    sysbus_create_simple(TYPE_AW_H3_EHCI, s->memmap[AW_H3_EHCI3],
                         qdev_get_gpio_in(DEVICE(&s->gic),
                                          AW_H3_GIC_SPI_EHCI3));

    sysbus_create_simple("sysbus-ohci", s->memmap[AW_H3_OHCI0],
                         qdev_get_gpio_in(DEVICE(&s->gic),
                                          AW_H3_GIC_SPI_OHCI0));
    sysbus_create_simple("sysbus-ohci", s->memmap[AW_H3_OHCI1],
                         qdev_get_gpio_in(DEVICE(&s->gic),
                                          AW_H3_GIC_SPI_OHCI1));
    sysbus_create_simple("sysbus-ohci", s->memmap[AW_H3_OHCI2],
                         qdev_get_gpio_in(DEVICE(&s->gic),
                                          AW_H3_GIC_SPI_OHCI2));
    sysbus_create_simple("sysbus-ohci", s->memmap[AW_H3_OHCI3],
                         qdev_get_gpio_in(DEVICE(&s->gic),
                                          AW_H3_GIC_SPI_OHCI3));

    /* UART0. For future clocktree API: All UARTS are connected to APB2_CLK. */
    serial_mm_init(get_system_memory(), s->memmap[AW_H3_UART0], 2,
                   qdev_get_gpio_in(DEVICE(&s->gic), AW_H3_GIC_SPI_UART0),
                   115200, serial_hd(0), DEVICE_NATIVE_ENDIAN);
    /* UART1 */
    serial_mm_init(get_system_memory(), s->memmap[AW_H3_UART1], 2,
                   qdev_get_gpio_in(DEVICE(&s->gic), AW_H3_GIC_SPI_UART1),
                   115200, serial_hd(1), DEVICE_NATIVE_ENDIAN);
    /* UART2 */
    serial_mm_init(get_system_memory(), s->memmap[AW_H3_UART2], 2,
                   qdev_get_gpio_in(DEVICE(&s->gic), AW_H3_GIC_SPI_UART2),
                   115200, serial_hd(2), DEVICE_NATIVE_ENDIAN);
    /* UART3 */
    serial_mm_init(get_system_memory(), s->memmap[AW_H3_UART3], 2,
                   qdev_get_gpio_in(DEVICE(&s->gic), AW_H3_GIC_SPI_UART3),
                   115200, serial_hd(3), DEVICE_NATIVE_ENDIAN);

    /* Unimplemented devices */
    for (i = 0; i < ARRAY_SIZE(unimplemented); i++) {
        create_unimplemented_device(unimplemented[i].device_name,
                                    unimplemented[i].base,
                                    unimplemented[i].size);
    }
}

static void allwinner_h3_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = allwinner_h3_realize;
    /* Reason: uses serial_hd() in realize function */
    dc->user_creatable = false;
}

static const TypeInfo allwinner_h3_type_info = {
    .name = TYPE_AW_H3,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(AwH3State),
    .instance_init = allwinner_h3_init,
    .class_init = allwinner_h3_class_init,
};

static void allwinner_h3_register_types(void)
{
    type_register_static(&allwinner_h3_type_info);
}

type_init(allwinner_h3_register_types)
