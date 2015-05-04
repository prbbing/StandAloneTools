//#ifndef TriggerAnalyze_TurnOnAnalyzer_h
//#define TriggerAnalyze_TurnOnAnalyzer_h

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
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TTree.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"

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
#include "DataFormats/GeometrySurface/interface/Line.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

using namespace std;

class TurnOnAnalyzer : public edm::EDProducer
{
    public:
      void produce (edm::Event &, const edm::EventSetup &);
      //void analyze (const edm::Event &, const edm::EventSetup &);
      double D0BeamSpot(double vx, double vy, double px, double py, double pt, double refx, double refy);
      double DZBeamSpot(double vx, double vy, double vz, double px, double py, double pz, double pt, double refx, double refy, double refz);
      vector<string> GetStringSegments(string input, char delim);
      double RecoMuon(const reco::Muon recoMuon, string variable); 
      double GenParticle(const reco::GenParticle genParticle, string variable); 
      double RecoTrack(const reco::Track recoTrack, string variable); 
      bool Match(double eta_1, double phi_1, double eta_2, double phi_2);
      bool PassMuonReco(const reco::Muon recoMuon);
      bool GenMatched(const reco::Muon recoMuon);
      bool PassTrackReco(const reco::Track recoTrack);
      bool PassElectronReco(const reco::GsfElectron recoElectron);
      TurnOnAnalyzer (const edm::ParameterSet &);
      ~TurnOnAnalyzer();
      virtual void endJob() override; 

      edm::Service<TFileService> fs;
    private:
      edm::Handle<reco::MuonCollection> MuonCollection;
      edm::Handle<reco::TrackCollection> TrackCollection;
      edm::Handle<trigger::TriggerObjectCollection> MuonLegObjects;
      edm::Handle<reco::BeamSpot> BeamSpot;
      edm::Handle<reco::VertexCollection> PrimaryVertex;
      edm::Handle<trigger::TriggerEvent> triggerSummary;
      edm::InputTag recoMuon_;
      edm::InputTag recoTrack_;
      edm::InputTag triggerSummaryLabel_;
      edm::InputTag triggerResultsLabel_;
      vector<edm::ParameterSet> histogramSets_;
      edm::InputTag beamSpot_;
      edm::InputTag genParticle_;
      edm::InputTag muonFilterTag_;
      edm::InputTag l3Muon_;
      edm::InputTag l2Muon_;
      double d0cut_;
      string filterTag_;
      bool FilterEff_;
      bool L3Eff_;
      bool RecoMuonEff_;
      bool RecoTrackEff_;
      bool L2Eff_;
      edm::InputTag primaryVertex_;
      string hltPath_;
      edm::Handle<reco::GenParticleCollection> genParticles;     
      edm::Handle<reco::RecoChargedCandidateCollection> L3CandidateCollection;
      edm::Handle<reco::RecoChargedCandidateCollection> L2CandidateCollection;
      edm::Handle<edm::TriggerResults> triggerResults; 
      TH1D* stopDecayLength; 
      double BeamSpotX; 
      double BeamSpotY; 
      double BeamSpotZ; 
      double NumberOfVertex; 
      double StopLength;
      map<string, map<string,TH1D*>> oneDHistSetMap;
      map<string, vector<string>> filterMap;
      map<string, map<string,TH2D*>> twoDHistSetMap;
      int triggerBit;
};  

//#endif
