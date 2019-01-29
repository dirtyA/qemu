/*
 * S390 Channel I/O
 *
 * Copyright (c) 2018 Jason J. Herne <jjherne@us.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or (at
 * your option) any later version. See the COPYING file in the top-level
 * directory.
 */

#include "libc.h"
#include "s390-ccw.h"
#include "s390-arch.h"
#include "cio.h"

static char chsc_page[PAGE_SIZE] __attribute__((__aligned__(PAGE_SIZE)));

int enable_mss_facility(void)
{
    int ret;
    ChscAreaSda *sda_area = (ChscAreaSda *) chsc_page;

    memset(sda_area, 0, PAGE_SIZE);
    sda_area->request.length = 0x0400;
    sda_area->request.code = 0x0031;
    sda_area->operation_code = 0x2;

    ret = chsc(sda_area);
    if ((ret == 0) && (sda_area->response.code == 0x0001)) {
        return 0;
    }
    return -EIO;
}

void enable_subchannel(SubChannelId schid)
{
    Schib schib;

    stsch_err(schid, &schib);
    schib.pmcw.ena = 1;
    msch(schid, &schib);
}

uint16_t cu_type(SubChannelId schid)
{
    Ccw1 sense_id_ccw;
    SenseId sense_data;

    sense_id_ccw.cmd_code = CCW_CMD_SENSE_ID;
    sense_id_ccw.cda = ptr2u32(&sense_data);
    sense_id_ccw.count = sizeof(sense_data);
    sense_id_ccw.flags |= CCW_FLAG_SLI;

    if (do_cio(schid, ptr2u32(&sense_id_ccw), CCW_FMT1)) {
        panic("Failed to run SenseID CCw\n");
    }

    return sense_data.cu_type;
}

void basic_sense(SubChannelId schid, void *sense_data, uint16_t data_size)
{
    Ccw1 senseCcw;

    senseCcw.cmd_code = CCW_CMD_BASIC_SENSE;
    senseCcw.cda = ptr2u32(sense_data);
    senseCcw.count = data_size;

    if (do_cio(schid, ptr2u32(&senseCcw), CCW_FMT1)) {
        panic("Failed to run Basic Sense CCW\n");
    }
}

static bool irb_error(Irb *irb)
{
    if (irb->scsw.cstat) {
        return true;
    }
    return irb->scsw.dstat != (SCSW_DSTAT_DEVEND | SCSW_DSTAT_CHEND);
}

/*
 * Executes a channel program at a given subchannel. The request to run the
 * channel program is sent to the subchannel, we then wait for the interrupt
 * signaling completion of the I/O operation(s) performed by the channel
 * program. Lastly we verify that the i/o operation completed without error and
 * that the interrupt we received was for the subchannel used to run the
 * channel program.
 *
 * Note: This function assumes it is running in an environment where no other
 * cpus are generating or receiving I/O interrupts. So either run it in a
 * single-cpu environment or make sure all other cpus are not doing I/O and
 * have I/O interrupts masked off.
 */
int do_cio(SubChannelId schid, uint32_t ccw_addr, int fmt)
{
    CmdOrb orb = {};
    Irb irb = {};
    sense_data_eckd_dasd sd;
    int rc, retries = 0;

    IPL_assert(fmt == 0 || fmt == 1, "Invalid ccw format");

    /* ccw_addr must be <= 24 bits and point to at least one whole ccw. */
    if (fmt == 0) {
        IPL_assert(ccw_addr <= 0xFFFFFF - 8, "Invalid ccw address");
    }

    orb.fmt = fmt ;
    orb.pfch = 1;  /* QEMU's cio implementation requires prefetch */
    orb.c64 = 1;   /* QEMU's cio implementation requires 64-bit idaws */
    orb.lpm = 0xFF; /* All paths allowed */
    orb.cpa = ccw_addr;

    while (true) {
        rc = ssch(schid, &orb);
        if (rc == 1) {
            /* Status pending, not sure why. Let's eat the status and retry. */
            tsch(schid, &irb);
            retries++;
            continue;
        }
        if (rc) {
            print_int("ssch failed with rc=", rc);
            break;
        }

        consume_io_int();

        /* collect status */
        rc = tsch(schid, &irb);
        if (rc) {
            print_int("tsch failed with rc=", rc);
            break;
        }

        if (!irb_error(&irb)) {
            break;
        }

        /*
         * Unexpected unit check, or interface-control-check. Use sense to
         * clear unit check then retry.
         */
        if ((unit_check(&irb) || iface_ctrl_check(&irb)) && retries <= 2) {
            basic_sense(schid, &sd, sizeof(sd));
            retries++;
            continue;
        }

        break;
    }

    return rc;
}
