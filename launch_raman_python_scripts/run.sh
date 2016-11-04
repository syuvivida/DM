#!/bin/sh

echo $1
cd $1
export SCRAM_ARCH=slc6_amd64_gcc530; eval `scramv1 runtime -sh`
python PrepareistogramsAllRegions.py
