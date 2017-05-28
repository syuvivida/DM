#include <TMath.h>
void Zp2HDM_zhah(const float mzp, const float ma0, const float tb=1)
{
  const float mh=125;
  const float ymt=172;
  const float ymb=4.7;
  const float gz=0.8;

  const float MZ=91.1876;
  
  const float cb= 1./sqrt(1 + tb*tb);
  const float sb = tb/sqrt(1 + tb*tb);
  cout << "cb = " << cb << " sb = " << sb << endl;
  const float sa= -(1./sqrt(1 + tb*tb));
  const float ca=sqrt(1-sa*sa);

  float pt= sqrt( (mzp*mzp-(mh+ma0)*(mh+ma0))*
		  (mzp*mzp-(ma0-mh)*(ma0-mh)))*0.5/mzp;

  float width_ah = pow(gz*ca*cb,2.)*pow(pt,3.)/mzp/mzp/24.0/TMath::Pi();

  cout << "width of Z'->A0 h = " << width_ah << " GeV" << endl;

  float width_zh = pow(gz*ca*sb,2.)*pt/24.0/TMath::Pi();
  width_zh *= (pt*pt/mzp/mzp+3.0*MZ*MZ/mzp/mzp);

  cout << "width of Z'->Z h = " << width_zh << " GeV" << endl;

  cout << "Ratio of Z'->A0 h to Z'->Zh = " << width_ah/width_zh << endl;
  
}
