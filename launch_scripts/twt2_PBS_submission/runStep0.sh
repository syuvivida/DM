#!/bin/sh
#PBS -N TESTJOB
#PBS -l nodes=1
#PBS -o OUT.log
#PBS -e ERR.log
#PBS -q long


source /cvmfs/cms.cern.ch/cmsset_default.sh
echo $PRODHOME
echo $lhedir
echo $OUTPUTFILE
dir=/u1/syu/genproductions/bin/MadGraph5_aMCatNLO/interactive/interactive_gridpack/work/MG5_aMC_v2_3_3/${lhedir}/Events/run_01/
gunzip $dir/unweighted_events.lhe.gz 
lhefile=$dir/unweighted_events.lhe
echo $PRODHOME
cd $PRODHOME
export SCRAM_ARCH=slc6_amd64_gcc472; eval `scramv1 runtime -sh`
cmsRun step0.py inputFiles=file:$lhefile outputFile=$OUTPUTFILE
