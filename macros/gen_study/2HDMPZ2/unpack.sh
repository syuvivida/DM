#!/bin/bash

WORKDIR=$1
dir=tmp_$2_$3_$4_$5
mkdir $WORKDIR/$dir
cd $WORKDIR/$dir
tar xvf $WORKDIR/MH3_$2_MH4_$3_MH2_$4_MHC_$5_slc6_amd64_gcc481_CMSSW_7_1_30_tarball.tar.xz
cd $WORKDIR
printf "$2,$3,$4,$5 `grep -a Cross $WORKDIR/$dir/gridpack_generation.log` \n" | tee -a log_cross
rm -rf $WORKDIR/$dir