/*
 * Device's clock input and output
 *
 * Copyright GreenSocs 2016-2020
 *
 * Authors:
 *  Frederic Konrad
 *  Damien Hedde
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#ifndef QDEV_CLOCK_H
#define QDEV_CLOCK_H

#include "hw/clock.h"

/**
 * qdev_init_clock_in:
 * @dev: the device to add an input clock to
 * @name: the name of the clock (can't be NULL).
 * @callback: optional callback to be called on update or NULL.
 * @opaque: argument for the callback
 * @returns: a pointer to the newly added clock
 *
 * Add an input clock to device @dev as a clock named @name.
 * This adds a child<> property.
 * The callback will be called with @opaque as opaque parameter.
 */
Clock *qdev_init_clock_in(DeviceState *dev, const char *name,
                          ClockCallback *callback, void *opaque);

/**
 * qdev_init_clock_out:
 * @dev: the device to add an output clock to
 * @name: the name of the clock (can't be NULL).
 * @callback: optional callback to be called on update or NULL.
 * @returns: a pointer to the newly added clock
 *
 * Add an output clock to device @dev as a clock named @name.
 * This adds a child<> property.
 */
Clock *qdev_init_clock_out(DeviceState *dev, const char *name);

/**
 * qdev_get_clock_in:
 * @dev: the device which has the clock
 * @name: the name of the clock (can't be NULL).
 * @returns: a pointer to the clock
 *
 * Get the input clock @name from @dev or NULL if does not exist.
 */
Clock *qdev_get_clock_in(DeviceState *dev, const char *name);

/**
 * qdev_get_clock_out:
 * @dev: the device which has the clock
 * @name: the name of the clock (can't be NULL).
 * @returns: a pointer to the clock
 *
 * Get the output clock @name from @dev or NULL if does not exist.
 */
Clock *qdev_get_clock_out(DeviceState *dev, const char *name);

/**
 * qdev_connect_clock_in:
 * @dev: a device
 * @name: the name of an input clock in @dev
 * @source: the source clock (an output clock of another device for example)
 *
 * Set the source clock of input clock @name of device @dev to @source.
 * @source period update will be propagated to @name clock.
 */
static inline void qdev_connect_clock_in(DeviceState *dev, const char *name,
                                         Clock *source)
{
    clock_set_source(qdev_get_clock_in(dev, name), source);
}

/**
 * qdev_alias_clock:
 * @dev: the device which has the clock
 * @name: the name of the clock in @dev (can't be NULL)
 * @alias_dev: the device to add the clock
 * @alias_name: the name of the clock in @container
 * @returns: a pointer to the clock
 *
 * Add a clock @alias_name in @alias_dev which is an alias of the clock @name
 * in @dev. The direction _in_ or _out_ will the same as the original.
 * An alias clock must not be modified or used by @alias_dev and should
 * typically be only only for device composition purpose.
 */
Clock *qdev_alias_clock(DeviceState *dev, const char *name,
                        DeviceState *alias_dev, const char *alias_name);

/**
 * qdev_finalize_clocklist:
 * @dev: the device being finalize
 *
 * Clear the clocklist from @dev. Only used internally in qdev.
 */
void qdev_finalize_clocklist(DeviceState *dev);

#endif /* QDEV_CLOCK_H */
