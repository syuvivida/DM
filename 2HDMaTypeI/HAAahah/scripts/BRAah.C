#include <TF1.h>
#include <TF2.h>
#include <TMath.h>

const Double_t mh=125;
const Double_t vev=246;
const int mA=230;
const int mH=500;
const int mHc=mH;
const int ma=100;
const int mchi=10;
const Double_t mtau=1.777;
const Double_t mb=4.7;
const Double_t mt=172;
const int tanbeta=2;
  

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

Double_t width_Aff(Double_t sint, Double_t mf, Double_t tb)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = mA>2*mf? 1.0/tb/tb/TMath::Pi()/8.0*mf*mf/vev/vev*mA*beta(mA,mf)*cost*cost : 0;
  return value;
}

Double_t width_Aah(Double_t sint, Double_t lam3)
{
  Double_t value = 1.0/16.0/TMath::Pi()*sqrt(lambda(mA,mh,ma))/mA*pow(gAah_value(sint,lam3),2);
  return value;
}


Double_t width_A(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t value = width_Aah(sint,lam3) + width_Achichi(sint) + width_Aff(sint,mtau,tb)+ width_Aff(sint,mb,tb)+width_Aff(sint,mt,tb);
  return value;
}



Double_t BRAtoah(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t w1 = width_Aah(sint,lam3);
  Double_t value = w1/width_A(sint,lam3, tb);
  return value;

}




Double_t varysintfunction(Double_t *x, Double_t *par)
{
   Double_t sint =x[0];
   Double_t lam3 =par[0];
   Double_t tb   =par[1];
   return BRAtoah(sint,lam3, tb);
 
}


Double_t varylam3function(Double_t *x, Double_t *par)
{
   Double_t lam3 =x[0];
   Double_t sint =par[0];
   Double_t tb   =par[1];
   return BRAtoah(sint,lam3, tb);
}


Double_t varysintfunction_width(Double_t *x, Double_t *par)
{
   Double_t sint =x[0];
   Double_t lam3 =par[0];
   Double_t tb   =par[1];
   return width_A(sint,lam3,tb)/mA;
 
}


Double_t varylam3function_width(Double_t *x, Double_t *par)
{
   Double_t lam3 =x[0];
   Double_t sint =par[0];
   Double_t tb   =par[1];
   return width_A(sint,lam3,tb)/mA;
}

Double_t BR2D_lam3_sint(Double_t* x, Double_t* par)
{
  Double_t lam3 =x[0];
  Double_t sint =x[1];
  Double_t tb   =par[0];
  return BRAtoah(sint,lam3,tb);
}

Double_t width2D_lam3_sint(Double_t* x, Double_t* par)
{
  Double_t lam3 =x[0];
  Double_t sint =x[1];
  Double_t tb   =par[0];
  return width_A(sint,lam3,tb)/mA;  
}

Double_t BR2D_lam3_tanbeta(Double_t* x, Double_t* par)
{
  Double_t lam3 =x[0];
  Double_t tb =x[1];
  Double_t sint  =par[0];
  return BRAtoah(sint,lam3,tb);
}

Double_t width2D_lam3_tanbeta(Double_t* x, Double_t* par)
{
  Double_t lam3 =x[0];
  Double_t tb =x[1];
  Double_t sint   =par[0];
  return width_A(sint,lam3,tb)/mA;  
}





