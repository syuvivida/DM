
## CMSSW setup for stepLHE and step0
```
setenv SCRAM_ARCH slc6_amd64_gcc481
cmsrel CMSSW_7_1_20
cd CMSSW_7_1_20/src
cmsenv
```

## Prepare the input files for stepLHE
The mono Higgs gridpacks are in these directories
/cvmfs/cms.cern.ch/phys_generator/gridpacks/slc6_amd64_gcc481/13TeV/madgraph/V5_2.3.0/monoHiggs
and 
/cvmfs/cms.cern.ch/phys_generator/gridpacks/slc6_amd64_gcc481/13TeV/madgraph/V5_2.3.2.2/monoHiggs/

```
python MultiCrab_stepLHE.py prepare <dirName> <NumberOfEventsPerjob> <NumberOfJobs>
```

A text file "dataset.txt" will be created. In this file, the first column lists the gridpack location while the second column lists the name of crab directory (to be used later when submitting jobs).

If you do not want to submit jobs for all the gridpacks listed in dataset.txt, you could modify dataset.txt.


## Submit the jobs of stepLHE
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh
python MultiCrab_stepLHE.py submit
```

## Check the status of jobs
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh
python MultiCrab_stepLHE.py status <work_directory>
```

## Prepare the input files for step0
Make sure your stepLHE jobs finish OK before you move to step0 (check the status).

The "dataset.txt" below is the input files for stepLHE:
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh
python MultiCrab_step0.py prepare dataset.txt
```
A text file "step0_inputdataset.txt" will be created. 
The first column lists the dataset name of stepLHE files and the second column lists the crab directory to be used when submitting jobs.

## Submit the jobs of step0
```
source /cvmfs/cms.cern.ch/crab3/crab_standalone.csh
python MultiCrab_step0.py submit
``` 
