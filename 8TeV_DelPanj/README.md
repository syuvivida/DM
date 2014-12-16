
cmsrel CMSSW_5_3_20
cd CMSSW_5_3_20
rm -rf src
tar xvzf /afs/cern.ch/work/s/syu/public/forStudents/src.tgz
cd src
rm -rf DelPanj


git clone git@github.com:syuvivida/DM.git DM

cd DM

git checkout v0.00

git remote add upstream git@github.com:syuvivida/DM.git

cd -

mv DM/8TeV_DelPanj/DelPanj .

rm -rf DM

scramv1 b clean

bsub -q2nd $PWD/runJob.sh $PWD