#!/bin/bash

cd /afs/cern.ch/work/s/syu/13tev/debug/CMSSW_7_4_7/src

export SCRAM_ARCH=slc6_amd64_gcc491; eval `scramv1 runtime -sh`
cd -

root -q -b xAna_recJetEff.C++\(\"/data7/syu/13TeV_signalMC/ZprimeToZhToZlephbb\",true\)
root -q -b xAna_recJetEff.C++\(\"/data7/syu/13TeV_signalMC/ZprimeToZhToZlephbb\",false\)

root -q -b call_plotAllEff.C\(\"jeteff_myDefinition_ZprimeToZhToZlephbb.root\"\)

root -q -b call_plotAllEff.C\(\"jeteff_JETMET_ZprimeToZhToZlephbb.root\"\)

mv jeteff_myDefinition_ZprimeToZhToZlephbb.root jeteff/.
mv jeteff_JETMET_ZprimeToZhToZlephbb.root jeteff/.