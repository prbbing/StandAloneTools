#include "NoBPTXAnalyzer.h"
using namespace std;
using namespace reco;
using namespace edm;

// An analyzer to select cosmic muons. There are cuts on angle and halo veto applied.


NoBPTXAnalyzer::NoBPTXAnalyzer (const edm::ParameterSet &cfg) :
  recoMuon_ (cfg.getParameter<edm::InputTag> ("recoMuons")),
  triggerResultsLabel_ (cfg.getParameter<edm::InputTag> ("triggerResultsLabel")),
  cscSegment_ (cfg.getParameter<edm::InputTag> ("cscSegments"))
  {
    TH1D::SetDefaultSumw2 ();  
    TH2D::SetDefaultSumw2 ();  
    RecoGlobalMuonPt = fs->make<TH1D>("RecoGlobalMuonPt","RecoGlobalMuonPt",500,0,500);
    RecoGlobalMuonPhi = fs->make<TH1D>("RecoGlobalMuonPhi","RecoGlobalMuonPhi",64,-3.2,3.2);
    RecoGlobalMuonEta = fs->make<TH1D>("RecoGlobalMuonEta","RecoGlobalMuonEta",50,-2.5,2.5);
    RecoGlobalMuonD0 = fs->make<TH1D>("RecoGlobalMuonD0","RecoGlobalMuonD0",500,0,50);
    RecoGlobalMuonDZ = fs->make<TH1D>("RecoGlobalMuonDZ","RecoGlobalMuonDZ",500,0,50);
    NearByCSCSegmentLayer = fs->make<TH1D>("NearByCSCSegmentLayer","NearByCSCSegmentLayer",6,0,6);
    GlobalMuonDeltaPhi = fs->make<TH1D>("GlobalMuonDeltaPhi","GlobalMuonDeltaPhi",32,0,3.2); 
    GlobalMuonDeltaDZ = fs->make<TH1D>("GlobalMuonDeltaDZ","GlobalMuonDeltaDZ",1000,0,100); 
    GlobalMuonDeltaD0 = fs->make<TH1D>("GlobalMuonDeltaD0","GlobalMuonDeltaD0",1000,0,100); 
    GlobalMuonDeltaPt = fs->make<TH1D>("GlobalMuonDeltaPt","GlobalMuonDeltaPt",1000,0,1000); 
    GlobalMuonPhiEta = fs->make<TH2D>("GlobalMuonPhiEta","GlobalMuonPhiEta",64,-3.2,3.2,50,-2.5,2.5); 
  }
NoBPTXAnalyzer::~NoBPTXAnalyzer()
{
}
void
NoBPTXAnalyzer::analyze(const edm::Event &event, const edm::EventSetup &setup)
{ 
  string pathString = "HLT_L2Mu20_NoVertex_2Cha_NoBPTX3BX_NoHalo";
  bool passTrigger = false;
  event.getByLabel(triggerResultsLabel_, triggerResults);
  event.getByLabel(cscSegment_, cscSegmentCollection);
  setup.get<MuonGeometryRecord>().get(cscGeometry);
  if(triggerResults.isValid()){
    const edm::TriggerNames & trigNames = event.triggerNames(*triggerResults);
    unsigned int numTriggers = trigNames.size();
    for( unsigned int hltIndex=0; hltIndex<numTriggers-1; ++hltIndex ){
      std::string currentTrigName = trigNames.triggerName(hltIndex);
      if (currentTrigName.find(pathString) < currentTrigName.length() && triggerResults->accept(hltIndex))
       {passTrigger = true;}
     } }
    if(event.getByLabel(recoMuon_, MuonCollection) && passTrigger )
    {
      for(reco::MuonCollection::const_iterator recoMuon = MuonCollection->begin(); recoMuon != MuonCollection->end(); recoMuon++)
        { 
          reco::Muon thisParticle = (*recoMuon);
          if(PassMuonReco(thisParticle) && PassHaloVeto(recoMuon->phi()))
            {
              bool isCosmic = false;
              double refPt = 0;
              double refPhi = 0;
              double refDZ = 0;
              double refD0 = 0;
              for(reco::MuonCollection::const_iterator recoMuon2 = MuonCollection->begin(); recoMuon2 != MuonCollection->end(); recoMuon2++)
                {
                  if(recoMuon->phi() != recoMuon2->phi() && recoMuon2->isGlobalMuon() && PassHaloVeto(recoMuon2->phi()))
                    {
                        reco::Muon thatParticle = (*recoMuon2); 
                        isCosmic = PassCosmic(thisParticle, thatParticle);      
                        if(isCosmic){
                           refPt = thatParticle.pt();
                           refPhi = thatParticle.phi();
                           refD0 = thatParticle.innerTrack()->dxy();
                           refDZ = thatParticle.innerTrack()->dz();
                    }
                }
             }
             if (isCosmic){
                      RecoGlobalMuonPt->Fill(recoMuon->pt());
                      RecoGlobalMuonEta->Fill(recoMuon->eta());
                      RecoGlobalMuonPhi->Fill(recoMuon->phi());
                      RecoGlobalMuonD0->Fill(fabs(recoMuon->innerTrack()->dxy()));
                      RecoGlobalMuonDZ->Fill(fabs(recoMuon->innerTrack()->dz()));
                      GlobalMuonDeltaPhi->Fill(fabs(deltaPhi(recoMuon->phi(),refPhi)));
                      GlobalMuonDeltaDZ->Fill(fabs(recoMuon->innerTrack()->dz() - refDZ));
                      GlobalMuonDeltaD0->Fill(fabs(recoMuon->innerTrack()->dxy() - refD0));
                      GlobalMuonDeltaPt->Fill(fabs(recoMuon->pt() - refPt));
                      GlobalMuonPhiEta->Fill(recoMuon->phi(),recoMuon->eta());
             }
            }
         }
     }
}

