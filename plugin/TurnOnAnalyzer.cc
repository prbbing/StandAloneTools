#include "TurnOnAnalyzer.h"
#include <array>
using namespace std;
using namespace reco;
using namespace edm;

//An analyzer to get the hlt/reco turn on curves. wrt gen
TurnOnAnalyzer::TurnOnAnalyzer (const edm::ParameterSet &cfg) :
  recoMuon_ (cfg.getParameter<edm::InputTag> ("recoMuons")),
  recoTrack_ (cfg.getParameter<edm::InputTag> ("recoTracks")),
  triggerSummaryLabel_ (cfg.getParameter<edm::InputTag> ("triggerSummaryLabel")),
  triggerResultsLabel_ (cfg.getParameter<edm::InputTag> ("triggerResultsLabel")),
  histogramSets_ (cfg.getParameter<vector<edm::ParameterSet>>("histogramSets")),
  beamSpot_ (cfg.getParameter<edm::InputTag> ("beamSpot")),
  genParticle_ (cfg.getParameter<edm::InputTag> ("genParticle")),
  l3Muon_ (cfg.getParameter<edm::InputTag> ("l3Muons")),
  l2Muon_ (cfg.getParameter<edm::InputTag> ("l2Muons")),
  d0cut_ (cfg.getParameter<double> ("d0cut")),
  filterTag_ (cfg.getParameter<string> ("filterTag")),
  FilterEff_ (cfg.getParameter<bool> ("FilterEff")),
  L3Eff_ (cfg.getParameter<bool> ("L3Eff")),
  RecoMuonEff_ (cfg.getParameter<bool> ("RecoMuonEff")),
  RecoTrackEff_ (cfg.getParameter<bool> ("RecoTrackEff")),
  L2Eff_ (cfg.getParameter<bool> ("L2Eff")),
  primaryVertex_ (cfg.getParameter<edm::InputTag> ("primaryVertex"))
 {
  TH1D::SetDefaultSumw2 ();  
  TH2D::SetDefaultSumw2 ();  
  stopDecayLength = fs->make<TH1D>("stopDecayLength","stopDecayLength",1000,0,1000);
  for (uint currentHistSet = 0; currentHistSet != histogramSets_.size();currentHistSet++)
    {
      string currentDirName = histogramSets_.at(currentHistSet).getParameter<string>("setName");
      TFileDirectory subDir = fs->mkdir(currentDirName);
      map<string,TH1D*> tmpOneDHistMap;
      oneDHistSetMap.insert(pair<string, map<string,TH1D*>>(currentDirName, tmpOneDHistMap));
      map<string,TH2D*> tmpTwoDHistMap;
      twoDHistSetMap.insert(pair<string, map<string,TH2D*>>(currentDirName, tmpTwoDHistMap));
      vector<edm::ParameterSet> hists = histogramSets_.at(currentHistSet).getParameter<vector<edm::ParameterSet>>("hists");
      vector<string> tmpFilters = histogramSets_.at(currentHistSet).getParameter<vector<string>>("filters"); 
      filterMap.insert(pair<string,vector<string>>(currentDirName,tmpFilters));
      for (uint currentHist = 0; currentHist != hists.size(); currentHist++)
        {
          string histName = hists.at(currentHist).getParameter<string>("name");
          vector<double> histBinning = hists.at(currentHist).getParameter<vector<double>>("binning");
          if (histBinning.size() == 3)
	    {
	      TH1D *tmpHist = subDir.make<TH1D> (TString(histName), "", int(histBinning[0]), histBinning[1], histBinning[2]);
              oneDHistSetMap.find(currentDirName)->second.insert(pair<string,TH1D*>(histName, tmpHist));
              for(uint currentFilter = 0; currentFilter != tmpFilters.size(); currentFilter++)
	       {
                 TH1D *tmpHist = subDir.make<TH1D> (TString(histName + "_MatchedWith_" + tmpFilters.at(currentFilter)), "", int(histBinning[0]), histBinning[1], histBinning[2]);
                 oneDHistSetMap.find(currentDirName)->second.insert(pair<string,TH1D*>(histName + "_MatchedWith_" + tmpFilters.at(currentFilter), tmpHist));
                 TH1D *tmpEffHist = subDir.make<TH1D> (TString(histName + "_Eff_" + tmpFilters.at(currentFilter)), "", int(histBinning[0]), histBinning[1], histBinning[2]);
                 oneDHistSetMap.find(currentDirName)->second.insert(pair<string,TH1D*>(histName + "_Eff_" + tmpFilters.at(currentFilter), tmpEffHist));
               }
	    }
          if (histBinning.size() > 7)
	    {
              int numBins = int(histBinning[0]);
              double *vbins=new double[numBins+1];
              for(uint i = 1; i != histBinning.size();i++)
                {
                   vbins[i-1] = histBinning[i];
                } 	      
              TH1D *tmpHist = subDir.make<TH1D> (TString(histName), "", numBins, vbins);
              oneDHistSetMap.find(currentDirName)->second.insert(pair<string,TH1D*>(histName, tmpHist));
              for(uint currentFilter = 0; currentFilter != tmpFilters.size(); currentFilter++)
	       {
                 TH1D *tmpHist = subDir.make<TH1D> (TString(histName + "_MatchedWith_" + tmpFilters.at(currentFilter)), "", numBins, vbins);
                 oneDHistSetMap.find(currentDirName)->second.insert(pair<string,TH1D*>(histName + "_MatchedWith_" + tmpFilters.at(currentFilter), tmpHist));
                 TH1D *tmpEffHist = subDir.make<TH1D> (TString(histName + "_Eff_" + tmpFilters.at(currentFilter)), "", numBins, vbins);
                 oneDHistSetMap.find(currentDirName)->second.insert(pair<string,TH1D*>(histName + "_Eff_" + tmpFilters.at(currentFilter), tmpEffHist));
               }
	      delete[] vbins;
            }
          if (histBinning.size() == 6)
	    {
	      TH2D *tmpHist = subDir.make<TH2D> (TString(histName), "", int(histBinning[0]), histBinning[1], histBinning[2],int(histBinning[3]), histBinning[4], histBinning[5]);
              twoDHistSetMap.find(currentDirName)->second.insert(pair<string,TH2D*>(histName, tmpHist));
              for(uint currentFilter = 0; currentFilter != tmpFilters.size(); currentFilter++)
	       {
                 TH2D *tmpHist = subDir.make<TH2D> (TString(histName + "_MatchedWith_" + tmpFilters.at(currentFilter)), "", int(histBinning[0]), histBinning[1], histBinning[2],int(histBinning[3]), histBinning[4], histBinning[5]);
                 twoDHistSetMap.find(currentDirName)->second.insert(pair<string,TH2D*>(histName + "_MatchedWith_" + tmpFilters.at(currentFilter), tmpHist));
                 TH2D *tmpEffHist = subDir.make<TH2D> (TString(histName + "_Eff_" + tmpFilters.at(currentFilter)), "", int(histBinning[0]), histBinning[1], histBinning[2],int(histBinning[3]), histBinning[4], histBinning[5]);
                 twoDHistSetMap.find(currentDirName)->second.insert(pair<string,TH2D*>(histName + "_Eff_" + tmpFilters.at(currentFilter), tmpEffHist));
               }
	    }
       fs->cd(); 
     }
   }
}


