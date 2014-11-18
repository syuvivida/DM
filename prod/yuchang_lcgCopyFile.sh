#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3

userid="syu"

if [ $# -eq $(( EXPECTED_ARGS - 1 )) ]
then
    echo "user ID is set to "$userid
else if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: ./$scriptname remoteDirectory string userid"
    echo "Example: ./$scriptname TTJets_MassiveBinDECAY_TuneZ2star_8TeV flattuple yuchang"
    exit 1
else 
    userid=$3
fi
fi

topdir="temp"
string=$2

if [[ ! -e $topdir ]]; then
    echo "creating directory "$topdir
    mkdir $topdir
fi

if [[ ! -e $topdir/$1 ]]; then
    echo "creating directory "$topdir/$1
    mkdir $topdir/$1
fi


prefix="srm://f-dpm001.grid.sinica.edu.tw:8446"
echo $prefix
dpmprefix=$prefix"/dpm/grid.sinica.edu.tw/home/cms/store/user/"$userid"/"$1
echo $dpmprefix

removeprefix="/dpm/grid.sinica.edu.tw/home/cms/store/user/"$userid"/"$1"/"

lcg-ls $dpmprefix | grep -a $string |  awk -F $removeprefix -v my_var=$dpmprefix -v my_dir=$topdir"/"$1 '{print "lcg-cp -v "my_var"/"$2" "my_dir"/"$2}' | bash