bool 
NoBPTXAnalyzer::PassMuonReco(const reco::Muon recoMuon)
{
  bool matched = false;
  //matched = recoMuon.pt() > 25 && recoMuon.isGlobalMuon() > 0 && recoMuon.isPFMuon() > 0 && recoMuon.globalTrack()->normalizedChi2() < 10 && recoMuon.globalTrack()->hitPattern().numberOfValidMuonHits() > 0 && recoMuon.numberOfMatchedStations() > 1 && recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && recoMuon.track()->hitPattern().trackerLayersWithMeasurement() > 5;
  matched = recoMuon.isGlobalMuon() > 0; //&& recoMuon.time().direction() != 0; //&& recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && recoMuon.isPFMuon() > 0 && recoMuon.globalTrack()->normalizedChi2() < 10 && recoMuon.globalTrack()->hitPattern().numberOfValidMuonHits() > 0 && recoMuon.numberOfMatchedStations() > 1 && recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && recoMuon.track()->hitPattern().trackerLayersWithMeasurement() > 5;
  //matched = recoMuon.pt() > 25 && recoMuon.isGlobalMuon() > 0;
  return matched;
}


bool
NoBPTXAnalyzer::PassHaloVeto(const double recoMuonPhi)
{
  bool pass = false;
  set<int> numLayers;
  if (cscSegmentCollection.isValid()){
    for(CSCSegmentCollection::const_iterator cscSegment = cscSegmentCollection->begin(); cscSegment != cscSegmentCollection->end(); cscSegment++){
      CSCDetId id  = (CSCDetId)cscSegment->cscDetId();
      LocalPoint localPos = cscSegment->localPosition();
      GlobalPoint globalPos = cscGeometry->chamber(id)->toGlobal(localPos);
      double cscSegmentPhi = globalPos.phi();
      int cscSegmentStation = id.station();
      int cscSegmentRing = id.ring();
      double cscSegmentZ = globalPos.z();
      int chamber = ChamberType(cscSegmentStation, cscSegmentRing);
      int endcap = (cscSegmentZ > 0) ? 1 : -1;
      int layer = chamber*endcap;
      if(deltaPhi(recoMuonPhi, cscSegmentPhi) < 0.4){
        numLayers.insert(layer);}
      }
    }
  NearByCSCSegmentLayer->Fill(numLayers.size());  
  if(numLayers.size() < 3){
    pass = true;
  }
  return pass;         
}


int 
NoBPTXAnalyzer::ChamberType(int Station, int Ring) {
    int i = 2*Station + Ring;
    if (Station == 1) {
      i = i - 1;
      if (i>4) i = 1;
    }
    return i;
  }

bool 
NoBPTXAnalyzer::PassCosmic(const reco::Muon recoMuon, const reco::Muon recoMuon2)
{
  bool matched = false;
  static const double pi = 3.1415926535897932384626433832795028841971693993751058;
  TVector3 threeVector1(recoMuon.px(), recoMuon.py(), recoMuon.pz());
  TVector3 threeVector2(recoMuon2.px(), recoMuon2.py(), recoMuon2.pz());
  double value = (pi-threeVector1.Angle(threeVector2));
  if(log(value) < -2.8){matched = true;} 
  return matched;
}
  
DEFINE_FWK_MODULE(NoBPTXAnalyzer);    
