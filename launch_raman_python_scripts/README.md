## Prepare a CMSSW area

```
cmsrel CMSSW_8_0_20
cd CMSSW_8_0_20/src
```

## Download codes from github
```
git clone git@github.com:ramankhurana/MonoH.git
mkdir code
cp -p MonoH/MonoHbb/* code/.
wget https://raw.githubusercontent.com/syuvivida/DM/v0.05/launch_raman_python_scripts/MonoHBranchReader.py
wget https://raw.githubusercontent.com/syuvivida/DM/v0.05/launch_raman_python_scripts/PrepareistogramsAllRegions.py
wget https://raw.githubusercontent.com/syuvivida/DM/v0.05/launch_raman_python_scripts/launch.sh
wget https://raw.githubusercontent.com/syuvivida/DM/v0.05/launch_raman_python_scripts/run.sh
chmod 755 *sh *py
mv *.py code/.
```

## Run the job
```
./launch.sh /store/user/khurana/Merged_Skim/V9
```