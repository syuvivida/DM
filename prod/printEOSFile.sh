#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3

userid="syu"
if [ $# -eq $(( EXPECTED_ARGS - 1 )) ]
then
    echo "user ID is set to "$userid
else if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: ./$scriptname directory outputFileName userid"
    echo "Example: ./$scriptname AbelianZPrime_ZH_lljj_M800-MADGRAPH filelist syu"
    exit 1
else
userid=$3
fi
fi

echo $1
echo "user ID is "$userid

cmsLs /store/user/$userid/$1 | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a $userid | awk '{print "cmsLs "$5}' | bash | grep -a root | awk '{print $5}' > $2


