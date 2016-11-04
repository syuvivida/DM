#!/usr/bin/env python
from ROOT import TFile, TTree, TH1F, TH1D, TH1, TCanvas, TChain,TGraphAsymmErrors, TMath, TH2D, TLorentzVector
import ROOT as ROOT
import os
import sys, optparse
from array import array
import math

ROOT.gROOT.SetBatch(True)
from MonoHbbQuantities import *
from PileUpWeights import PUWeight

ROOT.gROOT.ProcessLine('.L BTagCalibrationStandalone.cpp+') 

######################################
## set up running mode of the code.
######################################

#ROOT.gROOT.ProcessLine('.L PileUpWeights.h')

#print "puweight = ",PUWEIGHT(10)
usage = "usage: %prog [options] arg1 arg2"
parser = optparse.OptionParser(usage)

## data will be true if -d is passed and will be false if -m is passed
parser.add_option("-i", "--inputfile",  dest="inputfile")
parser.add_option("-o", "--outputfile", dest="outputfile")
parser.add_option("-D", "--outputdir", dest="outputdir")

parser.add_option("-a", "--analyze", action="store_true",  dest="analyze")

parser.add_option("-e", "--efficiency", action="store_true",  dest="efficiency")
parser.add_option("-F", "--farmout", action="store_true",  dest="farmout")
parser.add_option("-t", "--table", action="store_true",  dest="table")
parser.add_option("-P", "--OtherPlots", action="store_true",  dest="OtherPlots")

########################################################################################################################
########################## cut values########################################################################
########################################################################################################################

parser.add_option("-m", "--MLow", type=float,  dest="MLow")
parser.add_option("-M", "--MHigh", type=float,  dest="MHigh")

parser.add_option( "--MLow1", type=float,  dest="MLow1")
parser.add_option( "--MHigh1", type=float,  dest="MHigh1")

parser.add_option("-l", "--lepton", type=int, dest="lepton")
parser.add_option("-L", "--Lepton", type=int, dest="Lepton")

parser.add_option("-b", "--bjet", type=int, dest="bjet")

parser.add_option("-j", "--jet", type=int, dest="jet")
parser.add_option("-J", "--Jet", type=int, dest="Jet")

(options, args) = parser.parse_args()

massCutLow   = options.MLow 
massCutHigh  = options.MHigh

massCutLow1   = options.MLow1
massCutHigh1  = options.MHigh1

nlepton      = options.lepton
nLepton      = options.Lepton

njet        = options.jet
nJet        = options.Jet

nbjet       = options.bjet


isfarmout = options.farmout 
print (massCutLow,massCutHigh,nlepton,nLepton,njet,nJet, options.inputfile, options.outputfile )
#print 'options = ',[options.inputfile]
inputfilename = options.inputfile

#print inputfilename
pathlist = inputfilename.split("/")
sizeoflist = len(pathlist)
#print ('sizeoflist = ',sizeoflist)
rootfile='tmphist'
rootfile = pathlist[sizeoflist-1]
textfile = rootfile+"_" + options.outputdir+".txt"

#outputdir='MonoHSamples/'
#os.system('mkdir '+outputdir)

if not os.path.exists(options.outputdir):
    os.makedirs(options.outputdir)

outfilename=''
if not isfarmout:
    outfilename = options.outputdir + "/" + rootfile

if isfarmout:
    outfilename = options.outputfile


skimmedTree = TChain("outTree")

#print isfarmout



def WhichSample(filename):
    samplename = 'all'
    if filename.find('WJets')>-1:
        samplename = 'WJETS'
    elif filename.find('ZJets')>-1:
        samplename = 'ZJETS'
    elif filename.find('TT')>-1:
        samplename  = 'TT'
    else:
        samplename = 'all'
    return samplename
    

h_t = TH1F('h_t','h_t',2,0,2)
h_t_weight = TH1F('h_t_weight','h_t_weight',2,0,2)

samplename = 'all'
if isfarmout:
    infile = open(inputfilename)
    for ifile in infile: 
        print 'file = ', ifile
        print 'debug 1', ifile.rstrip()
        skimmedTree.Add(ifile.rstrip())
        print 'debug 2'
        samplename = WhichSample(ifile.rstrip())
        print 'debug 3'
        ## for histograms
        f_tmp = TFile.Open(ifile.rstrip(),'READ')
        print 'debug 4', f_tmp.GetSize()
        h_tmp = f_tmp.Get('h_total')
        print 'debug 5'
        h_tmp_weight = f_tmp.Get('h_total_mcweight')
        print 'debug 6'
        print 'integral', h_tmp_weight.Integral()
        h_t.Add(h_tmp)
        print 'debug 7'
        h_t_weight.Add(h_tmp_weight)
        print 'debug 8'

