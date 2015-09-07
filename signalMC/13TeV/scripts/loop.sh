#!/bin/bash
scriptname=`basename $0`
EXPECTED_ARGS=3
MIN_ARGS=2
maxevent=-1
if [ $# -eq $MIN_ARGS ]
then
    echo "maxEvents="$maxevent
else if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: $scriptname scriptfile queue nEventsPerJob"
    echo "Example: ./scripts/$scriptname runStep3.sh 2nd 1000"
    exit 1
else
    maxevent=$3
fi
fi

script=$1
queue=$2


if [ ! -e $PWD/$script ]; then
 echo $PWD/$script " does not exist!"
exit 1
fi

iteration=0
lastfile=50

while [ $iteration -lt $lastfile ]; 
do
  iteration=$(( iteration + 1 ))
  echo $iteration
  bsub -q $queue $PWD/$script $PWD $maxevent $iteration
done
