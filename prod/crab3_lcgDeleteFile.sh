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

removeprefix=`lcg-ls $dpmprefix`
echo $removeprefix
dpmprefix=$prefix$removeprefix
echo $dpmprefix

rm -rf temp3
    
if [[ $1 == test* ]]; then
    echo "This is a test directory"
    lcg-ls $dpmprefix | awk -v my_var=$prefix '{print "lcg-ls "my_var"/"$1}' | bash > temp3 
    lasttemp=`cat temp3 | wc -l`
    echo "There are "$lasttemp" directories"

    tempiteration=0    
    while [ $tempiteration -lt $lasttemp ]; 
    do
	tempiteration=$(( tempiteration + 1 ))
	infilename=(`head -n $tempiteration temp3  | tail -1`)
	echo $prefix$infilename
	lcg-ls $prefix$infilename | grep -a $string | awk -v my_var=$prefix '{print "lcg-del -l -v "my_var"/"$1}' | bash
	lcg-ls $prefix$infilename"/failed" | grep -a $string | awk -v my_var=$prefix '{print "lcg-del -l -v "my_var"/"$1}' | bash
    done
    lcg-del -d $prefix$infilename"/failed"
    lcg-del -d $prefix$infilename
    lcg-ls $dpmprefix | awk -v my_var=$prefix '{print "lcg-del -d "my_var"/"$1}' | bash
    lcg-del -d $dpmprefix
    lcg-del -d $prefix"/dpm/phy.ncu.edu.tw/home/cms/store/user/"$userid"/"$1
else
    echo "This is a real directory"
    lcg-ls $dpmprefix > temp3 
    lasttemp=`cat temp3 | wc -l`
    echo "There are "$lasttemp" directories"

    tempiteration=0    
    while [ $tempiteration -lt $lasttemp ]; 
    do
	tempiteration=$(( tempiteration + 1 ))
	infilename=(`head -n $tempiteration temp3  | tail -1`)
	echo $prefix$infilename
	lcg-ls $prefix$infilename | grep -a $string | awk -v my_var=$prefix '{print "lcg-del -l -v "my_var"/"$1}' | bash
	lcg-ls $prefix$infilename"/failed" | grep -a $string | awk -v my_var=$prefix '{print "lcg-del -l -v "my_var"/"$1}' | bash
    done
    lcg-del -d $prefix$infilename"/failed"
    lcg-del -d $prefix$infilename
    lcg-ls $dpmprefix | awk -v my_var=$prefix '{print "lcg-del -d "my_var"/"$1}' | bash
    lcg-del -d $dpmprefix
    lcg-del -d $prefix"/dpm/phy.ncu.edu.tw/home/cms/store/user/"$userid"/"$1

fi





