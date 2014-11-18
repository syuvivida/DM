#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3


if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: ./$scriptname remoteDirectory string userid"
    echo "Example: ./$scriptname TTJets_MassiveBinDECAY_TuneZ2star_8TeV flattuple syu"
    exit 1
fi

userid=$3



prefix="srm://grid71.phy.ncu.edu.tw/"
echo $prefix
dpmprefix=$prefix"/dpm/phy.ncu.edu.tw/home/cms/store/user/"$userid"/"$1
echo $dpmprefix

string=$2

if [[ $1 == */* ]];
then
    removeprefix=`lcg-ls $dpmprefix`
    echo $removeprefix
    dpmprefix=$prefix$removeprefix
    echo $dpmprefix
    
    lcg-ls $dpmprefix | grep -a $string |  awk -F $removeprefix -v my_var=$dpmprefix '{print "lcg-del -l -v "my_var"/"$2}' | bash

else
    removeprefix=`lcg-ls $dpmprefix | grep -a $userid |  awk -v my_var=$prefix '{print "lcg-ls "my_var""$1}' | bash`
    echo $removeprefix
    dpmprefix=$prefix$removeprefix
    echo $dpmprefix
    
    lcg-ls $dpmprefix | grep -a $string |  awk -F $removeprefix -v my_var=$dpmprefix '{print "lcg-del -l -v "my_var"/"$2}' | bash

fi



