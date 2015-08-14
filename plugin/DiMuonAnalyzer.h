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
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

using namespace std;

class DiMuonAnalyzer : public edm::EDAnalyzer
{
  public:
      void analyze (const edm::Event &, const edm::EventSetup &);
      double dxyBeamSpot(double vx, double px, double vy, double py, double pt, double refx, double refy);
      vector<reco::Muon> basicMuonCollection(const edm::InputTag recoMuonTag, const edm::Event &event);
      vector<vector<reco::Muon>> muonPairs (vector<reco::Muon> inputMuonSet);
      void fillMuonHistogram(vector<reco::Muon> recoMuonSet, TH1D* histogram);
      void fillMuon2DHistogram(vector<reco::Muon> recoMuonSet, TH2D* histogram);
      void fillMuonPairHistogram(vector<vector<reco::Muon>> recoMuonPair, TH1D* histogram);
      bool passPairSelection(const reco::Muon first, const reco::Muon second);
      bool evaluateTrigger(vector<string> triggerList, const edm::Event &event);
      double muonVariables(const reco::Muon muon, string variable);
      double muonPairVariables(const vector<reco::Muon> muonPair, string variable);
      vector<string> getStringSegments(string input, char delim);
      DiMuonAnalyzer (const edm::ParameterSet &);
      ~DiMuonAnalyzer();
      edm::Service<TFileService> fs;
  private:
      edm::Handle<reco::BeamSpot> BeamSpot;
      edm::Handle<edm::TriggerResults> triggerResults;
      edm::InputTag beamSpot_;
      edm::InputTag recoMuon_;
      edm::InputTag event_;
      edm::InputTag triggerResultsLabel_;
      string basicMuonSelections_;
      vector<edm::ParameterSet> histograms_;
      vector<edm::ParameterSet> twoDHistograms_;
      vector<TH1D*> histograms;
      vector<TH2D*> twoDHistograms;
      vector<string> triggerNames_;
      vector<edm::ParameterSet> pairHistograms_;
      vector<TH1D*> pairHistograms;
      double refx_;
      double refy_;
};


