#!/bin/bash

cd /afs/cern.ch/work/s/syu/13tev/debug/CMSSW_7_4_7/src

export SCRAM_ARCH=slc6_amd64_gcc491; eval `scramv1 runtime -sh`
cd -

root -q -b xAna_recJetEff.C++\(\"/data7/syu/13TeV_signalMC/ZprimeToZhToZlephbb\",0,true\)
root -q -b xAna_recJetEff.C++\(\"/data7/syu/13TeV_signalMC/ZprimeToZhToZlephbb\",0,false\)

root -q -b call_plotAllEff.C\(\"jeteff_myDefinition_ZprimeToZhToZlephbb.root\",0\)

root -q -b call_plotAllEff.C\(\"jeteff_JETMET_ZprimeToZhToZlephbb.root\",0\)

mv jeteff_myDefinition_ZprimeToZhToZlephbb.root jeteff_old/.
mv jeteff_JETMET_ZprimeToZhToZlephbb.root jeteff_old/.

root -q -b xAna_recJetEff.C++\(\"/data7/syu/jet_CMSSW747/ZprimeToZhToZlephbb_25ns\",1,true\)
root -q -b xAna_recJetEff.C++\(\"/data7/syu/jet_CMSSW747/ZprimeToZhToZlephbb_25ns\",1,false\)

root -q -b call_plotAllEff.C\(\"jeteff_myDefinition_ZprimeToZhToZlephbb_25ns.root\",1\)
root -q -b call_plotAllEff.C\(\"jeteff_JETMET_ZprimeToZhToZlephbb_25ns.root\",1\)

mv jeteff_myDefinition_ZprimeToZhToZlephbb_25ns.root jeteff_mass/.
mv jeteff_JETMET_ZprimeToZhToZlephbb_25ns.root jeteff_mass/.

for file in $(ls /data7/syu/jet_CMSSW747/ZprimeToZhToZlephbb_25ns/ZprimeToZhToZlephbb*)
do
    root -q -b xAna_recJetEff.C++\(\"$file\",1,true\)
    root -q -b xAna_recJetEff.C++\(\"$file\",1,false\)
    test=${file##*ZprimeToZhToZlephbb_25ns/};
    test3=${test%%-madgraph.root}
    dir=jeteff_mass/$test3
    root -q -b call2_plotAllEff.C\(\"jeteff_myDefinition_${test}\",\"$dir\",1\)
    root -q -b call2_plotAllEff.C\(\"jeteff_JETMET_${test}\",\"$dir\",1\)
    mv jeteff_myDefinition_${test} $dir/.
    mv jeteff_JETMET_${test} $dir/.
done


root -q -b xAna_recJetEff.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/MonoHSignalTreesMerged_20150919\",2,true\)
root -q -b xAna_recJetEff.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/MonoHSignalTreesMerged_20150919\",2,false\)

root -q -b call2_plotAllEff.C\(\"jeteff_myDefinition_MonoHSignalTreesMerged_20150919.root\",\"jeteff_monoHiggs\"\)
root -q -b call2_plotAllEff.C\(\"jeteff_JETMET_MonoHSignalTreesMerged_20150919.root\",\"jeteff_monoHiggs\"\)

mv jeteff_myDefinition_MonoHSignalTreesMerged_20150919.root jeteff_monoHiggs/.
mv jeteff_JETMET_MonoHSignalTreesMerged_20150919.root jeteff_monoHiggs/.

for file in $(ls /data7/khurana/NCUGlobalTuples/SPRING15/MonoHSignalTreesMerged_20150919/Merged_crab_MonoHToBBarMZp-*)
do
    root -q -b xAna_recJetEff.C++\(\"$file\",2,true\)
    root -q -b xAna_recJetEff.C++\(\"$file\",2,false\)
    test=${file##*MonoHSignalTreesMerged_20150919/};
    test2=${file##*MonoHToBBar}
    test3=${test2%%TreeMaker_0919.root}
    dir=jeteff_monoHiggs/$test3
    root -q -b call2_plotAllEff.C\(\"jeteff_myDefinition_${test}\",\"$dir\"\)
    root -q -b call2_plotAllEff.C\(\"jeteff_JETMET_${test}\",\"$dir\"\)
    mv jeteff_myDefinition_${test} $dir/.
    mv jeteff_JETMET_${test} $dir/.
done

