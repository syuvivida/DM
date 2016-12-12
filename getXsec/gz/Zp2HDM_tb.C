void Zp2HDM_tb(const float mzp, const float ma0, const float tb1, const float tb2, const float xs1, const float xs2)
{

  const float ymt=172;

  if(ma0<ymt*2){
    cout << "This program can only be run for ma0 > " << ymt*2 << " GeV !! " << endl;
    return;
  }

  float ratio1 = ((tb1*tb1)/pow(1+tb1*tb1,2))/((tb2*tb2)/pow(1+tb2*tb2,2));
  cout << "ratio of Gamma(Z'->A0h) = " << ratio1 << endl;

  const float gdm=1;
  const float mdm=100;
  float gamma_a0 = (gdm*gdm*pow(ma0,2)*sqrt(pow(ma0,4)-4*pow(ma0*mdm,2)))/8.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> chi chi = " << gamma_a0 << " GeV" << endl;

  const double aEW = 1.0/127.9;
  const double ee = 2*sqrt(aEW)*sqrt(TMath::Pi());
  const double MZ = 91.1876;
  const double Gf = 0.0000116637;
  const double MW = sqrt(pow(MZ,2)/2. + sqrt(pow(MZ,4)/4. - (aEW*TMath::Pi()*pow(MZ,2))/(Gf*sqrt(2))));
  const double sw = sqrt(1-MW*MW/MZ/MZ);
  const double gw = ee/sw;
  const double cw = sqrt(1-sw*sw);


  const float vu=(2*MW*sw)/ee;
  cout << "vu = " << vu << endl;
  float yt=sqrt(2)*ymt/vu;
  float gamma_a01 = (3./pow(tb1,2)*pow(ma0*yt,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymt,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> tt for tanbeta = " << tb1 << " is " << gamma_a01 << " GeV" << endl;
  cout << "BR(A0->chi chi) for tanbeta = " << tb1 << " is " << gamma_a0/(gamma_a01+gamma_a0) << endl;

  float gamma_a02 = (3./pow(tb2,2)*pow(ma0*yt,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymt,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> tt for tanbeta = " << tb2 << " is " << gamma_a02 << " GeV" << endl;
  cout << "BR(A0->chi chi) for tanbeta = " << tb2 << " is " << gamma_a0/(gamma_a02+gamma_a0) << endl;

  float ratio2 = (gamma_a02+gamma_a0)/(gamma_a01+gamma_a0);

  cout << "ratio of BR(A0->chi chi) = " << ratio2 << endl;

  float dr = xs1/xs2/ratio1/ratio2;
  cout << "ratio of Madgraph estimation divided by Eiko's prediction = " << dr << endl;
}
