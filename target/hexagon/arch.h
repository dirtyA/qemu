/*
 *  Copyright(c) 2019-2020 Qualcomm Innovation Center, Inc. All Rights Reserved.
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

#ifndef HEXAGON_ARCH_H
#define HEXAGON_ARCH_H

#include "cpu.h"
#include "hex_arch_types.h"

extern size8u_t interleave(size4u_t odd, size4u_t even);
extern size8u_t deinterleave(size8u_t src);
extern size4u_t carry_from_add64(size8u_t a, size8u_t b, size4u_t c);
extern size4s_t conv_round(size4s_t a, int n);
extern size16s_t cast8s_to_16s(size8s_t a);
extern size8s_t cast16s_to_8s(size16s_t a);
extern size16s_t add128(size16s_t a, size16s_t b);
extern size16s_t sub128(size16s_t a, size16s_t b);
extern size16s_t shiftr128(size16s_t a, size4u_t n);
extern size16s_t shiftl128(size16s_t a, size4u_t n);
extern size16s_t and128(size16s_t a, size16s_t b);
extern void arch_fpop_start(CPUHexagonState *env);
extern void arch_fpop_end(CPUHexagonState *env);
extern void arch_raise_fpflag(unsigned int flags);
extern int arch_sf_recip_common(size4s_t *Rs, size4s_t *Rt, size4s_t *Rd,
                                int *adjust);
extern int arch_sf_invsqrt_common(size4s_t *Rs, size4s_t *Rd, int *adjust);

#endif
