import csv
import sys
print sys.argv

filename=sys.argv[1]


from ROOT import TGraph, TFile, TCanvas, TH2F, gStyle
from ROOT import TGraph2D, TAttText
gStyle.SetOptStat(0)
f = TFile("2Dcrossection.root","recreate")
c1 = TCanvas("c1", "c1",2000,1500)
c1.SetGrid()
dmrange =[1,10,20,50,100,150,500,1000]
medrange=[10,15,20,50,95,100,195,200,295,300,400,500,600,700,800,900,995,1000,1100,1200,1300,1400,1500,1995,2000,10000]

#hist = TH2F("xsec1", "ZpBaryonic Mono-jet Cross Sections (pb)",len(medrange),0,len(medrange),len(dmrange),0,len(dmrange))
#hist = TH2F("xsec1", "DMSimp-spin1 LO Mono-jet Cross Sections (pb)",len(medrange),0,len(medrange),len(dmrange),0,len(dmrange))
#hist = TH2F("xsec1", "DMSimp-spin1 LO Mediator Width (GeV)",len(medrange),0,len(medrange),len(dmrange),0,len(dmrange))
hist = TH2F("xsec1", "ZpBaryonic Mediator Width (GeV)",len(medrange),0,len(medrange),len(dmrange),0,len(dmrange))
#hist = TH2F("xsec1", "Ratio of ZB/DMSimp LO Mono-jet Cross Sections",len(medrange),0,len(medrange),len(dmrange),0,len(dmrange))
#hist = TH2F("xsec1", "Ratio of ZB/DMSimp Mediator Width",len(medrange),0,len(medrange),len(dmrange),0,len(dmrange))
hist.SetXTitle("MED (GeV)")
hist.SetYTitle("DM (GeV)")
#hist.SetStats(0)
hist.SetMarkerSize(0.5)
TXT = open(filename, 'r')
myData = csv.DictReader(TXT, delimiter=' ')
for row in myData:
        hist.Fill(medrange.index(float(row["MMED"])),dmrange.index(float(row["MDM"])),float(row["CrossSection"]))
        print float(row["MMED"]),float(row["MDM"]),float(row["CrossSection"])

for i in range(len(medrange)): hist.GetXaxis().SetBinLabel(i+1,str(medrange[i]))
for i in range(len(dmrange)): hist.GetYaxis().SetBinLabel(i+1,str(dmrange[i]))
#hist.Draw("colztext")
f.Write()
f.Close()
c1.Update()
TXT.close()
