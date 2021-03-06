#include "setNCUStyle.C"
#include <TH2.h>
#include <TCanvas.h>
#include <fstream>
#include <TText.h>
#include <TLatex.h>

using namespace std;

void Draw2D_xsec_sinp(string inputFile)
{
  setNCUStyle();
  const Int_t nx = 7;
  const Int_t ny = 5;

  char* Ma[nx]={"50","100","200","300","400","490","600"};
  char* sinp[ny]={"0.1","0.3","0.5","#sqrt{2}/2","#sqrt{3}/2"};
  TH2F *h = new TH2F("h","pp#rightarrow h #chi#chi Production Cross Section (pb)",nx,0,nx,ny,0,ny);
  ifstream fin;
  fin.open(inputFile.data());

  for(int iy=0; iy<ny; iy++){
    for(int ix=0; ix<nx; ix++){
      double tempy, tempx, value;
      fin >> tempy >> tempx >> value;
      h->SetBinContent(ix+1,iy+1,value);

    }
  }

  TCanvas *c1 = new TCanvas("c1","demo bin labels",10,10,800,800);
  c1->SetGrid();
  c1->SetLeftMargin(0.15);
  c1->SetRightMargin(0.12);
  c1->SetBottomMargin(0.15);
  gStyle->SetPaintTextFormat("4.4f");


  h->SetStats(0);
  h->GetXaxis()->SetLabelOffset(90);
  h->GetXaxis()->SetTitle("M_{a} [GeV]");
  h->GetYaxis()->SetLabelOffset(90);
  h->GetYaxis()->SetTitle("sin#theta");
  h->Draw("textcolz");
  // draw labels along X
  Float_t x, y;
  y = gPad->GetUymin() - 0.2*h->GetYaxis()->GetBinWidth(1);
  TLatex t;
  //  t.SetTextAngle(30);
  t.SetTextSize(0.03);
  t.SetTextAlign(23);
  t.SetTextAngle(0);
  for (int i=0;i<nx;i++) {
    x = h->GetXaxis()->GetBinCenter(i+1);
    t.DrawLatex(x,y,Ma[i]);
  }
  // draw labels along y
  x = gPad->GetUxmin() - 0.1*h->GetXaxis()->GetBinWidth(1);
  t.SetTextAlign(32);
  t.SetTextAngle(0);
  for (int i=0;i<ny;i++) {
    y = h->GetYaxis()->GetBinCenter(i+1);
    t.DrawLatex(x,y,sinp[i]);
  }

  c1->Print("2HDMPZ2_xsec_varysinp.pdf");

}
