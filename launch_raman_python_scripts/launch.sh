#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=2
queue=2nd
if [ $# -eq $(( EXPECTED_ARGS - 1 )) ]
then
    echo "jobs will be submitted to queue "$queue
else if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname EOSDir queue"
    echo "Example: ./$scriptname /store/user/khurana/Merged_Skim/V9/ 2nd"
    exit 1
else
    queue=$2
fi
fi

export PRODHOME=`pwd`

## produce EOS root file text files
inputTextFile=eosfiles.txt
jobTextFile=MergedSkimmedFiles.txt
eosdir=$1
eos ls $eosdir | awk -v myVar=$eosdir '{print "root://eoscms//eos/cms/"myVar"/"$1}' > $inputTextFile

## Now start to submit jobs
while read p; do 
    temp=${p%%.root}
    name=${temp##*/}
    echo $name
    outputdir=joboutput/workdir\_$name
    mkdir -p $outputdir
    cp -p code/* $outputdir/.
    echo $p > $outputdir/$jobTextFile    
    bsub -q$queue -R "rusage[mem=12000]" $PWD/run.sh $PWD/$outputdir 
    
done < $inputTextFile




