#!/bin/bash
scriptname=`basename $0`
EXPECTED_ARGS=3
if [ $# -ne $EXPECTED_ARGS ]
then
echo "Usage: $scriptname tarball numberofLHEfiles numberofeventsPerLHE"
echo "Example: ./$scriptname 200 500"
exit 1
fi

workarea=$PWD
foldername=$PWD/${1%%_tarball.tar.xz}
echo $foldername
mkdir $foldername
mkdir $foldername/temp
cd /afs/cern.ch/work/s/syu/Big_Generators/MyPowHeg/v20/slc6_DMGG/CMSSW_7_1_17/src/
export SCRAM_ARCH=slc6_amd64_gcc481; eval `scramv1 runtime -sh`
cd $foldername/temp
iteration=0
lastfile=$2
while [ $iteration -lt $lastfile ]; 
do
  iteration=$(( iteration + 1 ))
  echo $iteration
  tar xf $workarea/$1
  ./runcmsgrid.sh $3 $RANDOM 1
  mv cmsgrid_final.lhe $foldername/0jet_ZpA0h_ZpM600A0M300_$iteration.lhe
  rm -rf *
done

