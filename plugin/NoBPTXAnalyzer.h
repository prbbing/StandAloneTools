#include <string>

#include <iostream>
#include <algorithm>
#include <regex>
#include "math.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/CSCRecHit/interface/CSCSegment.h"
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCChamber.h"
#include "Geometry/CSCGeometry/interface/CSCLayer.h"
#include "Geometry/CSCGeometryBuilder/plugins/CSCGeometryESModule.h"
#include "Geometry/CSCGeometry/interface/CSCLayerGeometry.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "TVector3.h"
using namespace std;

class NoBPTXAnalyzer : public edm::EDAnalyzer
{
    public:
      void analyze (const edm::Event &, const edm::EventSetup &);
      bool PassMuonReco(const reco::Muon recoMuon);
      bool PassHaloVeto(const double recoMuonPhi);
      bool PassCosmic(const reco::Muon recoMuon, const reco::Muon recoMuon2);
      int ChamberType(int Station, int Ring);
      NoBPTXAnalyzer (const edm::ParameterSet &);
      ~NoBPTXAnalyzer();
      edm::Service<TFileService> fs;
    private:
      edm::Handle<reco::MuonCollection> MuonCollection;
      edm::Handle<trigger::TriggerEvent> triggerSummary;
      edm::Handle<CSCSegmentCollection> cscSegmentCollection;
      edm::ESHandle<CSCGeometry> cscGeometry;
      edm::InputTag recoMuon_;
      edm::InputTag triggerResultsLabel_;
      edm::InputTag cscSegment_;
      edm::Handle<edm::TriggerResults> triggerResults; 
      TH1D* RecoGlobalMuonPt; 
      TH1D* RecoGlobalMuonPhi;
      TH1D* RecoGlobalMuonEta;
      TH1D* RecoGlobalMuonD0;
      TH1D* RecoGlobalMuonDZ;
      TH1D* GlobalMuonDeltaPhi;
      TH1D* GlobalMuonDeltaDZ;
      TH1D* GlobalMuonDeltaD0;
      TH1D* GlobalMuonDeltaPt;
      TH1D* NearByCSCSegmentLayer;
      TH2D* GlobalMuonPhiEta;
};  
