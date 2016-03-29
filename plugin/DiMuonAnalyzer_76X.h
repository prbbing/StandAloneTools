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
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

using namespace std;

class DiMuonAnalyzer : public edm::EDAnalyzer
{
  public:
      void analyze (const edm::Event &, const edm::EventSetup &);
      double dxyBeamSpot(double vx, double px, double vy, double py, double pt, double refx, double refy);
      double dxyBeamSpotError(double trackD0Error, double beamSpotX0Error, double beamSpotY0Error);
      vector<pat::Muon> basicMuonCollection(const edm::EDGetTokenT<vector<pat::Muon>> recoMuonTag, const edm::Event &event);
      vector<vector<pat::Muon>> muonPairs (vector<pat::Muon> inputMuonSet);
      vector<pat::Muon> probeMuonFinder (const edm::EDGetTokenT<vector<pat::Muon>> recoMuonTag, const edm::Event &event, vector<pat::Muon> tagMuonSet);
      vector<pat::Muon> probeMuonMatcher (pat::Muon probeMuon, vector<pat::TriggerObjectStandAlone> filterObjects);
      void fillMuonHistogram(vector<pat::Muon> recoMuonSet, TH1D* histogram);
      void fillMuon2DHistogram(vector<pat::Muon> recoMuonSet, TH2D* histogram);
      void fillMuonPairHistogram(vector<vector<pat::Muon>> recoMuonPair, TH1D* histogram);
      bool passPairSelection(const pat::Muon first, const pat::Muon second);
      bool evaluateTrigger(vector<string> triggerList, const edm::Event &event);
      double muonVariables(const pat::Muon muon, string variable);
      double muonPairVariables(const vector<pat::Muon> muonPair, string variable);
      vector<string> getStringSegments(string input, char delim);
      DiMuonAnalyzer (const edm::ParameterSet &);
      ~DiMuonAnalyzer();
      edm::Service<TFileService> fs;
  private:
      edm::Handle<reco::BeamSpot> BeamSpot;
      edm::Handle<edm::TriggerResults> triggerBits; 
      edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
      edm::EDGetTokenT<reco::BeamSpot> beamSpot_;
      edm::EDGetTokenT<vector<pat::Muon>> recoMuon_;
      edm::EDGetTokenT<edm::TriggerResults> triggerBitsLabel_;
      edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjectsLabel_;
      string basicMuonSelections_;
      string probeMuonSelections_;
      string filterName_;
      vector<edm::ParameterSet> histograms_;
      vector<edm::ParameterSet> probeHistograms_;
      vector<edm::ParameterSet> passedProbeHistograms_;
      vector<edm::ParameterSet> twoDHistograms_;
      vector<TH1D*> histograms;
      vector<TH1D*> probeHistograms;
      vector<TH1D*> passedProbeHistograms;
      vector<TH2D*> twoDHistograms;
      vector<string> triggerNames_;
      vector<edm::ParameterSet> pairHistograms_;
      vector<TH1D*> pairHistograms;
      double refx_;
      double refy_;
      double refxError_;
      double refyError_;
};


