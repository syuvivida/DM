#include <TF1.h>

const Double_t mh=125;
const Double_t lam3=6.0;
const Double_t vev=246;
const Double_t sint=0.2;
const Double_t cost=sqrt(1-sint*sint);

Double_t gAah_value(Double_t mA, Double_t mH, Double_t mHc, Double_t ma)
{
  Double_t value = mA<1e-6? 0:  1.0/mA/vev*(
					    mh*mh-2*mH*mH-mA*mA+4*mHc*mHc-ma*ma-2*lam3*vev*vev)*sint*cost;
  return value*value;
}


Double_t varymAfunction(Double_t *x, Double_t *par)
{
   Double_t mA =x[0];
   Double_t mH =par[0];
   Double_t mHc=par[1];
   Double_t ma =par[2];
   return gAah_value(mA,mH,mHc,ma);
}

Double_t varymHfunction(Double_t *x, Double_t *par)
{
   Double_t mH =x[0];
   Double_t mA =par[0];
   Double_t mHc=par[1];
   Double_t ma =par[2];
   return gAah_value(mA,mH,mHc,ma);
}

Double_t varymHcfunction(Double_t *x, Double_t *par)
{
   Double_t mHc =x[0];
   Double_t mA=par[0];
   Double_t mH =par[1];
   Double_t ma =par[2];
   return gAah_value(mA,mH,mHc,ma);
}


Double_t varymafunction(Double_t *x, Double_t *par)
{
   Double_t ma =x[0];
   Double_t mA =par[0];
   Double_t mH =par[1];
   Double_t mHc =par[2];
   return gAah_value(mA,mH,mHc,ma);
}

Double_t varybothmHfunction(Double_t *x, Double_t *par)
{
   Double_t mH =x[0];
   Double_t mA =par[0];
   Double_t ma =par[1];
   Double_t mdiff=par[2];
   Double_t mHc=mH-mdiff;
   return gAah_value(mA,mH,mHc,ma);
}



void myfunc()
{
   auto f1 = new TF1("f1",varybothmHfunction,1,800,3);
   f1->SetParameters(230,100,0);
   f1->SetParNames("mA","ma");
   f1->Draw();
}
