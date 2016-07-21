## Download genproduction from git
```
git clone git@github.com:cms-sw/genproductions.git genproductions
cd genproductions/bin/MadGraph5_aMCatNLO/
bash
source gridpack_generation.sh interactive
```

## Obtain the 2HDM models (gz as external parameter)
```
cd MG5_aMC_v2_3_3/models
tar xvzf /afs/cern.ch/cms/generators/www/MonoHiggs_Zp2HDM_v1.4.tgz
mv Zp2HDM_v1.4 Zp2HDM
```

## Obtain the 2HDM models (gz as formula)
```
cd MG5_aMC_v2_3_3/models
tar xvzf /afs/cern.ch/cms/generators/www/MonoHiggs_Zp2HDM_v1.5_gzformula.tgz
mv Zp2HDM_v1.5_gzformula Zp2HDM
```

## Download cards and shell scripts
```
cd ..
curl https://raw.githubusercontent.com/syuvivida/DM/v0.04/getXsec_allshellscript/proc_card.dat -o proc_card.dat
./bin/mg5_aMC proc_card.dat

cd ZphA0/Cards/
curl https://raw.githubusercontent.com/syuvivida/DM/v0.04/getXsec_allshellscript/run_card.dat -o run_card.dat
curl https://raw.githubusercontent.com/syuvivida/DM/v0.04/getXsec_allshellscript/param_card.dat -o param_card.dat
curl https://raw.githubusercontent.com/syuvivida/DM/v0.04/getXsec_allshellscript/makeXsec.sh -o makeXsec.sh
curl https://raw.githubusercontent.com/syuvivida/DM/v0.04/getXsec_allshellscript/makeXsec.C -o makeXsec.C
```

### Scanning the mass points
```
cd ..
./bin/generate_events -f
cd Cards
./makeXsec.sh >& xsec.txt
root -l makeXsec.C++
```