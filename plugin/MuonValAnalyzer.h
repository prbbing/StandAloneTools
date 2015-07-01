//#ifndef TriggerAnalyze_MuonValAnalyzer_h
//#define TriggerAnalyze_MuonValAnalyzer_h

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
#include "TH1D.h"
#include "TH2D.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TTree.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

using namespace std;

class MuonValAnalyzer : public edm::EDAnalyzer
{
    public:
      void analyze (const edm::Event &, const edm::EventSetup &);
      double LxyBeamSpot(double vx, double vy, double refx, double refy);
      bool Match(double eta_1, double phi_1, double eta_2, double phi_2);
      MuonValAnalyzer (const edm::ParameterSet &);
      ~MuonValAnalyzer();
      virtual void endJob() override; 

      edm::Service<TFileService> fs;
    private:
      edm::Handle<reco::TrackCollection> TrackCollection;
      edm::Handle<reco::BeamSpot> BeamSpot;
      edm::Handle<reco::VertexCollection> PrimaryVertex;
      edm::InputTag recoTrack_;
      edm::InputTag beamSpot_;
      edm::InputTag genParticle_;
      edm::InputTag primaryVertex_;
      double deltaRCut_;
      edm::Handle<reco::GenParticleCollection> genParticles;     
      TH1D* genMuFromStopLxy; 
      TH1D* recoEffLxy;
      TH1D* recoMatchedGenMuFromStopLxy; 
      TH1D* ptRes; 
      TH1D* numVertex; 
      TH2D* deltaRVsLxy; 
      vector<double> LxyBins_;
      vector<double> PtResBins_;
      double BeamSpotX; 
      double BeamSpotY; 
      double BeamSpotZ; 
};  

//#endif
