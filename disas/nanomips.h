
#ifndef NANOMIPS_DISASSEMBLER_H
#define NANOMIPS_DISASSEMBLER_H

#include <string>
#ifdef INCLUDE_STANDALONE_UNIT_TEST
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

namespace img
{
    typedef unsigned long long address;
}
#else
#include "imgleeds/imgleeds/address.h"
#endif



class NMD
{
public:
    enum TABLE_ENTRY_TYPE {
        instruction,
        call_instruction,
        branch_instruction,
        return_instruction,
        reserved_block,
        pool,
    };
    enum TABLE_ATTRIBUTE_TYPE {
        MIPS64_    = 0x00000001,
        XNP_       = 0x00000002,
        XMMS_      = 0x00000004,
        EVA_       = 0x00000008,
        DSP_       = 0x00000010,
        MT_        = 0x00000020,
        EJTAG_     = 0x00000040,
        TLBINV_    = 0x00000080,
        CP0_       = 0x00000100,
        CP1_       = 0x00000200,
        CP2_       = 0x00000400,
        UDI_       = 0x00000800,
        MCU_       = 0x00001000,
        VZ_        = 0x00002000,
        TLB_       = 0x00004000,
        MVH_       = 0x00008000,
        ALL_ATTRIBUTES = 0xffffffffull,
    };


    NMD(img::address pc, TABLE_ATTRIBUTE_TYPE requested_instruction_catagories)
        : m_pc(pc)
        , m_requested_instruction_catagories(requested_instruction_catagories)
    {
    }

    int Disassemble(const uint16 *data, std::string & dis,
                    TABLE_ENTRY_TYPE & type);

private:
    img::address           m_pc;
    TABLE_ATTRIBUTE_TYPE   m_requested_instruction_catagories;

    typedef std::string
            (NMD:: *disassembly_function)(uint64 instruction);
    typedef bool
            (NMD:: *conditional_function)(uint64 instruction);

    struct Pool {
        TABLE_ENTRY_TYPE     type;
        struct Pool          *next_table;
        int                  next_table_size;
        int                  instructions_size;
        uint64               mask;
        uint64               value;
        disassembly_function disassembly;
        conditional_function condition;
        uint64               attributes;
    };

    uint64 extract_op_code_value(const uint16 *data, int size);
    int Disassemble(const uint16 *data, std::string & dis,
                    TABLE_ENTRY_TYPE & type, const Pool *table, int table_size);

    uint64 renumber_registers(uint64 index, uint64 *register_list,
                              size_t register_list_size);
    uint64 encode_gpr3(uint64 d);
    uint64 encode_gpr3_store(uint64 d);
    uint64 encode_rd1_from_rd(uint64 d);
    uint64 encode_gpr4_zero(uint64 d);
    uint64 encode_gpr4(uint64 d);
    uint64 encode_rd2_reg1(uint64 d);
    uint64 encode_rd2_reg2(uint64 d);

    uint64 copy(uint64 d);
    int64 copy(int64 d);
    int64 neg_copy(uint64 d);
    int64 neg_copy(int64 d);
    uint64 encode_rs3_and_check_rs3_ge_rt3(uint64 d);
    uint64 encode_rs3_and_check_rs3_lt_rt3(uint64 d);
    uint64 encode_s_from_address(uint64 d);
    uint64 encode_u_from_address(uint64 d);
    uint64 encode_s_from_s_hi(uint64 d);
    uint64 encode_count3_from_count(uint64 d);
    uint64 encode_shift3_from_shift(uint64 d);
    int64 encode_eu_from_s_li16(uint64 d);
    uint64 encode_msbd_from_size(uint64 d);
    uint64 encode_eu_from_u_andi16(uint64 d);

    uint64 encode_msbd_from_pos_and_size(uint64 d);

    uint64 encode_rt1_from_rt(uint64 d);
    uint64 encode_lsb_from_pos_and_size(uint64 d);

    std::string save_restore_list(uint64 rt, uint64 count, uint64 gp);

    std::string GPR(uint64 reg);
    std::string FPR(uint64 reg);
    std::string AC(uint64 reg);
    std::string IMMEDIATE(uint64 value);
    std::string IMMEDIATE(int64 value);
    std::string CPR(uint64 reg);
    std::string ADDRESS(uint64 value, int instruction_size);

