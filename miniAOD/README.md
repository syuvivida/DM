To compile and run:

1. Follow the instruction in Diboson resonance pages to check out all 
other packages

https://twiki.cern.ch/twiki/bin/view/CMS/ExoDiBosonResonancesRun2#Instruction_to_download_and_comp

2. Check out our own code

Under CMSSW_7_2_2_patch1/src

git clone git@github.com:syuvivida/DM.git DM

cd DM

git checkout v0.00

git remote add upstream git@github.com:syuvivida/DM.git

cd -

mv DM/miniAOD/* .
 
mv goodAK4Jets_cff.py ExoDiBosonResonances/EDBRCommon/python/.

mv DM/prod/jec/ .

mv DM/prod/xml/ .

rm -rf DM

cmsenv

scramv1 b clean

scramv1 b 

cmsRun my.py ## You need to replace input miniAOD files with your own

### For commit later
git add xxxx

git commit -m "remove lines"

git pull --rebase upstream v0.00

git push origin v0.00
