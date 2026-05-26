#include <TF1.h>
#include <TMath.h>

const Double_t mh=125;
const Double_t vev=246;
const Double_t mA=230;
const Double_t mH=500;
const Double_t mHc=mH;
const Double_t ma=100;
const Double_t mchi=10;
const Double_t mtau=1.777;
const Double_t mb=4.7;
const Double_t mt=172;
const Double_t tanbeta=2.0;
  

Double_t gAah_value(Double_t sint, Double_t lam3)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 1.0/mA/vev*(mh*mh-2*mH*mH-mA*mA+4*mHc*mHc-ma*ma-2*lam3*vev*vev)*sint*cost;
  return value;
}

Double_t beta(Double_t m1, Double_t m2)
{
  return m1<2*m2? 99999: sqrt(1-4*m2*m2/m1/m1);
}

Double_t lambda(Double_t m1, Double_t m2, Double_t m3)
{
  return pow(m1*m1-m2*m2-m3*m3,2)-4*m2*m2*m3*m3;
}


Double_t width_Achichi(Double_t sint)
{

  Double_t value = 1.0/TMath::Pi()/8.0*mA*beta(mA,mchi)*sint*sint;
  return value;
}

Double_t width_Aff(Double_t sint, Double_t mf)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = mA>2*mf? 1.0/tanbeta/tanbeta/TMath::Pi()/8.0*mf*mf/vev/vev*mA*beta(mA,mf)*cost*cost : 0;
  return value;
}

Double_t width_Aah(Double_t sint, Double_t lam3)
{
  Double_t value = 1.0/16.0/TMath::Pi()*sqrt(lambda(mA,mh,ma))/mA*pow(gAah_value(sint,lam3),2);
  return value;
}


Double_t width_A(Double_t sint, Double_t lam3)
{
  Double_t value = width_Aah(sint,lam3) + width_Achichi(sint) + width_Aff(sint,mtau)+ width_Aff(sint,mb)+width_Aff(sint,mt);
  return value;
}



Double_t BRAtoah(Double_t sint, Double_t lam3)
{
  Double_t w1 = width_Aah(sint,lam3);
  Double_t value = w1/width_A(sint,lam3);
  return value;

}




Double_t varysintfunction(Double_t *x, Double_t *par)
{
   Double_t sint =x[0];
   Double_t lam3 =par[0];
   return BRAtoah(sint,lam3);
 
}


Double_t varylam3function(Double_t *x, Double_t *par)
{
   Double_t lam3 =x[0];
   Double_t sint =par[0];
   return BRAtoah(sint,lam3);
}


Double_t varysintfunction_width(Double_t *x, Double_t *par)
{
   Double_t sint =x[0];
   Double_t lam3 =par[0];
   return width_A(sint,lam3)/mA;
 
}


Double_t varylam3function_width(Double_t *x, Double_t *par)
{
   Double_t lam3 =x[0];
   Double_t sint =par[0];
   return width_A(sint,lam3)/mA;
}



void myfunc()
{
  gStyle->SetFuncWidth(8);
  gStyle->SetTitleSize(0.06, "XYZ");
  gStyle->SetTitleOffset(0.7, "X");
  TCanvas* c1 = new TCanvas("c1");
  auto f1 = new TF1("f1",varysintfunction,0,sqrt(2)/2.0,1);
  f1->SetParameter(0,2);
  f1->SetTitle("BR(A#rightarrow ah), #lambda_{3}=2");
  f1->GetXaxis()->SetTitle("sin#theta");
  f1->Draw();
  c1->Print("BRAah_lam3_2_varysint.gif");

  auto f2 = new TF1("f2",varysintfunction_width,0,sqrt(2)/2.0,1);
  f2->SetParameter(0,2);
  f2->SetTitle("#Gamma_{A}/M_{A}, #lambda_{3}=2");
  f2->GetXaxis()->SetTitle("sin#theta");
  f2->Draw();
  c1->Print("FracWA_lam3_2_varysint.gif");

  auto f3 = new TF1("f3",varysintfunction,0,sqrt(2)/2.0,1);
  f3->SetParameter(0,8);
  f3->SetTitle("BR(A#rightarrow ah), #lambda_{3}=8");
  f3->GetXaxis()->SetTitle("sin#theta");
  f3->Draw();
  c1->Print("BRAah_lam3_8_varysint.gif");

  auto f4 = new TF1("f4",varysintfunction_width,0,sqrt(2)/2.0,1);
  f4->SetParameter(0,8);
  f4->SetTitle("#Gamma_{A}/M_{A}, #lambda_{3}=8");
  f4->GetXaxis()->SetTitle("sin#theta");
  f4->Draw();
  c1->Print("FracWA_lam3_2_varysint.gif");
  
  auto f5 = new TF1("f5",varylam3function,1,10,1);
  f5->SetParameter(0,0.1);
  f5->SetTitle("BR(A#rightarrow ah), sin#theta=0.1");
  f5->GetXaxis()->SetTitle("#lambda_{3}");
  f5->Draw();
  c1->Print("BRAah_lam3_2_varylam3.gif");

  auto f6 = new TF1("f6",varylam3function_width,1,10,1);
  f6->SetParameter(0,0.1);
  f6->SetTitle("#Gamma_{A}/M_{A}, sin#theta=0.1");
  f6->GetXaxis()->SetTitle("#lambda_{3}");
  f6->Draw();
  c1->Print("FracWA_lam3_2_varylam3.gif");

  auto f7 = new TF1("f7",varylam3function,1,10,1);
  f7->SetParameter(0,sqrt(2)/2.0);
  f7->SetTitle("BR(A#rightarrow ah), sin#theta=#sqrt{2}/2");
  f7->GetXaxis()->SetTitle("#lambda_{3}");
  f7->Draw();
  c1->Print("BRAah_lam3_8_varylam3.gif");

  auto f8 = new TF1("f8",varylam3function_width,1,10,1);
  f8->SetParameter(0,sqrt(2)/2.0);
  f8->SetTitle("#Gamma_{A}/M_{A}, sin#theta=#sqrt{2}/2");
  f8->GetXaxis()->SetTitle("#lambda_{3}");
  f8->Draw();
  c1->Print("FracWA_lam3_2_varylam3.gif");
  
}

