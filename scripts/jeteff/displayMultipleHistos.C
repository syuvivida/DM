#include <TH1.h>
#include <TCanvas.h>
#include <string>
#include <vector>
#include <iostream>
#include <TFile.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TSystem.h>

using namespace std;

void displayMultipleHistos(vector<string> files,
			   vector<string> legs,
			   vector<string> histos,
			   string title="",
			   string title2="test",
			   float xmin=-9999, float xmax=-9999, 
			   bool rebin=true,
			   float ymin=0, float ymax=1.7)
{
  const unsigned int nfiles = files.size();
  TH1F* h[nfiles];
  TCanvas* c1 = new TCanvas("c1","",500,500);
  float max=-999;
  gStyle->SetOptStat(0);
  gStyle->SetTitleFontSize(0.07);
  int color[4]={kRed, kMagenta, kBlue, kGreen+2};
  vector<double> integrals; 
  for(unsigned i=0; i<nfiles; i++)
    {
      TFile *f = TFile::Open(files[i].data());
      cout << "Opening " << files[i].data() << endl;
      h[i] = dynamic_cast<TH1F*>(gDirectory->Get(histos[i].data()));
      h[i]->Sumw2();
      h[i]->SetName(Form("h%d",i));
      if(rebin)h[i]->Rebin(4);
      h[i]->SetTitle("");
      if(title2!="test")
	h[i]->SetXTitle(title2.data());
      integrals.push_back(h[i]->Integral(0,h[i]->GetNbinsX()+1));
      h[i]->Scale(1.0/integrals[i]);
      h[i]->SetMinimum(ymin); 
      if(xmax>xmin)
	h[i]->GetXaxis()->SetRangeUser(xmin,xmax);
      h[i]->SetMarkerStyle(20+i);
      h[i]->SetMarkerColor(color[i%4]);
      h[i]->SetLineColor(color[i%4]);
      h[i]->SetLineStyle(i+1);
      h[i]->GetXaxis()->SetTitleOffset(1.2);
      h[i]->GetYaxis()->SetTitleOffset(1.4);

      if(h[i]->GetMaximum()>max)
	max=h[i]->GetMaximum();
    }

  for(unsigned int i=0; i<nfiles; i++)
    {
      h[i]->SetMaximum(ymax*max);
      if(i==0)
	  h[i]->Draw("histe");

      else
	h[i]->Draw("histesame");
    }


  TLegend* leg = new TLegend(0.181452,0.662447,0.330645,0.883966) ;

  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetHeader(title.data());
  leg->AddEntry((TObject*)0, "","");
  for(unsigned i=0; i < nfiles; i++)
    {
      leg->AddEntry(h[i],legs[i].data(),"lpf");
    }


  leg->Draw("same");

  TString dirname ="compareHistos";
  gSystem->mkdir(dirname.Data());
  TString tempout = histos[0];
  cout << tempout << endl;
  TString prefix=dirname + "/" + tempout;
  TString temp= prefix +  ".eps";
  c1->Print(temp.Data());
  temp= prefix + ".gif";
  c1->Print(temp.Data());
  temp= prefix + ".pdf";
  c1->Print(temp.Data());

}
