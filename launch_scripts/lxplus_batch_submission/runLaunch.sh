#!/bin/bash
#set -o verbose

PRODHOME=$1
CARDNAME=$2

cd $PRODHOME

source gridpack_generation.sh interactive

cd MG5_aMC_v2_4_2

./bin/mg5_aMC $CARDNAME