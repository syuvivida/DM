Installation of Delphes and Pythuia8
==
```
install pythia8
install Delphes
```
### For running on Delphes output root files
```
python3 m4b.py ../Events/run_01_decayed_1/tag_1_delphes_events.root --out m4b_run01
```
### For installation of NNPDF 3.1 NNLO
```
cd /Users/yush/2HDMa/withPythia8D/MG5_aMC_v3_5_7/HEPTools/lhapdf6_py3/share/LHAPDF
curl -L https://lhapdfsets.web.cern.ch/current/NNPDF31_nnlo_as_0118.tar.gz | tar xz
ls NNPDF31_nnlo_as_0118 | head -3     # should list NNPDF31_nnlo_as_0118.info and *_0000.dat ...
```

### To read lhapdf properly
Update NNPDF31_nnlo_as_0118.info (in HEPTools/lhapdf6_py3/share/LHAPDF/ and in workdir_AHhhaa_4b/lib/PDFsets/)
Add the following lines
```
AlphaS_FlavorScheme: variable
AlphaS_NumFlavors: 5
```