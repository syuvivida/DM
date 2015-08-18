// quick macro to compute pt of Higgs for a X->Yh decay given X mass and Y mass

void higgsPt(float mzp, float m2=300)
{
  const float m1=125;

  float pt= sqrt( (mzp*mzp-(m1+m2)*(m1+m2))*
		  (mzp*mzp-(m1-m2)*(m1-m2)))*0.5/mzp;

  cout << "higgs pt = " << pt << " GeV " << endl;
  cout << "minimum deltaR between two b from Higgs is " <<
    2*m1/TMath::Max(0.1,pt) << endl;
  return;

}
