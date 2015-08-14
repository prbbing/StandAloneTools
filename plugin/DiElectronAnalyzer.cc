#include "DiElectronAnalyzer.h"

using namespace std;
using namespace reco;
using namespace edm;

DiElectronAnalyzer::DiElectronAnalyzer (const edm::ParameterSet &cfg) :
beamSpot_ (cfg.getParameter<edm::InputTag> ("beamSpot")),
recoElectron_ (cfg.getParameter<edm::InputTag> ("recoElectrons")),
triggerResultsLabel_ (cfg.getParameter<edm::InputTag> ("triggerResultsLabel")),
basicElectronSelections_ (cfg.getParameter<string> ("basicElectronSelections")),
histograms_ (cfg.getParameter<vector<edm::ParameterSet>>("histograms")),
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

DiElectronAnalyzer::~DiElectronAnalyzer ()
{
}

void
DiElectronAnalyzer::analyze (const edm::Event &event, const edm::EventSetup &setup)
{
  if(evaluateTrigger(triggerNames_, event))
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
    vector<reco::GsfElectron> basicRecoElectron = basicElectronCollection(recoElectron_, event);
    vector<vector<reco::GsfElectron>> basicElectronPair = electronPairs(basicRecoElectron);
    if(basicRecoElectron.size() >= 2)
      {
      for(uint j = 0; j < histograms.size(); j++)
        {
          fillElectronHistogram(basicRecoElectron, histograms[j]);
        }
      for(uint j = 0; j < twoDHistograms.size(); j++)
        {
          fillElectron2DHistogram(basicRecoElectron, twoDHistograms[j]);
        }
      }  
    for(uint k = 0; k < pairHistograms.size(); k++)
      {
        fillElectronPairHistogram(basicElectronPair, pairHistograms[k]);
      }  
    if(basicElectronPair.size() > 0)
      {
        bool passDxy = false;
        for(uint j = 0; j < basicRecoElectron.size(); j++)
          {
            if(fabs(electronVariables(basicRecoElectron[j], "Dxy") >= 0.02))
              passDxy = true;
          }
        if(passDxy)  
          cout<<"Pass Run: "<<event.id().run()<<" Lumi: "<<event.luminosityBlock()<<" Event: "<<event.id().event()<<endl;
      }
    basicRecoElectron.clear();
    basicElectronPair.clear();
  }
}

vector<reco::GsfElectron>
DiElectronAnalyzer::basicElectronCollection (const edm::InputTag recoElectronTag, const edm::Event &event)
{
  vector<reco::GsfElectron> basicRecoElectron;
  edm::Handle<reco::GsfElectronCollection> ElectronCollection;
  StringCutObjectSelector<reco::GsfElectron> *basicElectronSelector = new StringCutObjectSelector<reco::GsfElectron>(basicElectronSelections_);
  if(event.getByLabel(recoElectronTag, ElectronCollection))
    {
      for (reco::GsfElectronCollection::const_iterator recoElectron = ElectronCollection ->begin(); recoElectron != ElectronCollection->end(); recoElectron++) 
        {
          reco::GsfElectron thisParticle = (*recoElectron);
          if (basicElectronSelector->operator()(thisParticle))
            {
               basicRecoElectron.push_back(thisParticle);
            } 
        }
     }
  delete basicElectronSelector;
  return basicRecoElectron;
}

vector<vector<reco::GsfElectron>>
DiElectronAnalyzer::electronPairs (vector<reco::GsfElectron> inputElectronSet)
{
  vector<vector<reco::GsfElectron>> electronPairs;
  vector<vector<uint>> selectedIndices;
  for (uint j = 0; j < inputElectronSet.size(); j++)
    {
      reco::GsfElectron thisParticle = inputElectronSet.at(j);
      for (uint k = 0; k < inputElectronSet.size(); k++)
        {
          if(j == k)
            continue; 
          reco::GsfElectron thatParticle =inputElectronSet.at(k);
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
              vector<reco::GsfElectron> tmpPair;
              tmpPair.push_back(thisParticle);
              tmpPair.push_back(thatParticle);
              electronPairs.push_back(tmpPair);
              vector<uint> tmpIndices;
              tmpIndices.push_back(j);
              tmpIndices.push_back(k);
              selectedIndices.push_back(tmpIndices);
              tmpPair.clear();
              tmpIndices.clear();
            }
        }
     }
  vector<reco::GsfElectron> pairedElectrons;
  for(uint j = 0; j < inputElectronSet.size(); j++)
    {
      bool paired = false;
      for(uint l = 0; l < selectedIndices.size(); l++)
        {
          if(selectedIndices[l].at(0) == j || selectedIndices[l].at(1) == j)
            paired = true;
        }
      if(paired)
        pairedElectrons.push_back(inputElectronSet[j]);
    }
  inputElectronSet = pairedElectrons;
  return electronPairs;
  electronPairs.clear();
  selectedIndices.clear();
  pairedElectrons.clear();
}

