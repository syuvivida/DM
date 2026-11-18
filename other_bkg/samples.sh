# Sample table, sourced by make_bkg_gridpacks.sh and submit_bkg.sh.
#   PROC       process card            DIR   MadGraph process folder (also the folder name in the output tree)
#   MLM        1: + 0,1 jet with MLM matching (xqcut 15 GeV, qCut 30 GeV); 0: no matching
#   HBB        1: Higgs left undecayed, Pythia8 forces H -> b b~, cross section x 0.5824 in the job
#   DEFSLICES  default parton-level MET slices "lo:hi" in GeV (hi = -1: no upper limit; "0:-1" = inclusive).
#              The MET cut acts on the neutrinos that are in the hard process:
#                st_tw, st_tch  W -> l nu neutrino(s): sliced like tt~
#                zh, zz         Z -> nu nu, MET = pT(Z): one slice above 100 GeV, as the lowest Z + jets edge
#                wh, wz         W -> l nu: one slice above 50 GeV, as the lowest W + jets edge
#                ttz, tth       inclusive: the tops are decayed by Pythia8, so their neutrinos are not in the cut
ALL_SAMPLES="st_tw st_tch ttz tth zh wh zz wz"
sample_config() {
  case $1 in
    st_tw)  PROC=proc_st_tw.dat;     DIR=ST_tW_MET;     MLM=0; HBB=0; DEFSLICES="100:200 200:300 300:-1" ;;
    st_tch) PROC=proc_st_tch.dat;    DIR=ST_tch_MET;    MLM=0; HBB=0; DEFSLICES="100:200 200:300 300:-1" ;;
    ttz)    PROC=proc_ttz_nunu.dat;  DIR=TTZ_nunu_MET;  MLM=0; HBB=0; DEFSLICES="0:-1" ;;
    tth)    PROC=proc_tth.dat;       DIR=TTH_bb_MET;    MLM=0; HBB=1; DEFSLICES="0:-1" ;;
    zh)     PROC=proc_zh_nunu.dat;   DIR=ZH_nunubb_MET; MLM=1; HBB=1; DEFSLICES="100:-1" ;;
    wh)     PROC=proc_wh_lnu.dat;    DIR=WH_lnubb_MET;  MLM=1; HBB=1; DEFSLICES="50:-1" ;;
    zz)     PROC=proc_zz_nunubb.dat; DIR=ZZ_nunubb_MET; MLM=1; HBB=0; DEFSLICES="100:-1" ;;
    wz)     PROC=proc_wz_lnubb.dat;  DIR=WZ_lnubb_MET;  MLM=1; HBB=0; DEFSLICES="50:-1" ;;
    *) echo "unknown sample '$1' (use one of: $ALL_SAMPLES)"; return 1 ;;
  esac
}
