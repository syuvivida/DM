#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=2
if [ $# -ne $EXPECTED_ARGS ]
then
echo "Usage: $scriptname script queue"
echo "Example: ./$scriptname runStep1.sh 1nw"
exit 1
fi

if [ ! -e $PWD/$1 ]; then
 echo $PWD/$1 " does not exist!"
exit 1
fi

export iteration=0
lastfile=50
          
while [ $iteration -lt $lastfile ]; 
do
  iteration=$(( iteration + 1 ))
  echo $iteration
  bsub -q $2 -C 0 -R "rusage[mem=30000]" $PWD/$1 $PWD

done
