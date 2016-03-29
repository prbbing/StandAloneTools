#include "DiMuonAnalyzer_76X.h"

using namespace std;
using namespace reco;
using namespace edm;

DiMuonAnalyzer::DiMuonAnalyzer (const edm::ParameterSet &cfg) :
beamSpot_ (consumes<reco::BeamSpot>(cfg.getParameter<edm::InputTag> ("beamSpot"))),
recoMuon_ (consumes<vector<pat::Muon>>(cfg.getParameter<edm::InputTag> ("recoMuons"))),
triggerBitsLabel_(consumes<edm::TriggerResults>(cfg.getParameter<edm::InputTag>("bits"))),
triggerObjectsLabel_ (consumes<pat::TriggerObjectStandAloneCollection>(cfg.getParameter<edm::InputTag> ("triggerObjectsLabel"))),
basicMuonSelections_ (cfg.getParameter<string> ("basicMuonSelections")),
probeMuonSelections_ (cfg.getParameter<string> ("probeMuonSelections")),
filterName_ (cfg.getParameter<string> ("filterName")),
histograms_ (cfg.getParameter<vector<edm::ParameterSet>>("histograms")),
probeHistograms_ (cfg.getParameter<vector<edm::ParameterSet>>("probeHistograms")),
passedProbeHistograms_ (cfg.getParameter<vector<edm::ParameterSet>>("passedProbeHistograms")),
twoDHistograms_ (cfg.getParameter<vector<edm::ParameterSet>>("twoDHistograms")),
triggerNames_ (cfg.getParameter<vector<string>>("triggerNames")),
pairHistograms_ (cfg.getParameter<vector<edm::ParameterSet>>("pairHistograms"))
{
  TH1D::SetDefaultSumw2 ();  
  TH2D::SetDefaultSumw2 ();  
  for(uint i =0; i < histograms_.size(); i++)
    {
      TH1D* tmpHist =  fs->make<TH1D>(TString(histograms_[i].getParameter<string>("variable")),TString(histograms_[i].getParameter<string>("variable")),int(histograms_[i].getParameter<int>("nBins")),histograms_[i].getParameter<double>("lBin"),histograms_[i].getParameter<double>("hBin"));     
      histograms.push_back(tmpHist);  
    } 
  for(uint i =0; i < probeHistograms_.size(); i++)
    {
      TH1D* tmpHist =  fs->make<TH1D>(TString(probeHistograms_[i].getParameter<string>("variable") + "_probe"),TString(probeHistograms_[i].getParameter<string>("variable") + "_probe"),int(probeHistograms_[i].getParameter<int>("nBins")),probeHistograms_[i].getParameter<double>("lBin"),probeHistograms_[i].getParameter<double>("hBin"));     
      probeHistograms.push_back(tmpHist);  
    } 
  for(uint i =0; i < passedProbeHistograms_.size(); i++)
    {
      TH1D* tmpHist =  fs->make<TH1D>(TString(passedProbeHistograms_[i].getParameter<string>("variable") + "_passedProbe"),TString(passedProbeHistograms_[i].getParameter<string>("variable") + "_passedProbe"),int(passedProbeHistograms_[i].getParameter<int>("nBins")),passedProbeHistograms_[i].getParameter<double>("lBin"),passedProbeHistograms_[i].getParameter<double>("hBin"));     
      passedProbeHistograms.push_back(tmpHist);  
    } 
  for(uint i =0; i < twoDHistograms_.size(); i++)
    {
      TH2D* tmpHist =  fs->make<TH2D>(TString(twoDHistograms_[i].getParameter<string>("variable")),TString(twoDHistograms_[i].getParameter<string>("variable")),int(twoDHistograms_[i].getParameter<int>("nXBins")),twoDHistograms_[i].getParameter<double>("lXBin"),twoDHistograms_[i].getParameter<double>("hXBin"), int(twoDHistograms_[i].getParameter<int>("nYBins")),twoDHistograms_[i].getParameter<double>("lYBin"),twoDHistograms_[i].getParameter<double>("hYBin"));     
      twoDHistograms.push_back(tmpHist);  
    } 
  for(uint i =0; i < pairHistograms_.size(); i++)
    {
      TH1D* tmpHist =  fs->make<TH1D>(TString(pairHistograms_[i].getParameter<string>("variable")),TString(pairHistograms_[i].getParameter<string>("variable")),int(pairHistograms_[i].getParameter<int>("nBins")),pairHistograms_[i].getParameter<double>("lBin"),pairHistograms_[i].getParameter<double>("hBin"));     
      pairHistograms.push_back(tmpHist);  
    } 
}

DiMuonAnalyzer::~DiMuonAnalyzer ()
{
}

