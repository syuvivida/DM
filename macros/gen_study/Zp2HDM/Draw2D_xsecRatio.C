#include "setNCUStyle.C"
#include <TH2.h>
#include <TCanvas.h>
#include <fstream>
#include <string>
#include <TText.h>
#include <TLatex.h>

using namespace std;

void Draw2D_xsecRatio(string inputFile)
{
  setNCUStyle();

  const int nzp=15;
  double dzpc[nzp]={300,400,500,600,800,1000,1200,1400,1700,2000,2500,2750,3000,3500,4000};
  int zpc[nzp]={300,400,500,600,800,1000,1200,1400,1700,2000,2500,2750,3000,3500,4000};
  string szpc[nzp];
  for(int i=0;i<nzp;i++)szpc[i]=std::to_string(zpc[i]);
  TH1F* h1dzp = new TH1F("h1dzp","",nzp-1,dzpc);

  const int na0=7;
  double da0c[na0]={300,400,500,600,700,800,1000};
  int a0c[na0]={300,400,500,600,700,800,1000};
  string sa0c[nzp];
  for(int i=0;i<na0;i++)sa0c[i]=std::to_string(a0c[i]);
  TH1F* h1da0 = new TH1F("h1da0","",na0-1,da0c);
 
  TH2F *h = new TH2F("h","Mono-h Z'-2HDM Production Cross Section Ratio (320900/263400)",nzp,0,nzp,na0,0,na0);
  h->SetXTitle("MZp [GeV/c^{2}]");
  h->SetYTitle("MA_{0} [GeV/c^{2}]");

  ifstream fin;
  fin.open(inputFile.data());
  double tempx, tempy, value, valueerr;
  fin >> tempx >> tempy >> value >> valueerr;
  while(!fin.eof())
    {
      int binx = h1dzp->FindBin(tempx);
      int biny = h1da0->FindBin(tempy);
      h->SetBinContent(binx,biny,value);
      h->SetBinError(binx,biny,valueerr);
      fin >> tempx >> tempy >> value >> valueerr;
    }


  TCanvas *c1 = new TCanvas("c1","demo bin labels",10,10,1600,800);
  c1->SetGrid();
  c1->SetLeftMargin(0.15);
  c1->SetRightMargin(0.12);
  c1->SetBottomMargin(0.15);
  gStyle->SetPaintTextFormat("4.4f");
  gStyle->SetOptStat(0);

  //  h->SetStats(0);
  h->GetXaxis()->SetLabelOffset(90);
  h->GetYaxis()->SetLabelOffset(90);
  h->SetTitle("Mono-h Z'-2HDM Production Cross Section Ratio (320900/263400)");
  h->Draw("etextcolz");
  // draw labels along X
  Float_t x, y;
  y = gPad->GetUymin() - 0.2*h->GetYaxis()->GetBinWidth(1);
  TLatex t;
  //  t.SetTextAngle(30);
  t.SetTextSize(0.03);
  t.SetTextAlign(23);
  t.SetTextAngle(0);
  for (int i=0;i<nzp;i++) {
    x = h->GetXaxis()->GetBinCenter(i+1);
    t.DrawLatex(x,y,szpc[i].data());
  }
  // draw labels along y
  x = gPad->GetUxmin() - 0.1*h->GetXaxis()->GetBinWidth(1);
  t.SetTextAlign(32);
  t.SetTextAngle(0);
  for (int i=0;i<na0;i++) {
    y = h->GetYaxis()->GetBinCenter(i+1);
    t.DrawLatex(x,y,sa0c[i].data());
  }
  c1->Print("Zp2HDM_NNLOvsLOPDF_xsec.pdf");
  c1->Print("Zp2HDM_NNLOvsLOPDF_xsec.eps");

}
