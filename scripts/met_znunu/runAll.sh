#!/bin/bash

cd /afs/cern.ch/work/s/syu/13tev/debug/CMSSW_7_4_7/src

export SCRAM_ARCH=slc6_amd64_gcc491; eval `scramv1 runtime -sh`
cd -
## run on all samples

#data
root -q -b xAna_metzmm.C++\(\"/data7/khurana/NCUGlobalTuples/Run2015B/SingleMuon/crab_SingleMuon_Run2015B-PromptReco-v1_0825/150825_144712/0000/\"\)

#MC
root -q -b xAna_metznn.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/ZJetsNuNu/ZJetsToNuNu_HT-100To200_13TeV-madgraph/crab_ZJetsToNuNu_HT-100To200_13TeV-madgraph_0803/150817_090421/0000/\"\)
root -q -b xAna_metznn.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/ZJetsNuNu/ZJetsToNuNu_HT-200To400_13TeV-madgraph/crab_ZJetsToNuNu_HT-200To400_13TeV-madgraph_0803/150817_090503/0000/\"\) 
root -q -b xAna_metznn.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/ZJetsNuNu/ZJetsToNuNu_HT-400To600_13TeV-madgraph/crab_ZJetsToNuNu_HT-400To600_13TeV-madgraph_0803/150817_090547/0000/\"\) 
root -q -b xAna_metznn.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/ZJetsNuNu/ZJetsToNuNu_HT-600ToInf_13TeV-madgraph/crab_ZJetsToNuNu_HT-600ToInf_13TeV-madgraph_0803/150817_090630/0000/\"\)


root -q -b xAna_metzmm.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/DYJetsHTBins25nsSamples/DYJetsToLL_M-50_HT-100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_DYJetsToLL_M-50_HT-100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_0803/150812_162742/0000\"\)
root -q -b xAna_metzmm.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/DYJetsHTBins25nsSamples/DYJetsToLL_M-50_HT-200to400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_DYJetsToLL_M-50_HT-200to400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_0803/150812_162821/0000\"\)
root -q -b xAna_metzmm.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/DYJetsHTBins25nsSamples/DYJetsToLL_M-50_HT-400to600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_DYJetsToLL_M-50_HT-400to600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_0803/150812_162858/0000\"\)
root -q -b xAna_metzmm.C++\(\"/data7/khurana/NCUGlobalTuples/SPRING15/DYJetsHTBins25nsSamples/DYJetsToLL_M-50_HT-600toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/crab_DYJetsToLL_M-50_HT-600toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_0803/150812_162937/0000\"\)

root -q -b scaleHistos.C++\(\"inputznn.txt\",\"gencut100recocut150_met_znn.root\"\)
root -q -b scaleHistos.C++\(\"inputzmm.txt\",\"gencut100recocut150_met_zmm.root\"\)

 
 
 

root -q -b produceCorr.C++\(\"gencut100recocut150_met_znn.root\",\"gencut100recocut150_met_zmm.root\",\"corr_Znn.root\"\)

root -q -b final.C++\(\"gencut100recocut150_met_znn.root\",\"gencut100recocut150_met_zmm.root\",\"corr_Znn.root\",\"histo_singlemuon.root\",\"final_Znn.root\"\)