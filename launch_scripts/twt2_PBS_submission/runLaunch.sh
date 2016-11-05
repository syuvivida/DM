#!/bin/bash
#PBS -N TESTJOB
#PBS -l nodes=1
#PBS -o OUT.log
#PBS -e ERR.log
#PBS -q long

echo $PRODHOME
echo $CARDNAME
cd $PRODHOME
source /cvmfs/cms.cern.ch/cmsset_default.sh
source gridpack_generation.sh interactive

cd MG5_aMC_v2_3_3

./bin/mg5_aMC $CARDNAME