    uint64 extr_codeil0il0bs19Fmsb18(
               uint64 instruction);
    uint64 extr_shift3il0il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_uil3il3bs9Fmsb11(
               uint64 instruction);
    uint64 extr_countil0il0bs4Fmsb3(
               uint64 instruction);
    uint64 extr_rtz3il7il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_uil1il1bs17Fmsb17(
               uint64 instruction);
    int64 extr_sil11il0bs10Tmsb9(
               uint64 instruction);
    int64 extr_sil0il11bs1_il1il1bs10Tmsb11(
               uint64 instruction);
    uint64 extr_uil10il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_rtz4il21il0bs3_il25il3bs1Fmsb3(
               uint64 instruction);
    uint64 extr_sail11il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_shiftil0il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_shiftxil7il1bs4Fmsb4(
               uint64 instruction);
    uint64 extr_hintil21il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_count3il12il0bs3Fmsb2(
               uint64 instruction);
    int64 extr_sil0il31bs1_il2il21bs10_il12il12bs9Tmsb31(
               uint64 instruction);
    int64 extr_sil0il7bs1_il1il1bs6Tmsb7(
               uint64 instruction);
    uint64 extr_u2il9il0bs2Fmsb1(
               uint64 instruction);
    uint64 extr_codeil16il0bs10Fmsb9(
               uint64 instruction);
    uint64 extr_rsil16il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_uil1il1bs2Fmsb2(
               uint64 instruction);
    uint64 extr_stripeil6il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_xil17il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_xil2il0bs1_il15il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_acil14il0bs2Fmsb1(
               uint64 instruction);
    uint64 extr_shiftil16il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_rd1il24il0bs1Fmsb0(
               uint64 instruction);
    int64 extr_sil0il10bs1_il1il1bs9Tmsb10(
               uint64 instruction);
    uint64 extr_euil0il0bs7Fmsb6(
               uint64 instruction);
    uint64 extr_shiftil0il0bs6Fmsb5(
               uint64 instruction);
    uint64 extr_xil10il0bs6Fmsb5(
               uint64 instruction);
    uint64 extr_countil16il0bs4Fmsb3(
               uint64 instruction);
    uint64 extr_codeil0il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_xil10il0bs4_il22il0bs4Fmsb3(
               uint64 instruction);
    uint64 extr_uil0il0bs12Fmsb11(
               uint64 instruction);
    uint64 extr_rsil0il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_uil3il3bs18Fmsb20(
               uint64 instruction);
    uint64 extr_xil12il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_uil0il2bs4Fmsb5(
               uint64 instruction);
    uint64 extr_cofunil3il0bs23Fmsb22(
               uint64 instruction);
    uint64 extr_uil0il2bs3Fmsb4(
               uint64 instruction);
    uint64 extr_xil10il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_rd3il1il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_sail12il0bs4Fmsb3(
               uint64 instruction);
    uint64 extr_rtil21il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_ruil3il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_xil21il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_xil9il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_uil0il0bs18Fmsb17(
               uint64 instruction);
    uint64 extr_xil14il0bs1_il15il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_rsz4il0il0bs3_il4il3bs1Fmsb3(
               uint64 instruction);
    uint64 extr_xil24il0bs1Fmsb0(
               uint64 instruction);
    int64 extr_sil0il21bs1_il1il1bs20Tmsb21(
               uint64 instruction);
    uint64 extr_opil3il0bs23Fmsb22(
               uint64 instruction);
    uint64 extr_rs4il0il0bs3_il4il3bs1Fmsb3(
               uint64 instruction);
    uint64 extr_bitil21il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_rtil37il0bs5Fmsb4(
               uint64 instruction);
    int64 extr_sil16il0bs6Tmsb5(
               uint64 instruction);
    uint64 extr_xil6il0bs3_il10il0bs1Fmsb2(
               uint64 instruction);
    uint64 extr_rd2il3il1bs1_il8il0bs1Fmsb1(
               uint64 instruction);
    uint64 extr_xil16il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_codeil0il0bs18Fmsb17(
               uint64 instruction);
    uint64 extr_xil0il0bs12Fmsb11(
               uint64 instruction);
    uint64 extr_sizeil16il0bs5Fmsb4(
               uint64 instruction);
    int64 extr_sil2il2bs6_il15il8bs1Tmsb8(
               uint64 instruction);
    uint64 extr_uil0il0bs16Fmsb15(
               uint64 instruction);
    uint64 extr_fsil16il0bs5Fmsb4(
               uint64 instruction);
    int64 extr_sil0il0bs8_il15il8bs1Tmsb8(
               uint64 instruction);
    uint64 extr_stypeil16il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_rt1il9il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_hsil16il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_xil10il0bs1_il14il0bs2Fmsb1(
               uint64 instruction);
    uint64 extr_selil11il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_lsbil0il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_xil14il0bs2Fmsb1(
               uint64 instruction);
    uint64 extr_gpil2il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_rt3il7il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_ftil21il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_uil11il0bs7Fmsb6(
               uint64 instruction);
    uint64 extr_csil16il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_xil16il0bs10Fmsb9(
               uint64 instruction);
    uint64 extr_rt4il5il0bs3_il9il3bs1Fmsb3(
               uint64 instruction);
    uint64 extr_msbdil6il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_uil0il2bs6Fmsb7(
               uint64 instruction);
    uint64 extr_xil17il0bs9Fmsb8(
               uint64 instruction);
    uint64 extr_sail13il0bs3Fmsb2(
               uint64 instruction);
    int64 extr_sil0il14bs1_il1il1bs13Tmsb14(
               uint64 instruction);
    uint64 extr_rs3il4il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_uil0il32bs32Fmsb63(
               uint64 instruction);
    uint64 extr_shiftil6il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_csil21il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_shiftxil6il0bs6Fmsb5(
               uint64 instruction);
    uint64 extr_rtil5il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_opil21il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_uil0il2bs7Fmsb8(
               uint64 instruction);
    uint64 extr_bitil11il0bs6Fmsb5(
               uint64 instruction);
    uint64 extr_xil10il0bs1_il11il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_maskil14il0bs7Fmsb6(
               uint64 instruction);
    uint64 extr_euil0il0bs4Fmsb3(
               uint64 instruction);
    uint64 extr_uil4il4bs4Fmsb7(
               uint64 instruction);
    int64 extr_sil3il3bs5_il15il8bs1Tmsb8(
               uint64 instruction);
    uint64 extr_ftil11il0bs5Fmsb4(
               uint64 instruction);
    int64 extr_sil0il16bs16_il16il0bs16Tmsb31(
               uint64 instruction);
    uint64 extr_uil13il0bs8Fmsb7(
               uint64 instruction);
    uint64 extr_xil15il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_xil11il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_uil2il2bs16Fmsb17(
               uint64 instruction);
    uint64 extr_rdil11il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_c0sil16il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_codeil0il0bs2Fmsb1(
               uint64 instruction);
    int64 extr_sil0il25bs1_il1il1bs24Tmsb25(
               uint64 instruction);
    uint64 extr_xil0il0bs3_il4il0bs1Fmsb2(
               uint64 instruction);
    uint64 extr_uil0il0bs2Fmsb1(
               uint64 instruction);
    uint64 extr_uil3il3bs1_il8il2bs1Fmsb3(
               uint64 instruction);
    uint64 extr_xil9il0bs3_il16il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_fdil11il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_xil6il0bs3Fmsb2(
               uint64 instruction);
    uint64 extr_uil0il2bs5Fmsb6(
               uint64 instruction);
    uint64 extr_rtz4il5il0bs3_il9il3bs1Fmsb3(
               uint64 instruction);
    uint64 extr_selil11il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_ctil21il0bs5Fmsb4(
               uint64 instruction);
    uint64 extr_xil11il0bs1Fmsb0(
               uint64 instruction);
    uint64 extr_uil2il2bs19Fmsb20(
               uint64 instruction);
    int64 extr_sil0il0bs3_il4il3bs1Tmsb3(
               uint64 instruction);
    uint64 extr_uil0il1bs4Fmsb4(
               uint64 instruction);
    uint64 extr_xil9il0bs2Fmsb1(
               uint64 instruction);

    bool BNEC_16__cond(uint64 instruction);
    bool ADDIU_32__cond(uint64 instruction);
    bool P16_BR1_cond(uint64 instruction);
    bool ADDIU_RS5__cond(uint64 instruction);
    bool BEQC_16__cond(uint64 instruction);
    bool SLTU_cond(uint64 instruction);
    bool PREF_S9__cond(uint64 instruction);
    bool BALRSC_cond(uint64 instruction);
    bool MOVE_cond(uint64 instruction);
    bool PREFE_cond(uint64 instruction);