if not isfarmout:
    print 'inputfile =', inputfilename
    skimmedTree.Add(inputfilename)
    samplename = WhichSample(inputfilename)
    ## for histograms
    f_tmp = TFile.Open(inputfilename,'READ')
    h_tmp = f_tmp.Get('h_total')
    h_tmp_weight = f_tmp.Get('h_total_mcweight')
    h_t.Add(h_tmp)
    h_t_weight.Add(h_tmp_weight)

debug = False 

def AnalyzeDataSet():
    ## Input rootfile name
    
    #rootfilename = inputfilename
    #print (rootfilename,inputfilename)
    #f = TFile(rootfilename,'READ')
    #skimmedTree = f.Get('tree/treeMaker')
    NEntries = skimmedTree.GetEntries()
    print ('NEntries = ',NEntries)
    npass = 0
    #print [rootfilename, NEntries]
    cutStatus={'total':NEntries}
    
    cutStatus['trigger'] = 0
    cutStatus['filter'] = 0
    cutStatus['pfmet'] =  0
    cutStatus['HiggsID'] = 0
    cutStatus['HMass'] = 0
    cutStatus['btag'] = 0
    cutStatus['dphi'] = 0
    cutStatus['ThinJetVeto'] = 0
    cutStatus['bVeto'] = 0
    cutStatus['eleveto'] = 0
    cutStatus['muveto'] = 0
    cutStatus['tauveto'] = 0
    
    allquantitiesBoosted            = MonoHbbQuantities(outfilename)
    allquantitiesBoosted.defineHisto()

    
    # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    # BTag Scale Factor Initialisation--------------------------------------------------------------------------------------------------------------------------------
    # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    # ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    ## FatJets
    calib = ROOT.BTagCalibrationStandalone('csvv1', 'subjet_CSVv2_ichep.csv')
    v_sys = getattr(ROOT, 'vector<string>')()
    v_sys.push_back('up')
    v_sys.push_back('down')
    ### working point, central systematic type, vector of other sys. types 
    ###  0 is for loose op, 1: medium, 2: tight, 3: discr. reshaping
    reader = ROOT.BTagCalibrationStandaloneReader( 0, "central", v_sys )    
    reader.load(calib, 0,  "lt" )  
    reader.load(calib, 1,  "lt" )  
    reader.load(calib, 2,  "incl" )  
    


    ## ThinJets
    calib1 = ROOT.BTagCalibrationStandalone('csvv2', 'CSVv2_ichep.csv')
    reader1 = ROOT.BTagCalibrationStandaloneReader( 0, "central", v_sys )    
    reader1.load(calib1, 0,  "comb" )  
    reader1.load(calib1, 1,  "comb" )  
    reader1.load(calib1, 2,  "incl" )  
    
    #h_total = TH1F('h_total','h_total',2,0,2)
    #h_total_mcweight = TH1F('h_total_mcweight','h_total_mcweight',2,0,2)
    
    
    
    for ievent in range(NEntries):
    #for ievent in range(501):
        
        ##
        sf_resolved1 = []
        sf_resolved2 = []
        sf_boosted1 = []
        sf_boosted2 = []
        #print "event number = ",ievent
        skimmedTree.GetEntry(ievent)
        
        ## Get all relevant branches
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Extract branches
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        run                        = skimmedTree.__getattr__('st_runId')
        lumi                       = skimmedTree.__getattr__('st_lumiSection')
        event                      = skimmedTree.__getattr__('st_eventId')

        #if event != 4126: continue                                
        #if lumi  != 42: continue                                
       # print ("run,lumi,event")
        #trigName                   = skimmedTree.__getattr__('st_hlt_trigName')
        #trigResult                 = skimmedTree.__getattr__('st_hlt_trigResult')
        #filterName                 = skimmedTree.__getattr__('st_hlt_filterName')
        #filterResult               = skimmedTree.__getattr__('st_hlt_filterResult')
                                   
        pfMet                      = skimmedTree.__getattr__('st_pfMetCorrPt')
        pfMetPhi                   = skimmedTree.__getattr__('st_pfMetCorrPhi')
        
        nFatJets                   = skimmedTree.__getattr__('st_nFatJets')
        fatjetP4                   = skimmedTree.__getattr__('st_FATjetP4')
        fatjetPRmassL2L3Corr       = skimmedTree.__getattr__('st_FATjetPRmassL2L3Corr')
        nSubSoftDropJet            = skimmedTree.__getattr__('st_FATnSubSDJet')
        subjetSDCSV                = skimmedTree.__getattr__('st_subjetSDCSV')
        #subjetSDPx                 = skimmedTree.__getattr__('st_FATsubjetSDPx')
        #subjetSDPy                 = skimmedTree.__getattr__('st_FATsubjetSDPy')
        #subjetSDPz                 = skimmedTree.__getattr__('st_FATsubjetSDPz')
        #subjetSDE                  = skimmedTree.__getattr__('st_FATsubjetSDE')
        subjetSDPt                  = skimmedTree.__getattr__('st_subjetPt')
        subjetSDEta                  = skimmedTree.__getattr__('st_subjetEta')
        #passFatJetTightID          = skimmedTree.__getattr__('st_FATjetPassIDTight')
        subjetHadronFlavor         = skimmedTree.__getattr__('st_subjetFlav')

        nTHINJets                  = skimmedTree.__getattr__('st_THINnJet')
        thinjetP4                  = skimmedTree.__getattr__('st_THINjetP4')
        thinJetCSV                 = skimmedTree.__getattr__('st_THINjetCISVV2')
        #passThinJetLooseID         = skimmedTree.__getattr__('st_THINjetPassIDLoose')
        #passThinJetPUID            = skimmedTree.__getattr__('st_THINisPUJetID')
        THINjetHadronFlavor        = skimmedTree.__getattr__('st_THINjetHadronFlavor')
        
        nEle                       = skimmedTree.__getattr__('st_nEle')
        eleP4                      = skimmedTree.__getattr__('st_eleP4')
        #eleIsPassLoose             = skimmedTree.__getattr__('st_eleIsPassLoose')
        
        nMu                        = skimmedTree.__getattr__('st_nMu')
        muP4                       = skimmedTree.__getattr__('st_muP4')
        #isLooseMuon                = skimmedTree.__getattr__('st_isLooseMuon')
        #muChHadIso                 = skimmedTree.__getattr__('st_muChHadIso')
        #muNeHadIso                 = skimmedTree.__getattr__('st_muNeHadIso')
        #muGamIso                   = skimmedTree.__getattr__('st_muGamIso')
        #muPUPt                     = skimmedTree.__getattr__('st_muPUPt')
        
        nTau                       = skimmedTree.__getattr__('st_HPSTau_n')
        tauP4                      = skimmedTree.__getattr__('st_HPSTau_4Momentum')
        #isDecayModeFinding         = skimmedTree.__getattr__('st_disc_decayModeFinding')
        #passLooseTauIso            = skimmedTree.__getattr__('st_disc_byLooseIsolationMVA3oldDMwLT')
        
        isData                     = skimmedTree.__getattr__('st_isData')
        mcWeight                   = skimmedTree.__getattr__('mcweight')
        pu_nTrueInt                = int(skimmedTree.__getattr__('st_pu_nTrueInt'))
        
        nGenPar                    = skimmedTree.__getattr__('st_nGenPar')
        genParId                   = skimmedTree.__getattr__('st_genParId')
        genMomParId                = skimmedTree.__getattr__('st_genMomParId')
        genParSt                   = skimmedTree.__getattr__('st_genParSt')
        genParP4                   = skimmedTree.__getattr__('st_genParP4')
        #= skimmedTree.__getattr__('st_')
        #= skimmedTree.__getattr__('st_')
             
        HiggsInfo_sorted           = []
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # MC Weights ----------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        mcweight = 0.0 
        if isData==1:   mcweight =  1.0
        if not isData :
            if mcWeight<0:  mcweight = -1.0
            if mcWeight>0:  mcweight =  1.0
        

        #h_total.Fill(1.);
        #h_total_mcweight.Fill(1.,mcweight);
        
        
        
        '''
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # EWK Reweighting And Top pT Reweighting--------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #print "samplename", samplename
        genpTReweighting = 1.0 
        if isData==1:   genpTReweighting  =  1.0
        if not isData :  genpTReweighting = GenWeightProducer(samplename, nGenPar, genParId, genMomParId, genParSt,genParP4)
        #print genpTReweighting
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Pileup weight
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        allpuweights = PUWeight()
        len_puweight = len(allpuweights)
        puweight = 0.0
        if isData: puweight = 1.0 
        if not isData:
            if pu_nTrueInt  <= len_puweight: puweight = allpuweights[pu_nTrueInt-1]
            if pu_nTrueInt  > len_puweight : puweight = 0.0 
        #print (len_puweight, pu_nTrueInt, puweight)
        

        #----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #All Weights ----------------------------------------------------------------------------------------------------------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #allweights = puweight * mcweight * genpTReweighting
        allweights = mcweight * genpTReweighting 
        
        '''            
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Trigger selection
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        '''
        itrig_=0; trig1 = False; trig2 = False;
        trig1 = CheckFilter(trigName, trigResult, 'HLT_PFMET170_NoiseCleaned')
        trig2 = CheckFilter(trigName, trigResult, 'HLT_PFMET90_PFMHT90_')
        #trigstatus =  trig1 | trig2
        if not isData:
            trigstatus  = True
        if isData:
            trigstatus =  trig1 | trig2
        
        if trigstatus == False : continue
        cutStatus['trigger'] += 1
            
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Filter selection
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        filterstatus = False
        filter1 = False; filter2 = False;filter3 = False;filter4 = False
        ifilter_=0
        filter1 = CheckFilter(filterName, filterResult, 'Flag_CSCTightHaloFilter')
        filter2 = CheckFilter(filterName, filterResult, 'Flag_CSCTightHaloFilter')
        filter3 = CheckFilter(filterName, filterResult, 'Flag_CSCTightHaloFilter')
        filter4 = CheckFilter(filterName, filterResult, 'Flag_CSCTightHaloFilter')
        #filterstatus =  filter1 | filter2 & filter3 & filter4
        filterstatus =  True
        #if filterstatus == False : continue 
        cutStatus['filter'] += 1
        '''
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## PFMET Selection
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------        
        pfmetstatus = ( pfMet > 200.0 )
        if pfmetstatus == False : continue 
        cutStatus['pfmet'] += 1
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Fat-Jet Selection
        ## Higgs Tagging
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        
        '''
        if nFATJets>0 :
            print nFATJets, fatjetPRmassL2L3Corr[0], nSubSoftDropJet[0],  bool(passFatJetTightID[0]), fatjetP4[0].Pt(), fatjetP4[0].M()
            if nSubSoftDropJet > 0:
                print subjetSDE[0][0], subjetSDCSV[0][0], subjetSDPx[0][0], subjetSDPy[0][0], subjetSDPz[0][0]
        '''
        ## list comprehensation
        ## y = [s for s in x if len(s) == 2]
        #''' boosted higgs tagging 
        HIndex = -1
        HThinIndex = -1
        nsubjetstatus = False
        higgstag = False
        #print ('nFATJets = ',nFatJets, ' nfatjetPRmassL2L3Corr = ', len(fatjetPRmassL2L3Corr))
        
        #nFATJets = st_nFatJets#len(fatjetP4)
        for ifatjet in range(nFatJets):
            if fatjetP4[ifatjet].Pt() > 200.0  : 
                if abs(fatjetP4[ifatjet].Eta())  < 2.4 : 
                    #if (bool(passFatJetTightID[ifatjet]) == True) : 
                    HIndex = ifatjet
                    #print 'ifatjet = ',ifatjet
                    break
    
        
        if HIndex > -1 :
            cutStatus['HiggsID'] += 1
            #print "HIndex = ", HIndex
            if ((fatjetPRmassL2L3Corr[HIndex] > massCutLow) & (fatjetPRmassL2L3Corr[HIndex] < massCutHigh)) | ((fatjetPRmassL2L3Corr[HIndex] > massCutLow1) & (fatjetPRmassL2L3Corr[HIndex] < massCutHigh1)) : 
                if pfMet > 200.0:
                    fatJetMassStatus = True
                    cutStatus['HMass'] += 1
                    nSubBJet=0;
                    for isj in range(nSubSoftDropJet[HIndex]):
                        if subjetSDCSV[HIndex][isj] > 0.46 : 
                            nSubBJet = nSubBJet + 1
        
                    if nSubBJet>1 : 
                        nsubjetstatus = True
                        cutStatus['btag'] += 1
    
        if nsubjetstatus: 
            if False: print "this is boosted regime"
        else:   
        #if True:
            ''' resolved Higgs boson tagging 
            '''    
            HPtVec=[]
            HMassVec=[]
            HPhiVec=[]
            pairindex=[]
            HiggsInfo=[]
            for ithinjet in range(nTHINJets):
                j1 = thinjetP4[ithinjet]
                if (j1.Pt() > 30.0)&(abs(j1.Eta())<2.4)& (thinJetCSV[ithinjet] > 0.8):   
                    for jthinjet in range(nTHINJets):
                        if (jthinjet != ithinjet ) & ( jthinjet > ithinjet ) & (jthinjet < nTHINJets) : 
                            j2 = thinjetP4[jthinjet]
                            if (j2.Pt() > 30.0) & (abs(j2.Eta()) <2.4) &(thinJetCSV[jthinjet] > 0.8) :
                                
                                Hpt = (j1 + j2 ).Pt()
                                HMass = (j1 + j2 ).M()
                                HPhi  = (j1 + j2 ).Phi()
                                HEta  = (j1 + j2 ).Eta()
                                
                                HPtVec.append(Hpt)
                                HMassVec.append(HMass)
                                HPhiVec.append(HPhi)
                                
                                pair=[]
                                pair.append(ithinjet); pair.append(jthinjet)
                                pairindex.append(pair)
                                p =[ithinjet, jthinjet, HMass, Hpt, HPhi, HEta]
                                HiggsInfo.append(p)

            HiggsInfo_sorted = sorted (HiggsInfo, key=lambda student: student[3], reverse=True)   
            if len(HiggsInfo_sorted) > 0: HThinIndex = len(HiggsInfo_sorted) 
            #print "student=",HiggsInfo_sorted
            

            if HThinIndex > 0:
                mass_ = HiggsInfo_sorted[0][2]
                pt_   = HiggsInfo_sorted[0][3]
                if (( mass_ > massCutLow) & ( mass_ < massCutHigh)) | ((mass_ > massCutLow1) & (mass_ < massCutHigh1)) : 
                    cutStatus['HMass'] += 1
                    if (pt_>150.): 
                        cutStatus['btag'] += 1
                        higgstag = True
            ''' resolved Higgs boson tagging 
            '''
        ####
        if nsubjetstatus == True : isboosted = True #; print "this is boosted"
        else : isboosted = False
        isresolved = False
        if  (higgstag): isresolved = True;
        else : isresolved = False
        
        if ( isboosted | isresolved ) == False: continue 
        #if ( isresolved ) == False: continue 
        
        #if (nsubjetstatus == False) & (HThinIndex < 1) : continue 
        #if  (HThinIndex < 1) : continue 
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Delta Phi
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        nGoodTHINJets = 0
        nGoodTHINBJets = 0
        jetIndex = -1
        dphiVec=[]
        
        print (nTHINJets,nTHINJets)
        for ijet in range(nTHINJets):
            p4_j = thinjetP4[ijet]
            #print (ijet, DeltaR(p4_j, fatjetP4[HIndex]), p4_j.Pt() , abs(p4_j.Eta()), bool(passThinJetLooseID[ijet]), bool(passThinJetPUID[ijet]), thinJetCSV[ijet])
            if p4_j.Pt() < 30 : continue
            if abs(p4_j.Eta())>4.5 : continue
            #if bool(passThinJetLooseID[ijet]) == False : continue
            #if bool(passThinJetPUID[ijet]) == False : continue 
            
            if isboosted : 
                #print (isboosted,isresolved)
                if DeltaR(p4_j, fatjetP4[HIndex])  > 0.8:
                    nGoodTHINJets += 1
                    jetIndex=ijet
            if isresolved :
                jet1 = HiggsInfo_sorted[0][0]
                jet2 = HiggsInfo_sorted[0][1]
                if DeltaR(p4_j, thinjetP4[jet1])  > 0.4:
                    if DeltaR(p4_j, thinjetP4[jet2])  > 0.4:
                        nGoodTHINJets += 1
                        jetIndex=ijet
            #print "nGoodTHINJets = ",nGoodTHINJets
            #print ((p4_j.Eta()>2.4), thinJetCSV[ijet]<0.46 )
            
            if abs(p4_j.Eta())>2.4 : continue
            dphi = Phi_mpi_pi(pfMetPhi - p4_j.Phi())
            #print dphi
            dphiVec.append(abs(dphi))
            
            if thinJetCSV[ijet]<0.46 : continue
            if isboosted :
                if DeltaR(p4_j, fatjetP4[HIndex])  > 0.8:
                    nGoodTHINBJets = nGoodTHINBJets + 1
            if isresolved : 
                #print "jet number", ijet
                jet1 = HiggsInfo_sorted[0][0]
                jet2 = HiggsInfo_sorted[0][1]
                if DeltaR(p4_j, thinjetP4[jet1])  > 0.4:
                    if DeltaR(p4_j, thinjetP4[jet2])  > 0.4:
                        nGoodTHINBJets = nGoodTHINBJets + 1
            
            
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## min DPhi
        ## nT<hinJets
        ## b-jet Veto
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------

        if len(dphiVec) > 0:  
            #print min(dphiVec) 
            if min(dphiVec) < 0.4 : continue 
        cutStatus['dphi'] += 1
        
        if not (nGoodTHINJets >= njet) : continue 
        if not (nGoodTHINJets < nJet): continue 
        cutStatus['ThinJetVeto'] += 1
        
        if nGoodTHINBJets > 0: continue 
        cutStatus['bVeto'] += 1
        
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Electron Veto
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        myEles=[]
        for iele in range(nEle):
            if eleP4[iele].Pt() < 10 : continue
            if abs(eleP4[iele].Eta()) >2.5: continue
            #if bool(eleIsPassLoose[iele]) == False : continue
            myEles.append(iele)

        #if len(myEles) > 0 : continue
        cutStatus['eleveto'] += 1
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Muon Veto
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        myMuos = []
        for imu in range(nMu):
            if muP4[imu].Pt()<10 : continue
            if abs(muP4[imu].Eta()) > 2.4  : continue
            #if  bool(isLooseMuon[imu]) == False  : continue
            #relPFIso = (muChHadIso[imu]+ max(0., muNeHadIso[imu] + muGamIso[imu] - 0.5*muPUPt[imu]))/muP4[imu].Pt();
            #if relPFIso>0.4 : continue
            myMuos.append(imu)
        #if len(myMuos) > 0: continue
        cutStatus['muveto'] += 1
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Tau Veto
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        myTaus=[]
        for itau in range(nTau):
            #print ("tau properties", tauP4[itau].Pt(), abs(tauP4[itau].Eta()), bool(isDecayModeFinding[itau]), bool(passLooseTauIso[itau]))
            if tauP4[itau].Pt()<20 : continue
            if abs(tauP4[itau].Eta())>2.3 : continue
            #if bool(isDecayModeFinding[itau]) == False : continue
            #if bool(passLooseTauIso[itau]) == False : continue
            myTaus.append(itau);
        
        #if len(myTaus)>0 : continue
        cutStatus['tauveto'] += 1
        #print 'before lepton veto'
        nleptons_ = (len(myTaus) + len(myMuos) + len(myEles))
        
        if not (nleptons_ >= nlepton) : continue 
        if not (nleptons_ < nLepton) : continue  
        
        regime = False
        if isboosted: regime = True
        if isresolved: regime = False
        #print (isboosted,isresolved, regime)
        
        if regime: 
            mt_ = MT(fatjetP4[HIndex].Pt(), pfMet, Phi_mpi_pi(pfMetPhi-fatjetP4[HIndex].Phi()) )
        if not regime: 
            mt_ = MT(HiggsInfo[0][3], pfMet, Phi_mpi_pi(pfMetPhi-HiggsInfo[0][4]) )
        
        if mt_ < 450.0: continue 
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Photon Veto
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----to be added in future---------------------------------------------------------------------------------------------------------------------------------------
        #print (run,lumi,event)
        
        npass = npass +1
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # EWK Reweighting And Top pT Reweighting--------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #print "samplename", samplename
        genpTReweighting = 1.0 
        if isData==1:   genpTReweighting  =  1.0
        if not isData :  genpTReweighting = GenWeightProducer(samplename, nGenPar, genParId, genMomParId, genParSt,genParP4)
        #print genpTReweighting
        
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## Pileup weight
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        allpuweights = PUWeight()
        len_puweight = len(allpuweights)
        puweight = 0.0
        if isData: puweight = 1.0 
        if not isData:
            if pu_nTrueInt  <= len_puweight: puweight = allpuweights[pu_nTrueInt-1]
            if pu_nTrueInt  > len_puweight : puweight = 0.0 
        #print (len_puweight, pu_nTrueInt, puweight)
        

        #----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #All Weights ----------------------------------------------------------------------------------------------------------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #----------------------------------------------------------------------------------------------------------------------------------------------------------------
        #allweights = puweight * mcweight * genpTReweighting
        allweights = mcweight * genpTReweighting 
        

        
        
        ##-------------------------------------------------------------------------------------------------
        ##-------------------------------------------------------------------------------------------------
        ##------------fill all variables needed for further processing-------------------------------------
        ##-------------------------------------------------------------------------------------------------
        ##-------------------------------------------------------------------------------------------------


        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        ## BTag Scale Factor ---------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------
        # ----------------------------------------------------------------------------------------------------------------------------------------------------------------        
        
        if regime:
            #print "inside regime"
            #p4_1 = TLorentzVector(subjetSDPx[HIndex][0], subjetSDPy[HIndex][0], subjetSDPz[HIndex][0], subjetSDE[HIndex][0])
            #p4_2 = TLorentzVector(subjetSDPx[HIndex][1], subjetSDPy[HIndex][1], subjetSDPz[HIndex][1], subjetSDE[HIndex][1])
            
            pt1 = subjetSDPt[HIndex][0]
            eta1 = subjetSDEta[HIndex][0]
            pt2 = subjetSDPt[HIndex][1]
            eta2 = subjetSDEta[HIndex][1]
            
            flav1 = jetflav(subjetHadronFlavor[HIndex][0])
            flav2 = jetflav(subjetHadronFlavor[HIndex][1])
            
            sf_boosted1 = weightbtag(reader, flav1, pt1, eta1)
            sf_boosted2 = weightbtag(reader, flav2, pt2, eta2)
            
            #print (sf_boosted1, sf_boosted2)
            
        
        if not regime: 
            
            #print "inside not regime"
            ij = HiggsInfo_sorted[0][0]
            jj = HiggsInfo_sorted[0][1]
            
            flav1 = jetflav(THINjetHadronFlavor[ij])
            flav2 = jetflav(THINjetHadronFlavor[jj])

            #print ("ij, flav, pt, eta, ",ij, flav1, thinjetP4[ij].Pt(), thinjetP4[ij].Eta())
            #reader1.eval_auto_bounds('central', 0, 1.2, 50.)
            sf_resolved1 = weightbtag(reader1, flav1, thinjetP4[ij].Pt(), thinjetP4[ij].Eta())
            sf_resolved2 = weightbtag(reader1, flav2, thinjetP4[jj].Pt(), thinjetP4[jj].Eta())
            
            #print (sf_resolved1, sf_resolved2)

        
        #########################################
        ## Regime Loop ends here
        #########################################
        #print (sf_resolved1, sf_resolved2,sf_boosted1, sf_boosted2)
        if regime: 
            allweights = allweights * sf_boosted1[0] * sf_boosted2[0]
        if not regime:
            allweights = allweights * sf_resolved1[0] * sf_resolved2[0]
        
        if isData: allweights = 1.0 
        #print "allweights = ", allweights 
        allquantitiesBoosted.regime     = regime
        allquantitiesBoosted.met        = pfMet
        allquantitiesBoosted.mt         = mt_
        if not  ((len(myTaus) + len(myMuos) + len(myEles)) < nLepton) : continue

        if len(dphiVec)>0: allquantitiesBoosted.dPhi            = min(dphiVec)
        allquantitiesBoosted.N_e             = len(myEles)
        allquantitiesBoosted.N_mu            = len(myMuos)
        allquantitiesBoosted.N_tau           = len(myTaus)
        allquantitiesBoosted.N_Pho           = 0
        allquantitiesBoosted.N_b             = nGoodTHINBJets
        allquantitiesBoosted.N_j             = nGoodTHINJets

        allquantitiesBoosted.weight    = allweights
        allquantitiesBoosted.totalevents = 1
        
        if regime: 
            allquantitiesBoosted.mass            = fatjetPRmassL2L3Corr[HIndex]
            allquantitiesBoosted.HiggsPt         = fatjetP4[HIndex].Pt()
            allquantitiesBoosted.HiggsEta        = fatjetP4[HIndex].Eta()
            allquantitiesBoosted.HiggsPhi        = fatjetP4[HIndex].Phi()
            
        if not regime:  
            allquantitiesBoosted.mass            = HiggsInfo_sorted[0][2]
            allquantitiesBoosted.HiggsPt         = HiggsInfo_sorted[0][3]
            allquantitiesBoosted.HiggsEta        = HiggsInfo_sorted[0][5]
            allquantitiesBoosted.HiggsPhi        = HiggsInfo_sorted[0][4]
            
        #print (allquantitiesBoosted.regime, allquantitiesBoosted.met,allquantitiesBoosted.mass )
        allquantitiesBoosted.FillHisto()
    

    #print cutStatus
    #print "npass = ", npass
    NEntries_Weight = h_t.Integral()
    NEntries_total        = h_t_weight.Integral()
    allquantitiesBoosted.WriteHisto((NEntries_total,NEntries_Weight))
    #print " efficiency = ", float(npass/float(NEntries))
    if not os.path.exists('efficiencyfiles'):
        os.makedirs('efficiencyfiles')
    f = open('efficiencyfiles/'+textfile, 'w')
    f.write(str(float(npass/float(NEntries))))
    f.close()



        