TurnOnAnalyzer::~TurnOnAnalyzer ()

{
}

void
TurnOnAnalyzer::produce(edm::Event &event, const edm::EventSetup &setup)
{  
  event.getByLabel(triggerSummaryLabel_, triggerSummary);
  event.getByLabel(l3Muon_, L3CandidateCollection);
  event.getByLabel(genParticle_, genParticles);  
  event.getByLabel(triggerResultsLabel_, triggerResults); 
  event.getByLabel(beamSpot_, BeamSpot);
  BeamSpotX = BeamSpot -> x0();
  BeamSpotY = BeamSpot -> y0();
  BeamSpotZ = BeamSpot -> z0(); 
  StopLength = 0;
  
  for (reco::GenParticleCollection::const_iterator genParticle = genParticles->begin(); genParticle != genParticles->end(); genParticle++) 
    {
      if(fabs(genParticle->pdgId()) == 13)
      {
        if(fabs(genParticle->mother()->pdgId()) == 1000006)
        {
          StopLength = sqrt(pow(genParticle->vx(),2) + pow(genParticle->vy(),2)); 
          stopDecayLength->Fill(StopLength);
        }
      }
    }

  edm::InputTag L2FilterTag = edm::InputTag("hltDimuonL2Filtered10", "", filterTag_);
  trigger::TriggerObjectCollection allTriggerObjects = triggerSummary->getObjects();
  size_t L2FilterIndex = (*triggerSummary).filterIndex(L2FilterTag);
  trigger::TriggerObjectCollection L2Objects;
  if(L2FilterIndex < (*triggerSummary).sizeFilters()) 
     { 	
       const trigger::Keys &keysL2Objects = (*triggerSummary).filterKeys(L2FilterIndex);
       for (size_t j = 0; j < keysL2Objects.size(); j++)
         {
           trigger::TriggerObject foundL2Object = (allTriggerObjects)[keysL2Objects[j]];
           L2Objects.push_back(foundL2Object);
         }
     }
  for(map<string,vector<string>>::iterator currentFilterSet = filterMap.begin(); currentFilterSet != filterMap.end(); currentFilterSet++)
    {
      for(uint currentFilter = 0; currentFilter != currentFilterSet->second.size(); currentFilter++)
        {
          string filterName = currentFilterSet->second[currentFilter];
          edm::InputTag currentFilterTag = edm::InputTag(filterName, "", filterTag_);
          size_t filterIndex = (*triggerSummary).filterIndex(currentFilterTag);
          trigger::TriggerObjectCollection objects;
          if(filterIndex < (*triggerSummary).sizeFilters())
            { 
              const trigger::Keys &keysObjects = (*triggerSummary).filterKeys(filterIndex);
              for(size_t j = 0; j < keysObjects.size(); j++)
                 {
                   trigger::TriggerObject foundObject = (allTriggerObjects)[keysObjects[j]];
                   objects.push_back(foundObject);
                 }
            }
          if(1 == 1)
          //if(event.getByLabel(recoMuon_, MuonCollection))
            {
             //for (reco::MuonCollection::const_iterator recoMuon = MuonCollection ->begin(); recoMuon != MuonCollection->end(); recoMuon++) 
             for (reco::GenParticleCollection::const_iterator genParticle = genParticles->begin(); genParticle != genParticles->end(); genParticle++)
              {
              string particleType = currentFilterSet->first;
              reco::GenParticle thisParticle = (*genParticle);
              //reco::Muon thisParticle = (*recoMuon);
              double d0 = fabs(D0BeamSpot(genParticle->vx(), genParticle->vy(), genParticle->px(), genParticle->py(),genParticle->pt(), BeamSpotX, BeamSpotY));
              if(fabs(genParticle->pdgId()) == 13 && fabs(genParticle->mother()->pdgId()) == 1000006 && genParticle->pt() > 15){
              //if(PassMuonReco(thisParticle) && GenMatched(thisParticle)){
              for(map<string,TH2D*>::iterator currentHistMap = twoDHistSetMap.find(particleType)->second.begin(); currentHistMap != twoDHistSetMap.find(particleType)->second.end(); currentHistMap++)
                {
	           vector<string> currentNameVec = GetStringSegments(currentHistMap->first, '_');
                   if(currentNameVec.size() == 2 && currentFilter == 0 && d0 < d0cut_) twoDHistSetMap.find(particleType)->second.find(currentHistMap->first)->second->Fill(GenParticle(thisParticle, currentNameVec[0]),GenParticle(thisParticle, currentNameVec[1]));
                }
              for(map<string,TH1D*>::iterator currentHistMap = oneDHistSetMap.find(particleType)->second.begin(); currentHistMap != oneDHistSetMap.find(particleType)->second.end(); currentHistMap++)
                {
	          vector<string> currentNameVec = GetStringSegments(currentHistMap->first, '_');
                  if(currentNameVec.size() == 1 && currentFilter == 0 && d0 < d0cut_ ) oneDHistSetMap.find(particleType)->second.find(currentNameVec[0])->second->Fill(GenParticle(thisParticle, currentNameVec[0]));
                }
                  bool toFill1D = false;
                  bool toFill2D = false;
                  if(FilterEff_)
                    {
                       for(unsigned int i = 0 ; i < objects.size() ; i++)
                        { 
                          if(Match(genParticle->eta(),genParticle->phi(),objects[i].eta(),objects[i].phi()))
        	           {
                             toFill1D = true;
                             toFill2D = true;
                           }
                        }
                    }
                  if(L3Eff_)
                    {
                      if(event.getByLabel(l3Muon_, L3CandidateCollection))
                       { 
                         for(reco::RecoChargedCandidateCollection::const_iterator l3Muon = L3CandidateCollection->begin(); l3Muon != L3CandidateCollection->end(); l3Muon++)
                           {
                             if(Match(genParticle->eta(),genParticle->phi(),l3Muon->eta(),l3Muon->phi()))
                              {
                                toFill1D = true;
                                toFill2D = true;
                              } 
                           }
                       }
                     }
                  if(RecoTrackEff_)
                    {
                      if(event.getByLabel(recoTrack_, TrackCollection))
                       { 
                         for(reco::TrackCollection::const_iterator recoTrack = TrackCollection ->begin(); recoTrack != TrackCollection->end(); recoTrack++)
                           {
                             if(Match(genParticle->eta(),genParticle->phi(),recoTrack->eta(),recoTrack->phi()))
                              {
                                toFill1D = true;
                                toFill2D = true;
                              } 
                           }
                       }
                     }
                  if(RecoMuonEff_)
                    {
                      if(event.getByLabel(recoMuon_, MuonCollection))
                       { 
                         for(reco::MuonCollection::const_iterator recoMuon = MuonCollection ->begin(); recoMuon != MuonCollection->end(); recoMuon++)
                           {
                             if(Match(genParticle->eta(),genParticle->phi(),recoMuon->eta(),recoMuon->phi())&& recoMuon->isGlobalMuon())
                              {
                                toFill1D = true;
                                toFill2D = true;
                              } 
                           }
                       }
                     }
                  if(L2Eff_)
                    {
                      if(event.getByLabel(l2Muon_, L2CandidateCollection))
                       { 
                         for(reco::RecoChargedCandidateCollection::const_iterator l2Muon = L2CandidateCollection->begin(); l2Muon != L2CandidateCollection->end(); l2Muon++)
                           {
                             if(Match(genParticle->eta(),genParticle->phi(),l2Muon->eta(),l2Muon->phi()))
                              {
                                toFill1D = true;
                                toFill2D = true;
                              } 
                           }
                       }
                     }
                  if(toFill1D && d0 < d0cut_ )
                   {
                     for(map<string,TH1D*>::iterator currentHistMap = oneDHistSetMap.find(particleType)->second.begin(); currentHistMap != oneDHistSetMap.find(particleType)->second.end(); currentHistMap++)
                       {
                         vector<string> currentNameVec = GetStringSegments(currentHistMap->first, '_');                    
                         if(currentNameVec.size() == 1 )oneDHistSetMap.find(particleType)->second.find(currentNameVec[0] + "_MatchedWith_" + filterName)->second->Fill(GenParticle(thisParticle,currentNameVec[0]));                  
                       }
                   }
                  if(toFill2D && d0 < d0cut_ ) 
                   {
                     for(map<string,TH2D*>::iterator currentHistMap = twoDHistSetMap.find(particleType)->second.begin(); currentHistMap != twoDHistSetMap.find(particleType)->second.end(); currentHistMap++)
                       {
                         vector<string> currentNameVec = GetStringSegments(currentHistMap->first, '_');                    
                         if(currentNameVec.size() == 2 )twoDHistSetMap.find(particleType)->second.find(currentHistMap->first + "_MatchedWith_" + filterName)->second->Fill(GenParticle(thisParticle,currentNameVec[0]),GenParticle(thisParticle,currentNameVec[1]));                  
                       }
                   }
               }
          }
       }
    }
  }
}
double 
TurnOnAnalyzer::D0BeamSpot(double vx, double vy, double px, double py, double pt, double refx, double refy)
{
  return -(-(vx-refx) * py + (vy-refy) * px)/pt;
  //return fabs(sqrt(pow(vx-refx,2) + pow(vy-refy,2)));
}