bool
DiElectronAnalyzer::passPairSelection(const reco::GsfElectron first, const reco::GsfElectron second)
{
  vector<reco::GsfElectron> electronPair;
  electronPair.push_back(first);
  electronPair.push_back(second);
  bool wellSeparated = deltaR(first.eta(),first.phi(),second.eta(),second.phi()) > 0.5 ? true:false; 
  bool oppositeCharged = first.charge()*second.charge() < 0 ? true:false;
  bool withinZWindow = electronPairVariables(electronPair, "invMass") > 80 && electronPairVariables(electronPair, "invMass") < 100 ? true:false; 
  return wellSeparated && oppositeCharged && withinZWindow;
}


double
DiElectronAnalyzer::electronPairVariables(const vector<reco::GsfElectron> electronPair, string variable)
{ 
  double value = 0;
  if(variable == "chargedProduct") value = electronPair[0].charge()*electronPair[1].charge();
  if(variable == "deltaR") value = deltaR(electronPair[0].eta(),electronPair[0].phi(),electronPair[1].eta(),electronPair[1].phi());
  if(variable == "3DAngle") 
    {
      static const double pi = 3.1415926535897932384626433832795028841971693993751058;
      TVector3 threeVector1(electronPair[0].px(), electronPair[0].py(), electronPair[0].pz());
      TVector3 threeVector2(electronPair[1].px(), electronPair[1].py(), electronPair[1].pz());
      value = log((pi-threeVector1.Angle(threeVector2)));
    }
  if(variable == "invMass") 
    {
      TLorentzVector fourVector1(electronPair[0].px(), electronPair[0].py(), electronPair[0].pz(), electronPair[0].energy());
      TLorentzVector fourVector2(electronPair[1].px(), electronPair[1].py(), electronPair[1].pz(), electronPair[1].energy());
      value = (fourVector1 + fourVector2).M();
    }
  return value;
}

double 
DiElectronAnalyzer::dxyBeamSpot(double vx, double px, double vy, double py, double pt, double refx, double refy)
{
  return (-(vx-refx)*py + (vy-refy)*px)/pt;
}

double
DiElectronAnalyzer::electronVariables(const reco::GsfElectron electron, string variable)
{
  double value = 0;
  if(variable == "Pt") value = electron.pt(); 
  if(variable == "Eta") value = electron.eta(); 
  if(variable == "Phi") value = electron.phi(); 
  if(variable == "Dxy") value = dxyBeamSpot(electron.vx(), electron.px(), electron.vy(), electron.py(), electron.pt(), refx_, refy_); 
  return value;
}

void
DiElectronAnalyzer::fillElectronHistogram(vector<reco::GsfElectron> recoElectronSet, TH1D* histogram)
{
  for (unsigned int j = 0 ; j < recoElectronSet.size() ; j++)
    {
       vector<string> name = getStringSegments(string(histogram->GetName()), '_');
       double value = electronVariables(recoElectronSet[j], name[0]);
       histogram->Fill(value);
    }
}

void
DiElectronAnalyzer::fillElectron2DHistogram(vector<reco::GsfElectron> recoElectronSet, TH2D* histogram)
{
  for (unsigned int j = 0 ; j < recoElectronSet.size() ; j++)
    {
       vector<string> name = getStringSegments(string(histogram->GetName()), '_');
       double valuex = electronVariables(recoElectronSet[j], name[0]);
       double valuey = electronVariables(recoElectronSet[j], name[1]);
       histogram->Fill(valuex,valuey);
    }
}

void
DiElectronAnalyzer::fillElectronPairHistogram(vector<vector<reco::GsfElectron>> recoElectronPair, TH1D* histogram)
{
  for (unsigned int j = 0 ; j < recoElectronPair.size() ; j++)
    {
       vector<string> name = getStringSegments(string(histogram->GetName()), '_');
       double value = electronPairVariables(recoElectronPair[j], name[0]);
       histogram->Fill(value);
    }
}

bool
DiElectronAnalyzer::evaluateTrigger(vector<string> triggerList, const edm::Event &event)
{ 
  bool passTrigger = false;
  if(triggerList.size() == 0)
    passTrigger = true; 
  else
    {
      for(unsigned int j = 0 ; j < triggerList.size() ; j++)    
        {
          event.getByLabel(triggerResultsLabel_, triggerResults);
          if(triggerResults.isValid())
            {
              const edm::TriggerNames & trigNames = event.triggerNames(*triggerResults);
              unsigned int numTriggers = trigNames.size();
              for( unsigned int hltIndex=0; hltIndex<numTriggers-1; ++hltIndex )
                {
                  std::string currentTrigName = trigNames.triggerName(hltIndex);
                  if (currentTrigName.find(triggerList[j]) < currentTrigName.length() && triggerResults->accept(hltIndex))
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
DiElectronAnalyzer::getStringSegments(string input, char delim)
{
    vector<string> elems;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

DEFINE_FWK_MODULE(DiElectronAnalyzer);
