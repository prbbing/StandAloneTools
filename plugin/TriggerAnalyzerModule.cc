#include "TriggerAnalyzerModule.h"
#include <array>
using namespace std;
using namespace reco;
using namespace edm;

//An analyzer to get the efficiency w.r.t reco quantities. 
TriggerAnalyzerModule::TriggerAnalyzerModule (const edm::ParameterSet &cfg) :
  beamSpot_ (cfg.getParameter<edm::InputTag> ("beamSpot")),
  recoMuon_ (cfg.getParameter<edm::InputTag> ("recoMuons")),
  triggerSummaryLabel_ (cfg.getParameter<edm::InputTag> ("triggerSummaryLabel")),
  triggerResultsLabel_ (cfg.getParameter<edm::InputTag> ("triggerResultsLabel")),
  filterTag_ (cfg.getParameter<string> ("filterTag")),
  ptCut_ (cfg.getParameter<double> ("ptCut")),
  l1FilterName_ (cfg.getParameter<string> ("l1FilterName")),
  l3FilterName_ (cfg.getParameter<string> ("l3FilterName"))
 {
  TH1D::SetDefaultSumw2 ();
  double vbins[17] = {0,20,25,30,35,36,37,38,39,40,41,42,45,50,60,100,200};
  muonPt = fs->make<TH1D>("muonPt","muonPt",16,vbins);
  muonMatchedPt = fs->make<TH1D>("muonMatchedPt","muonMatchedPt",16,vbins);
 }

TriggerAnalyzerModule::~TriggerAnalyzerModule ()

{
}

void
TriggerAnalyzerModule::produce(edm::Event &event, const edm::EventSetup &setup)
{
  event.getByLabel(beamSpot_, BeamSpot);
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
  vector<reco::Muon> slimmedRecoMuon = recoMuonSelector(recoMuon_, event);
  vector<reco::Muon> l1MatchedRecoMuon = recoMuonLooper(l1Objects, slimmedRecoMuon);
  vector<reco::Muon> l3MatchedRecoMuon = recoMuonLooper(l3Objects, l1MatchedRecoMuon);
  fillHistogram(l1MatchedRecoMuon, muonPt);
  fillHistogram(l3MatchedRecoMuon, muonMatchedPt);
  slimmedRecoMuon.clear();
  l1MatchedRecoMuon.clear();
  l3MatchedRecoMuon.clear();
}

double
TriggerAnalyzerModule::dxyBeamSpot(double vx, double px, double vy, double py, double pt, double refx, double refy)
{
  return (-(vx-refx)*py + (vy-refy)*px)/pt;
}


bool
TriggerAnalyzerModule::match(double eta_1, double phi_1, double eta_2, double phi_2)
{
  bool matched = deltaR(eta_1,phi_1,eta_2,phi_2) < 0.1 ? true:false;
  return matched;
}


void
TriggerAnalyzerModule::fillHistogram(vector<reco::Muon> recoMuonSet, TH1D* histogram)
{
  for (unsigned int j = 0 ; j < recoMuonSet.size() ; j++)
    {
       histogram->Fill(recoMuonSet[j].pt());
    }
}
vector<reco::Muon>
TriggerAnalyzerModule::recoMuonSelector(const edm::InputTag recoMuonTag, edm::Event &event)
{
  vector<reco::Muon> selectedRecoMuon;
  edm::Handle<reco::MuonCollection> MuonCollection;
  if(event.getByLabel(recoMuonTag, MuonCollection))
    {
      for (reco::MuonCollection::const_iterator recoMuon = MuonCollection ->begin(); recoMuon != MuonCollection->end(); recoMuon++)
        {
          reco::Muon thisParticle = (*recoMuon);
          bool tightId = recoMuon->isGlobalMuon() &&
                         recoMuon->globalTrack()->normalizedChi2() < 10 &&
                         recoMuon->globalTrack()->hitPattern().numberOfValidMuonHits() > 0 &&
                         recoMuon->numberOfMatchedStations() > 1 &&
                         recoMuon->innerTrack()->hitPattern().numberOfValidPixelHits() > 0 &&
                         recoMuon->innerTrack()->hitPattern().trackerLayersWithMeasurement() > 5 &&
                         recoMuon->pt() > ptCut_ && fabs(recoMuon->eta()) < 2.4 && recoMuon->isPFMuon();
          if(tightId)
            {
              selectedRecoMuon.push_back(thisParticle);
            }
          }
       }
  return selectedRecoMuon;
  selectedRecoMuon.clear();
}

vector<reco::Muon>
TriggerAnalyzerModule::recoMuonLooper(const trigger::TriggerObjectCollection Objects, vector<reco::Muon> recoMuonSet)
{
  vector<reco::Muon> matchedRecoMuon;
  if(recoMuonSet.size())
    {
      for (unsigned int j = 0 ; j < recoMuonSet.size() ; j++)
        {
          reco::Muon thisParticle = recoMuonSet[j];
          for(unsigned int i = 0 ; i < Objects.size() ; i++)
            {
              if(match(thisParticle.eta(),thisParticle.phi(),Objects[i].eta(),Objects[i].phi()))
                {
                  matchedRecoMuon.push_back(thisParticle);
                }
            }
         }
     }
  return matchedRecoMuon;
  matchedRecoMuon.clear();
}

void
TriggerAnalyzerModule::endJob()
{
 muonMatchedPt->Divide(muonMatchedPt, muonPt,1,1,"B") ;
}

DEFINE_FWK_MODULE(TriggerAnalyzerModule);
~                                                     