void myfunc()
{
  gStyle->SetFuncWidth(8);
  gStyle->SetTitleSize(0.06, "XYZ");
  gStyle->SetTitleOffset(0.7, "XY");
  gStyle->SetLabelSize(0.05, "XYZ");
  gStyle->SetStripDecimals(kFALSE);
  const int lam3_1 = 2;
  const int lam3_2 = 8;
  const double sint_1 = 0.1;
  const double sint_2 = sqrt(2.0)/2.0;
  
  TCanvas* c1 = new TCanvas("c1");
  auto f1 = new TF1("f1",varysintfunction,0,sqrt(2)/2.0,2);
  f1->SetParameter(0,lam3_1);
  f1->SetParameter(1,tanbeta);
  f1->SetTitle(Form("BR(A#rightarrow ah), #lambda_{3}=%d",lam3_1));
  f1->GetXaxis()->SetTitle("sin#theta");
  f1->Draw();
  TLatex *lar = new TLatex(0.12, 0.17, Form("tan#beta=%d, M_{H}=%d GeV, M_{A}=%d GeV, M_{a}=%d GeV, M_{#chi}=%d GeV",
					    tanbeta,mH,mA,ma,mchi));
  lar->SetNDC(kTRUE);
  lar->SetTextSize(0.045);
  lar->Draw();
  c1->Print(Form("BRAah_lam3_%d_varysint_tanbeta%d_MH%d_MA%d.gif",lam3_1,tanbeta,mH,mA));

  auto f2 = new TF1("f2",varysintfunction_width,0,sqrt(2)/2.0,2);
  f2->SetParameter(0,lam3_1);
  f2->SetParameter(1,tanbeta);
  f2->SetTitle(Form("#Gamma_{A}/M_{A}, #lambda_{3}=%d",lam3_1));
  f2->GetXaxis()->SetTitle("sin#theta");
  f2->Draw();
  TLatex *lar2 = new TLatex(0.12, 0.8, Form("tan#beta=%d, M_{H}=%d GeV, M_{A}=%d GeV, M_{a}=%d GeV, M_{#chi}=%d GeV",
					    tanbeta,mH,mA,ma,mchi));
  lar2->SetNDC(kTRUE);
  lar2->SetTextSize(0.045);
  lar2->Draw();
  lar2->Draw();
  c1->Print(Form("FracWA_lam3_%d_varysint_tanbeta%d_MH%d_MA%d.gif",lam3_1,tanbeta,mH,mA));

  auto f3 = new TF1("f3",varysintfunction,0,sqrt(2)/2.0,2);
  f3->SetParameter(0,lam3_2);
  f3->SetParameter(1,tanbeta);
  f3->SetTitle(Form("BR(A#rightarrow ah), #lambda_{3}=%d",lam3_2));
  f3->GetXaxis()->SetTitle("sin#theta");
  f3->Draw();
  lar->Draw();
  c1->Print(Form("BRAah_lam3_%d_varysint_tanbeta%d_MH%d_MA%d.gif",lam3_2,tanbeta,mH,mA));

  auto f4 = new TF1("f4",varysintfunction_width,0,sqrt(2)/2.0,2);
  f4->SetParameter(0,lam3_2);
  f4->SetParameter(1,tanbeta);
  f4->SetTitle(Form("#Gamma_{A}/M_{A}, #lambda_{3}=%d",lam3_2));
  f4->GetXaxis()->SetTitle("sin#theta");
  f4->Draw();
  lar2->Draw();
  c1->Print(Form("FracWA_lam3_%d_varysint_tanbeta%d_MH%d_MA%d.gif",lam3_2,tanbeta,mH,mA));
  
  auto f5 = new TF1("f5",varylam3function,1,10,2);
  f5->SetParameter(0,sint_1);
  f5->SetParameter(1,tanbeta);
  f5->SetTitle(Form("BR(A#rightarrow ah), sin#theta=%.1f",sint_1));
  f5->GetXaxis()->SetTitle("#lambda_{3}");
  f5->Draw();
  lar->Draw();
  c1->Print(Form("BRAah_sin%.1f_varylam3_tanbeta%d_MH%d_MA%d.gif",sint_1,tanbeta,mH,mA));

  auto f6 = new TF1("f6",varylam3function_width,1,10,2);
  f6->SetParameter(0,sint_1);
  f6->SetParameter(1,tanbeta);
  f6->SetTitle(Form("#Gamma_{A}/M_{A}, sin#theta=%.1f",sint_1));
  f6->GetXaxis()->SetTitle("#lambda_{3}");
  f6->Draw();
  lar2->Draw();
  c1->Print(Form("FracWA_sin%.1f_varylam3_tanbeta%d_MH%d_MA%d.gif",sint_1,tanbeta,mH,mA));

  auto f7 = new TF1("f7",varylam3function,1,10,2);
  f7->SetParameter(0,sint_2);
  f7->SetParameter(1,tanbeta);
  f7->SetTitle("BR(A#rightarrow ah), sin#theta=#sqrt{2}/2");
  f7->GetXaxis()->SetTitle("#lambda_{3}");
  f7->Draw();
  lar->Draw();
  c1->Print(Form("BRAah_sin%.1f_varylam3_tanbeta%d_MH%d_MA%d.gif",sint_2,tanbeta,mH,mA));

  auto f8 = new TF1("f8",varylam3function_width,1,10,2);
  f8->SetParameter(0,sint_2);
  f8->SetParameter(1,tanbeta);
  f8->SetTitle("#Gamma_{A}/M_{A}, sin#theta=#sqrt{2}/2");
  f8->GetXaxis()->SetTitle("#lambda_{3}");
  f8->Draw();
  lar2->Draw();
  c1->Print(Form("FracWA_sin%.1f_varylam3_tanbeta%d_MH%d_MA%d.gif",sint_2,tanbeta,mH,mA));

  auto f9 = new TF2("f9",BR2D_lam3_sint,1,10,0,sqrt(2)/2.0,1);
  f9->SetParameter(0,tanbeta);
  f9->SetTitle(Form("BR(A#rightarrow ah), tan#beta=%d, M_{H}=%d GeV, M_{A}=%d GeV, M_{a}=%d GeV, M_{#chi}=%d GeV",
		    tanbeta,mH,mA,ma,mchi));
  f9->GetXaxis()->SetTitle("#lambda_{3}");
  f9->GetYaxis()->SetTitle("sin#theta");

  auto f10 = new TF2("f10",width2D_lam3_sint,1,10,0,sqrt(2)/2.0,1);
  f10->SetParameter(0,tanbeta);
  f10->SetTitle(Form("#Gamma_{A}/M_{A}, tan#beta=%d, M_{H}=%d GeV, M_{A}=%d GeV, M_{a}=%d GeV, M_{#chi}=%d GeV",
		     tanbeta,mH,mA,ma,mchi));
  f10->GetXaxis()->SetTitle("#lambda_{3}");
  f10->GetYaxis()->SetTitle("sin#theta");


  f9->GetXaxis()->SetTitleOffset(1.5);
  f9->GetYaxis()->SetTitleOffset(1.5);
  f9->Draw("surf1");
  c1->Print(Form("BRAah_surf1_tanbeta%d_MH%d_MA%d.gif",tanbeta,mH,mA));
  f10->GetXaxis()->SetTitleOffset(1.5);
  f10->GetYaxis()->SetTitleOffset(1.5);
  f10->Draw("surf1");
  c1->Print(Form("FracWA_surf1_tanbeta%d_MH%d_MA%d.gif",tanbeta,mH,mA));


  auto f11= new TF2("f11",BR2D_lam3_tanbeta,1,10,1,10,1);
  f11->SetParameter(0,sint_1);
  f11->SetTitle(Form("BR(A#rightarrow ah), sin#theta=%.1f, M_{H}=%d GeV, M_{A}=%d GeV, M_{a}=%d GeV, M_{#chi}=%d GeV",
		     sint_1,mH,mA,ma,mchi));
  f11->GetXaxis()->SetTitle("#lambda_{3}");
  f11->GetYaxis()->SetTitle("tan#beta");
  f11->GetXaxis()->SetTitleOffset(1.5);
  f11->GetYaxis()->SetTitleOffset(1.5);
  f11->Draw("surf1");
  c1->Print(Form("BRAah_surf1_sin%.1f_MH%d_MA%d.gif",sint_1,mH,mA));


  auto f12= new TF2("f11",width2D_lam3_tanbeta,1,10,1,10,1);
  f12->SetParameter(0,sint_1);
  f12->SetTitle(Form("#Gamma_{A}/M_{A}, sin#theta=%.1f, M_{H}=%d GeV, M_{A}=%d GeV, M_{a}=%d GeV, M_{#chi}=%d GeV",
		     sint_1,mH,mA,ma,mchi));
  f12->GetXaxis()->SetTitle("#lambda_{3}");
  f12->GetYaxis()->SetTitle("tan#beta");
  f12->GetXaxis()->SetTitleOffset(1.5);
  f12->GetYaxis()->SetTitleOffset(1.5);
  f12->Draw("surf1");
  c1->Print(Form("FracWA_surf1_sin%.1f_MH%d_MA%d.gif",sint_1,mH,mA));


  
  
}

