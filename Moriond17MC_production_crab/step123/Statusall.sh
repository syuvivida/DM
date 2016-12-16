source /cvmfs/cms.cern.ch/crab3/crab.sh

if [ -f crabtaskslist.txt ] ; then 
    rm crabtaskslist.txt
else echo "file crabtaskslist.txt does not exist will list the directory and create one now"
fi

if [ $# -lt 1 ] ; then
    echo $crabProjectDir " does not exist"
    echo "give correct crabProjectDir name"
    exit 0;
fi

crabProjectDir=$1
if [ -d $crabProjectDir ] ; then 
    ls -1 $crabProjectDir >& crabtaskslist.txt
    for which in `less crabtaskslist.txt` ; do
	echo " "
	echo " "
	echo "---------------- CHECKING STATUS OF THE JOBS ---------------------"
	echo $which
	echo " "
	echo " "
	crab status $crabProjectDir/$which
    done
else 
    echo $crabProjectDir " does not exist"
    echo "give correct crabProjectDir name"
fi