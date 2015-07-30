#include "TriggerAnalyzerModule.h"
#include <array>
using namespace std;
using namespace reco;
using namespace edm;

//An analyzer to get the hlt/reco turn on curves. wrt gen
TriggerAnalyzerModule::TriggerAnalyzerModule (const edm::ParameterSet &cfg) :
  beamSpot_ (cfg.getParameter<edm::InputTag> ("beamSpot")),
  recoMuon_ (cfg.getParameter<edm::InputTag> ("recoMuons")),
  recoTrack_ (cfg.getParameter<edm::InputTag> ("recoTracks")),
  triggerSummaryLabel_ (cfg.getParameter<edm::InputTag> ("triggerSummaryLabel")),
  triggerResultsLabel_ (cfg.getParameter<edm::InputTag> ("triggerResultsLabel")),
  filterTag_ (cfg.getParameter<string> ("filterTag")),
  ptCut_ (cfg.getParameter<double> ("ptCut")),
  histograms_ (cfg.getParameter<vector<edm::ParameterSet>>("histograms_")),
  l1FilterName_ (cfg.getParameter<string> ("l1FilterName")),
  l2FilterName_ (cfg.getParameter<string> ("l2FilterName")),
  iDTag_ (cfg.getParameter<string> ("iDTag")),
  muonStudy_ (cfg.getParameter<bool> ("muonStudy")),
  trackStudy_ (cfg.getParameter<bool> ("trackStudy")),
  l1OnRaw_ (cfg.getParameter<bool> ("l1OnRaw")),
  l2Onl1_ (cfg.getParameter<bool> ("l2Onl1"))
 {
  TH1D::SetDefaultSumw2 ();  
  for(uint i =0; i < histograms_.size(); i++)
    {
      TH1D* tmpHist =  fs->make<TH1D>(TString(histograms_[i].getParameter<string>("variable")),TString(histograms_[i].getParameter<string>("variable")),int(histograms_[i].getParameter<int>("nBins")),histograms_[i].getParameter<double>("lBin"),histograms_[i].getParameter<double>("hBin"));     
      vanilaHistograms.push_back(tmpHist);  
      TH1D* tmpMatchedHist = fs->make<TH1D>(TString(histograms_[i].getParameter<string>("variable") + "_matched"),TString(histograms_[i].getParameter<string>("variable") + "_matched"),int(histograms_[i].getParameter<int>("nBins")),histograms_[i].getParameter<double>("lBin"),histograms_[i].getParameter<double>("hBin"));
      matchedHistograms.push_back(tmpMatchedHist);  
      TH1D* tmpEffHist = fs->make<TH1D>(TString(histograms_[i].getParameter<string>("variable") + "_eff"),TString(histograms_[i].getParameter<string>("variable") + "_eff"),int(histograms_[i].getParameter<int>("nBins")),histograms_[i].getParameter<double>("lBin"),histograms_[i].getParameter<double>("hBin"));
      effHistograms.push_back(tmpEffHist);  
    } 
 }

TriggerAnalyzerModule::~TriggerAnalyzerModule ()

{
}