    std::string SIGRIE(uint64 instruction);
    std::string SYSCALL_32_(uint64 instruction);
    std::string HYPCALL(uint64 instruction);
    std::string BREAK_32_(uint64 instruction);
    std::string SDBBP_32_(uint64 instruction);
    std::string ADDIU_32_(uint64 instruction);
    std::string TEQ(uint64 instruction);
    std::string TNE(uint64 instruction);
    std::string SEB(uint64 instruction);
    std::string SLLV(uint64 instruction);
    std::string MUL_32_(uint64 instruction);
    std::string MFC0(uint64 instruction);
    std::string MFHC0(uint64 instruction);
    std::string SEH(uint64 instruction);
    std::string SRLV(uint64 instruction);
    std::string MUH(uint64 instruction);
    std::string MTC0(uint64 instruction);
    std::string MTHC0(uint64 instruction);
    std::string SRAV(uint64 instruction);
    std::string MULU(uint64 instruction);
    std::string MFGC0(uint64 instruction);
    std::string MFHGC0(uint64 instruction);
    std::string ROTRV(uint64 instruction);
    std::string MUHU(uint64 instruction);
    std::string MTGC0(uint64 instruction);
    std::string MTHGC0(uint64 instruction);
    std::string ADD(uint64 instruction);
    std::string DIV(uint64 instruction);
    std::string DMFC0(uint64 instruction);
    std::string ADDU_32_(uint64 instruction);
    std::string MOD(uint64 instruction);
    std::string DMTC0(uint64 instruction);
    std::string SUB(uint64 instruction);
    std::string DIVU(uint64 instruction);
    std::string DMFGC0(uint64 instruction);
    std::string RDHWR(uint64 instruction);
    std::string SUBU_32_(uint64 instruction);
    std::string MODU(uint64 instruction);
    std::string DMTGC0(uint64 instruction);
    std::string MOVZ(uint64 instruction);
    std::string MOVN(uint64 instruction);
    std::string FORK(uint64 instruction);
    std::string MFTR(uint64 instruction);
    std::string MFHTR(uint64 instruction);
    std::string AND_32_(uint64 instruction);
    std::string YIELD(uint64 instruction);
    std::string MTTR(uint64 instruction);
    std::string MTHTR(uint64 instruction);
    std::string OR_32_(uint64 instruction);
    std::string DMT(uint64 instruction);
    std::string DVPE(uint64 instruction);
    std::string EMT(uint64 instruction);
    std::string EVPE(uint64 instruction);
    std::string NOR(uint64 instruction);
    std::string XOR_32_(uint64 instruction);
    std::string SLT(uint64 instruction);
    std::string DVP(uint64 instruction);
    std::string EVP(uint64 instruction);
    std::string SLTU(uint64 instruction);
    std::string SOV(uint64 instruction);
    std::string SPECIAL2(uint64 instruction);
    std::string COP2_1(uint64 instruction);
    std::string UDI(uint64 instruction);
    std::string CMP_EQ_PH(uint64 instruction);
    std::string ADDQ_PH(uint64 instruction);
    std::string ADDQ_S_PH(uint64 instruction);
    std::string SHILO(uint64 instruction);
    std::string MULEQ_S_W_PHL(uint64 instruction);
    std::string MUL_PH(uint64 instruction);
    std::string MUL_S_PH(uint64 instruction);
    std::string REPL_PH(uint64 instruction);
    std::string CMP_LT_PH(uint64 instruction);
    std::string ADDQH_PH(uint64 instruction);
    std::string ADDQH_R_PH(uint64 instruction);
    std::string MULEQ_S_W_PHR(uint64 instruction);
    std::string PRECR_QB_PH(uint64 instruction);
    std::string CMP_LE_PH(uint64 instruction);
    std::string ADDQH_W(uint64 instruction);
    std::string ADDQH_R_W(uint64 instruction);
    std::string MULEU_S_PH_QBL(uint64 instruction);
    std::string PRECRQ_QB_PH(uint64 instruction);
    std::string CMPGU_EQ_QB(uint64 instruction);
    std::string ADDU_QB(uint64 instruction);
    std::string ADDU_S_QB(uint64 instruction);
    std::string MULEU_S_PH_QBR(uint64 instruction);
    std::string PRECRQ_PH_W(uint64 instruction);
    std::string CMPGU_LT_QB(uint64 instruction);
    std::string ADDU_PH(uint64 instruction);
    std::string ADDU_S_PH(uint64 instruction);
    std::string MULQ_RS_PH(uint64 instruction);
    std::string PRECRQ_RS_PH_W(uint64 instruction);
    std::string CMPGU_LE_QB(uint64 instruction);
    std::string ADDUH_QB(uint64 instruction);
    std::string ADDUH_R_QB(uint64 instruction);
    std::string MULQ_S_PH(uint64 instruction);
    std::string PRECRQU_S_QB_PH(uint64 instruction);
    std::string CMPGDU_EQ_QB(uint64 instruction);
    std::string SHRAV_PH(uint64 instruction);
    std::string SHRAV_R_PH(uint64 instruction);
    std::string MULQ_RS_W(uint64 instruction);
    std::string PACKRL_PH(uint64 instruction);
    std::string CMPGDU_LT_QB(uint64 instruction);
    std::string SHRAV_QB(uint64 instruction);
    std::string SHRAV_R_QB(uint64 instruction);
    std::string MULQ_S_W(uint64 instruction);
    std::string PICK_QB(uint64 instruction);
    std::string CMPGDU_LE_QB(uint64 instruction);
    std::string SUBQ_PH(uint64 instruction);
    std::string SUBQ_S_PH(uint64 instruction);
    std::string APPEND(uint64 instruction);
    std::string PICK_PH(uint64 instruction);
    std::string CMPU_EQ_QB(uint64 instruction);
    std::string SUBQH_PH(uint64 instruction);
    std::string SUBQH_R_PH(uint64 instruction);
    std::string PREPEND(uint64 instruction);
    std::string CMPU_LT_QB(uint64 instruction);
    std::string SUBQH_W(uint64 instruction);
    std::string SUBQH_R_W(uint64 instruction);
    std::string MODSUB(uint64 instruction);
    std::string CMPU_LE_QB(uint64 instruction);
    std::string SUBU_QB(uint64 instruction);
    std::string SUBU_S_QB(uint64 instruction);
    std::string SHRAV_R_W(uint64 instruction);
    std::string SHRA_R_W(uint64 instruction);
    std::string ADDQ_S_W(uint64 instruction);
    std::string SUBU_PH(uint64 instruction);
    std::string SUBU_S_PH(uint64 instruction);
    std::string SHRLV_PH(uint64 instruction);
    std::string SHRA_PH(uint64 instruction);
    std::string SHRA_R_PH(uint64 instruction);
    std::string SUBQ_S_W(uint64 instruction);
    std::string SUBUH_QB(uint64 instruction);
    std::string SUBUH_R_QB(uint64 instruction);
    std::string SHRLV_QB(uint64 instruction);
    std::string ADDSC(uint64 instruction);
    std::string SHLLV_PH(uint64 instruction);
    std::string SHLLV_S_PH(uint64 instruction);
    std::string SHLLV_QB(uint64 instruction);
    std::string SHLL_PH(uint64 instruction);
    std::string SHLL_S_PH(uint64 instruction);
    std::string ADDWC(uint64 instruction);
    std::string PRECR_SRA_PH_W(uint64 instruction);
    std::string PRECR_SRA_R_PH_W(uint64 instruction);
    std::string SHLLV_S_W(uint64 instruction);
    std::string SHLL_S_W(uint64 instruction);
    std::string LBX(uint64 instruction);
    std::string SBX(uint64 instruction);
    std::string LBUX(uint64 instruction);
    std::string LHX(uint64 instruction);
    std::string SHX(uint64 instruction);
    std::string LHUX(uint64 instruction);
    std::string LWUX(uint64 instruction);
    std::string LWX(uint64 instruction);
    std::string SWX(uint64 instruction);
    std::string LWC1X(uint64 instruction);
    std::string SWC1X(uint64 instruction);
    std::string LDX(uint64 instruction);
    std::string SDX(uint64 instruction);
    std::string LDC1X(uint64 instruction);
    std::string SDC1X(uint64 instruction);
    std::string LHXS(uint64 instruction);
    std::string SHXS(uint64 instruction);
    std::string LHUXS(uint64 instruction);
    std::string LWUXS(uint64 instruction);
    std::string LWXS_32_(uint64 instruction);
    std::string SWXS(uint64 instruction);
    std::string LWC1XS(uint64 instruction);
    std::string SWC1XS(uint64 instruction);
    std::string LDXS(uint64 instruction);
    std::string SDXS(uint64 instruction);
    std::string LDC1XS(uint64 instruction);
    std::string SDC1XS(uint64 instruction);
    std::string LSA(uint64 instruction);
    std::string EXTW(uint64 instruction);
    std::string MFHI_DSP_(uint64 instruction);
    std::string MFLO_DSP_(uint64 instruction);
    std::string MTHI_DSP_(uint64 instruction);
    std::string MTLO_DSP_(uint64 instruction);
    std::string MTHLIP(uint64 instruction);
    std::string SHILOV(uint64 instruction);
    std::string RDDSP(uint64 instruction);
    std::string WRDSP(uint64 instruction);
    std::string EXTP(uint64 instruction);
    std::string EXTPDP(uint64 instruction);
    std::string SHLL_QB(uint64 instruction);
    std::string SHRL_QB(uint64 instruction);
    std::string MAQ_S_W_PHR(uint64 instruction);
    std::string MAQ_SA_W_PHR(uint64 instruction);
    std::string MAQ_S_W_PHL(uint64 instruction);
    std::string MAQ_SA_W_PHL(uint64 instruction);
    std::string EXTR_W(uint64 instruction);
    std::string EXTR_R_W(uint64 instruction);
    std::string EXTR_RS_W(uint64 instruction);
    std::string EXTR_S_H(uint64 instruction);
    std::string DPA_W_PH(uint64 instruction);
    std::string DPAQ_S_W_PH(uint64 instruction);
    std::string DPS_W_PH(uint64 instruction);
    std::string DPSQ_S_W_PH(uint64 instruction);
    std::string MADD_DSP_(uint64 instruction);
    std::string MULT_DSP_(uint64 instruction);
    std::string EXTRV_W(uint64 instruction);
    std::string DPAX_W_PH(uint64 instruction);
    std::string DPAQ_SA_L_W(uint64 instruction);
    std::string DPSX_W_PH(uint64 instruction);
    std::string DPSQ_SA_L_W(uint64 instruction);
    std::string MADDU_DSP_(uint64 instruction);
    std::string MULTU_DSP_(uint64 instruction);
    std::string EXTRV_R_W(uint64 instruction);
    std::string DPAU_H_QBL(uint64 instruction);
    std::string DPAQX_S_W_PH(uint64 instruction);
    std::string DPSU_H_QBL(uint64 instruction);
    std::string DPSQX_S_W_PH(uint64 instruction);
    std::string EXTPV(uint64 instruction);
    std::string MSUB_DSP_(uint64 instruction);
    std::string MULSA_W_PH(uint64 instruction);
    std::string EXTRV_RS_W(uint64 instruction);
    std::string DPAU_H_QBR(uint64 instruction);
    std::string DPAQX_SA_W_PH(uint64 instruction);
    std::string DPSU_H_QBR(uint64 instruction);
    std::string DPSQX_SA_W_PH(uint64 instruction);
    std::string EXTPDPV(uint64 instruction);
    std::string MSUBU_DSP_(uint64 instruction);
    std::string MULSAQ_S_W_PH(uint64 instruction);
    std::string EXTRV_S_H(uint64 instruction);
    std::string ABSQ_S_QB(uint64 instruction);
    std::string REPLV_PH(uint64 instruction);
    std::string ABSQ_S_PH(uint64 instruction);
    std::string REPLV_QB(uint64 instruction);
    std::string ABSQ_S_W(uint64 instruction);
    std::string INSV(uint64 instruction);
    std::string CLO(uint64 instruction);
    std::string MFC2(uint64 instruction);
    std::string PRECEQ_W_PHL(uint64 instruction);
    std::string CLZ(uint64 instruction);
    std::string MTC2(uint64 instruction);
    std::string PRECEQ_W_PHR(uint64 instruction);
    std::string DMFC2(uint64 instruction);
    std::string PRECEQU_PH_QBL(uint64 instruction);
    std::string PRECEQU_PH_QBLA(uint64 instruction);
    std::string DMTC2(uint64 instruction);
    std::string MFHC2(uint64 instruction);
    std::string PRECEQU_PH_QBR(uint64 instruction);
    std::string PRECEQU_PH_QBRA(uint64 instruction);
    std::string MTHC2(uint64 instruction);
    std::string PRECEU_PH_QBL(uint64 instruction);
    std::string PRECEU_PH_QBLA(uint64 instruction);
    std::string CFC2(uint64 instruction);
    std::string PRECEU_PH_QBR(uint64 instruction);
    std::string PRECEU_PH_QBRA(uint64 instruction);
    std::string CTC2(uint64 instruction);
    std::string RADDU_W_QB(uint64 instruction);
    std::string TLBGP(uint64 instruction);
    std::string TLBP(uint64 instruction);
    std::string TLBGINV(uint64 instruction);
    std::string TLBINV(uint64 instruction);
    std::string TLBGR(uint64 instruction);
    std::string TLBR(uint64 instruction);
    std::string TLBGINVF(uint64 instruction);
    std::string TLBINVF(uint64 instruction);
    std::string TLBGWI(uint64 instruction);
    std::string TLBWI(uint64 instruction);
    std::string TLBGWR(uint64 instruction);
    std::string TLBWR(uint64 instruction);
    std::string DI(uint64 instruction);
    std::string EI(uint64 instruction);
    std::string WAIT(uint64 instruction);
    std::string IRET(uint64 instruction);
    std::string RDPGPR(uint64 instruction);
    std::string DERET(uint64 instruction);
    std::string WRPGPR(uint64 instruction);
    std::string ERET(uint64 instruction);
    std::string ERETNC(uint64 instruction);
    std::string SHRA_QB(uint64 instruction);
    std::string SHRA_R_QB(uint64 instruction);
    std::string SHRL_PH(uint64 instruction);
    std::string REPL_QB(uint64 instruction);
    std::string ADDIU_GP_W_(uint64 instruction);
    std::string LD_GP_(uint64 instruction);
    std::string SD_GP_(uint64 instruction);
    std::string LW_GP_(uint64 instruction);
    std::string SW_GP_(uint64 instruction);
    std::string LI_48_(uint64 instruction);
    std::string ADDIU_48_(uint64 instruction);
    std::string ADDIU_GP48_(uint64 instruction);
    std::string ADDIUPC_48_(uint64 instruction);
    std::string LWPC_48_(uint64 instruction);
    std::string SWPC_48_(uint64 instruction);
    std::string DADDIU_48_(uint64 instruction);
    std::string DLUI_48_(uint64 instruction);
    std::string LDPC_48_(uint64 instruction);
    std::string SDPC_48_(uint64 instruction);
    std::string ORI(uint64 instruction);
    std::string XORI(uint64 instruction);
    std::string ANDI_32_(uint64 instruction);
    std::string SAVE_32_(uint64 instruction);
    std::string RESTORE_32_(uint64 instruction);
    std::string RESTORE_JRC_32_(uint64 instruction);
    std::string SAVEF(uint64 instruction);
    std::string RESTOREF(uint64 instruction);
    std::string SLTI(uint64 instruction);
    std::string SLTIU(uint64 instruction);
    std::string SEQI(uint64 instruction);
    std::string ADDIU_NEG_(uint64 instruction);
    std::string DADDIU_U12_(uint64 instruction);
    std::string DADDIU_NEG_(uint64 instruction);
    std::string DROTX(uint64 instruction);
    std::string NOP_32_(uint64 instruction);
    std::string EHB(uint64 instruction);
    std::string PAUSE(uint64 instruction);
    std::string SYNC(uint64 instruction);
    std::string SLL_32_(uint64 instruction);
    std::string SRL_32_(uint64 instruction);
    std::string SRA(uint64 instruction);
    std::string ROTR(uint64 instruction);
    std::string DSLL(uint64 instruction);
    std::string DSLL32(uint64 instruction);
    std::string DSRL(uint64 instruction);
    std::string DSRL32(uint64 instruction);
    std::string DSRA(uint64 instruction);
    std::string DSRA32(uint64 instruction);
    std::string DROTR(uint64 instruction);
    std::string DROTR32(uint64 instruction);
    std::string ROTX(uint64 instruction);
    std::string INS(uint64 instruction);
    std::string DINSU(uint64 instruction);
    std::string DINSM(uint64 instruction);
    std::string DINS(uint64 instruction);
    std::string EXT(uint64 instruction);
    std::string DEXTU(uint64 instruction);
    std::string DEXTM(uint64 instruction);
    std::string DEXT(uint64 instruction);
    std::string RINT_S(uint64 instruction);
    std::string RINT_D(uint64 instruction);
    std::string ADD_S(uint64 instruction);
    std::string SELEQZ_S(uint64 instruction);
    std::string SELEQZ_D(uint64 instruction);
    std::string CLASS_S(uint64 instruction);
    std::string CLASS_D(uint64 instruction);
    std::string SUB_S(uint64 instruction);
    std::string SELNEZ_S(uint64 instruction);
    std::string SELNEZ_D(uint64 instruction);
    std::string MUL_S(uint64 instruction);
    std::string SEL_S(uint64 instruction);
    std::string SEL_D(uint64 instruction);
    std::string DIV_S(uint64 instruction);
    std::string ADD_D(uint64 instruction);
    std::string SUB_D(uint64 instruction);
    std::string MUL_D(uint64 instruction);
    std::string MADDF_S(uint64 instruction);
    std::string MADDF_D(uint64 instruction);
    std::string DIV_D(uint64 instruction);
    std::string MSUBF_S(uint64 instruction);
    std::string MSUBF_D(uint64 instruction);
    std::string MIN_S(uint64 instruction);
    std::string MIN_D(uint64 instruction);
    std::string MAX_S(uint64 instruction);
    std::string MAX_D(uint64 instruction);
    std::string MINA_S(uint64 instruction);
    std::string MINA_D(uint64 instruction);
    std::string MAXA_S(uint64 instruction);
    std::string MAXA_D(uint64 instruction);
    std::string CVT_L_S(uint64 instruction);
    std::string CVT_L_D(uint64 instruction);
    std::string RSQRT_S(uint64 instruction);
    std::string RSQRT_D(uint64 instruction);
    std::string FLOOR_L_S(uint64 instruction);
    std::string FLOOR_L_D(uint64 instruction);
    std::string CVT_W_S(uint64 instruction);
    std::string CVT_W_D(uint64 instruction);
    std::string SQRT_S(uint64 instruction);
    std::string SQRT_D(uint64 instruction);
    std::string FLOOR_W_S(uint64 instruction);
    std::string FLOOR_W_D(uint64 instruction);
    std::string CFC1(uint64 instruction);
    std::string RECIP_S(uint64 instruction);
    std::string RECIP_D(uint64 instruction);
    std::string CEIL_L_S(uint64 instruction);
    std::string CEIL_L_D(uint64 instruction);
    std::string CTC1(uint64 instruction);
    std::string CEIL_W_S(uint64 instruction);
    std::string CEIL_W_D(uint64 instruction);
    std::string MFC1(uint64 instruction);
    std::string CVT_S_PL(uint64 instruction);
    std::string TRUNC_L_S(uint64 instruction);
    std::string TRUNC_L_D(uint64 instruction);
    std::string DMFC1(uint64 instruction);
    std::string MTC1(uint64 instruction);
    std::string CVT_S_PU(uint64 instruction);
    std::string TRUNC_W_S(uint64 instruction);
    std::string TRUNC_W_D(uint64 instruction);
    std::string DMTC1(uint64 instruction);
    std::string MFHC1(uint64 instruction);
    std::string ROUND_L_S(uint64 instruction);
    std::string ROUND_L_D(uint64 instruction);
    std::string MTHC1(uint64 instruction);
    std::string ROUND_W_S(uint64 instruction);
    std::string ROUND_W_D(uint64 instruction);
    std::string MOV_S(uint64 instruction);
    std::string MOV_D(uint64 instruction);
    std::string ABS_S(uint64 instruction);
    std::string ABS_D(uint64 instruction);
    std::string NEG_S(uint64 instruction);
    std::string NEG_D(uint64 instruction);
    std::string CVT_D_S(uint64 instruction);
    std::string CVT_D_W(uint64 instruction);
    std::string CVT_D_L(uint64 instruction);
    std::string CVT_S_D(uint64 instruction);
    std::string CVT_S_W(uint64 instruction);
    std::string CVT_S_L(uint64 instruction);
    std::string CMP_AF_S(uint64 instruction);
    std::string CMP_UN_S(uint64 instruction);
    std::string CMP_EQ_S(uint64 instruction);
    std::string CMP_UEQ_S(uint64 instruction);
    std::string CMP_LT_S(uint64 instruction);
    std::string CMP_ULT_S(uint64 instruction);
    std::string CMP_LE_S(uint64 instruction);
    std::string CMP_ULE_S(uint64 instruction);
    std::string CMP_SAF_S(uint64 instruction);
    std::string CMP_SUN_S(uint64 instruction);
    std::string CMP_SEQ_S(uint64 instruction);
    std::string CMP_SUEQ_S(uint64 instruction);
    std::string CMP_SLT_S(uint64 instruction);
    std::string CMP_SULT_S(uint64 instruction);
    std::string CMP_SLE_S(uint64 instruction);
    std::string CMP_SULE_S(uint64 instruction);
    std::string CMP_OR_S(uint64 instruction);
    std::string CMP_UNE_S(uint64 instruction);
    std::string CMP_NE_S(uint64 instruction);
    std::string CMP_SOR_S(uint64 instruction);
    std::string CMP_SUNE_S(uint64 instruction);
    std::string CMP_SNE_S(uint64 instruction);
    std::string CMP_AF_D(uint64 instruction);
    std::string CMP_UN_D(uint64 instruction);
    std::string CMP_EQ_D(uint64 instruction);
    std::string CMP_UEQ_D(uint64 instruction);
    std::string CMP_LT_D(uint64 instruction);
    std::string CMP_ULT_D(uint64 instruction);
    std::string CMP_LE_D(uint64 instruction);
    std::string CMP_ULE_D(uint64 instruction);
    std::string CMP_SAF_D(uint64 instruction);
    std::string CMP_SUN_D(uint64 instruction);
    std::string CMP_SEQ_D(uint64 instruction);
    std::string CMP_SUEQ_D(uint64 instruction);
    std::string CMP_SLT_D(uint64 instruction);
    std::string CMP_SULT_D(uint64 instruction);
    std::string CMP_SLE_D(uint64 instruction);
    std::string CMP_SULE_D(uint64 instruction);
    std::string CMP_OR_D(uint64 instruction);
    std::string CMP_UNE_D(uint64 instruction);
    std::string CMP_NE_D(uint64 instruction);
    std::string CMP_SOR_D(uint64 instruction);
    std::string CMP_SUNE_D(uint64 instruction);
    std::string CMP_SNE_D(uint64 instruction);
    std::string DLSA(uint64 instruction);
    std::string DSLLV(uint64 instruction);
    std::string DMUL(uint64 instruction);
    std::string DSRLV(uint64 instruction);
    std::string DMUH(uint64 instruction);
    std::string DSRAV(uint64 instruction);
    std::string DMULU(uint64 instruction);
    std::string DROTRV(uint64 instruction);
    std::string DMUHU(uint64 instruction);
    std::string DADD(uint64 instruction);
    std::string DDIV(uint64 instruction);
    std::string DADDU(uint64 instruction);
    std::string DMOD(uint64 instruction);
    std::string DSUB(uint64 instruction);
    std::string DDIVU(uint64 instruction);
    std::string DSUBU(uint64 instruction);
    std::string DMODU(uint64 instruction);
    std::string EXTD(uint64 instruction);
    std::string EXTD32(uint64 instruction);
    std::string DCLO(uint64 instruction);
    std::string DCLZ(uint64 instruction);
    std::string LUI(uint64 instruction);
    std::string ALUIPC(uint64 instruction);
    std::string ADDIUPC_32_(uint64 instruction);
    std::string LB_GP_(uint64 instruction);
    std::string SB_GP_(uint64 instruction);
    std::string LBU_GP_(uint64 instruction);
    std::string ADDIU_GP_B_(uint64 instruction);
    std::string LH_GP_(uint64 instruction);
    std::string LHU_GP_(uint64 instruction);
    std::string SH_GP_(uint64 instruction);
    std::string LWC1_GP_(uint64 instruction);
    std::string SWC1_GP_(uint64 instruction);
    std::string LDC1_GP_(uint64 instruction);
    std::string SDC1_GP_(uint64 instruction);
    std::string LWU_GP_(uint64 instruction);
    std::string LB_U12_(uint64 instruction);
    std::string SB_U12_(uint64 instruction);
    std::string LBU_U12_(uint64 instruction);
    std::string PREF_U12_(uint64 instruction);
    std::string LH_U12_(uint64 instruction);
    std::string SH_U12_(uint64 instruction);
    std::string LHU_U12_(uint64 instruction);
    std::string LWU_U12_(uint64 instruction);
    std::string LW_U12_(uint64 instruction);
    std::string SW_U12_(uint64 instruction);
    std::string LWC1_U12_(uint64 instruction);
    std::string SWC1_U12_(uint64 instruction);
    std::string LD_U12_(uint64 instruction);
    std::string SD_U12_(uint64 instruction);
    std::string LDC1_U12_(uint64 instruction);
    std::string SDC1_U12_(uint64 instruction);
    std::string LB_S9_(uint64 instruction);
    std::string SB_S9_(uint64 instruction);
    std::string LBU_S9_(uint64 instruction);
    std::string SYNCI(uint64 instruction);
    std::string PREF_S9_(uint64 instruction);
    std::string LH_S9_(uint64 instruction);
    std::string SH_S9_(uint64 instruction);
    std::string LHU_S9_(uint64 instruction);
    std::string LWU_S9_(uint64 instruction);
    std::string LW_S9_(uint64 instruction);
    std::string SW_S9_(uint64 instruction);
    std::string LWC1_S9_(uint64 instruction);
    std::string SWC1_S9_(uint64 instruction);
    std::string LD_S9_(uint64 instruction);
    std::string SD_S9_(uint64 instruction);
    std::string LDC1_S9_(uint64 instruction);
    std::string SDC1_S9_(uint64 instruction);
    std::string ASET(uint64 instruction);
    std::string ACLR(uint64 instruction);
    std::string UALH(uint64 instruction);
    std::string UASH(uint64 instruction);
    std::string CACHE(uint64 instruction);
    std::string LWC2(uint64 instruction);
    std::string SWC2(uint64 instruction);
    std::string LL(uint64 instruction);
    std::string LLWP(uint64 instruction);
    std::string SC(uint64 instruction);
    std::string SCWP(uint64 instruction);
    std::string LDC2(uint64 instruction);
    std::string SDC2(uint64 instruction);
    std::string LLD(uint64 instruction);
    std::string LLDP(uint64 instruction);
    std::string SCD(uint64 instruction);
    std::string SCDP(uint64 instruction);
    std::string LBE(uint64 instruction);
    std::string SBE(uint64 instruction);
    std::string LBUE(uint64 instruction);
    std::string SYNCIE(uint64 instruction);
    std::string PREFE(uint64 instruction);
    std::string LHE(uint64 instruction);
    std::string SHE(uint64 instruction);
    std::string LHUE(uint64 instruction);
    std::string CACHEE(uint64 instruction);
    std::string LWE(uint64 instruction);
    std::string SWE(uint64 instruction);
    std::string LLE(uint64 instruction);
    std::string LLWPE(uint64 instruction);
    std::string SCE(uint64 instruction);
    std::string SCWPE(uint64 instruction);
    std::string LWM(uint64 instruction);
    std::string SWM(uint64 instruction);
    std::string UALWM(uint64 instruction);
    std::string UASWM(uint64 instruction);
    std::string LDM(uint64 instruction);
    std::string SDM(uint64 instruction);
    std::string UALDM(uint64 instruction);
    std::string UASDM(uint64 instruction);
    std::string MOVE_BALC(uint64 instruction);
    std::string BC_32_(uint64 instruction);
    std::string BALC_32_(uint64 instruction);
    std::string JALRC_32_(uint64 instruction);
    std::string JALRC_HB(uint64 instruction);
    std::string BRSC(uint64 instruction);
    std::string BALRSC(uint64 instruction);
    std::string BEQC_32_(uint64 instruction);
    std::string BC1EQZC(uint64 instruction);
    std::string BC1NEZC(uint64 instruction);
    std::string BC2EQZC(uint64 instruction);
    std::string BC2NEZC(uint64 instruction);
    std::string BPOSGE32C(uint64 instruction);
    std::string BGEC(uint64 instruction);
    std::string BGEUC(uint64 instruction);
    std::string BNEC_32_(uint64 instruction);
    std::string BLTC(uint64 instruction);
    std::string BLTUC(uint64 instruction);
    std::string BEQIC(uint64 instruction);
    std::string BBEQZC(uint64 instruction);
    std::string BGEIC(uint64 instruction);
    std::string BGEIUC(uint64 instruction);
    std::string BNEIC(uint64 instruction);
    std::string BBNEZC(uint64 instruction);
    std::string BLTIC(uint64 instruction);
    std::string BLTIUC(uint64 instruction);
    std::string SYSCALL_16_(uint64 instruction);
    std::string HYPCALL_16_(uint64 instruction);
    std::string BREAK_16_(uint64 instruction);
    std::string SDBBP_16_(uint64 instruction);
    std::string MOVE(uint64 instruction);
    std::string SLL_16_(uint64 instruction);
    std::string SRL_16_(uint64 instruction);
    std::string NOT_16_(uint64 instruction);
    std::string XOR_16_(uint64 instruction);
    std::string AND_16_(uint64 instruction);
    std::string OR_16_(uint64 instruction);
    std::string LWXS_16_(uint64 instruction);
    std::string ADDIU_R1_SP_(uint64 instruction);
    std::string ADDIU_R2_(uint64 instruction);
    std::string NOP_16_(uint64 instruction);
    std::string ADDIU_RS5_(uint64 instruction);
    std::string ADDU_16_(uint64 instruction);
    std::string SUBU_16_(uint64 instruction);
    std::string LI_16_(uint64 instruction);
    std::string ANDI_16_(uint64 instruction);
    std::string LW_16_(uint64 instruction);
    std::string LW_SP_(uint64 instruction);
    std::string LW_GP16_(uint64 instruction);
    std::string LW_4X4_(uint64 instruction);
    std::string SW_16_(uint64 instruction);
    std::string SW_SP_(uint64 instruction);
    std::string SW_GP16_(uint64 instruction);
    std::string SW_4X4_(uint64 instruction);
    std::string BC_16_(uint64 instruction);
    std::string BALC_16_(uint64 instruction);
    std::string BEQZC_16_(uint64 instruction);
    std::string BNEZC_16_(uint64 instruction);
    std::string JRC(uint64 instruction);
    std::string JALRC_16_(uint64 instruction);
    std::string BEQC_16_(uint64 instruction);
    std::string BNEC_16_(uint64 instruction);
    std::string SAVE_16_(uint64 instruction);
    std::string RESTORE_JRC_16_(uint64 instruction);
    std::string ADDU_4X4_(uint64 instruction);
    std::string MUL_4X4_(uint64 instruction);
    std::string LB_16_(uint64 instruction);
    std::string SB_16_(uint64 instruction);
    std::string LBU_16_(uint64 instruction);
    std::string LH_16_(uint64 instruction);
    std::string SH_16_(uint64 instruction);
    std::string LHU_16_(uint64 instruction);
    std::string MOVEP(uint64 instruction);
    std::string MOVEP_REV_(uint64 instruction);

