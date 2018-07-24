#!/bin/bash

cd /afs/cern.ch/work/s/syu/CMSSW_7_1_30/src
export SCRAM_ARCH=slc6_amd64_gcc481; eval `scramv1 runtime -sh`

cd $1
tarball=$2
dir=job_${tarball%%_slc6_*}
echo $dir
WORKDIR=$1/job_$2
echo $WORKDIR
mkdir -p $WORKDIR
cd $WORKDIR
tar xvf $1/$tarball
./runcmsgrid.sh 5000 $RANDOM 1