def CheckFilter(filterName, filterResult,filtercompare):
    ifilter_=0
    filter1 = False
    for ifilter in filterName:
        filter1 = (ifilter.find(filtercompare) != -1)  & (bool(filterResult[ifilter_]) == True)   
        if filter1: break
        ifilter_ = ifilter_ + 1
    return filter1





######################################
######################################
######################################
def MakeTable():
    print "called MakeTable"
    files= [inputfilename]
    legend=legendTemplate
    prefix="V_met_"
    effnamelist = [prefix + ihisto  for ihisto in namelist]
    inputfile={}
    histList=[]
    for ifile_ in range(len(files)):
        print ("opening file  "+files[ifile_])
        inputfile[ifile_] = TFile( files[ifile_] )
        print "fetching histograms"
        for ihisto_ in range(len(effnamelist)):
            histo = inputfile[ifile_].Get(effnamelist[ihisto_])
            histList.append(histo)
    
    for ih in range(len(histList)):
        eff = ("%0.4f" % float(histList[ih].Integral()/histList[0].Integral()))
        toprint =  legendTemplate[ih] + " & " + str(eff) + " \\\\"
        print toprint


def DeltaR(p4_1, p4_2):
    eta1 = p4_1.Eta()
    eta2 = p4_2.Eta()
    eta = eta1 - eta2
    eta_2 = eta * eta
    
    phi1 = p4_1.Phi()
    phi2 = p4_2.Phi()
    phi = Phi_mpi_pi(phi1-phi2)
    phi_2 = phi * phi

    return math.sqrt(eta_2 + phi_2)
    
