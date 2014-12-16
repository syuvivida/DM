// -*- C++ -*-
//
// Package:    TrigFilter
// Class:      TrigFilter
// 
/**\class TrigFilter TrigFilter.cc DelPanj/TrigFilter/src/TrigFilter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Anil Pratap Singh,,,
//         Created:  Mon Sep 19 01:43:09 CEST 2011
// $Id: TrigFilter.cc,v 1.1 2012/03/15 09:58:20 lovedeep Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "FWCore/Common/interface/TriggerNames.h" 
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"




//
// class declaration
//

class TrigFilter : public edm::EDFilter {
   public:
      explicit TrigFilter(const edm::ParameterSet&);
      ~TrigFilter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      virtual bool beginRun(edm::Run&, edm::EventSetup const&);
      virtual bool endRun(edm::Run&, edm::EventSetup const&);
      virtual bool beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
      virtual bool endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

      // ----------member data ---------------------------
      edm::InputTag trigTag_;
      std::vector<std::string> trigPaths_;
  
      bool isValidHltConfig_;
      HLTConfigProvider  hltConfigProvider_;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TrigFilter::TrigFilter(const edm::ParameterSet& iConfig):
 trigTag_(iConfig.getParameter<edm::InputTag> ("TrigTag")),//, edm::InputTag("TriggerResults::HLT"))),
 trigPaths_(iConfig.getParameter<std::vector< std::string > >("TrigPaths"))
 
{
   //now do what ever initialization is needed
   isValidHltConfig_ = false;

}


TrigFilter::~TrigFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
TrigFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

   edm::Handle<edm::TriggerResults> trigResults;
     //edm::InputTag trigTag("TriggerResults::HLT");
     if (not iEvent.getByLabel(trigTag_, trigResults)) {
       std::cout << ">>> TRIGGER collection does not exist !!!\n";
       return false;
     } 
     
     const edm::TriggerNames & trigNames 
       = iEvent.triggerNames(*trigResults);
     bool decision = false;
     for (unsigned int i=0; i<trigResults->size(); i++)
       {
	 std::string trigName   = trigNames.triggerName(i);
	 bool  trigResult = trigResults->accept(i);
         //std::vector<std::string>::const_iterator iter = trigPaths_.begin();
         //Not interested in presclaed paths. 
         bool prescaled = 0;
         const unsigned int prescaleSize=  hltConfigProvider_.prescaleSize();
         for(unsigned int ps=0; ps<prescaleSize;ps++){
           const unsigned int prescaleValue=hltConfigProvider_.prescaleValue(ps,trigName);
           if(prescaleValue !=1 )prescaled = true;
          }
         if(prescaled) continue;
         //std::cout<<trigName<<std::endl; 
	 if( find(trigPaths_.begin(), trigPaths_.end(), trigName)  == trigPaths_.end() )continue;
         //std::cout<<trigName<<std::endl; 
	 decision = decision||trigResult;
       }
  return decision;
}

// ------------ method called once each job just before starting event loop  ------------
void 
TrigFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TrigFilter::endJob() {
}

// ------------ method called when starting to processes a run  ------------
bool 
TrigFilter::beginRun(edm::Run& r, edm::EventSetup const& iSetup)
{ 
  bool isConfigChanged = false;
  isValidHltConfig_ = hltConfigProvider_.init( r, iSetup, trigTag_.process(), isConfigChanged );
//  return isValidHltConfig_;
  return true;
}

// ------------ method called when ending the processing of a run  ------------
bool 
TrigFilter::endRun(edm::Run&, edm::EventSetup const& iSetup)
{
  return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool 
TrigFilter::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool 
TrigFilter::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TrigFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(TrigFilter);
