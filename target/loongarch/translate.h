/*
 * LoongArch translation routines.
 *
 * Copyright (c) 2021 Loongson Technology Corporation Limited
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#ifndef TARGET_LOONGARCH_TRANSLATE_H
#define TARGET_LOONGARCH_TRANSLATE_H

#include "exec/translator.h"

#define LOONGARCH_DEBUG_DISAS 0

typedef struct DisasContext {
    DisasContextBase base;
    target_ulong saved_pc;
    target_ulong page_start;
    uint32_t opcode;
    uint64_t insn_flags;
    /* Routine used to access memory */
    int mem_idx;
    MemOp default_tcg_memop_mask;
    uint32_t hflags, saved_hflags;
    target_ulong btarget;
    uint64_t PAMask;
} DisasContext;

void generate_exception_err(DisasContext *ctx, int excp, int err);
void generate_exception_end(DisasContext *ctx, int excp);
void gen_reserved_instruction(DisasContext *ctx);

void check_insn(DisasContext *ctx, uint64_t flags);
void check_loongarch_64(DisasContext *ctx);
void check_fpu_enabled(DisasContext *ctx);

void gen_base_offset_addr(DisasContext *ctx, TCGv addr, int base, int offset);
void gen_move_low32(TCGv ret, TCGv_i64 arg);
void gen_move_high32(TCGv ret, TCGv_i64 arg);
void gen_load_gpr(TCGv t, int reg);
void gen_store_gpr(TCGv t, int reg);
void gen_load_fpr32(DisasContext *ctx, TCGv_i32 t, int reg);
void gen_load_fpr64(DisasContext *ctx, TCGv_i64 t, int reg);
void gen_store_fpr32(DisasContext *ctx, TCGv_i32 t, int reg);
void gen_store_fpr64(DisasContext *ctx, TCGv_i64 t, int reg);

/*
 * Address Computation and Large Constant Instructions
 */
void gen_op_addr_add(DisasContext *ctx, TCGv ret, TCGv arg0, TCGv arg1);

extern TCGv cpu_gpr[32], cpu_PC;
extern TCGv_i32 fpu_fscr0;
extern TCGv_i64 fpu_f64[32];
extern TCGv bcond;

#endif
