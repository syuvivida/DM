import sys; import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import scan2hdma as S
BENCH = dict(tanbeta=2., sinbma=0.9949874, lam3=2., laP1=0., laP2=0., sinp=0.3, mh2=260., mh3=610., mhc=610.,
             mh4=100., MXd=10., gPXd=1., MT=172., MB=4.7, mh1=125.)
lumi = S.Lumi()
r = S.point(lumi, **BENCH)
print("Gamma_A %.3f (card 84.339)  Gamma_H %.3f (card 11.224)  Gamma_a %.4f (card 3.5477)" % (r["GA"], r["GH"], r["Ga"]))
print("BR(A->Ha) %.4f (card 0.1845)  BR(H->hh) %.4f (card 0.8632)  BR(a->chichi) %.5f (card 0.99997)" % (r["brAHa"], r["brHhh"], r["brachi"]))
print("kappa_t(A) from model = %.4f ;  cos(theta)/tan(beta) = %.4f" % (r["kt"], (1-0.09)**0.5/2))
print("LO sigma(gg->A) formula = %.4f pb ; MadGraph LO = 0.26413 pb ; K_MG = %.3f" % (r["sigA"], 0.26413 / r["sigA"]))