double
TurnOnAnalyzer::DZBeamSpot(double vx, double vy, double vz, double px, double py, double pz, double pt, double refx, double refy, double refz)
{
  return (vz-refz)-((vx-refx)*px+(vy-refy)*py)/pt * (pz/pt);
}

bool
TurnOnAnalyzer::GenMatched(const reco::Muon recoMuon)
{
  bool match = false;
  for (reco::GenParticleCollection::const_iterator genParticle = genParticles->begin(); genParticle != genParticles->end(); genParticle++)
    {
      if(fabs(genParticle->pdgId()) == 13 && fabs(genParticle->mother()->pdgId()) == 1000006)
      {
        if(Match(genParticle->eta(), genParticle->phi(), recoMuon.eta(), recoMuon.phi()))
           {match = true;}
      }
    }
  return match;
}

double
TurnOnAnalyzer::RecoMuon(const reco::Muon recoMuon, string variable)
{
  double value = 0;
  if(variable == "Pt")  value = recoMuon.pt();
  if(variable == "Eta") value = recoMuon.eta();
  if(variable == "Phi") value = recoMuon.phi();
  if(variable == "D0")  value = fabs(D0BeamSpot(recoMuon.vx(), recoMuon.vy(), recoMuon.px(), recoMuon.py(),recoMuon.pt(), BeamSpotX, BeamSpotY));
  if(variable == "Dz")  value = fabs(DZBeamSpot(recoMuon.vx(), recoMuon.vy(), recoMuon.vz(),recoMuon.px(), recoMuon.py(),recoMuon.pz(),recoMuon.pt(), BeamSpotX, BeamSpotY, BeamSpotZ));
  return value;
}

