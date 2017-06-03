# Enviornment setup
## Create directory in the lxplus work area
The following instruction assumes the shell is tcsh.
### Download genproduction, model files, and default run card
```
mkdir forXSec
cd forXSec
git clone git@github.com:cms-sw/genproductions.git
cd genproductions/bin/MadGraph5_aMCatNLO/
setenv BASEDIR $PWD
bash
source gridpack_generation.sh interactive
cd $BASEDIR/interactive/interactive_gridpack/work/MG5_aMC_v2_4_2/models
tar xvf /afs/cern.ch/cms/generators/www/Higgs_ZB_UFO.tar
cd $BASEDIR/interactive/interactive_gridpack/work/MG5_aMC_v2_4_2/Template/LO/Cards
wget https://raw.githubusercontent.com/syuvivida/DM/2016ana/launch_scripts/cards/run_card.dat -O run_card.dat
exit
```

### Download shell scripts and process cards
```
cd $BASEDIR
wget https://raw.githubusercontent.com/syuvivida/DM/2016ana/launch_scripts/cards/default_ZpBaryonic_proc_card.dat 
wget https://raw.githubusercontent.com/syuvivida/DM/2016ana/launch_scripts/mass_files/inputbaryon
wget https://raw.githubusercontent.com/syuvivida/DM/2016ana/launch_scripts/lxplus_batch_submission/runLaunch.sh 
wget https://raw.githubusercontent.com/syuvivida/DM/2016ana/launch_scripts/lxplus_batch_submission/launch_default_ZpBaryonic.sh
chmod 755 *sh
```

### Modify the mass text file 
The first column is the Z' mass and the second column represents the DM mass
Put in the mass grid you want to have in the text file "inputbaryon"

## Ready to run
```
cd $BASEDIR

awk '{print "./launch_default_ZpBaryonic.sh "$1,$2}' inputbaryon | bash
```

## Obtain the cross section
```
cd $BASEDIR/interactive/interactive_gridpack/work/MG5_aMC_v2_4_2/
wget https://raw.githubusercontent.com/syuvivida/DM/2016ana/launch_scripts/python_files/getCrossSection.py

## Now you need to modify the dmrange and medrange in getCrossSection.py so to match your mass points
python getCrossSection.py
```