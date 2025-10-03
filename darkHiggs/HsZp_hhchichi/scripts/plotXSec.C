#include <TH2.h>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <iostream>
#include <TStyle.h>
#include <TCanvas.h>
using namespace std;


void plotXSec(bool debug=false)
{
  // mHs, mDM, mZp, xSec
  map<int,map<int,map<int,float>>> myXSecMap;
  // For accessing mHs
  map<int, map<int,map<int,float>>>::iterator itr;
  // For accessing mDM
  map<int, map<int,float>>::iterator jtr;
  // For accessing mZp
  map<int, float>::iterator ktr;

  
  TCanvas* c1 = new TCanvas("c1","",800,500);
  ifstream fin;
  int mHs, mDM, mZp;
  float br, xsec;
  fin.open("xSec_HsZp_hhchichi_13p6TeV_sinTheta0p01_gq0p25_gDM_1.dat");
  // will plot the 2D MDM vs MZp cross section plots for different
  // dark Higgs mass
  fin >> mHs >> mDM >> mZp >> br >>  xsec;
  
  do
    {
      myXSecMap.insert(make_pair(mHs,map<int,map<int,float>>()));
      myXSecMap[mHs].insert(make_pair(mDM,map<int,float>()));
      myXSecMap[mHs][mDM].insert(make_pair(mZp,xsec));
      fin >> mHs >> mDM >> mZp >> br >>  xsec;
    }
  while(!fin.eof());
  const int nHistos=myXSecMap.size();
  cout << "There are " << nHistos << " mHs values " << endl;
  TH2F* hXSec[nHistos];
  TH2F* hCol[nHistos];
  unsigned int HistoLabel[nHistos];
  // do a first pass to create labels and define histograms
  unsigned int nHistoCount=0;
  for(itr= myXSecMap.begin(); itr != myXSecMap.end(); itr++, nHistoCount++){

    HistoLabel[nHistoCount] = itr->first;
    
    const unsigned int nYLabels = itr->second.size();
    unsigned int yLabel[nYLabels];
    unsigned int yCount=0;
    std::vector<unsigned int> xLabel;
    for(jtr= itr->second.begin(); jtr != itr->second.end(); jtr++, yCount++){
      yLabel[yCount] = jtr->first;

      if(yCount>0)continue;

      // impose a cross section cut for 1fb
      for(ktr= jtr->second.begin(); ktr != jtr->second.end(); ktr++){
	double xSec_current = ktr->second;
	if(xSec_current<1e-3)continue;
	xLabel.push_back(ktr->first);
      }
    }
    unsigned int nXLabels = xLabel.size();
    if(nXLabels<1)break;
    
    hXSec[nHistoCount] = new TH2F(Form("hXSec_mHs%d",itr->first),
				  Form("Dark Higgs hh+#chi#chi cross sections @ #sqrt{s}=13.6 TeV [pb], M_{Hs}=%d GeV",itr->first),
				  nXLabels, 0.5, nXLabels+0.5, nYLabels, 0.5, nYLabels+0.5);
    hXSec[nHistoCount]->GetXaxis()->SetLabelSize(0.055);
    hXSec[nHistoCount]->GetYaxis()->SetLabelSize(0.055);
    hXSec[nHistoCount]->GetXaxis()->SetTitleSize(0.05);
    hXSec[nHistoCount]->GetYaxis()->SetTitleSize(0.05);
    hXSec[nHistoCount]->GetZaxis()->SetLabelSize(0.03);
    hXSec[nHistoCount]->SetTitleOffset(0.9,"Y");
    hXSec[nHistoCount]->SetTitleOffset(0.9,"X");
    hXSec[nHistoCount]->SetXTitle("M_{Z'} [GeV]");
    hXSec[nHistoCount]->SetYTitle("M_{DM} [GeV]");
    hCol[nHistoCount]= (TH2F*)hXSec[nHistoCount]->Clone(Form("hCol_mHs%d",itr->first));
    for(unsigned int ix=0; ix< nXLabels; ix++)
      hCol[nHistoCount]->GetXaxis()->SetBinLabel(ix+1,std::to_string(xLabel[ix]).data());
    for(unsigned int iy=0; iy< nYLabels; iy++)
      hCol[nHistoCount]->GetYaxis()->SetBinLabel(iy+1,std::to_string(yLabel[iy]).data());
    gStyle->SetOptStat(0);
  } // only do this once for each histogram

  // do a second pass to fill bin content
 
  for(unsigned int ih = 0; ih < nHistoCount; ih++){
  //  for(unsigned int ih = 0; ih < 1; ih++){
    
    double maxXsecValue = -999;
    // do a mapping
    int mHs = HistoLabel[ih];

    // For accessing mHs
    map<int, map<int,map<int,float>>>::iterator itr_0 = myXSecMap.find(mHs);
    if(itr_0 == myXSecMap.end())continue;

    for(int iy=1; iy < hCol[ih]->GetNbinsY()+1; iy ++) {

      int mDM= stoi(hCol[ih]->GetYaxis()->GetBinLabel(iy));
      // For accessing mDM
      map<int, map<int,float>> map_1 = itr_0->second;
      map<int, map<int,float>>::iterator itr_1 = map_1.find(mDM);
      if(itr_1 == map_1.end())continue;      
      for(int ix=1; ix < hCol[ih]->GetNbinsX()+1; ix++) {

	int mZp = stoi(hCol[ih]->GetXaxis()->GetBinLabel(ix));

	// For accessing mZp
	map<int, float> map_2 = itr_1->second;
	map<int, float>::iterator itr_2 = map_2.find(mZp);
	if(itr_2 == map_2.end())continue;
	gStyle->SetOptStat(0);
	double currentXSec = itr_2->second;
	if(currentXSec > maxXsecValue) maxXsecValue = currentXSec;
	gStyle->SetPaintTextFormat("1.3f");
	//	if(maxXsecValue < 1e-4)
	//	  gStyle->SetPaintTextFormat("1.5f");
	//	if(maxXsecValue < 1e-3)
	//	  gStyle->SetPaintTextFormat("1.4f");

	hXSec[ih]->SetBinContent(ix,iy, currentXSec);
	hCol[ih]->SetBinContent(ix,iy, currentXSec);
	if(debug)
	  cout << mHs << "\t" << mDM << "\t" << mZp
	       << "\t"<< currentXSec << endl;
      }
    }
    
    hCol[ih]->Draw("colz");
    hXSec[ih]->SetMarkerSize(1.5);
    hXSec[ih]->Draw("TEXT SAME");

    c1->Print(Form("DarkHiggs_hh2chi_mHs%d.pdf",mHs));
    c1->Print(Form("DarkHiggs_hh2chi_mHs%d.gif",mHs));
    
  }

}
