/*
 *  file related system call shims and definitions
 *
 *  Copyright (c) 2013 Stacey D. Son
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BSD_FILE_H
#define BSD_FILE_H

#include "qemu/path.h"

extern struct iovec *lock_iovec(int type, abi_ulong target_addr, int count,
        int copy);
extern void unlock_iovec(struct iovec *vec, abi_ulong target_addr, int count,
        int copy);

#endif /* BSD_FILE_H */
