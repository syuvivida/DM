
## CMSSW setup
```
setenv SCRAM_ARCH slc6_amd64_gcc481
cmsrel CMSSW_7_1_20
cd CMSSW_7_1_20/src
cmsenv
```

## Prepare the input files
The mono Higgs gridpacks are in these directories
/cvmfs/cms.cern.ch/phys_generator/gridpacks/slc6_amd64_gcc481/13TeV/madgraph/V5_2.3.0/monoHiggs
and 
/cvmfs/cms.cern.ch/phys_generator/gridpacks/slc6_amd64_gcc481/13TeV/madgraph/V5_2.3.2.2/monoHiggs/
```
python python MultiCrab_stepLHE.py prepare <dirName> <NumberOfEventsPerjob> <NumberOfJobs>
```

If you do not want to submit jobs for all the gridpacks listed in dataset.txt, you could modify dataset.txt.

## Submit the jobs
```
python MultiCrab_stepLHE.py submit
```

## Check the status of jobs
```
python MultiCrab_stepLHE.py status <work_directory>
```
 
