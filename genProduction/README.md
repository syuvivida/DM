
To produce root files that contain LHE information and generator-level 
objects, 

```
cmsrel CMSSW_7_1_20
cd CMSSW_7_1_20/src
cmsenv 
git cms-addpkg GeneratorInterface/LHEInterface
git cms-merge-topic syuvivida:71X_genMET
scramv1 b clean
scramv1 b
wget https://raw.githubusercontent.com/syuvivida/DM/v0.04/genProduction/step0.py
wget https://raw.githubusercontent.com/syuvivida/DM/v0.04/genProduction/step1.py

cmsRun step0.py maxEvents=10 outputFile=step0.root tarball=$WORK/public/DMLHE/Scalar_MZp1000_MChi1_tarball.tar.xz
cmsRun step1.py inputFiles=file:step0_numEvent10.root outputFile=step1.root
cmsRun GeneratorInterface/LHEInterface/test/dumpLHE_cfg.py inputFiles=file:step1.root outputFile=test.root
```
