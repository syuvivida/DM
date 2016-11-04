#!/usr/bin/env python                                                                                                                                                                
#from MonoHBranchReader import AnalyzeDataSet, CheckFilter, MakeTable, DeltaR, Phi_mpi_pi
import os

mode='DEMOMODE'

#inputfilename = os.environ['INPUT']
#outfilename   = os.environ['OUTPUT']

inputfilename = ''

def RunOneRegion(mode,inputfilename):
## Signal Region
    if mode == 'signal':
        os.system('./MonoHBranchReader.py  -m 100.0 -M 150.0 -i '+inputfilename+'  -a -j 0 -J 2 -l 0 -L 1  --MLow1 100.0 --MHigh1 150.0 -D HistogramsSignalRegion')
        
## Mass Sidebands
## inverting the mass cut
        
    if mode == 'zj':
        os.system('./MonoHBranchReader.py  -m 30.0 -M 100.0 -i '+inputfilename+'  -a -j 0 -J 2 -l 0 -L 1  --MLow1 150.0 --MHigh1 250.0 -D HistogramsZjRegion')

##WJets
## 1 additinal lepton
## remove the mass cut
    if mode == 'wj':
        os.system('./MonoHBranchReader.py  -m 30.0 -M 250.0 -i '+inputfilename+'  -a -j 1 -J 2 -l 1 -L 2  --MLow1 30.0 --MHigh1 250.0 -D HistogramsWjRegion')


##TT
## 1 additional lepton 
## >1 additional jets
    if mode == 'tt':
        os.system('./MonoHBranchReader.py  -m 30.0 -M 250.0 -i '+inputfilename+'  -a -j 2 -J 10 -l 1 -L 2  --MLow1 30.0 --MHigh1 250.0 -D HistogramsTTRegion')


modes = ['signal','zj','wj','tt']

for whichmode in modes:
    textfile = open('MergedSkimmedFiles.txt')
    for whichfile in textfile:
        RunOneRegion(whichmode,whichfile.rstrip())
    
