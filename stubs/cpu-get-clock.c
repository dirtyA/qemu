#include "qemu/osdep.h"
#include "sysemu/cpu-timers.h"

int64_t cpu_get_clock(void)
{
    return get_clock_realtime();
}
