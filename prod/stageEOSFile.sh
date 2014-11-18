#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3

userid="syu"
if [ $# -eq $(( EXPECTED_ARGS - 1 )) ]
then
    echo "user ID is set to "$userid
else if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: ./$scriptname remote_directory string userID"
    echo "Example: ./$scriptname AbelianZPrime_ZH_lljj_M800-MADGRAPH flattuple syu"
    exit 1
else
userid=$3
fi
fi

echo $1
echo "user id is "$userid
string=$2
topdir="temp"

if [[ ! -e $topdir ]]; then
    echo "creating directory "$topdir
    mkdir $topdir
fi


if [[ $1 == */* ]];
then
#    echo "It's double-layer directory";
    top=`echo "${1%%/*}"`
    echo "creating directory "$topdir"/"$top
    mkdir $topdir/$top
    echo "creating directory "$topdir"/"$1 
    mkdir $topdir/$1
    cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $string | awk -v my_var=$topdir"/"$1 '{print "xrdcp root://eoscms//eos/cms"$5" "my_var"/."}'  | bash
    sub=`echo "${1##*/}"`
    echo "adding root files "$topdir"/"$sub".root"
    hadd $topdir/$sub.root $topdir/$1/*root
else
#    echo "It's single-layer directory";
    echo "creating directory "$topdir"/"$1 
    mkdir $topdir/$1
    cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $string | awk -v my_var=$topdir"/"$1 '{print "xrdcp root://eoscms//eos/cms"$5" "my_var"/."}'  | bash
    echo "adding root files "$topdir"/"$1".root"
    hadd $topdir/$1.root $topdir/$1/*root
fi






