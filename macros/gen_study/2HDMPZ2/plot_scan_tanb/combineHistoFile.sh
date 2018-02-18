#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=1
if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname xsecRatioFileName"
    echo "Example: ./$scriptname xsec_scantb_ggoverbb.txt"
    exit 1
fi

rfile=$1

dirname=combinedHistos
rm -rf $dirname
mkdir $dirname

while read line
do
    mh3=`echo $line | cut -d ' ' -f1`   
    mh4=`echo $line | cut -d ' ' -f2`
    temptb=`echo $line | cut -d ' ' -f3`  
    tb=${temptb/./p}
    rxsec=`echo $line | cut -d ' ' -f4`
    
#check if cross section ratio is 0
    if (( $(awk 'BEGIN {print ("'$rxsec'" == 0)}') )); then
	echo "gg/bb xsec ratio = 0"
	continue
    fi


    ggfile=scaledHistos/scaled_histo_gentuple_2HDMa_gg_tb_${tb}_MH3_${mh3}_MH4_${mh4}_MH2_${mh3}_MHC_${mh3}.root
    bbfile=scaledHistos/scaled_histo_gentuple_2HDMa_bb_tb_${tb}_MH3_${mh3}_MH4_${mh4}_MH2_${mh3}_MHC_${mh3}.root
    combinefile=$dirname/combined_histo_2HDMa_tb_${tb}_MH3_${mh3}_MH4_${mh4}_MH2_${mh3}_MHC_${mh3}.root

    if [ ! -f $ggfile ]; then
        echo "File $ggfile does not exist."
        continue
    fi
    
    if [ ! -f $bbfile ]; then
	if (( $(awk 'BEGIN {print ("'$rxsec'" < 100)}') )); then
            echo "File $bbfile does not exist."
            continue	
	fi

	if (( $(awk 'BEGIN {print ("'$rxsec'" >= 100)}') )); then
            echo "File $bbfile does not exist but contribution small"
	    cp -p $ggfile $combinefile
            continue	
	fi
    fi
    hadd $combinefile $ggfile $bbfile
done < $rfile