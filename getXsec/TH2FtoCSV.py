#!/usr/bin/env python
import sys
import csv
from ROOT import TFile, TH2, TCanvas

def readRootFile():
    return  sys.argv[1]
def test_readhis(his, drawOption=''): 
    Canvas1 = TCanvas('','',800,600)
    his.Draw(drawOption)
    Canvas1.Print('test.pdf')
    return 'test import ROOT file'

def getXindex(h_TH2, xvalue):
    return h_TH2.GetXaxis().FindBin(xvalue)
def getYindex(h_TH2, yvalue):
    return h_TH2.GetYaxis().FindBin(yvalue)
def getBinValue(h_TH2, xIndex, yIndex):
    binValue = h_TH2.GetBinContent(xIndex,yIndex)
    return binValue

def writetoCSV(fname, dataList, deli='\t'):
    with open(fname, 'wb') as f:
        wr = csv.writer(f, delimiter=deli)
        wr.writerows(dataList)
    return None
def csvfileName(rootFileName):
    csvFileName = rootFileName[0:len(rootFileName)-len('.root')] + '.txt'       
    return csvFileName

def main():
    ma0List = [300,400,500,600,700,800]
    mzpList = [600,800,1000,1200,1400,1700,2000,2500]
    fname = readRootFile()
    f_root = TFile(fname)
    xsec1 = f_root.Get("xsec1")

    #print 'Ma0\tMzp\tCross-section'
    data = []
    data.append(['Ma0','Mzp','Cross-section'])
    
    for ma0 in ma0List:
        for mzp in mzpList:
            xBin = getXindex(xsec1,mzp)
            yBin = getYindex(xsec1,ma0)
            crossSection = getBinValue(xsec1,xBin,yBin)
            data.append([ma0,mzp,crossSection])
            #print str(ma0) + '\t' + str(mzp) + '\t' + str(crossSection)
    outputName = csvfileName(fname)       
    writetoCSV(outputName,data)

if __name__ == "__main__":
    main()
