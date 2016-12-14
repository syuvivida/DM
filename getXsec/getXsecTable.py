#! /usr/bin/env python
#-------------------------------------------------------------
# File: getXsecTable.py
# Created: 15 July 2016 Fang-Ying Tsai
# Modified 15 Dec 2016 Shu-Xiao Liu
#-------------------------------------------------------------  
import glob
import errno
from ROOT import TGraph, TFile, TCanvas, TH2F, gStyle
from ROOT import TGraph2D
from array import array
import os
class GetValue():
    def __init__(self):
    	self.fileNum = []
    	self.zpMass = []
    	self.ma0Mass = []
    	self.mdmMass = []
    	self.weight = []
    	self.tanbeta = []

hAList = []
hApath = 'run_*.txt'  
hA_files = glob.glob(hApath) 

def getFile(fileName):
		num = str(fileName).split("_")[1].split(".")[0]
		global s
		s = GetValue()
		s.fileNum = int(num)
		return s

def getMZpValue(readLine):
	mzpValue = float(readLine.strip().split(' # ')[0].split('32 ')[1])
	return mzpValue

def getMA0Value(readLine):
	ma0Value = float(readLine.strip().split(' # ')[0].split('28 ')[1])
	return ma0Value

def getMDMValue(readLine):
	mdmValue = float(readLine.strip().split(' # ')[0].split('18 ')[1])
	return mdmValue

def getWeightValue(readLine):
	weightValue = float(readLine.split(' : ')[1])
	return weightValue	

def getTanBeta(readLine):
	tbValue  = float(readLine.split(' # ')[0].split()[1])
	return tbValue 

def gethAList(lhefile):
	for name in lhefile: 
		s = getFile(name)
	    	try:
	    	    with open(name) as f: 
	    	    	for lheFile in f:
	    	    		if lheFile.find('mzp') > 0 and lheFile.find('32') > 0:
	    	    			s.zpMass = getMZpValue(lheFile)
	    	    		elif lheFile.find('ma0') > 0:
	    	    			s.ma0Mass = getMA0Value(lheFile)
	    	    		elif lheFile.find('mx') > 0:
	    	    			s.mdmMass = getMDMValue(lheFile)
	    	    		elif lheFile.find('Integrated weight') > 0:
	    	    			s.weight = getWeightValue(lheFile)
	    	    		elif lheFile.find('tb') > 0 and lheFile.find('#') > 0:
	    	    			s.tanbeta = getTanBeta(lheFile)
					hAList.append(s)
	    	except IOError as exc:
	    	    if exc.errno != errno.EISDIR: 
	    	        raise 
	return hAList

def main():
	f = TFile("ScanXsec.root","recreate")
	c1 = TCanvas( 'c1', 'A Simple Graph', 200, 10, 700, 500 )
	c1.SetGrid()

	hAList = gethAList(hA_files)

	histo_hA0 = TH2F('xsec1','Xsec(gz=0.8, tb=10.0) pdf setting 263400', 12, 500,2800,8,200,900)
	histo_hA0.SetXTitle("M_Zp (GeV)")
	histo_hA0.SetYTitle("M_A0 (GeV)")
	histo_hA0.SetStats(0)

	for a in hAList:
		histo_hA0.Fill(a.zpMass,a.ma0Mass,a.weight)
	gStyle.SetPalette(1)

	f.Write()
	f.Close()
	c1.Update()
	print 'path of root file'
        print os.getcwd()+'/*.root'
if __name__ == "__main__":
   main()