void
DiMuonAnalyzer::analyze (const edm::Event &event, const edm::EventSetup &setup)
{
  if(evaluateTrigger(triggerNames_, event))
    {  
      if(event.getByToken(beamSpot_, BeamSpot))
        {
          refx_ = BeamSpot->x0();
          refy_ = BeamSpot->y0();
          refxError_ = BeamSpot->x0Error();
          refyError_ = BeamSpot->y0Error();
        }
      else
        {
          refx_ = 0;
          refy_ = 0;
          refxError_ = 0;
          refyError_ = 0;
        }
    
    event.getByToken(triggerBitsLabel_, triggerBits);
    event.getByToken(triggerObjectsLabel_, triggerObjects);
    const edm::TriggerNames &names = event.triggerNames(*triggerBits);
    string filterName = filterName_;
    vector<pat::TriggerObjectStandAlone> filterObjects;
    for (pat::TriggerObjectStandAlone obj : *triggerObjects) {
      obj.unpackPathNames(names);    
      if(obj.hasFilterLabel(filterName))
           filterObjects.push_back(obj);
    } 
    vector<pat::Muon> basicRecoMuon = basicMuonCollection(recoMuon_, event);
    vector<pat::Muon> probeMuon = probeMuonFinder (recoMuon_, event, basicRecoMuon);
    vector<pat::Muon> passedProbeMuon;
    if(probeMuon.size() > 0) 
      {
        passedProbeMuon = probeMuonMatcher (probeMuon[0], filterObjects); 
      }
    vector<vector<pat::Muon>> basicMuonPair = muonPairs(basicRecoMuon);
    if(probeMuon.size() > 0)
      {
        for(uint j = 0; j < probeHistograms.size(); j++)
          {
            fillMuonHistogram(probeMuon, probeHistograms[j]);
          }
        if(passedProbeMuon.size() > 0)
          {
            for(uint j = 0; j < passedProbeHistograms.size(); j++)
             {
               fillMuonHistogram(passedProbeMuon, passedProbeHistograms[j]);
             }
          }
      }
    if(basicRecoMuon.size() >= 2)
      {
      for(uint j = 0; j < histograms.size(); j++)
        {
          fillMuonHistogram(basicRecoMuon, histograms[j]);
        }
      for(uint j = 0; j < twoDHistograms.size(); j++)
        {
          fillMuon2DHistogram(basicRecoMuon, twoDHistograms[j]);
        }
      }  
    for(uint k = 0; k < pairHistograms.size(); k++)
      {
        fillMuonPairHistogram(basicMuonPair, pairHistograms[k]);
      }  
    if(basicMuonPair.size() > 0)
      {
        bool passDxy = false;
        for(uint j = 0; j < basicRecoMuon.size(); j++)
          {
            if(fabs(muonVariables(basicRecoMuon[j], "Dxy") >= 0.02))
              passDxy = true;
          }
        if(passDxy)  
          cout<<"Pass Run: "<<event.id().run()<<" Lumi: "<<event.luminosityBlock()<<" Event: "<<event.id().event()<<endl;
      }
    basicRecoMuon.clear();
    basicMuonPair.clear();
  }
}

vector<pat::Muon>
DiMuonAnalyzer::basicMuonCollection (const edm::EDGetTokenT<vector<pat::Muon>> recoMuonTag, const edm::Event &event)
{
  vector<pat::Muon> basicRecoMuon;
  edm::Handle<pat::MuonCollection> MuonCollection;
  StringCutObjectSelector<pat::Muon> *basicMuonSelector = new StringCutObjectSelector<pat::Muon>(basicMuonSelections_);
  if(event.getByToken(recoMuonTag, MuonCollection))
    {
      for (pat::MuonCollection::const_iterator recoMuon = MuonCollection ->begin(); recoMuon != MuonCollection->end(); recoMuon++) 
        {
          pat::Muon thisParticle = (*recoMuon);
          if (basicMuonSelector->operator()(thisParticle))
            {
               basicRecoMuon.push_back(thisParticle);
            } 
        }
     }
  delete basicMuonSelector;
  return basicRecoMuon;
}

vector<pat::Muon>
DiMuonAnalyzer::probeMuonMatcher (pat::Muon probeMuon, vector<pat::TriggerObjectStandAlone> filterObjects)
{
  vector<pat::Muon> passedProbeMuon;
  for(uint i = 0; i < filterObjects.size(); i++)
    {
      if(deltaR(probeMuon.eta(),probeMuon.phi(),filterObjects[i].eta(),filterObjects[i].phi()) < 0.1)
        {
          passedProbeMuon.push_back(probeMuon);
          break;
        }
    }
  return passedProbeMuon;
}

