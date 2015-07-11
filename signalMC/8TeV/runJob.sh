#!/bin/tcsh

cd $1

setenv SCRAM_ARCH slc6_amd64_gcc472; eval `scramv1 runtime -csh`
setenv X509_USER_PROXY $HOME/private/grid.proxy
set string=$3
set file="file:"$3$2".lhe"
echo $file
set outfile="file:step1_"$3"_"$2".root"
echo $outfile
set outfile2="file:step2_"$3"_"$2".root"
echo $outfile2
set finalfile="file:step3_"$3"_"$2".root"
echo $finalfile

#cmsRun python_step1.py inputFiles=$file outputFile=$outfile maxEvents=-1
#cmsRun python_step2.py inputFiles=$outfile outputFile=$outfile2 mixHistFile=histProbFunction_$2.root
cmsRun python_step3.py inputFiles=$outfile2 outputFile=$finalfile mixHistFile=histProbFunction_$2.root
