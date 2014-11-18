#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=2

if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: ./$scriptname cfgTemplate inutFile"
    echo "Example: ./$scriptname crabmaker_MC.cfg mcsample"
    exit 1
fi


awk '{print "sed -e \"s/INPUT/"$2"/g\" -e \"s/OUTPUT/"$1"/g\" '${1}' > crabTreeMaker_"$1".cfg"}' ${2} | bash
