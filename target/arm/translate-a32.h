/*
 *  AArch32 translation, common definitions.
 *
 * Copyright (c) 2021 Linaro, Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TARGET_ARM_TRANSLATE_A64_H
#define TARGET_ARM_TRANSLATE_A64_H

/* Prototypes for autogenerated disassembler functions */
bool disas_m_nocp(DisasContext *dc, uint32_t insn);
bool disas_vfp(DisasContext *s, uint32_t insn);
bool disas_vfp_uncond(DisasContext *s, uint32_t insn);

void load_reg_var(DisasContext *s, TCGv_i32 var, int reg);
void arm_gen_condlabel(DisasContext *s);
bool vfp_access_check(DisasContext *s);
void read_neon_element32(TCGv_i32 dest, int reg, int ele, MemOp memop);
void read_neon_element64(TCGv_i64 dest, int reg, int ele, MemOp memop);
void write_neon_element32(TCGv_i32 src, int reg, int ele, MemOp memop);
void write_neon_element64(TCGv_i64 src, int reg, int ele, MemOp memop);
TCGv_i32 add_reg_for_lit(DisasContext *s, int reg, int ofs);
void gen_set_cpsr(TCGv_i32 var, uint32_t mask);
void gen_set_condexec(DisasContext *s);
void gen_set_pc_im(DisasContext *s, target_ulong val);
void gen_lookup_tb(DisasContext *s);
long vfp_reg_offset(bool dp, unsigned reg);
long neon_full_reg_offset(unsigned reg);
long neon_element_offset(int reg, int element, MemOp memop);
void gen_rev16(TCGv_i32 dest, TCGv_i32 var);

static inline TCGv_i32 load_cpu_offset(int offset)
{
    TCGv_i32 tmp = tcg_temp_new_i32();
    tcg_gen_ld_i32(tmp, cpu_env, offset);
    return tmp;
}

#define load_cpu_field(name) load_cpu_offset(offsetof(CPUARMState, name))

static inline void store_cpu_offset(TCGv_i32 var, int offset)
{
    tcg_gen_st_i32(var, cpu_env, offset);
    tcg_temp_free_i32(var);
}

#define store_cpu_field(var, name) \
    store_cpu_offset(var, offsetof(CPUARMState, name))

/* Create a new temporary and set it to the value of a CPU register.  */
static inline TCGv_i32 load_reg(DisasContext *s, int reg)
{
    TCGv_i32 tmp = tcg_temp_new_i32();
    load_reg_var(s, tmp, reg);
    return tmp;
}

void store_reg(DisasContext *s, int reg, TCGv_i32 var);

void gen_aa32_ld_internal_i32(DisasContext *s, TCGv_i32 val,
                              TCGv_i32 a32, int index, MemOp opc);
void gen_aa32_st_internal_i32(DisasContext *s, TCGv_i32 val,
                              TCGv_i32 a32, int index, MemOp opc);
void gen_aa32_ld_internal_i64(DisasContext *s, TCGv_i64 val,
                              TCGv_i32 a32, int index, MemOp opc);
void gen_aa32_st_internal_i64(DisasContext *s, TCGv_i64 val,
                              TCGv_i32 a32, int index, MemOp opc);
void gen_aa32_ld_i32(DisasContext *s, TCGv_i32 val, TCGv_i32 a32,
                     int index, MemOp opc);
void gen_aa32_st_i32(DisasContext *s, TCGv_i32 val, TCGv_i32 a32,
                     int index, MemOp opc);
void gen_aa32_ld_i64(DisasContext *s, TCGv_i64 val, TCGv_i32 a32,
                     int index, MemOp opc);
void gen_aa32_st_i64(DisasContext *s, TCGv_i64 val, TCGv_i32 a32,
                     int index, MemOp opc);

#define DO_GEN_LD(SUFF, OPC)                                            \
    static inline void gen_aa32_ld##SUFF(DisasContext *s, TCGv_i32 val, \
                                         TCGv_i32 a32, int index)       \
    {                                                                   \
        gen_aa32_ld_i32(s, val, a32, index, OPC);                       \
    }

#define DO_GEN_ST(SUFF, OPC)                                            \
    static inline void gen_aa32_st##SUFF(DisasContext *s, TCGv_i32 val, \
                                         TCGv_i32 a32, int index)       \
    {                                                                   \
        gen_aa32_st_i32(s, val, a32, index, OPC);                       \
    }

static inline void gen_aa32_ld64(DisasContext *s, TCGv_i64 val,
                                 TCGv_i32 a32, int index)
{
    gen_aa32_ld_i64(s, val, a32, index, MO_Q);
}

static inline void gen_aa32_st64(DisasContext *s, TCGv_i64 val,
                                 TCGv_i32 a32, int index)
{
    gen_aa32_st_i64(s, val, a32, index, MO_Q);
}

DO_GEN_LD(8u, MO_UB)
DO_GEN_LD(16u, MO_UW)
DO_GEN_LD(32u, MO_UL)
DO_GEN_ST(8, MO_UB)
DO_GEN_ST(16, MO_UW)
DO_GEN_ST(32, MO_UL)

#undef DO_GEN_LD
#undef DO_GEN_ST

#if defined(CONFIG_USER_ONLY)
#define IS_USER(s) 1
#else
#define IS_USER(s) (s->user)
#endif

/* Set NZCV flags from the high 4 bits of var.  */
#define gen_set_nzcv(var) gen_set_cpsr(var, CPSR_NZCV)

/* Swap low and high halfwords.  */
static inline void gen_swap_half(TCGv_i32 dest, TCGv_i32 var)
{
    tcg_gen_rotri_i32(dest, var, 16);
}

#endif
