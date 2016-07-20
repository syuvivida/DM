
#include <TMath.h>
#include <TF1.h>
#include <TGraph.h>
#include "setNCUStyle.C"
#include <TLegend.h>


double x[8]={600,800,1000,1200,1400,1700,2000,2500};
double y_atlas[8]={0.45217,
		   0.27765,
		   0.14383,
		   0.075451,
		   0.041208,
		   0.017786,
		   0.0082317,
                   0.0025458};

double y_ours[8]={0.061433,
		  0.067725,
		  0.054942,
		  0.041651,
		  0.030986,
		  0.01981,
		  0.0128,
		  0.0063925
};


double fitf(double *x,double *par) {
  
   double arg = x[0];
   const double gz_forum = 0.8;
   const double aEW = 1.0/127.9;
   const double ee = 2*sqrt(aEW)*sqrt(TMath::Pi());
   const double MZ = 91.1876;
   const double Gf = 0.0000116637;
   const double MW = sqrt(pow(MZ,2)/2. + sqrt(pow(MZ,4)/4. - (aEW*TMath::Pi()*pow(MZ,2))/(Gf*sqrt(2))));
   const double sw = sqrt(1-MW*MW/MZ/MZ);
   const double gw = ee/sw;
   const double cw = sqrt(1-sw*sw);
   const double Tb = par[0];
   const double Sb = Tb/sqrt(1+Tb*Tb);
   Double_t fitval = 0.03*(gw/(cw*pow(Sb,2)))*sqrt(pow(arg,2) - pow(MZ,2))/MZ;
   return fitval;
 }

double fitfr(double *x,double *par) {

  return 0.8*0.8/(fitf(x,par)*fitf(x,par));
}

void gz()
{
  setNCUStyle(true);
  TCanvas* c1 = new TCanvas("c1","",500,500);

  TF1* f1 = new TF1("f1",fitf,600,3000,1);
  f1->SetParameter(0,1);
  f1->SetNpx(2500);
  f1->GetXaxis()->SetTitle("M_{Zp} [GeV]");
  f1->GetYaxis()->SetTitle("gz_{formula}");
  f1->Draw();

  for(int i=0; i<8; i++)
    {
      cout << "Mass= " << x[i] << " GeV: " << f1->Eval(x[i]) << endl;
    }


  TF1* f2 = new TF1("f2","[0]",600,3000);
  f2->SetParameter(0,0.8);
  f2->SetNpx(2500);
  f2->SetLineColor(4);
  f2->Draw("same");

  c1->Print("gz.pdf");
  

  TF1* f3 = new TF1("f3",fitfr,600,3000,1);
  f3->SetParameter(0,1);
  f3->SetNpx(2500);
  f3->GetXaxis()->SetTitle("M_{Zp} [GeV]");
  f3->GetYaxis()->SetTitle("0.8*0.8/gz_{formula}^{2}");

  TF1* f4 = new TF1("f4","[0]",600,3000);
  f4->SetParameter(0,1);
  f4->SetNpx(2500);
  f4->SetLineColor(4);
  f4->GetXaxis()->SetTitle("M_{Zp} [GeV]");
  f4->GetYaxis()->SetTitle("0.8*0.8/gz_{formula}^2");

  f3->Draw();
  f4->Draw("same");
  
  cout << "Ratio =1 when x = " << f3->GetX(1) << endl;
  c1->Print("gz2.pdf");


  double ratio[8];
  for(int i=0; i<8; i++)
    {
      ratio[i]=y_atlas[i]/y_ours[i];
      cout << "Mass= " << x[i] << " GeV: " << f3->Eval(x[i]) << ", " << ratio[i] << endl;
    }
  TGraph* xsec = new TGraph(8,x,ratio);
  xsec->SetName("xsec");
  xsec->GetXaxis()->SetTitle("M_{Zp} [GeV]");
  xsec->GetYaxis()->SetTitle("Ratio");
  xsec->Draw("ACP");
  f3->Draw("same");


  TLegend* leg = new TLegend(0.3,0.763,0.931,0.912);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->AddEntry("f3","Ratio of gz^{2}","l");
  leg->AddEntry("xsec","Ratio of cross sections","p");
  leg->Draw("same");
  

  c1->Print("gz3.pdf");
}
