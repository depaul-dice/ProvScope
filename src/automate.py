import os

executable = "./provScope"

uniqs = [("c", "uniqc.ftr"), ("d", "uniqd.ftr"), ("cd", "uniq_cd.ftr"), ("f2", "uniq_f2.ftr"), ("i", "uniq_i.ftr"), ("normal", "uniq_normal.ftr"), ("s3", "uniq_s3.ftr"), ("u", "uniq_u.ftr"), ("w3", "uniq_w3.ftr"), ("z", "uniq_z.ftr"), ("cdu", "uniq_cdu.ftr"), ("cu", "uniq_cu.ftr"), ("out", "uniq_out.ftr"), ("cout", "uniq_cout.ftr"), ("dout", "uniq_dout.ftr"), ("uout", "uniq_uout.ftr"), ("D", "uniq_D.ftr"), ("Dpre", "uniq_Dpre.ftr"), ("Dsep", "uniq_Dsep.ftr"), ("w5", "uniq_w5.ftr")]

cats = [("uniq1", "cat_uniq1.ftr"), ("uniq1n", "cat_uniq1n.ftr"), ("uniq1uniq2", "cat_uniq1uniq2.ftr"), ("uniq1uniq2n", "cat_uniq1uniq2n.ftr"), ("s", "cat_s.ftr"), ("gtgt", "cat_gtgt.ftr"), ("gt", "cat_gt.ftr"), ("eT", "cat_eT.ftr"), ("e", "cat_e.ftr"), ("b", "cat_b.ftr"), ("scat1uniq2", "cat_scat1uniq2.ftr"), ("uniq1uniq2gt", "cat_uniq1uniq2gt.ftr"), ("uniq1uniq2gtgt", "cat_uniq1uniq2gtgt.ftr"), ("eTgt", "cat_eTgt.ftr"), ("eTgtgt", "cat_eTgtgt.ftr"), ("A", "cat_A.ftr"), ("sgt", "cat_sgt.ftr"), ("sgtgt", "cat_sgtgt.ftr")]  

rms = [("normal", "rm.ftr"), ("2", "rm2.ftr"), ("f", "rm_f.ftr"), ("in", "rm_in.ftr"), ("iy", "rm_iy.ftr"), ("r", "rm_r.ftr"), ("rv", "rm_rv.ftr"), ("v", "rm_v.ftr"), ("vin", "rm_vin.ftr"), ("viy", "rm_viy.ftr"), ("rin", "rm_rin.ftr"), ("riy", "rm_riy.ftr"), ("rf", "rm_rf.ftr"), ("rI", "rm_rI.ftr"), ("r1fs", "rm_r1fs.ftr"), ("rviy", "rm_rviy.ftr"), ("rvin", "rm_rvin.ftr"), ("rvIy", "rm_rvIy.ftr"), ("rvIn", "rm_rvIn.ftr"), ("rvf", "rm_rvf.ftr")]

dates = [("normal", "date.ftr"), ("d", "date_d.ftr"), ("dnt", "date_dnt.ftr"), ("d2ya", "date_d2ya.ftr"), ("d5sa", "date_d5sa.ftr"), ("fD", "date_fD.ftr"), ("fDT", "date_fDT.ftr"), ("f", "date_f.ftr"), ("r", "date_r.ftr"), ("u", "date_u.ftr"), ("fymd", "date_fymd.ftr"), ("R", "date_R.ftr"), ("rfc-3339date", "date_rfc-3339date.ftr"), ("rfc-3339seconds", "date_rfc-3339seconds.ftr"), ("rfc-3339ns", "date_rfc-3339ns.ftr"), ("s", "date_s.ftr")]

sorts = [("normal", "sort.ftr"), ("c", "sort_c.ftr"), ("k2", "sort_k2.ftr"), ("k2r", "sort_k2r.ftr"), ("M", "sort_M.ftr"), ("n", "sort_n.ftr"), ("nr", "sort_nr.ftr"), ("o", "sort_o.ftr"), ("r", "sort_r.ftr"), ("u", "sort_u.ftr"), ("sort1sort3", "sort_sort1sort3.ftr"), ("rsort1sort3", "sort_rsort1sort3.ftr"), ("osort1sort3", "sort_osort1sort3.ftr"), ("nsort2sort6", "sort_nsort2sort6.ftr"), ("nrsort2sort6", "sort_nrsort2sort6.ftr"), ("nosort2sort6", "sort_nosort2sort6.ftr"), ("ysort1sort3", "sort_usort1sort3.ftr"), ("Msort5sort7", "sort_Msort5sort7.ftr"), ("Musort5sort7", "sort_Musort5sort7.ftr"), ("k2n", "sort_k2n.ftr")]

