#!/bin/tcsh

cd $1

setenv SCRAM_ARCH slc6_amd64_gcc472; eval `scramv1 runtime -csh`
setenv X509_USER_PROXY $HOME/private/grid.proxy

root -q -b xAna_rec.C++\(\"$2\"\) 
