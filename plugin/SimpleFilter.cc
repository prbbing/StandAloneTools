#include "SimpleFilter.h"
// An analyzer to filter out events at the reco level.

SimpleFilter::SimpleFilter (const edm::ParameterSet &cfg):
    recoMuon_ (cfg.getParameter<edm::InputTag> ("recoMuon")),
    recoElectron_ (cfg.getParameter<edm::InputTag> ("recoElectron")),
    recoJet_ (cfg.getParameter<edm::InputTag> ("recoJet")),
    ptThresholdOfMuon_ (cfg.getParameter<double> ("ptThresholdOfMuon")),
    ptThresholdOfElectron_ (cfg.getParameter<double> ("ptThresholdOfElectron")),
    ptThresholdOfJet_ (cfg.getParameter<double> ("ptThresholdOfJet")),
    numOfMuon_ (cfg.getParameter<int> ("numOfMuon")),
    numOfElectron_ (cfg.getParameter<int> ("numOfElectron")),
    numOfJet_ (cfg.getParameter<int> ("numOfJet"))
{
}

SimpleFilter::~SimpleFilter ()
{
}

bool
SimpleFilter::filter (edm::Event &event, const edm::EventSetup &setup)
{
   bool flag = false; 
   int numM = 0;
   int numE = 0;
   int numJ = 0;
   if(event.getByLabel(recoMuon_, MuonCollection)){
       for(reco::MuonCollection::const_iterator recoMuon = MuonCollection ->begin(); recoMuon != MuonCollection->end(); recoMuon++){
          if(recoMuon->pt() > ptThresholdOfMuon_ && recoMuon->isGlobalMuon()){
               numM++;}}}
   if(event.getByLabel(recoElectron_, ElectronCollection)){
       for(reco::GsfElectronCollection::const_iterator recoElectron = ElectronCollection ->begin(); recoElectron != ElectronCollection->end(); recoElectron++){
          if(recoElectron->pt() > ptThresholdOfElectron_){
               numE++;}}}
   if(event.getByLabel(recoJet_, JetCollection)){
       for(reco::PFJetCollection::const_iterator recoJet = JetCollection ->begin(); recoJet != JetCollection->end(); recoJet++){
          if(recoJet->pt() > ptThresholdOfJet_ ){
               numJ++;}}}
   if(numM >= numOfMuon_ && numE >= numOfElectron_ && numJ >= numOfJet_){
       flag = true;}
   return flag;
}

DEFINE_FWK_MODULE(SimpleFilter);
