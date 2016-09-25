#!/bin/sh

echo $1
cd $1
export SCRAM_ARCH=slc6_amd64_gcc530; eval `scramv1 runtime -sh`
export X509_USER_PROXY=$HOME/private/grid.proxy
dirName=MonoH-$4
cmsRun treeMaker_genOnly.py inputFiles=root://eoscms//eos/cms/store/caf/user/syu/$dirName/$2 outputFile=$3
