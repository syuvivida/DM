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
    echo "Example: ./$scriptname TTJets_MassiveBinDECAY_TuneZ2star_8TeV flattuple syu"
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


prefix="srm://f-dpm001.grid.sinica.edu.tw:8446"
echo $prefix
dpmprefix=$prefix"/dpm/grid.sinica.edu.tw/home/cms/store/user/"$userid"/"$1
echo $dpmprefix



if [[ $1 == */* ]];
then
    top=`echo "${1%%/*}"`
    if [[ ! -e $topdir/$top ]]; then
    echo "creating directory "$topdir/$top
    mkdir $topdir/$top
    fi
    if [[ ! -e $topdir/$1 ]]; then
    echo "creating directory "$topdir"/"$1 
    mkdir $topdir/$1
    fi
    removeprefix=`lcg-ls $dpmprefix`
    echo $removeprefix
    dpmprefix=$prefix$removeprefix
    echo $dpmprefix
    
    lcg-ls $dpmprefix | grep -a $string |  awk -F $removeprefix -v my_var=$dpmprefix -v my_dir=$topdir"/"$1 '{print "lcg-cp -v "my_var"/"$2" "my_dir"/"$2}' | bash

else

    if [[ ! -e $topdir/$1 ]]; then
	echo "creating directory "$topdir/$1
	mkdir $topdir/$1
    fi
    removeprefix=`lcg-ls $dpmprefix | grep -a $userid |  awk -v my_var=$prefix '{print "lcg-ls "my_var""$1}' | bash`
    echo $removeprefix
    dpmprefix=$prefix$removeprefix
    echo $dpmprefix
    
    lcg-ls $dpmprefix | grep -a $string |  awk -F $removeprefix -v my_var=$dpmprefix -v my_dir=$topdir"/"$1 '{print "lcg-cp -v "my_var"/"$2" "my_dir"/"$2}' | bash

fi



