#!/bin/bash
#set -o verbose

PRODHOME=$1
CARDNAME=$2

cd $PRODHOME

source gridpack_generation.sh interactive

cd MG5_aMC_v2_3_0

./bin/mg5_aMC $CARDNAME