#include "QuickTriggerAnalyzer.h"
#include <array>
using namespace std;
using namespace reco;
using namespace edm;

//An analyzer to get the hlt/reco turn on curves. wrt gen
QuickTriggerAnalyzer::QuickTriggerAnalyzer (const edm::ParameterSet &cfg) :
  beamSpot_ (cfg.getParameter<edm::InputTag> ("beamSpot")),
  recoMuon_ (cfg.getParameter<edm::InputTag> ("recoMuons")),
  triggerSummaryLabel_ (cfg.getParameter<edm::InputTag> ("triggerSummaryLabel")),
  triggerResultsLabel_ (cfg.getParameter<edm::InputTag> ("triggerResultsLabel")),
  filterTag_ (cfg.getParameter<string> ("filterTag")),
  l1FilterName_ (cfg.getParameter<string> ("l1FilterName")),
  l3FilterName_ (cfg.getParameter<string> ("l3FilterName"))
 {
  TH1D::SetDefaultSumw2 ();
  double vbins[13] = {0,20,25,30,35,38,38,40,42,46,50,60,100};
  muonPt = fs->make<TH1D>("muonPt","muonPt",12,vbins);
  muonMatchedPt = fs->make<TH1D>("muonMatchedPt","muonMatchedPt",12,vbins);
  muonEta = fs->make<TH1D>("muonEta","muonEta",50,-2.5,2.5);
  muonMatchedEta = fs->make<TH1D>("muonMatchedEta","muonMatchedEta",50,-2.5,2.5);
  muonPhi = fs->make<TH1D>("muonPhi","muonPhi",64,-3.2,3.2);
  muonMatchedPhi = fs->make<TH1D>("muonMatchedPhi","muonMatchedPhi",64,-3.2,3.2);
  muonDxy = fs->make<TH1D>("muonDxy","muonDxy",50,0,5);
  muonMatchedDxy = fs->make<TH1D>("muonMatchedDxy","muonMatchedDxy",50,0,5);
 }

QuickTriggerAnalyzer::~QuickTriggerAnalyzer ()

{
}

void
QuickTriggerAnalyzer::produce(edm::Event &event, const edm::EventSetup &setup)
{
  event.getByLabel(beamSpot_, BeamSpot);
  double BeamSpotX = BeamSpot -> x0();
  double BeamSpotY = BeamSpot -> y0();
  event.getByLabel(triggerSummaryLabel_, triggerSummary);
  event.getByLabel(triggerResultsLabel_, triggerResults);
  trigger::TriggerObjectCollection allTriggerObjects = triggerSummary->getObjects();
  string l3FilterName = l3FilterName_;
  edm::InputTag l3FilterTag = edm::InputTag(l3FilterName, "", filterTag_);
  size_t l3FilterIndex = (*triggerSummary).filterIndex(l3FilterTag);
  trigger::TriggerObjectCollection l3Objects;
  if(l3FilterIndex < (*triggerSummary).sizeFilters())
    {
       const trigger::Keys &keysObjects = (*triggerSummary).filterKeys(l3FilterIndex);
       for(size_t j = 0; j < keysObjects.size(); j++)
         {
           trigger::TriggerObject foundObject = (allTriggerObjects)[keysObjects[j]];
           l3Objects.push_back(foundObject);
         }
    }
  string l1FilterName = l1FilterName_;
  edm::InputTag l1FilterTag = edm::InputTag(l1FilterName, "", filterTag_);
  size_t l1FilterIndex = (*triggerSummary).filterIndex(l1FilterTag);
  trigger::TriggerObjectCollection l1Objects;
  if(l1FilterIndex < (*triggerSummary).sizeFilters())
    {
       const trigger::Keys &keysObjects = (*triggerSummary).filterKeys(l1FilterIndex);
       for(size_t j = 0; j < keysObjects.size(); j++)
         {
           trigger::TriggerObject foundObject = (allTriggerObjects)[keysObjects[j]];
           l1Objects.push_back(foundObject);
         }
    }
  if(event.getByLabel(recoMuon_, MuonCollection))
    {
      for (reco::MuonCollection::const_iterator recoMuon = MuonCollection ->begin(); recoMuon != MuonCollection->end(); recoMuon++)
        {
          reco::Muon thisParticle = (*recoMuon);
          if(PassMuonReco(thisParticle))
            {
              bool passL1 = false;
              for(unsigned int i = 0 ; i < l1Objects.size() ; i++)
               {
                 if(Match(thisParticle.eta(),thisParticle.phi(),l1Objects[i].eta(),l1Objects[i].phi()))
                   {
                     passL1 = true;
                   }
               }
              if(passL1)
                {
                   muonPt->Fill(thisParticle.pt());
                   muonEta->Fill(thisParticle.eta());
                   muonPhi->Fill(thisParticle.phi());
                   muonDxy->Fill(DxyBeamSpot(thisParticle.vx(),thisParticle.px(),thisParticle.vy(),thisParticle.py(),thisParticle.pt(),BeamSpotX, BeamSpotY));
                }
              bool toFill1D = false;
              for(unsigned int i = 0 ; i < l3Objects.size() ; i++)
               {
                 if(Match(thisParticle.eta(),thisParticle.phi(),l3Objects[i].eta(),l3Objects[i].phi()))
                   {
                     toFill1D = true;
                   }
               }
             if(toFill1D && passL1)
               {
                 muonMatchedPt->Fill(thisParticle.pt());
                 muonMatchedEta->Fill(thisParticle.eta());
                 muonMatchedPhi->Fill(thisParticle.phi());
                 muonMatchedDxy->Fill(DxyBeamSpot(thisParticle.vx(),thisParticle.px(),thisParticle.vy(),thisParticle.py(),thisParticle.pt(),BeamSpotX, BeamSpotY));
               }
            }
       }
   }
}