    static Pool P_SYSCALL[2];
    static Pool P_RI[4];
    static Pool P_ADDIU[2];
    static Pool P_TRAP[2];
    static Pool P_CMOVE[2];
    static Pool P_D_MT_VPE[2];
    static Pool P_E_MT_VPE[2];
    static Pool _P_MT_VPE[2];
    static Pool P_MT_VPE[8];
    static Pool P_DVP[2];
    static Pool P_SLTU[2];
    static Pool _POOL32A0[128];
    static Pool ADDQ__S__PH[2];
    static Pool MUL__S__PH[2];
    static Pool ADDQH__R__PH[2];
    static Pool ADDQH__R__W[2];
    static Pool ADDU__S__QB[2];
    static Pool ADDU__S__PH[2];
    static Pool ADDUH__R__QB[2];
    static Pool SHRAV__R__PH[2];
    static Pool SHRAV__R__QB[2];
    static Pool SUBQ__S__PH[2];
    static Pool SUBQH__R__PH[2];
    static Pool SUBQH__R__W[2];
    static Pool SUBU__S__QB[2];
    static Pool SUBU__S__PH[2];
    static Pool SHRA__R__PH[2];
    static Pool SUBUH__R__QB[2];
    static Pool SHLLV__S__PH[2];
    static Pool SHLL__S__PH[4];
    static Pool PRECR_SRA__R__PH_W[2];
    static Pool _POOL32A5[128];
    static Pool PP_LSX[16];
    static Pool PP_LSXS[16];
    static Pool P_LSX[2];
    static Pool POOL32Axf_1_0[4];
    static Pool POOL32Axf_1_1[4];
    static Pool POOL32Axf_1_3[4];
    static Pool POOL32Axf_1_4[2];
    static Pool MAQ_S_A__W_PHR[2];
    static Pool MAQ_S_A__W_PHL[2];
    static Pool POOL32Axf_1_5[2];
    static Pool POOL32Axf_1_7[4];
    static Pool POOL32Axf_1[8];
    static Pool POOL32Axf_2_DSP__0_7[8];
    static Pool POOL32Axf_2_DSP__8_15[8];
    static Pool POOL32Axf_2_DSP__16_23[8];
    static Pool POOL32Axf_2_DSP__24_31[8];
    static Pool POOL32Axf_2[4];
    static Pool POOL32Axf_4[128];
    static Pool POOL32Axf_5_group0[32];
    static Pool POOL32Axf_5_group1[32];
    static Pool ERETx[2];
    static Pool POOL32Axf_5_group3[32];
    static Pool POOL32Axf_5[4];
    static Pool SHRA__R__QB[2];
    static Pool POOL32Axf_7[8];
    static Pool POOL32Axf[8];
    static Pool _POOL32A7[8];
    static Pool P32A[8];
    static Pool P_GP_D[2];
    static Pool P_GP_W[4];
    static Pool POOL48I[32];
    static Pool PP_SR[4];
    static Pool P_SR_F[8];
    static Pool P_SR[2];
    static Pool P_SLL[5];
    static Pool P_SHIFT[16];
    static Pool P_ROTX[4];
    static Pool P_INS[4];
    static Pool P_EXT[4];
    static Pool P_U12[16];
    static Pool RINT_fmt[2];
    static Pool ADD_fmt0[2];
    static Pool SELEQZ_fmt[2];
    static Pool CLASS_fmt[2];
    static Pool SUB_fmt0[2];
    static Pool SELNEZ_fmt[2];
    static Pool MUL_fmt0[2];
    static Pool SEL_fmt[2];
    static Pool DIV_fmt0[2];
    static Pool ADD_fmt1[2];
    static Pool SUB_fmt1[2];
    static Pool MUL_fmt1[2];
    static Pool MADDF_fmt[2];
    static Pool DIV_fmt1[2];
    static Pool MSUBF_fmt[2];
    static Pool POOL32F_0[64];
    static Pool MIN_fmt[2];
    static Pool MAX_fmt[2];
    static Pool MINA_fmt[2];
    static Pool MAXA_fmt[2];
    static Pool CVT_L_fmt[2];
    static Pool RSQRT_fmt[2];
    static Pool FLOOR_L_fmt[2];
    static Pool CVT_W_fmt[2];
    static Pool SQRT_fmt[2];
    static Pool FLOOR_W_fmt[2];
    static Pool RECIP_fmt[2];
    static Pool CEIL_L_fmt[2];
    static Pool CEIL_W_fmt[2];
    static Pool TRUNC_L_fmt[2];
    static Pool TRUNC_W_fmt[2];
    static Pool ROUND_L_fmt[2];
    static Pool ROUND_W_fmt[2];
    static Pool POOL32Fxf_0[64];
    static Pool MOV_fmt[4];
    static Pool ABS_fmt[4];
    static Pool NEG_fmt[4];
    static Pool CVT_D_fmt[4];
    static Pool CVT_S_fmt[4];
    static Pool POOL32Fxf_1[32];
    static Pool POOL32Fxf[4];
    static Pool POOL32F_3[8];
    static Pool CMP_condn_S[32];
    static Pool CMP_condn_D[32];
    static Pool POOL32F_5[8];
    static Pool POOL32F[8];
    static Pool POOL32S_0[64];
    static Pool POOL32Sxf_4[128];
    static Pool POOL32Sxf[8];
    static Pool POOL32S_4[8];
    static Pool POOL32S[8];
    static Pool P_LUI[2];
    static Pool P_GP_LH[2];
    static Pool P_GP_SH[2];
    static Pool P_GP_CP1[4];
    static Pool P_GP_M64[4];
    static Pool P_GP_BH[8];
    static Pool P_LS_U12[16];
    static Pool P_PREF_S9_[2];
    static Pool P_LS_S0[16];
    static Pool ASET_ACLR[2];
    static Pool P_LL[4];
    static Pool P_SC[4];
    static Pool P_LLD[8];
    static Pool P_SCD[8];
    static Pool P_LS_S1[16];
    static Pool P_PREFE[2];
    static Pool P_LLE[4];
    static Pool P_SCE[4];
    static Pool P_LS_E0[16];
    static Pool P_LS_WM[2];
    static Pool P_LS_UAWM[2];
    static Pool P_LS_DM[2];
    static Pool P_LS_UADM[2];
    static Pool P_LS_S9[8];
    static Pool P_BAL[2];
    static Pool P_BALRSC[2];
    static Pool P_J[16];
    static Pool P_BR3A[32];
    static Pool P_BR1[4];
    static Pool P_BR2[4];
    static Pool P_BRI[8];
    static Pool P32[32];
    static Pool P16_SYSCALL[2];
    static Pool P16_RI[4];
    static Pool P16_MV[2];
    static Pool P16_SHIFT[2];
    static Pool POOL16C_00[4];
    static Pool POOL16C_0[2];
    static Pool P16C[2];
    static Pool P16_A1[2];
    static Pool P_ADDIU_RS5_[2];
    static Pool P16_A2[2];
    static Pool P16_ADDU[2];
    static Pool P16_JRC[2];
    static Pool P16_BR1[2];
    static Pool P16_BR[2];
    static Pool P16_SR[2];
    static Pool P16_4X4[4];
    static Pool P16_LB[4];
    static Pool P16_LH[4];
    static Pool P16[32];
    static Pool MAJOR[2];


};

#endif
