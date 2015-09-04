#include <string>
#include <iostream>
#include <TPad.h>
#include <TH1F.h>
#include <TFile.h>
#include <TLine.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <THStack.h>
#include <TLegend.h>

using namespace std;

const float dummy=-9999;

void myPlot(vector<TH1F*>,
	    vector<string>,
	    float ymin=dummy, float ymax=dummy
	    ); // first one is always data histogram
void myRatio(vector<TH1F*>,
	     string xtitle="test"); // first one is always data histogram

// first file is always data
void stackHisto(string histoName, vector<string> inputFiles,
		vector<string> legends,
		string xtitle="test", 
		float ymin=dummy, float ymax=dummy,
		bool logY=false){
  
  const unsigned int nfiles=inputFiles.size();
  TFile *file[nfiles];
  vector<TH1F*> myHistos;
  for(unsigned int i=0;i<nfiles;i++)
    {
      file[i]= TFile::Open(inputFiles[i].data());
      myHistos.push_back((TH1F*)(file[i]->Get(histoName.data())));
    }

  
  gStyle->SetOptStat(0);
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetPadGridX(kTRUE);

  Double_t up_height     = 0.8;
  Double_t dw_correction = 1.35;
  Double_t dw_height     = (1-up_height)*dw_correction;

  TCanvas c("c","",0,0,1000,800);
  c.Divide(1,2);

  TPad* c_up = (TPad*) c.GetListOfPrimitives()->FindObject("c_1");
  TPad* c_dw = (TPad*) c.GetListOfPrimitives()->FindObject("c_2"); 

  c_up->SetPad(0,1-up_height,1,1);
  c_dw->SetPad(0,0,1,dw_height);
  c_dw->SetBottomMargin(0.25);
 
  

  c_up->cd()->SetLogy((int)logY);
  
  myPlot(myHistos,legends,ymin,ymax);

  c_up->RedrawAxis();
    
  c_dw->cd();
  myRatio(myHistos, xtitle);

  c.Draw();
    
  string outputfilename;
  outputfilename = histoName + ".pdf";
  c.Print(outputfilename.data());
  outputfilename = histoName + ".png";
  c.Print(outputfilename.data());
  outputfilename = histoName + ".eps";
  c.Print(outputfilename.data());


}


void myPlot(vector<TH1F*> histos,
	    vector<string> legends,
	    float ymin, float ymax){

  histos[0]->SetMarkerColor(1);
  histos[0]->SetLineColor(1);
  histos[0]->SetMarkerStyle(8);
  histos[0]->SetMarkerSize(1);

  THStack *hstack = new THStack("hstack", "");
  
  for(unsigned int i=1; i < histos.size(); i++){
    int colorcode=kBlue+2-i*2;
    histos[i]->SetFillColor(colorcode);
    histos[i]->SetFillStyle(1001);
    histos[i]->SetLineColor(colorcode);
    hstack->Add(histos[i]);    
  }

   
  histos[0]->GetXaxis()->SetTitle("");
  histos[0]->GetXaxis()->SetLabelOffset(999);
  histos[0]->GetXaxis()->SetLabelSize(0);
  if(ymin>dummy)
    {
      histos[0]->SetMinimum(ymin);
      hstack->SetMinimum(ymin);
    }
  if(ymax>dummy)
    {
      histos[0]->SetMaximum(ymax);
      hstack->SetMaximum(ymax);
    }
  hstack->Draw("histe");
  hstack->GetHistogram()->GetYaxis()->SetTitle("Event Numbers");
  hstack->GetHistogram()->GetXaxis()->SetTickLength(0);
  hstack->GetHistogram()->GetXaxis()->SetLabelOffset(999);
  histos[0]->Draw("e1same");

  TLegend *leg = new TLegend(0.5, 0.8, 0.9, 0.9);

  leg->SetFillStyle(0);
  leg->SetBorderSize(1);
  for(unsigned int i=0; i< histos.size(); i++)
    {
      leg->AddEntry(histos[i],legends[i].data(),"lpf");
    }
  leg->Draw();

}


void myRatio(vector<TH1F*> histos,
	     string xtitle){
  
  TH1F* h_data = (TH1F*)histos[0]->Clone("h_data");

  TH1F *h_bkg = (TH1F*)h_data->Clone("h_bkg");
  h_bkg->Reset();
  h_bkg->Sumw2();
  for(unsigned int i=1;i<histos.size();i++)
    h_bkg->Add(histos[i]);

  TH1F* h_ratio = (TH1F*)h_data->Clone("h_ratio");
  h_ratio->Reset();
  h_ratio->Sumw2();

  Int_t nbin = h_ratio->GetNbinsX();
  Double_t ratio[nbin];
  Double_t error[nbin];
  Double_t numer_nbincontent[nbin];
  Double_t denom_nbincontent[nbin];
  Double_t numer_binerror[nbin];
  Double_t denom_binerror[nbin];
 
  for(Int_t i=1; i<=nbin; i++){

    numer_nbincontent[i] = h_data->GetBinContent(i);
    denom_nbincontent[i] = h_bkg->GetBinContent(i);
    numer_binerror[i] = h_data->GetBinError(i);
    denom_binerror[i] = h_bkg->GetBinError(i);

    if( denom_nbincontent[i] <= 0 || numer_nbincontent[i] <= 0 ) continue; 
    if( denom_binerror[i] <= 0 || numer_binerror[i] <= 0 ) continue;

    ratio[i] = (Double_t)numer_nbincontent[i]/denom_nbincontent[i];
    error[i] = (ratio[i])*sqrt(pow(numer_binerror[i]/numer_nbincontent[i],2)+pow(denom_binerror[i]/denom_nbincontent[i],2));

    h_ratio->SetBinContent(i,ratio[i]);
    h_ratio->SetBinError(i,error[i]);

  }
  
  Double_t font_size_dw = 0.1; 

  h_ratio->SetMarkerStyle(8);
  h_ratio->SetMarkerSize(0.6);
  h_ratio->SetTitle("");
  h_ratio->GetYaxis()->SetTitle("data/MC");
  h_ratio->GetYaxis()->SetTitleOffset(0.3);
  h_ratio->GetXaxis()->SetLabelSize(font_size_dw);
  h_ratio->GetXaxis()->SetTitleSize(font_size_dw);
  h_ratio->GetYaxis()->SetLabelSize(font_size_dw);
  h_ratio->GetYaxis()->SetTitleSize(font_size_dw);
  h_ratio->GetYaxis()->SetNdivisions(505);
  h_ratio->GetYaxis()->SetRangeUser(0,2);
  if(xtitle!="test")
    h_ratio->SetXTitle(xtitle.data());


  h_ratio->Draw();

  Double_t x0 = h_bkg->GetXaxis()->GetXmin();
  Double_t x1 = h_bkg->GetXaxis()->GetXmax();
  Double_t y0 = 1.; 
  Double_t y1 = 1.;
 
  TLine* one = new TLine(x0,y0,x1,y1);
  one->SetLineColor(2);
  one->SetLineStyle(1);
  one->SetLineWidth(2);
  one->Draw("same");

  h_ratio->Draw("same");

}