vector<pat::Muon>
DiMuonAnalyzer::probeMuonFinder (const edm::EDGetTokenT<vector<pat::Muon>> recoMuonTag, const edm::Event &event, vector<pat::Muon> tagMuonSet)
{
  vector<pat::Muon> probeRecoMuon;
  edm::Handle<pat::MuonCollection> MuonCollection;
  double phi = 0;
  pat::Muon tagMuon;
  for(uint j = 0; j < tagMuonSet.size(); j++)
    {
      if(abs(tagMuonSet[j].phi()) > phi)
        {
           tagMuon = tagMuonSet[j];
           phi = abs(tagMuonSet[j].phi()); 
        } 
    }
  double dZMass = 10;
  StringCutObjectSelector<pat::Muon> *probeMuonSelector = new StringCutObjectSelector<pat::Muon>(probeMuonSelections_);
    {
      if(event.getByToken(recoMuonTag, MuonCollection))
        {
          for (pat::MuonCollection::const_iterator recoMuon = MuonCollection ->begin(); recoMuon != MuonCollection->end(); recoMuon++) 
            {
              pat::Muon thisParticle = (*recoMuon);
              if (probeMuonSelector->operator()(thisParticle))
                {
                  vector<pat::Muon> tmpPair;
                  tmpPair.push_back(thisParticle);
                  tmpPair.push_back(tagMuon);
                  if(abs(muonPairVariables(tmpPair, "invMass") - 90) < dZMass && muonPairVariables(tmpPair, "invMass") > 80 && muonPairVariables(tmpPair, "invMass") < 100 && muonPairVariables(tmpPair, "chargedProduct") < 0)   
                    {
                      dZMass = abs(muonPairVariables(tmpPair, "invMass") - 90);
                      probeRecoMuon.clear();  
                      probeRecoMuon.push_back(thisParticle);
                    }
                } 
            }
         }
    }
  delete probeMuonSelector;
  return probeRecoMuon;
}


vector<vector<pat::Muon>>
DiMuonAnalyzer::muonPairs (vector<pat::Muon> inputMuonSet)
{
  vector<vector<pat::Muon>> muonPairs;
  vector<vector<uint>> selectedIndices;
  for (uint j = 0; j < inputMuonSet.size(); j++)
    {
      pat::Muon thisParticle = inputMuonSet.at(j);
      for (uint k = 0; k < inputMuonSet.size(); k++)
        {
          if(j == k)
            continue; 
          pat::Muon thatParticle =inputMuonSet.at(k);
          bool alreadySelected = false;
          for(uint l = 0; l < selectedIndices.size(); l++)
            {
              if((j == selectedIndices[l].at(0) && k == selectedIndices[l].at(1)) ||(j == selectedIndices[l].at(1) && k == selectedIndices[l].at(0)))
              alreadySelected = true;
            }
          if(alreadySelected)
            continue;
          if(passPairSelection(thisParticle, thatParticle))
            {
              vector<pat::Muon> tmpPair;
              tmpPair.push_back(thisParticle);
              tmpPair.push_back(thatParticle);
              muonPairs.push_back(tmpPair);
              vector<uint> tmpIndices;
              tmpIndices.push_back(j);
              tmpIndices.push_back(k);
              selectedIndices.push_back(tmpIndices);
              tmpPair.clear();
              tmpIndices.clear();
            }
        }
     }
  vector<pat::Muon> pairedMuons;
  for(uint j = 0; j < inputMuonSet.size(); j++)
    {
      bool paired = false;
      for(uint l = 0; l < selectedIndices.size(); l++)
        {
          if(selectedIndices[l].at(0) == j || selectedIndices[l].at(1) == j)
            paired = true;
        }
      if(paired)
        pairedMuons.push_back(inputMuonSet[j]);
    }
  inputMuonSet = pairedMuons;
  return muonPairs;
  muonPairs.clear();
  selectedIndices.clear();
  pairedMuons.clear();
}

bool
DiMuonAnalyzer::passPairSelection(const pat::Muon first, const pat::Muon second)
{
  vector<pat::Muon> muonPair;
  muonPair.push_back(first);
  muonPair.push_back(second);
  bool wellSeparated = deltaR(first.eta(),first.phi(),second.eta(),second.phi()) > 0.5 ? true:false; 
  bool oppositeCharged = first.charge()*second.charge() < 0 ? true:false;
  bool withinZWindow = muonPairVariables(muonPair, "invMass") > 80 && muonPairVariables(muonPair, "invMass") < 100 ? true:false; 
  return wellSeparated && oppositeCharged && withinZWindow;
}


