//#ifndef FILTER_ON_CHANNEL_DECISION

//#define FILTER_ON_CHANNEL_DECISION

#include <iostream>
#include <map>
#include <string>

#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
using namespace std;

class SimpleFilter : public edm::EDFilter
  {
    public:
      SimpleFilter(const edm::ParameterSet &cfg);
      ~SimpleFilter();

      bool filter (edm::Event &, const edm::EventSetup &);

    private:
     edm::Handle<reco::MuonCollection> MuonCollection;
     edm::Handle<reco::GsfElectronCollection> ElectronCollection;
     edm::Handle<reco::PFJetCollection> JetCollection;
     edm::InputTag recoMuon_;
     edm::InputTag recoElectron_;
     edm::InputTag recoJet_;
     double ptThresholdOfMuon_;
     double ptThresholdOfElectron_;
     double ptThresholdOfJet_;
     int numOfMuon_;
     int numOfElectron_;
     int numOfJet_;
  };

//#endif