double
QuickTriggerAnalyzer::RecoMuon(const reco::Muon recoMuon, string variable)
{
  double value = 0;
  if(variable == "Pt")  value = recoMuon.pt();
  if(variable == "Eta") value = recoMuon.eta();
  if(variable == "Phi") value = recoMuon.phi();
  return value;
}

double
QuickTriggerAnalyzer::DxyBeamSpot(double vx, double px, double vy, double py, double pt, double refx, double refy)
{
  return (-(vx-refx)*py + (vy-refy)*px)/pt;
}

bool
QuickTriggerAnalyzer::Match(double eta_1, double phi_1, double eta_2, double phi_2)
{
  bool matched = deltaR(eta_1,phi_1,eta_2,phi_2) < 0.1 ? true:false;
  return matched;
}


bool
QuickTriggerAnalyzer::PassMuonReco(const reco::Muon recoMuon)
{
  //bool goodGlob = recoMuon.isGlobalMuon() && 
                      //recoMuon.globalTrack()->normalizedChi2() < 3 && 
                      //recoMuon.combinedQuality().chi2LocalPosition < 12 && 
                      //recoMuon.combinedQuality().trkKink < 20; 
  bool tightGlob = recoMuon.isGlobalMuon() &&
                      recoMuon.globalTrack()->normalizedChi2() < 10 &&
                      recoMuon.globalTrack()->hitPattern().numberOfValidMuonHits() > 0 &&
                      recoMuon.numberOfMatchedStations() > 1 &&
                      recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 &&
                      recoMuon.innerTrack()->hitPattern().trackerLayersWithMeasurement() > 5;
  //bool isMedium = recoMuon.pt() > 20 && fabs(recoMuon.eta()) < 2.4 && muon::isLooseMuon(recoMuon) && 
  //                    recoMuon.innerTrack()->validFraction() > 0.8 && 
    //                  muon::segmentCompatibility(recoMuon) > (goodGlob ? 0.303 : 0.451); 
  bool isTight = recoMuon.pt() > 42 && fabs(recoMuon.eta()) < 2.4 && tightGlob && recoMuon.isPFMuon();
  //matched = (recoMuon.isGlobalMuon() || recoMuon.isTrackerMuon())&& recoMuon.isPFMuon() && recoMuon.pt() > 20 && fabs(recoMuon.eta()) < 2.4;
  return isTight;
}

void
QuickTriggerAnalyzer::endJob()
{
 muonMatchedPt->Divide(muonMatchedPt, muonPt,1,1,"B") ;
 muonMatchedEta->Divide(muonMatchedEta, muonEta,1,1,"B") ;
 muonMatchedPhi->Divide(muonMatchedPhi, muonPhi,1,1,"B") ;
 muonMatchedDxy->Divide(muonMatchedDxy, muonDxy,1,1,"B") ;
}

DEFINE_FWK_MODULE(QuickTriggerAnalyzer);
~                                      