def Phi_mpi_pi(x):
    kPI = 3.14159265358979323846
    kTWOPI = 2 * kPI
    
    while (x >= kPI): x = x - kTWOPI;
    while (x < -kPI): x = x + kTWOPI;
    return x;
    
def weightbtag(reader, flav, pt, eta):
    sf_c = reader.eval_auto_bounds('central', flav, eta, pt) 
    sf_low = reader.eval_auto_bounds('down', flav, eta, pt)
    sf_up  = reader.eval_auto_bounds('up', flav, eta, pt)
    btagsf = [sf_c, sf_low, sf_up]
    return btagsf

def jetflav(flav):
    if flav == 5: 
        flavor = 0
    elif flav == 4:
        flavor = 1
    else:
        flavor = 2
    return flavor




def GenWeightProducer(sample,nGenPar, genParId, genMomParId, genParSt,genParP4):
    pt__=0;
    #print " inside gen weight "
    k2=1.0
    #################
    # WJets
    #################
    if sample=="WJETS":
        goodLepID = []
        for ig in range(nGenPar): 
            PID    = genParId[ig]
            momPID = genMomParId[ig]
            status = genParSt[ig]
            #print "inside WJ loop pdgid", PID
            #print ("if status =",      (abs(PID) != 11),( abs(PID) != 12),(  abs(PID) != 13 ),(  abs(PID) != 14),(  abs(PID) != 15),(  abs(PID) != 16))
            #print "and of if status ", ( (abs(PID) != 11) & (abs(PID) != 12) &  (abs(PID) != 13) & (abs(PID) != 14) &  (abs(PID) != 15) &  (abs(PID) != 16) )
            
            if ( (abs(PID) != 11) & (abs(PID) != 12) &  (abs(PID) != 13) & (abs(PID) != 14) &  (abs(PID) != 15) &  (abs(PID) != 16) ): continue
            #print "lepton found"
            if ( ( (status != 1) & (abs(PID) != 15)) | ( (status != 2) & (abs(PID) == 15)) ): continue
            #print "tau found"
            if ( (abs(momPID) != 24) & (momPID != PID) ): continue
            #print "W found"
            #print "aftrer WJ if statement"
            goodLepID.append(ig)
        #print "length = ",len(goodLepID)
        if len(goodLepID) == 2 :
            l4_thisLep = genParP4[goodLepID[0]]
            l4_thatLep = genParP4[goodLepID[1]]
            l4_z = l4_thisLep + l4_thatLep
            
            pt = l4_z.Pt()
            pt__ = pt
            #print " pt inside "
            k2 = -0.830041 + 7.93714 *TMath.Power( pt - (-877.978) ,(-0.213831) ) ;
    
    #################        
    #ZJets
    #################
    if sample == "ZJETS":
        #print " inside zjets "
        goodLepID = []
        for ig in range(nGenPar):
         #   print " inside loop "
            PID    = genParId[ig]
            momPID = genMomParId[ig]
            status = genParSt[ig]
          #  print " after vars "

            if ( (abs(PID) != 12) &  (abs(PID) != 14) &  (abs(PID) != 16) ) : continue
            if ( status != 1 ) : continue 
            if ( (momPID != 23) & (momPID != PID) ) : continue
            goodLepID.append(ig)
        
        if len(goodLepID) == 2 :
            l4_thisLep = genParP4[goodLepID[0]]
            l4_thatLep = genParP4[goodLepID[1]]
            l4_z = l4_thisLep + l4_thatLep
            pt = l4_z.Pt()
            #print " pt inside "
            k2 = -0.180805 + 6.04146 *TMath.Power( pt - (-759.098) ,(-0.242556) ) ;

    #################        
    #TTBar
    #################        
    if (sample=="TT"):
        #print " inside ttbar "
        goodLepID = []
        for ig in range(nGenPar):
         #   print "inside TT loop "
            PID    = genParId[ig]
            momPID = genMomParId[ig]
            status = genParSt[ig]
            if ( abs(PID) == 6) :
                goodLepID.append(ig)
        if(len(goodLepID)==2):
            l4_thisLep = genParP4[goodLepID[0]]
            l4_thatLep = genParP4[goodLepID[1]]
            pt1 = TMath.Min(400.0, l4_thisLep.Pt())
            pt2 = TMath.Min(400.0, l4_thatLep.Pt())
            
            w1 = TMath.Exp(0.156 - 0.00137*pt1);
            w2 = TMath.Exp(0.156 - 0.00137*pt2);
            k2 =  1.001*TMath.Sqrt(w1*w2);
            
    if(sample=="all"):
        k2 = 1.0
        
    return k2


def MT(Pt, met, dphi):
    return ROOT.TMath.Sqrt( 2 * Pt * met * (1.0 - ROOT.TMath.Cos(dphi)) )

if __name__ == "__main__":
    ## analyze the tree and make histograms and all the 2D plots and Efficiency plots. 
    if options.analyze:
        print "now calling analyzedataset"
        AnalyzeDataSet()
    
    
    

