#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3

userid="syu"
if [ $# -eq $(( EXPECTED_ARGS - 1 )) ]
then
    echo "user ID is set to "$userid
else if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: ./$scriptname remote_directory string userid"
    echo "Example: ./$scriptname AbelianZPrime_ZH_lljj_M800-MADGRAPH flattuple syu"
    exit 1
else
userid=$3
fi
fi

echo $1
echo "user id is "$userid
string=$2
if [[ $1 == */* ]]
then
cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $string | awk -v my_var=$1 '{print "cmsRm "$5}' | bash
#cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsRmdir "$5}' | bash

#cmsRmdir /store/user/$userid/$1

else
cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $string | awk -v my_var=$1 '{print "cmsRm "$5}' | bash
#cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $userid | awk '{print "cmsRmdir "$5}' | bash

#cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsRmdir "$5}' | bash

#cmsRmdir /store/user/$userid/$1

fi
