#!/bin/bash


cd $1
mkdir tarball_$iteration
cd tarball_$iteration
tar -xavf $2
export SCRAM_ARCH=slc6_amd64_gcc481; eval `scramv1 runtime -sh`


i=`date '+%s'`
echo $i
a=`ksh 'echo $RANDOM'`
echo $a
z=$((i / a))
echo $z

./runcmsgrid.sh 1000 $z 1

mv events_presys.lhe events_presys_$iteration.lhe
cp events_presys_$iteration.lhe ../step1Storage
cd $1