double
TurnOnAnalyzer::GenParticle(const reco::GenParticle genParticle, string variable)
{
  double value = 0;
  if(variable == "Pt")  value = genParticle.pt();
  if(variable == "Eta") value = genParticle.eta();
  if(variable == "Phi") value = genParticle.phi();
  if(variable == "D0")  value = fabs(D0BeamSpot(genParticle.vx(), genParticle.vy(), genParticle.px(), genParticle.py(),genParticle.pt(), BeamSpotX, BeamSpotY));
  if(variable == "Dz")  value = fabs(DZBeamSpot(genParticle.vx(), genParticle.vy(), genParticle.vz(),genParticle.px(), genParticle.py(),genParticle.pz(),genParticle.pt(), BeamSpotX, BeamSpotY, BeamSpotZ));
  return value;
}

double
TurnOnAnalyzer::RecoTrack(const reco::Track recoTrack, string variable)
{
  double value = 0;
  if(variable == "Pt")  value = recoTrack.pt();
  if(variable == "Eta") value = recoTrack.eta();
  if(variable == "Phi") value = recoTrack.phi();
  if(variable == "D0")  value = fabs(D0BeamSpot(recoTrack.vx(), recoTrack.vy(), recoTrack.px(), recoTrack.py(),recoTrack.pt(), BeamSpotX, BeamSpotY));
  if(variable == "Dz")  value = fabs(DZBeamSpot(recoTrack.vx(), recoTrack.vy(), recoTrack.vz(),recoTrack.px(), recoTrack.py(),recoTrack.pz(),recoTrack.pt(), BeamSpotX, BeamSpotY, BeamSpotZ));
  return value;
}