double
DiMuonAnalyzer::muonPairVariables(const vector<pat::Muon> muonPair, string variable)
{ 
  double value = 0;
  if(variable == "chargedProduct") value = muonPair[0].charge()*muonPair[1].charge();
  if(variable == "deltaR") value = deltaR(muonPair[0].eta(),muonPair[0].phi(),muonPair[1].eta(),muonPair[1].phi());
  if(variable == "3DAngle") 
    {
      static const double pi = 3.1415926535897932384626433832795028841971693993751058;
      TVector3 threeVector1(muonPair[0].px(), muonPair[0].py(), muonPair[0].pz());
      TVector3 threeVector2(muonPair[1].px(), muonPair[1].py(), muonPair[1].pz());
      value = log((pi-threeVector1.Angle(threeVector2)));
    }
  if(variable == "invMass") 
    {
      TLorentzVector fourVector1(muonPair[0].px(), muonPair[0].py(), muonPair[0].pz(), muonPair[0].energy());
      TLorentzVector fourVector2(muonPair[1].px(), muonPair[1].py(), muonPair[1].pz(), muonPair[1].energy());
      value = (fourVector1 + fourVector2).M();
    }
  return value;
}

double 
DiMuonAnalyzer::dxyBeamSpot(double vx, double px, double vy, double py, double pt, double refx, double refy)
{
  return (-(vx-refx)*py + (vy-refy)*px)/pt;
}

double 
DiMuonAnalyzer::dxyBeamSpotError(double trackD0Error, double beamSpotX0Error, double beamSpotY0Error)
{
  return hypot(trackD0Error, hypot(beamSpotX0Error, beamSpotY0Error));
}

double
DiMuonAnalyzer::muonVariables(const pat::Muon muon, string variable)
{
  double value = 0;
  if(variable == "Pt") value = muon.pt(); 
  if(variable == "Eta") value = muon.eta(); 
  if(variable == "Phi") value = muon.phi(); 
  if(variable == "PixelHit") value = muon.innerTrack()->hitPattern().numberOfValidPixelHits(); 
  if(variable == "Dxy") value = dxyBeamSpot(muon.vx(), muon.px(), muon.vy(), muon.py(), muon.pt(), refx_, refy_); 
  if(variable == "DxyError") value = dxyBeamSpotError(muon.innerTrack()->d0Error(), refxError_, refyError_); 
  return value;
}

void
DiMuonAnalyzer::fillMuonHistogram(vector<pat::Muon> recoMuonSet, TH1D* histogram)
{
  for (unsigned int j = 0 ; j < recoMuonSet.size() ; j++)
    {
       vector<string> name = getStringSegments(string(histogram->GetName()), '_');
       double value = muonVariables(recoMuonSet[j], name[0]);
       histogram->Fill(value);
    }
}

void
DiMuonAnalyzer::fillMuon2DHistogram(vector<pat::Muon> recoMuonSet, TH2D* histogram)
{
  for (unsigned int j = 0 ; j < recoMuonSet.size() ; j++)
    {
       vector<string> name = getStringSegments(string(histogram->GetName()), '_');
       double valuex = muonVariables(recoMuonSet[j], name[0]);
       double valuey = muonVariables(recoMuonSet[j], name[1]);
       histogram->Fill(valuex,valuey);
    }
}

void
DiMuonAnalyzer::fillMuonPairHistogram(vector<vector<pat::Muon>> recoMuonPair, TH1D* histogram)
{
  for (unsigned int j = 0 ; j < recoMuonPair.size() ; j++)
    {
       vector<string> name = getStringSegments(string(histogram->GetName()), '_');
       double value = muonPairVariables(recoMuonPair[j], name[0]);
       histogram->Fill(value);
    }
}

bool
DiMuonAnalyzer::evaluateTrigger(vector<string> triggerList, const edm::Event &event)
{ 
  bool passTrigger = false;
  if(triggerList.size() == 0)
    passTrigger = true; 
  else
    {
      for(unsigned int j = 0 ; j < triggerList.size() ; j++)    
        {
          event.getByToken(triggerBitsLabel_, triggerBits);
          if(triggerBits.isValid())
            {
              const edm::TriggerNames & trigNames = event.triggerNames(*triggerBits);
              unsigned int numTriggers = trigNames.size();
              for( unsigned int hltIndex=0; hltIndex<numTriggers-1; ++hltIndex )
                {
                  std::string currentTrigName = trigNames.triggerName(hltIndex);
                  if (currentTrigName.find(triggerList[j]) < currentTrigName.length() && triggerBits->accept(hltIndex))
                    {
                      passTrigger = true;
                    }
                } 
            }
        }
    } 
 return passTrigger;
}

vector<string>
DiMuonAnalyzer::getStringSegments(string input, char delim)
{
    vector<string> elems;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

DEFINE_FWK_MODULE(DiMuonAnalyzer);
