#include <memory>
#include <map>
#include <string>
#include <vector>
#include <sstream>

#include <fstream>
#include <iostream>
#include <algorithm>
#include <regex>
#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TH2D.h"

#include "TVector3.h"
#include "TLorentzVector.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

using namespace std;

class IsolationCalculator : public edm::EDAnalyzer
{
  public:
      void analyze (const edm::Event &, const edm::EventSetup &);
      int chargedHadronVertex(int numVertex, const pat::PackedCandidate &pfcand);
      bool genMatching(reco::GenParticle &genParticle, double selectId, double vetoId);  
      IsolationCalculator (const edm::ParameterSet &);
      ~IsolationCalculator();
      edm::Service<TFileService> fs;
  private:
      edm::EDGetTokenT<vector<pat::PackedCandidate>> pfCandidateToken_;
      edm::EDGetTokenT<vector<pat::Muon>> muonToken_;
      edm::EDGetTokenT<vector<reco::GenParticle>> genParticleToken_;
      TH1D* PFIso;
      TH1D* chargedHadronPtHist;
      TH1D* sumPUPtHist;
      TH1D* muonPVIndexHist;
      TH1D* chargedHadronPVIndexHist;
      bool New_;
      double match_;
      double veto_;
      edm::InputTag vertex_;
};