bool 
TurnOnAnalyzer::Match(double eta_1, double phi_1, double eta_2, double phi_2)
{
  bool matched = deltaR(eta_1,phi_1,eta_2,phi_2) < 0.1 ? true:false;
  return matched;
}


bool 
TurnOnAnalyzer::PassMuonReco(const reco::Muon recoMuon)
{
  bool matched = false;
  //matched = recoMuon.pt() > 25 && recoMuon.isGlobalMuon() > 0 && recoMuon.isPFMuon() > 0 && recoMuon.globalTrack()->normalizedChi2() < 10 && recoMuon.globalTrack()->hitPattern().numberOfValidMuonHits() > 0 && recoMuon.numberOfMatchedStations() > 1 && recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && recoMuon.track()->hitPattern().trackerLayersWithMeasurement() > 5;
  matched = recoMuon.isGlobalMuon() > 0; //&& recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && recoMuon.isPFMuon() > 0 && recoMuon.globalTrack()->normalizedChi2() < 10 && recoMuon.globalTrack()->hitPattern().numberOfValidMuonHits() > 0 && recoMuon.numberOfMatchedStations() > 1 && recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && recoMuon.track()->hitPattern().trackerLayersWithMeasurement() > 5;
  //matched = recoMuon.pt() > 25 && recoMuon.isGlobalMuon() > 0;
  return matched;
}

