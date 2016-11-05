#!/bin/sh
lhedir=$2
cp -p /afs/cern.ch/work/s/syu/genproductions/bin/MadGraph5_aMCatNLO/interactive/interactive_gridpack/work/MG5_aMC_v2_3_3/${lhedir}/Events/run_01/unweighted_events.lhe.gz /tmp/syu/${lhedir}.lhe.gz
gunzip /tmp/syu/${lhedir}.lhe.gz
lhefile=/tmp/syu/${lhedir}.lhe
echo $1
cd $1
export SCRAM_ARCH=slc6_amd64_gcc472; eval `scramv1 runtime -sh`
export X509_USER_PROXY=$HOME/private/grid.proxy
cmsRun step0.py inputFiles=file:$lhefile outputFile=/tmp/syu/$3
dirName=MonoH-$4
xrdcp /tmp/syu/$3 root://eoscms//eos/cms//store/user/syu/$dirName/$3
rm -rf /tmp/syu/$3