DEF_HELPER_2(exception, noreturn, env, i32)
DEF_HELPER_2(data_exception, noreturn, env, i32)
DEF_HELPER_FLAGS_4(nc, TCG_CALL_NO_WG, i32, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(oc, TCG_CALL_NO_WG, i32, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(xc, TCG_CALL_NO_WG, i32, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(mvc, TCG_CALL_NO_WG, void, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(mvcin, TCG_CALL_NO_WG, void, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(clc, TCG_CALL_NO_WG, i32, env, i32, i64, i64)
DEF_HELPER_3(mvcl, i32, env, i32, i32)
DEF_HELPER_3(clcl, i32, env, i32, i32)
DEF_HELPER_FLAGS_4(clm, TCG_CALL_NO_WG, i32, env, i32, i32, i64)
DEF_HELPER_FLAGS_3(divs32, TCG_CALL_NO_WG, s64, env, s64, s64)
DEF_HELPER_FLAGS_3(divu32, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_3(divs64, TCG_CALL_NO_WG, s64, env, s64, s64)
DEF_HELPER_FLAGS_4(divu64, TCG_CALL_NO_WG, i64, env, i64, i64, i64)
DEF_HELPER_3(srst, void, env, i32, i32)
DEF_HELPER_3(srstu, void, env, i32, i32)
DEF_HELPER_4(clst, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(mvn, TCG_CALL_NO_WG, void, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(mvo, TCG_CALL_NO_WG, void, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(mvpg, TCG_CALL_NO_WG, i32, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(mvz, TCG_CALL_NO_WG, void, env, i32, i64, i64)
DEF_HELPER_4(mvst, i64, env, i64, i64, i64)
DEF_HELPER_4(ex, void, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(stam, TCG_CALL_NO_WG, void, env, i32, i64, i32)
DEF_HELPER_FLAGS_4(lam, TCG_CALL_NO_WG, void, env, i32, i64, i32)
DEF_HELPER_4(mvcle, i32, env, i32, i64, i32)
DEF_HELPER_4(mvclu, i32, env, i32, i64, i32)
DEF_HELPER_4(clcle, i32, env, i32, i64, i32)
DEF_HELPER_4(clclu, i32, env, i32, i64, i32)
DEF_HELPER_3(cegb, i64, env, s64, i32)
DEF_HELPER_3(cdgb, i64, env, s64, i32)
DEF_HELPER_3(cxgb, i64, env, s64, i32)
DEF_HELPER_3(celgb, i64, env, i64, i32)
DEF_HELPER_3(cdlgb, i64, env, i64, i32)
DEF_HELPER_3(cxlgb, i64, env, i64, i32)
DEF_HELPER_4(cdsg, void, env, i64, i32, i32)
DEF_HELPER_4(cdsg_parallel, void, env, i64, i32, i32)
DEF_HELPER_4(csst, i32, env, i32, i64, i64)
DEF_HELPER_4(csst_parallel, i32, env, i32, i64, i64)
DEF_HELPER_FLAGS_3(aeb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_3(adb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_5(axb, TCG_CALL_NO_WG, i64, env, i64, i64, i64, i64)
DEF_HELPER_FLAGS_3(seb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_3(sdb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_5(sxb, TCG_CALL_NO_WG, i64, env, i64, i64, i64, i64)
DEF_HELPER_FLAGS_3(deb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_3(ddb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_5(dxb, TCG_CALL_NO_WG, i64, env, i64, i64, i64, i64)
DEF_HELPER_FLAGS_3(meeb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_3(mdeb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_3(mdb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_5(mxb, TCG_CALL_NO_WG, i64, env, i64, i64, i64, i64)
DEF_HELPER_FLAGS_4(mxdb, TCG_CALL_NO_WG, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_2(ldeb, TCG_CALL_NO_WG, i64, env, i64)
DEF_HELPER_FLAGS_4(ldxb, TCG_CALL_NO_WG, i64, env, i64, i64, i32)
DEF_HELPER_FLAGS_2(lxdb, TCG_CALL_NO_WG, i64, env, i64)
DEF_HELPER_FLAGS_2(lxeb, TCG_CALL_NO_WG, i64, env, i64)
DEF_HELPER_FLAGS_3(ledb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_4(lexb, TCG_CALL_NO_WG, i64, env, i64, i64, i32)
DEF_HELPER_FLAGS_3(ceb, TCG_CALL_NO_WG_SE, i32, env, i64, i64)
DEF_HELPER_FLAGS_3(cdb, TCG_CALL_NO_WG_SE, i32, env, i64, i64)
DEF_HELPER_FLAGS_5(cxb, TCG_CALL_NO_WG_SE, i32, env, i64, i64, i64, i64)
DEF_HELPER_FLAGS_3(keb, TCG_CALL_NO_WG, i32, env, i64, i64)
DEF_HELPER_FLAGS_3(kdb, TCG_CALL_NO_WG, i32, env, i64, i64)
DEF_HELPER_FLAGS_5(kxb, TCG_CALL_NO_WG, i32, env, i64, i64, i64, i64)
DEF_HELPER_FLAGS_3(cgeb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_3(cgdb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_4(cgxb, TCG_CALL_NO_WG, i64, env, i64, i64, i32)
DEF_HELPER_FLAGS_3(cfeb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_3(cfdb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_4(cfxb, TCG_CALL_NO_WG, i64, env, i64, i64, i32)
DEF_HELPER_FLAGS_3(clgeb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_3(clgdb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_4(clgxb, TCG_CALL_NO_WG, i64, env, i64, i64, i32)
DEF_HELPER_FLAGS_3(clfeb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_3(clfdb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_4(clfxb, TCG_CALL_NO_WG, i64, env, i64, i64, i32)
DEF_HELPER_FLAGS_3(fieb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_3(fidb, TCG_CALL_NO_WG, i64, env, i64, i32)
DEF_HELPER_FLAGS_4(fixb, TCG_CALL_NO_WG, i64, env, i64, i64, i32)
DEF_HELPER_FLAGS_4(maeb, TCG_CALL_NO_WG, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(madb, TCG_CALL_NO_WG, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(mseb, TCG_CALL_NO_WG, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(msdb, TCG_CALL_NO_WG, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_3(tceb, TCG_CALL_NO_RWG_SE, i32, env, i64, i64)
DEF_HELPER_FLAGS_3(tcdb, TCG_CALL_NO_RWG_SE, i32, env, i64, i64)
DEF_HELPER_FLAGS_4(tcxb, TCG_CALL_NO_RWG_SE, i32, env, i64, i64, i64)
DEF_HELPER_FLAGS_2(sqeb, TCG_CALL_NO_WG, i64, env, i64)
DEF_HELPER_FLAGS_2(sqdb, TCG_CALL_NO_WG, i64, env, i64)
DEF_HELPER_FLAGS_3(sqxb, TCG_CALL_NO_WG, i64, env, i64, i64)
DEF_HELPER_FLAGS_1(cvd, TCG_CALL_NO_RWG_SE, i64, s32)
DEF_HELPER_FLAGS_4(pack, TCG_CALL_NO_WG, void, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(pka, TCG_CALL_NO_WG, void, env, i64, i64, i32)
DEF_HELPER_FLAGS_4(pku, TCG_CALL_NO_WG, void, env, i64, i64, i32)
DEF_HELPER_FLAGS_4(unpk, TCG_CALL_NO_WG, void, env, i32, i64, i64)
DEF_HELPER_FLAGS_4(unpka, TCG_CALL_NO_WG, i32, env, i64, i32, i64)
DEF_HELPER_FLAGS_4(unpku, TCG_CALL_NO_WG, i32, env, i64, i32, i64)
DEF_HELPER_FLAGS_3(tp, TCG_CALL_NO_WG, i32, env, i64, i32)
DEF_HELPER_FLAGS_4(tr, TCG_CALL_NO_WG, void, env, i32, i64, i64)
DEF_HELPER_4(tre, i64, env, i64, i64, i64)
DEF_HELPER_4(trt, i32, env, i32, i64, i64)
DEF_HELPER_4(trtr, i32, env, i32, i64, i64)
DEF_HELPER_5(trXX, i32, env, i32, i32, i32, i32)
DEF_HELPER_4(cksm, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_5(calc_cc, TCG_CALL_NO_RWG_SE, i32, env, i32, i64, i64, i64)
DEF_HELPER_FLAGS_2(sfpc, TCG_CALL_NO_WG, void, env, i64)
DEF_HELPER_FLAGS_2(sfas, TCG_CALL_NO_WG, void, env, i64)
DEF_HELPER_FLAGS_2(srnm, TCG_CALL_NO_WG, void, env, i64)
DEF_HELPER_FLAGS_1(popcnt, TCG_CALL_NO_RWG_SE, i64, i64)
DEF_HELPER_2(stfle, i32, env, i64)
DEF_HELPER_FLAGS_2(lpq, TCG_CALL_NO_WG, i64, env, i64)
DEF_HELPER_FLAGS_2(lpq_parallel, TCG_CALL_NO_WG, i64, env, i64)
DEF_HELPER_FLAGS_4(stpq, TCG_CALL_NO_WG, void, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(stpq_parallel, TCG_CALL_NO_WG, void, env, i64, i64, i64)
DEF_HELPER_4(mvcos, i32, env, i64, i64, i64)
DEF_HELPER_4(cu12, i32, env, i32, i32, i32)
DEF_HELPER_4(cu14, i32, env, i32, i32, i32)
DEF_HELPER_4(cu21, i32, env, i32, i32, i32)
DEF_HELPER_4(cu24, i32, env, i32, i32, i32)
DEF_HELPER_4(cu41, i32, env, i32, i32, i32)
DEF_HELPER_4(cu42, i32, env, i32, i32, i32)
DEF_HELPER_5(msa, i32, env, i32, i32, i32, i32)
DEF_HELPER_FLAGS_1(stpt, TCG_CALL_NO_RWG, i64, env)
DEF_HELPER_FLAGS_1(stck, TCG_CALL_NO_RWG_SE, i64, env)
DEF_HELPER_FLAGS_3(probe_write_access, TCG_CALL_NO_WG, void, env, i64, i64)

/* === Vector Support Instructions === */
DEF_HELPER_FLAGS_4(vll, TCG_CALL_NO_WG, void, env, ptr, i64, i64)
DEF_HELPER_FLAGS_4(gvec_vpk16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vpk32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vpk64, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vpks16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vpks32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vpks64, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_5(gvec_vpks_cc16, void, ptr, cptr, cptr, env, i32)
DEF_HELPER_5(gvec_vpks_cc32, void, ptr, cptr, cptr, env, i32)
DEF_HELPER_5(gvec_vpks_cc64, void, ptr, cptr, cptr, env, i32)
DEF_HELPER_FLAGS_4(gvec_vpkls16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vpkls32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vpkls64, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_5(gvec_vpkls_cc16, void, ptr, cptr, cptr, env, i32)
DEF_HELPER_5(gvec_vpkls_cc32, void, ptr, cptr, cptr, env, i32)
DEF_HELPER_5(gvec_vpkls_cc64, void, ptr, cptr, cptr, env, i32)
DEF_HELPER_FLAGS_5(gvec_vperm, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(vstl, TCG_CALL_NO_WG, void, env, cptr, i64, i64)

/* === Vector Integer Instructions === */
DEF_HELPER_FLAGS_4(gvec_vacc128, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vaccc128, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vavg8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vavg16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vavgl8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vavgl16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_3(gvec_vclz8, TCG_CALL_NO_RWG, void, ptr, cptr, i32)
DEF_HELPER_FLAGS_3(gvec_vclz16, TCG_CALL_NO_RWG, void, ptr, cptr, i32)
DEF_HELPER_FLAGS_3(gvec_vctz8, TCG_CALL_NO_RWG, void, ptr, cptr, i32)
DEF_HELPER_FLAGS_3(gvec_vctz16, TCG_CALL_NO_RWG, void, ptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vgfm8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vgfm16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vgfm32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vgfm64, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vgfma8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vgfma16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vgfma32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vgfma64, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_3(gvec_vlp8, TCG_CALL_NO_RWG, void, ptr, cptr, i32)
DEF_HELPER_FLAGS_3(gvec_vlp16, TCG_CALL_NO_RWG, void, ptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmal8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmal16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmah8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmah16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmalh8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmalh16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmae8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmae16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmae32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmale8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmale16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmale32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmao8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmao16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmao32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmalo8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmalo16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_5(gvec_vmalo32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmh8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmh16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmlh8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmlh16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vme8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vme16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vme32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmle8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmle16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmle32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmo8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmo16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmo32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmlo8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmlo16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vmlo32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_3(gvec_vpopct8, TCG_CALL_NO_RWG, void, ptr, cptr, i32)
DEF_HELPER_FLAGS_3(gvec_vpopct16, TCG_CALL_NO_RWG, void, ptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_verllv8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_verllv16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_verll8, TCG_CALL_NO_RWG, void, ptr, cptr, i64, i32)
DEF_HELPER_FLAGS_4(gvec_verll16, TCG_CALL_NO_RWG, void, ptr, cptr, i64, i32)
DEF_HELPER_FLAGS_4(gvec_verim8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_verim16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_veslv8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_veslv16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_veslv32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_veslv64, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vesrav8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vesrav16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vesrav32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vesrav64, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vesrlv8, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vesrlv16, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vesrlv32, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vesrlv64, TCG_CALL_NO_RWG, void, ptr, cptr, cptr, i32)
DEF_HELPER_FLAGS_4(gvec_vesl8, TCG_CALL_NO_RWG, void, ptr, cptr, i64, i32)
DEF_HELPER_FLAGS_4(gvec_vesl16, TCG_CALL_NO_RWG, void, ptr, cptr, i64, i32)
DEF_HELPER_FLAGS_4(gvec_vesra8, TCG_CALL_NO_RWG, void, ptr, cptr, i64, i32)
DEF_HELPER_FLAGS_4(gvec_vesra16, TCG_CALL_NO_RWG, void, ptr, cptr, i64, i32)
DEF_HELPER_FLAGS_4(gvec_vesrl8, TCG_CALL_NO_RWG, void, ptr, cptr, i64, i32)
DEF_HELPER_FLAGS_4(gvec_vesrl16, TCG_CALL_NO_RWG, void, ptr, cptr, i64, i32)

#ifndef CONFIG_USER_ONLY
DEF_HELPER_3(servc, i32, env, i64, i64)
DEF_HELPER_4(diag, void, env, i32, i32, i32)
DEF_HELPER_3(load_psw, noreturn, env, i64, i64)
DEF_HELPER_FLAGS_2(spx, TCG_CALL_NO_RWG, void, env, i64)
DEF_HELPER_FLAGS_2(sck, TCG_CALL_NO_RWG, i32, env, i64)
DEF_HELPER_FLAGS_2(sckc, TCG_CALL_NO_RWG, void, env, i64)
DEF_HELPER_FLAGS_2(sckpf, TCG_CALL_NO_RWG, void, env, i64)
DEF_HELPER_FLAGS_1(stckc, TCG_CALL_NO_RWG, i64, env)
DEF_HELPER_FLAGS_2(spt, TCG_CALL_NO_RWG, void, env, i64)
DEF_HELPER_4(stsi, i32, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(lctl, TCG_CALL_NO_WG, void, env, i32, i64, i32)
DEF_HELPER_FLAGS_4(lctlg, TCG_CALL_NO_WG, void, env, i32, i64, i32)
DEF_HELPER_FLAGS_4(stctl, TCG_CALL_NO_WG, void, env, i32, i64, i32)
DEF_HELPER_FLAGS_4(stctg, TCG_CALL_NO_WG, void, env, i32, i64, i32)
DEF_HELPER_FLAGS_2(testblock, TCG_CALL_NO_WG, i32, env, i64)
DEF_HELPER_FLAGS_3(tprot, TCG_CALL_NO_WG, i32, env, i64, i64)
DEF_HELPER_FLAGS_2(iske, TCG_CALL_NO_RWG_SE, i64, env, i64)
DEF_HELPER_FLAGS_3(sske, TCG_CALL_NO_RWG, void, env, i64, i64)
DEF_HELPER_FLAGS_2(rrbe, TCG_CALL_NO_RWG, i32, env, i64)
DEF_HELPER_4(mvcs, i32, env, i64, i64, i64)
DEF_HELPER_4(mvcp, i32, env, i64, i64, i64)
DEF_HELPER_4(sigp, i32, env, i64, i32, i32)
DEF_HELPER_FLAGS_2(sacf, TCG_CALL_NO_WG, void, env, i64)
DEF_HELPER_FLAGS_4(idte, TCG_CALL_NO_RWG, void, env, i64, i64, i32)
DEF_HELPER_FLAGS_4(ipte, TCG_CALL_NO_RWG, void, env, i64, i64, i32)
DEF_HELPER_FLAGS_1(ptlb, TCG_CALL_NO_RWG, void, env)
DEF_HELPER_FLAGS_1(purge, TCG_CALL_NO_RWG, void, env)
DEF_HELPER_2(lra, i64, env, i64)
DEF_HELPER_FLAGS_2(lura, TCG_CALL_NO_WG, i64, env, i64)
DEF_HELPER_FLAGS_2(lurag, TCG_CALL_NO_WG, i64, env, i64)
DEF_HELPER_FLAGS_3(stura, TCG_CALL_NO_WG, void, env, i64, i64)
DEF_HELPER_FLAGS_3(sturg, TCG_CALL_NO_WG, void, env, i64, i64)
DEF_HELPER_1(per_check_exception, void, env)
DEF_HELPER_FLAGS_3(per_branch, TCG_CALL_NO_RWG, void, env, i64, i64)
DEF_HELPER_FLAGS_2(per_ifetch, TCG_CALL_NO_RWG, void, env, i64)
DEF_HELPER_FLAGS_1(stfl, TCG_CALL_NO_RWG, void, env)

DEF_HELPER_2(xsch, void, env, i64)
DEF_HELPER_2(csch, void, env, i64)
DEF_HELPER_2(hsch, void, env, i64)
DEF_HELPER_3(msch, void, env, i64, i64)
DEF_HELPER_2(rchp, void, env, i64)
DEF_HELPER_2(rsch, void, env, i64)
DEF_HELPER_2(sal, void, env, i64)
DEF_HELPER_4(schm, void, env, i64, i64, i64)
DEF_HELPER_3(ssch, void, env, i64, i64)
DEF_HELPER_2(stcrw, void, env, i64)
DEF_HELPER_3(stsch, void, env, i64, i64)
DEF_HELPER_2(tpi, i32, env, i64)
DEF_HELPER_3(tsch, void, env, i64, i64)
DEF_HELPER_2(chsc, void, env, i64)

DEF_HELPER_2(clp, void, env, i32)
DEF_HELPER_3(pcilg, void, env, i32, i32)
DEF_HELPER_3(pcistg, void, env, i32, i32)
DEF_HELPER_4(stpcifc, void, env, i32, i64, i32)
DEF_HELPER_3(sic, void, env, i64, i64)
DEF_HELPER_3(rpcit, void, env, i32, i32)
DEF_HELPER_5(pcistb, void, env, i32, i32, i64, i32)
DEF_HELPER_4(mpcifc, void, env, i32, i64, i32)
#endif
