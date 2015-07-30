//#ifndef TriggerAnalyze_TriggerAnalyzerModule_h
//#define TriggerAnalyze_TriggerAnalyzerModule_h

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
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

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
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

using namespace std;

class TriggerAnalyzerModule : public edm::EDProducer
{
    public:
      void produce (edm::Event &, const edm::EventSetup &);
      //void analyze (const edm::Event &, const edm::EventSetup &);
      bool match(double eta_1, double phi_1, double eta_2, double phi_2);
      double dxyBeamSpot(double vx, double px, double vy, double py, double pt, double refx, double refy);
      bool tagToBool(const string tag, bool loose, bool medium, bool tight);
      vector<reco::Muon> recoMuonSelector(const edm::InputTag recoMuonTag, edm::Event &event);
      vector<reco::Muon> recoMuonLooper(const trigger::TriggerObjectCollection Objects, vector<reco::Muon> recoMuonSet);
      vector<reco::Track> recoTrackSelector(const edm::InputTag recoTrackTag, edm::Event &event);
      vector<reco::Track> recoTrackLooper(const trigger::TriggerObjectCollection Objects, vector<reco::Track> recoTrackSet);
      void fillMuonHistogram(vector<reco::Muon> recoMuonSet, TH1D* histogram);
      void fillTrackHistogram(vector<reco::Track> recoTrackSet, TH1D* histogram);
      vector<string> getStringSegments(string input, char delim);
      double trackVariables(const reco::Track track, string variable);
      double muonVariables(const reco::Muon muon, string variable);
      TriggerAnalyzerModule (const edm::ParameterSet &);
      ~TriggerAnalyzerModule();
      virtual void endJob() override; 

      edm::Service<TFileService> fs;
    private:
      edm::Handle<reco::BeamSpot> BeamSpot;
      edm::Handle<trigger::TriggerEvent> triggerSummary;
      edm::InputTag beamSpot_;
      edm::InputTag recoMuon_;
      edm::InputTag recoTrack_;
      edm::InputTag triggerSummaryLabel_;
      edm::InputTag triggerResultsLabel_;
      string filterTag_;
      double ptCut_;
      vector<edm::ParameterSet> histograms_;
      edm::Handle<edm::TriggerResults> triggerResults; 
      string l1FilterName_;
      string l2FilterName_;
      string iDTag_;
      bool muonStudy_;
      bool trackStudy_;
      bool l1OnRaw_;
      bool l2Onl1_;
      vector<TH1D*> vanilaHistograms;
      vector<TH1D*> matchedHistograms;
      vector<TH1D*> effHistograms;
      double refx_;
      double refy_;
};  

//#endif
