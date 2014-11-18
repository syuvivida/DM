for ((  i = 0 ;  i <= 144;  i++  ))
  do
  echo "Chacking status $i th times"

  for crabdir in `less crabdir.txt`;do

##########################################
####Resubmit Aborted jobs
#########################################
if [ -f tmp0.txt ] 
    then
    rm tmp0.txt
fi
if [ -f tmp.txt ]
    then
    rm tmp.txt
fi
if [ -f jobnumber.txt ] 
    then 
    rm jobnumber.txt
fi

echo "cheching for aborted jobs"
echo $crabdir
crab -status all -c $crabdir
crab -status all -c $crabdir | grep Aborted >& tmp0.txt
echo "list of aborted jobs"
if [ -s tmp0.txt ]
    then
    cat tmp0.txt | grep -v Jobs >> tmp.txt
    cut -d' ' -f1 tmp.txt >> jobnumber.txt
    cat jobnumber.txt
    echo "echoing jobnumber.txt"
fi
if [ -s jobnumber.txt ] 
    then
    JOBNUMBER=`cat jobnumber.txt | gawk 'BEGIN {ORS=","}{print $1}'`
    echo "crab -resubmit $JOBNUMBER -c $crabdir"
    crab -resubmit $JOBNUMBER -c $crabdir
fi



##########################################
####Resubmit Cancelled jobs
#########################################
if [ -f tmp0.txt ] 
    then
    rm tmp0.txt
fi
if [ -f tmp.txt ]
    then
    rm tmp.txt
fi
if [ -f jobnumber.txt ] 
    then 
    rm jobnumber.txt
fi

echo "cheching for aborted jobs"
echo $crabdir
crab -status all -c $crabdir
crab -status all -c $crabdir | grep Cancelled >& tmp0.txt
echo "list of aborted jobs"
if [ -s tmp0.txt ]
    then
    cat tmp0.txt | grep -v Jobs >> tmp.txt
    cut -d' ' -f1 tmp.txt >> jobnumber.txt
    cat jobnumber.txt
    echo "echoing jobnumber.txt"
fi
if [ -s jobnumber.txt ] 
    then
    JOBNUMBER=`cat jobnumber.txt | gawk 'BEGIN {ORS=","}{print $1}'`
    echo "crab -resubmit $JOBNUMBER -c $crabdir"
    crab -kill $JOBNUMBER -c $crabdir
    crab -resubmit $JOBNUMBER -c $crabdir
    crab -submit $JOBNUMBER -c $crabdir
fi

#########################################
#### Retrieve Done jobs##################
#########################################

if [ -f tmp0.txt ] 
    then
    rm tmp0.txt
fi
if [ -f tmp.txt ]
    then
    rm tmp.txt
fi
if [ -f jobnumber.txt ] 
    then 
    rm jobnumber.txt
fi
crab -status all -c $crabdir | grep Done >& tmp0.txt
cat tmp0.txt | grep -v Jobs >> tmp.txt
cut -d' ' -f1 tmp.txt >> jobnumber.txt
if [ -s jobnumber.txt ] 
    then 
    JOBNUMBER=`cat jobnumber.txt | gawk 'BEGIN {ORS=","}{print $1}'`
    crab -getoutput $JOBNUMBER -c $crabdir 
fi

#########################################################
####### Resubmit jobs with non zero exit code ###########
#########################################################

if [ -f tmp0.txt ] 
    then
    rm tmp0.txt
fi
if [ -f tmp.txt ]
    then
    rm tmp.txt
fi
if [ -f jobnumber.txt ] 
    then 
    rm jobnumber.txt
fi
if [ -f out.txt ] 
    then
    rm out.txt
fi

crab -status all -c $crabdir  >& out.txt 
cat out.txt  | grep -v "\-\-\-\-" | gawk '{ print $1"  "$3"    "$6}'  | grep -v "crab" | grep -v "Jobs" | grep -v "List" | grep -v "Code" | grep -v "CMS" | grep -v "Your" | grep -v "Log" | gawk '{if($2 == "Retrieved") print $1 "  "$2 "   " $3}' | gawk '{ORS=","}{if($3!=0) print $1}' >> jobnumber.txt
if [ -s jobnumber.txt ] 
    then
    jobnumber=`cat jobnumber.txt`
    crab -resubmit $jobnumber -c $crabdir
fi

#####################################
############3 delete files###########
#####################################

if [ -f tmp0.txt ] 
    then
    rm tmp0.txt
fi
if [ -f tmp.txt ]
    then
    rm tmp.txt
fi
if [ -f jobnumber.txt ] 
    then 
    rm jobnumber.txt
fi
if [ -f out.txt ] 
    then
    rm out.txt
fi

done
sleep 1000
done