void
TriggerAnalyzerModule::produce(edm::Event &event, const edm::EventSetup &setup)
{  
  if(event.getByLabel(beamSpot_, BeamSpot))
    {
      refx_ = BeamSpot->x0();
      refy_ = BeamSpot->y0();
    }
  else
   {
     refx_ = 0;
     refy_ = 0;
   }
  event.getByLabel(triggerSummaryLabel_, triggerSummary);
  event.getByLabel(triggerResultsLabel_, triggerResults); 
  trigger::TriggerObjectCollection allTriggerObjects = triggerSummary->getObjects(); 
  string l2FilterName = l2FilterName_;
  edm::InputTag l2FilterTag = edm::InputTag(l2FilterName, "", filterTag_);
  size_t l2FilterIndex = (*triggerSummary).filterIndex(l2FilterTag);
  trigger::TriggerObjectCollection l2Objects;
  if(l2FilterIndex < (*triggerSummary).sizeFilters())
    { 
       const trigger::Keys &keysObjects = (*triggerSummary).filterKeys(l2FilterIndex);
       for(size_t j = 0; j < keysObjects.size(); j++)
         {
           trigger::TriggerObject foundObject = (allTriggerObjects)[keysObjects[j]];
           l2Objects.push_back(foundObject);
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
  if(muonStudy_)
    {
      vector<reco::Muon> slimmedRecoMuon = recoMuonSelector(recoMuon_, event);
      vector<reco::Muon> l1MatchedRecoMuon = recoMuonLooper(l1Objects, slimmedRecoMuon);
      vector<reco::Muon> l2MatchedRecoMuon = recoMuonLooper(l2Objects, l1MatchedRecoMuon); 
      if(l1OnRaw_)
        {
          for(uint j = 0; j < vanilaHistograms.size(); j++)
            {
              fillMuonHistogram(slimmedRecoMuon, vanilaHistograms[j]);    
              fillMuonHistogram(l1MatchedRecoMuon, matchedHistograms[j]);
              fillMuonHistogram(l1MatchedRecoMuon, effHistograms[j]);
            }
        }  
      if(l2Onl1_)
        {
          for(uint j = 0; j < vanilaHistograms.size(); j++)
            {
              fillMuonHistogram(l1MatchedRecoMuon, vanilaHistograms[j]);    
              fillMuonHistogram(l2MatchedRecoMuon, matchedHistograms[j]);
              fillMuonHistogram(l2MatchedRecoMuon, effHistograms[j]);
            }
         }
       l2MatchedRecoMuon.clear();
       l1MatchedRecoMuon.clear();
       slimmedRecoMuon.clear();
    }
  if(trackStudy_)
    {
      vector<reco::Track> slimmedRecoTrack = recoTrackSelector(recoTrack_, event);
      vector<reco::Track> l1MatchedRecoTrack = recoTrackLooper(l1Objects, slimmedRecoTrack);
      vector<reco::Track> l2MatchedRecoTrack = recoTrackLooper(l2Objects, l1MatchedRecoTrack); 
      if(l1OnRaw_)
        {
          for(uint j = 0; j < vanilaHistograms.size(); j++)
            {
              fillTrackHistogram(slimmedRecoTrack, vanilaHistograms[j]);    
              fillTrackHistogram(l1MatchedRecoTrack, matchedHistograms[j]);
              fillTrackHistogram(l1MatchedRecoTrack, effHistograms[j]);
            }
        }  
      if(l2Onl1_)
        {
          for(uint j = 0; j < vanilaHistograms.size(); j++)
            {
              fillTrackHistogram(l1MatchedRecoTrack, vanilaHistograms[j]);    
              fillTrackHistogram(l2MatchedRecoTrack, matchedHistograms[j]);
              fillTrackHistogram(l2MatchedRecoTrack, effHistograms[j]);
            }
         }
       l2MatchedRecoTrack.clear();
       l1MatchedRecoTrack.clear();
       slimmedRecoTrack.clear();
     }
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
TriggerAnalyzerModule::fillMuonHistogram(vector<reco::Muon> recoMuonSet, TH1D* histogram)
{
  for (unsigned int j = 0 ; j < recoMuonSet.size() ; j++)
    {
       vector<string> name = getStringSegments(string(histogram->GetName()), '_');
       double value = muonVariables(recoMuonSet[j], name[0]);
       histogram->Fill(value);
    }
}

void
TriggerAnalyzerModule::fillTrackHistogram(vector<reco::Track> recoTrackSet, TH1D* histogram)
{
  for (unsigned int j = 0 ; j < recoTrackSet.size() ; j++)
    {
       vector<string> name = getStringSegments(string(histogram->GetName()), '_');
       double value = trackVariables(recoTrackSet[j], name[0]);
       histogram->Fill(value);
    }
}

double
TriggerAnalyzerModule::trackVariables(const reco::Track track, string variable)
{
  double value = 0;
  if(variable == "Pt") value = track.pt(); 
  if(variable == "Eta") value = track.eta(); 
  if(variable == "Phi") value = track.phi(); 
  if(variable == "Dxy") value = dxyBeamSpot(track.vx(), track.px(), track.vy(), track.py(), track.pt(), refx_, refy_); 
  return value;
}

double
TriggerAnalyzerModule::muonVariables(const reco::Muon muon, string variable)
{
  double value = 0;
  if(variable == "Pt") value = muon.pt(); 
  if(variable == "Eta") value = muon.eta(); 
  if(variable == "Phi") value = muon.phi(); 
  if(variable == "Dxy") value = dxyBeamSpot(muon.vx(), muon.px(), muon.vy(), muon.py(), muon.pt(), refx_, refy_); 
  return value;
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
          bool tight = recoMuon->isGlobalMuon() && 
                       recoMuon->globalTrack()->normalizedChi2() < 10 && 
                       recoMuon->globalTrack()->hitPattern().numberOfValidMuonHits() > 0 && 
                       recoMuon->numberOfMatchedStations() > 1 &&
                       recoMuon->innerTrack()->hitPattern().numberOfValidPixelHits() > 0 &&
                       recoMuon->innerTrack()->hitPattern().trackerLayersWithMeasurement() > 5 &&
                       recoMuon->pt() > ptCut_ && fabs(recoMuon->eta()) < 2.4 && recoMuon->isPFMuon(); 
          //bool goodGlob = recoMuon->isGlobalMuon() && 
          //                recoMuon->globalTrack()->normalizedChi2() < 3; 
          //                recoMuon->combinedQuality().chi2LocalPosition < 12 && 
          //                recoMuon->combinedQuality().trkKink < 20;
          //bool medium =  recoMuon->innerTrack()->validFraction() > 0.8 &&
          //               muon::segmentCompatibility(thisParticle) > (goodGlob ? 0.303 : 0.451) &&
          //               muon::isLooseMuon(thisParticle) &&
          //               recoMuon->pt() > ptCut_ && fabs(recoMuon->eta()) < 2.4 &&  goodGlob;
          //bool loose = muon::isLooseMuon(thisParticle);
          //             recoMuon->isGlobalMuon() && recoMuon->isPFMuon();
          //bool passId = tagToBool(iDTag_, loose, medium, tight);
          //if(passId)
          if(tight)
            {
              selectedRecoMuon.push_back(thisParticle);
            }
          }
       }
  return selectedRecoMuon;
  selectedRecoMuon.clear();
}

vector<reco::Track> 
TriggerAnalyzerModule::recoTrackSelector(const edm::InputTag recoTrackTag, edm::Event &event)
{
  vector<reco::Track> selectedRecoTrack;
  edm::Handle<reco::TrackCollection> TrackCollection;
  if(event.getByLabel(recoTrackTag, TrackCollection))
    {
      for (reco::TrackCollection::const_iterator recoTrack = TrackCollection ->begin(); recoTrack != TrackCollection->end(); recoTrack++) 
        {
          reco::Track thisParticle = (*recoTrack);
          bool tight =  recoTrack->normalizedChi2() < 10 && 
                                recoTrack->hitPattern().numberOfValidMuonHits() > 0 && 
                                recoTrack->hitPattern().muonStationsWithValidHits() > 1 &&
                                recoTrack->hitPattern().numberOfValidPixelHits() > 0 &&
                                recoTrack->hitPattern().trackerLayersWithMeasurement() > 5 &&
                                recoTrack->pt() > ptCut_ && fabs(recoTrack->eta()) < 2.4;
          bool medium = recoTrack->normalizedChi2() < 10 && 
                                recoTrack->hitPattern().numberOfValidMuonHits() > 0 && 
                                recoTrack->hitPattern().muonStationsWithValidHits() > 1 &&
                                recoTrack->hitPattern().trackerLayersWithMeasurement() > 3 &&
                                recoTrack->pt() > ptCut_ && fabs(recoTrack->eta()) < 2.4;
          bool loose = recoTrack->normalizedChi2() < 10 && 
                                recoTrack->hitPattern().numberOfValidMuonHits() > 0 && 
                                recoTrack->hitPattern().muonStationsWithValidHits() > 1 &&
                                recoTrack->pt() > ptCut_ && fabs(recoTrack->eta()) < 2.4;
          bool passId = tagToBool(iDTag_, loose, medium, tight);  
          if(passId)
            {
              selectedRecoTrack.push_back(thisParticle);
            }
          }
       }
  return selectedRecoTrack;
  selectedRecoTrack.clear();
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

vector<reco::Track>
TriggerAnalyzerModule::recoTrackLooper(const trigger::TriggerObjectCollection Objects, vector<reco::Track> recoTrackSet)
{
  vector<reco::Track> matchedRecoTrack;
  if(recoTrackSet.size()) 
    {
      for (unsigned int j = 0 ; j < recoTrackSet.size() ; j++)
        {
          reco::Track thisParticle = recoTrackSet[j];
          for(unsigned int i = 0 ; i < Objects.size() ; i++)
            {                 
              if(match(thisParticle.eta(),thisParticle.phi(),Objects[i].eta(),Objects[i].phi()))
                {
                  matchedRecoTrack.push_back(thisParticle);
                }
            }  
         }
     }
  return matchedRecoTrack;
  matchedRecoTrack.clear();
}

bool
TriggerAnalyzerModule::tagToBool(const string tag, bool loose, bool medium, bool tight)
{
 bool iD = false;
 if(tag == "loose") iD = loose;
 if(tag == "medium") iD = medium;
 if(tag == "tight") iD = tight;
 return iD;
}

vector<string>
TriggerAnalyzerModule::getStringSegments(string input, char delim)
{
    vector<string> elems;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


void
TriggerAnalyzerModule::endJob()
{
  for(uint m = 0; m < effHistograms.size(); m++)
    {
      effHistograms[m]->Divide(effHistograms[m], vanilaHistograms[m],1,1,"B"); 
    }
}
DEFINE_FWK_MODULE(TriggerAnalyzerModule);
