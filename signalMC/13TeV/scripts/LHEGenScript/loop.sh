#!/bin/bash

scriptname=`basename $0`
EXPECTED_ARGS=3
if [ $# -ne $EXPECTED_ARGS ]
then
echo "Usage: $scriptname script queue"
echo "Example: ./$scriptname tarballRun.sh 1nw path_of_gridpack_directory"
exit 1
fi

if [ ! -e $PWD/$1 ]; then
 echo $PWD/$1 " does not exist!"
exit 1
fi

export iteration=0
lastfile=50
mkdir lhefile_test
while [ $iteration -lt $lastfile ]; 
do
  iteration=$(( iteration + 1 ))
  echo $iteration
  bsub -q $2 $PWD/$1 $PWD $3

done
