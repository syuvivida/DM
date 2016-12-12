void Zp2HDM_gz(const float gz1, const float gz2, const float xs1, const float xs2)
{
  float ratio = gz1*gz1/gz2/gz2;
  cout << "cross section ratio based on gz = " << ratio << endl;
  float ratio2 = xs1/xs2;

  float dr = ratio2/ratio;
  cout << "ratio of Madgraph estimation divided by Eiko's prediction = " << dr << endl;
}
