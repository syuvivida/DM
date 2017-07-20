#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=4
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname MH3 MH4 MH2 MHC"
    echo "Example: ./$scriptname 600 400 600 600"
    exit 1
fi

export PRODHOME=`pwd`
name=2HDMPZ2II

mh3=$1
mh4=$2
mh2=$3
mhc=$4

#echo ""
#echo "Getting cross sections for A mass = "$mh3" GeV "
#echo "Getting cross sections for a mass = "$mh4" GeV "
#echo "Getting cross sections for h2 mass = "$mh2" GeV "
#echo "Getting cross sections for hc mass = "$mhc" GeV "
#echo ""

newname=${name}\_MH3\_${mh3}\_MH4\_${mh4}\_MH2\_${mh2}\_MHC\_${mhc}
echo $newname
grep -a pb $newname/Events/run_01/run_01_tag_1_banner.txt | awk '{print "'$mh3' '$mh4' '$mh2' '$mhc' 0.35 1.0 3.0 3.0 3.0 1.0 1.0 1.0", $6}' >> 2HDMPZ2II_ATLASsetting.txt

#&& echo $? || echo $?


