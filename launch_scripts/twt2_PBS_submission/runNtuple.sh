#!/bin/sh
#PBS -N TESTJOB
#PBS -l nodes=1
#PBS -o OUT.log
#PBS -e ERR.log
#PBS -q long


source /cvmfs/cms.cern.ch/cmsset_default.sh
echo $PRODHOME
cd $PRODHOME

export SCRAM_ARCH=slc6_amd64_gcc530; eval `scramv1 runtime -sh`
cmsRun treeMaker_genOnly.py inputFiles=file:/u1/syu/CMSSW_7_1_20/src/$INPUTFILE outputFile=$OUTPUTFILE