bool 
TurnOnAnalyzer::PassTrackReco(const reco::Track recoTrack)
{
  bool matched = false;
  //matched = recoMuon.pt() > 25 && recoMuon.isGlobalMuon() > 0 && recoMuon.isPFMuon() > 0 && recoMuon.globalTrack()->normalizedChi2() < 10 && recoMuon.globalTrack()->hitPattern().numberOfValidMuonHits() > 0 && recoMuon.numberOfMatchedStations() > 1 && recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && recoMuon.track()->hitPattern().trackerLayersWithMeasurement() > 5;
  matched = recoTrack.pt() > 5; //&& recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && recoMuon.isPFMuon() > 0 && recoMuon.globalTrack()->normalizedChi2() < 10 && recoMuon.globalTrack()->hitPattern().numberOfValidMuonHits() > 0 && recoMuon.numberOfMatchedStations() > 1 && recoMuon.innerTrack()->hitPattern().numberOfValidPixelHits() > 0 && recoMuon.track()->hitPattern().trackerLayersWithMeasurement() > 5;
  //matched = recoMuon.pt() > 25 && recoMuon.isGlobalMuon() > 0;
  return matched;
}
bool 
TurnOnAnalyzer::PassElectronReco(const reco::GsfElectron recoElectron)
{
  bool matched = false;
  if (recoElectron.isEB() && recoElectron.isElectron()) matched = fabs(recoElectron.deltaEtaSuperClusterTrackAtVtx()) < 0.004 && fabs (recoElectron.deltaPhiSuperClusterTrackAtVtx()) < 0.03 && recoElectron.scSigmaIEtaIEta() < 0.01 && recoElectron.hadronicOverEm() < 0.12 && fabs(1/recoElectron.ecalEnergy()-(recoElectron.eSuperClusterOverP()/recoElectron.ecalEnergy())) < 0.05;
  else if(recoElectron.isElectron()) matched = fabs(recoElectron.deltaEtaSuperClusterTrackAtVtx()) < 0.005 && fabs (recoElectron.deltaPhiSuperClusterTrackAtVtx()) < 0.02 && recoElectron.scSigmaIEtaIEta() < 0.03 && recoElectron.hadronicOverEm() < 0.10 && fabs(1/recoElectron.ecalEnergy()-(recoElectron.eSuperClusterOverP()/recoElectron.ecalEnergy())) < 0.05;
  return matched;
}

vector<string>
TurnOnAnalyzer::GetStringSegments(string input, char delim)
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
TurnOnAnalyzer::endJob()
{
  for(map<string,vector<string>>::iterator currentFilterSet = filterMap.begin(); currentFilterSet != filterMap.end(); currentFilterSet++)
    {
      string particleType = currentFilterSet->first;
      for(uint currentFilter = 0; currentFilter != currentFilterSet->second.size(); currentFilter++)
         {
            string filterName = currentFilterSet->second[currentFilter];
            for(map<string,TH1D*>::iterator currentHistMap = oneDHistSetMap.find(particleType)->second.begin(); currentHistMap != oneDHistSetMap.find(particleType)->second.end(); currentHistMap++)
                {
                   vector<string> currentNameVec = GetStringSegments(currentHistMap->first, '_');
		   if(currentNameVec.size() == 3)
                      {
                         if(currentNameVec[1].compare("Eff") == 0 && currentNameVec[2].compare(filterName) == 0)
			    {
                              currentHistMap->second->Sumw2();
			      currentHistMap->second->Divide(oneDHistSetMap.find(particleType)->second.find(currentNameVec[0] + "_MatchedWith_" + filterName)->second,oneDHistSetMap.find(particleType)->second.find(currentNameVec[0])->second,1,1);
                            }
                      }
                 }
            for(map<string,TH2D*>::iterator currentHistMap = twoDHistSetMap.find(particleType)->second.begin(); currentHistMap != twoDHistSetMap.find(particleType)->second.end(); currentHistMap++)
                {
                   vector<string> currentNameVec = GetStringSegments(currentHistMap->first, '_');
		   if(currentNameVec.size() == 4)
                      {
                         if(currentNameVec[2].compare("Eff") == 0 && currentNameVec[3].compare(filterName) == 0)
			    {
                              currentHistMap->second->Sumw2();
			      currentHistMap->second->Divide(twoDHistSetMap.find(particleType)->second.find(currentNameVec[0] + "_" + currentNameVec[1] + "_MatchedWith_" + filterName)->second,twoDHistSetMap.find(particleType)->second.find(currentNameVec[0] + "_" + currentNameVec[1])->second,1,1);
                            }
                      }
                 }
          }
      }
}

DEFINE_FWK_MODULE(TurnOnAnalyzer);
