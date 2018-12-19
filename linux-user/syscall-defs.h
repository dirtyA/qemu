/*
 *  Linux syscall definitions
 *  Copyright (c) 2003 Fabrice Bellard
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

SYSCALL_DEF(close, ARG_DEC);
#ifdef TARGET_NR_open
SYSCALL_DEF(open, ARG_STR, ARG_OPENFLAG, ARG_MODEFLAG);
#endif
SYSCALL_DEF(openat, ARG_ATDIRFD, ARG_STR, ARG_OPENFLAG, ARG_MODEFLAG);
SYSCALL_DEF(read, ARG_DEC, ARG_PTR, ARG_DEC);
#ifdef TARGET_NR_readlink
SYSCALL_DEF(readlink, ARG_STR, ARG_PTR, ARG_DEC);
#endif
#ifdef TARGET_NR_readlinkat
SYSCALL_DEF(readlinkat, ARG_ATDIRFD, ARG_STR, ARG_PTR, ARG_DEC);
#endif
SYSCALL_DEF(write, ARG_DEC, ARG_PTR, ARG_DEC);
