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



//Added new for MVA MET
// To get the MVA MET in your tree check out the following packages
// and training files
git cms-addpkg PhysicsTools/PatAlgos
git cms-addpkg FWCore/Version
//MET Recipe ()
//MVA & No-PU MET Recipe
git-cms-merge-topic -u cms-met:72X-13TeV-Training-30Jan15
// check at https://twiki.cern.ch/twiki/bin/viewauth/CMS/MVAMet#CMSSW_7_2_X_requires_slc6_MiniAO to know if reciepe is updated

// clone the data files 
cd RecoMET/METPUSubtraction/
git clone https://github.com/rfriese/RecoMET-METPUSubtraction data
// check for new files on the link https://twiki.cern.ch/twiki/bin/viewauth/CMS/MVAMet#Status_of_files_for_72X_as_of_30
// Other necessary changes are done in the .py and .cc files
cmsenv
//adapted changes for cmssw_7_4_0_pre9

mv reco* ExoDiBosonResonances/EDBRJets/python/
scramv1 b clean

scramv1 b 

cmsRun MVA-MET-PFUncorrectedMET-Jet_cfg.py // This make the PFMEt and uses the PF raw MET (not MVA) You may not to checkut thepackages for MVA met in 740. 

// cmsRun Final_mvaMETOnMiniAOD_cfg.py /// this is for MVA MET and old met both

//   cmsRun my.py // You need to replace input miniAOD files with your own

### For commit later
git add xxxx

git commit -m "remove lines"

git pull --rebase upstream v0.00

git push origin v0.00
