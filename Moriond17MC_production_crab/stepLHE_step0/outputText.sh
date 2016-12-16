#!/bin/bash                                                                                                                                                                     

EXPECTED_ARGS=1

if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: ./`basename $0` dirName"
    echo "Example: ./`basename $0` /cvmfs/cms.cern.ch/phys_generator/gridpacks/slc6_amd64_gcc481/13TeV/madgraph/V5_2.3.0/monoHiggs/Zp2HDM/Zprime_A0h_A0chichi/v1/"
    exit 1
fi

ls -lrt $1/*xz | awk '{print $9}' | grep -a xz > column1
ls -lrt $1 | awk '{print $9}' | grep -a xz | sed 's/_tarball.tar.xz/_hbb/g' > column2
a=${1##/*monoHiggs/}
echo $a
substring=${a%%/*}
echo $substring
sed -i -e 's/^/MonoH-'$substring'_/' column2

paste -d ' ' column1 column2 > dataset.txt