chowns = [("normal", "chown.ftr"), ("c", "chown_c.ftr"), ("multc", "chown_multc.ftr"), ("mult", "chown_mult.ftr"), ("vf", "chown_vf.ftr"), ("vfg", "chown_vfg.ftr"), ("v", "chown_v.ftr"), ("vg", "chown_vg.ftr"), ("vgo", "chown_vgo.ftr"), ("vr", "chown_vr.ftr"), ("vsort1sort2", "chown_vsort1sort2.ftr"), ("vgsort1sort2", "chown_vgsort1sort2.ftr"), ("vgosort1sort2", "chown_vgosort1sort2.ftr"), ("vfsort1sort2", "chown_vfsort1sort2.ftr"), ("vfgsort1sort2", "chown_vfgsort1sort2.ftr"), ("vrsort1sort2", "chown_vfsort1sort2.ftr"), ("g", "chown_g.ftr"), ("go", "chown_go.ftr"), ("f", "chown_f.ftr"), ("fg", "chown_fg.ftr")]

def main():
    for i in range(len(uniqs)):
        for j in range(i + 1, len(uniqs)):
            name1, ftr1 = uniqs[i]
            name2, ftr2 = uniqs[j]
            output = "uniq_outs/uniq_" + name1 + "_vs_" + name2 + ".out"
            execution = executable + " -c ../funcList.txt ../noRetFuncs/uniq_all.nr ../coreutil_parsed/uniq_parsed ../coreFuncTraceInput/uniq/" + ftr1 + " ../coreFuncTraceInput/uniq/" + ftr2 + " > " + output 
            # os.system(execution)
            # print(execution)

    for i in range(len(cats)):
        for j in range(i + 1, len(cats)):
            name1, ftr1 = cats[i]
            name2, ftr2 = cats[j]
            output = "cat_outs/cat_" + name1 + "_vs_" + name2 + ".out"
            execution = executable + " -c ../funcList.txt ../noRetFuncs/cat_all.nr ../coreutil_parsed/cat_parsed ../coreFuncTraceInput/cat/" + ftr1 + " ../coreFuncTraceInput/cat/" + ftr2 + " > " + output 
            # print(execution)
            # os.system(execution)

    for i in range(len(rms)):
        for j in range(i + 1, len(rms)):
            name1, ftr1 = rms[i]
            name2, ftr2 = rms[j]
            output = "rm_outs/rm_" + name1 + "_vs_" + name2 + ".out"
            execution = executable + " -c ../funcList.txt ../noRetFuncs/rm_all.nr ../coreutil_parsed/rm_parsed ../coreFuncTraceInput/rm/" + ftr1 + " ../coreFuncTraceInput/rm/" + ftr2 + " > " + output 
            # print(execution)
            # os.system(execution)

    for i in range(len(dates)):
        for j in range(i + 1, len(dates)):
            name1, ftr1 = dates[i]
            name2, ftr2 = dates[j]
            output = "date_outs/date_" + name1 + "_vs_" + name2 + ".out"
            execution = executable + " -c ../funcList.txt ../noRetFuncs/date_all.nr ../coreutil_parsed/date_parsed ../coreFuncTraceInput/date/" + ftr1 + " ../coreFuncTraceInput/date/" + ftr2 + " > " + output 
            # print(execution)
            # os.system(execution)

    for i in range(len(sorts)):
        for j in range(i + 1, len(sorts)):
            name1, ftr1 = sorts[i]
            name2, ftr2 = sorts[j]
            output = "sort_outs/sort_" + name1 + "_vs_" + name2 + ".out"
            execution = executable + " -c ../funcList.txt ../noRetFuncs/sort_all.nr ../coreutil_parsed/sort_parsed ../coreFuncTraceInput/sort/" + ftr1 + " ../coreFuncTraceInput/sort/" + ftr2 + " > " + output 
            # print(execution)
            os.system(execution)

    for i in range(len(chowns)):
        for j in range(i + 1, len(chowns)):
            name1, ftr1 = chowns[i]
            name2, ftr2 = chowns[j]
            output = "chown_outs/chown_" + name1 + "_vs_" + name2 + ".out"
            execution = executable + " -c ../funcList.txt ../noRetFuncs/chown_all.nr ../coreutil_parsed/chown_parsed ../coreFuncTraceInput/chown/" + ftr1 + " ../coreFuncTraceInput/chown/" + ftr2 + " > " + output 
            # print(execution)
            os.system(execution)



if __name__ == "__main__":
    main()
