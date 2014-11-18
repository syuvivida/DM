#ifndef CUTVALUES_H
#define CUTVALUES_H

// jet kinematic cut

const double DUMMY = -99999.0;

const int LEPZ=0;
const int HADZ=1;

const double MIN_JETPT  =30.0;
const double MIN_JETBETA= 0.2;
const double MAX_JETETA = 2.4;

// b-tagging jet probability cuts
const double MIN_BTAG_JP_LOOSE  = 0.275; 
const double MIN_BTAG_JP_MEDIUM = 0.545;

// b-tagging track counting high efficiency
const double MIN_BTAG_TCH_LOOSE = 1.7; 
const double MIN_BTAG_TCH_MEDIUM= 3.3;

// b-tagging combined secondary vertex 
const double MIN_BTAG_CSV_LOOSE = 0.244;
const double MIN_BTAG_CSV_MEDIUM= 0.679;


// lepton kinematic cut
const double MIN_LEPPT1=40.0; 
const double MIN_LEPPT2=20.0; 
const double MIN_DR_JETLEP=0.5;

// mass cut on Z candidate
const double MIN_MZ_LL=76.0;
const double MAX_MZ_LL=106.0;

const double LOOSE_MIN_MZ_JJ=60.0;
const double LOOSE_MAX_MZ_JJ=130.0;
const double MORELOOSE_MAX_MZ_JJ=160.0;

const double MIN_MZ_JJ=71.0;
const double MAX_MZ_JJ=111.0;

const double MZ_PDG=91.1876;

const double MIN_MH_RATIO = 0.94;
const double MAX_MH_RATIO = 1.10;

// met cut
const double MAX_MET_SIG[3] = 
  {
    10.0,
    10.0,
    10.0
  };

// angular variables
const double MIN_HELD_CONSTANT[3]= 
  {
    0.500,
    0.500,
    0.500
  };
const double MIN_HELD_SLOPE[3]=
  {
    0,
    0,
    0
  };


//quark gluon discriminant

const double MIN_QUARK_GLUON_LD_0BTAG= 0.10;

enum {
  MZJJ_SIGNAL      = 0x1 << 1,   
  MZJJ_SIDEBAND    = 0x1 << 2,
  MH_SIGNAL        = 0x1 << 3,
  PFMET_SIG        = 0x1 << 4,
  HELI_LD          = 0x1 << 5,
  QG_LD            = 0x1 << 6,
  ALL_SIGNAL       = 0x1 << 7,
  ALL_SIDEBAND     = 0x1 << 8
};

#endif
