#include "MuonValAnalyzer.h"
#include <array>
using namespace std;
using namespace reco;
using namespace edm;

MuonValAnalyzer::MuonValAnalyzer (const edm::ParameterSet &cfg) :
  recoTrack_ (cfg.getParameter<edm::InputTag> ("recoTracks")),
  beamSpot_ (cfg.getParameter<edm::InputTag> ("beamSpot")),
  genParticle_ (cfg.getParameter<edm::InputTag> ("genParticle")),
  primaryVertex_ (cfg.getParameter<edm::InputTag> ("primaryVertex")),
  matchDelRCut_ (cfg.getParameter<double> ("matchDelRCut")),
  matchRelPtCut_ (cfg.getParameter<double> ("matchRelPtCut")),
  LxyCut_ (cfg.getParameter<double> ("LxyCut")),
  LxyBins_ (cfg.getParameter<vector<double>> ("LxyBins")),
  PtResBins_ (cfg.getParameter<vector<double>> ("PtResBins"))
 {
  TH1D::SetDefaultSumw2 ();  
  genMuFromStopLxy = fs->make<TH1D>("genMuFromStopLxy","genMuFromStopLxy",int(LxyBins_[0]),LxyBins_[1],LxyBins_[2]);
  recoMatchedGenMuFromStopLxy = fs->make<TH1D>("recoMatchedGenMuFromStopLxy","recoMatchedGenMuFromStopLxy",int(LxyBins_[0]),LxyBins_[1],LxyBins_[2]);
  recoEffLxy = fs->make<TH1D>("effic_vs_vertpos","effic_vs_vertpos",int(LxyBins_[0]),LxyBins_[1],LxyBins_[2]);
  ptRes = fs->make<TH1D>("ptRes","ptRes",int(PtResBins_[0]),PtResBins_[1],PtResBins_[2]);
  numVertex = fs->make<TH1D>("numVertex","numVertex",60,0,60);
  deltaRVsLxy = fs->make<TH2D>("deltaRVsLxy","deltaRVsLxy",int(LxyBins_[0]),LxyBins_[1],LxyBins_[2],100,0,1);
 }

MuonValAnalyzer::~MuonValAnalyzer ()
{
}

void
MuonValAnalyzer::analyze(const edm::Event &event, const edm::EventSetup &setup)
{  
  event.getByLabel(genParticle_, genParticles);  
  event.getByLabel(beamSpot_, BeamSpot);
  BeamSpotX = BeamSpot -> x0();
  BeamSpotY = BeamSpot -> y0();
  BeamSpotZ = BeamSpot -> z0(); 
  
  event.getByLabel(primaryVertex_, PrimaryVertex);  
  double numOfVertex = 0;
  for (reco::VertexCollection::const_iterator primaryVertex = PrimaryVertex->begin(); primaryVertex != PrimaryVertex->end(); primaryVertex++)
    {
      if(primaryVertex->isValid())
        numOfVertex++;
    } 
  numVertex->Fill(numOfVertex);  

  for (reco::GenParticleCollection::const_iterator genParticle = genParticles->begin(); genParticle != genParticles->end(); genParticle++)
    {
      double Lxy = fabs(LxyBeamSpot(genParticle->vx(), genParticle->vy(), BeamSpotX, BeamSpotY));
      if(Lxy > LxyCut_)
        continue;
      double recoPt = 0;
      double recoCharge = 0;
      if(fabs(genParticle->pdgId()) == 13 && fabs(genParticle->mother()->pdgId()) == 1000006 && genParticle->pt() > 10 && fabs(genParticle->eta()) < 2.4)
        {
          genMuFromStopLxy->Fill(Lxy);
          bool toFill1D = false;
          reco::GenParticle currentGen = (*genParticle); 
          if(event.getByLabel(recoTrack_, TrackCollection))
            { 
              map<double, reco::Track> matchedMapTracks;
              for(reco::TrackCollection::const_iterator recoTrack = TrackCollection ->begin(); recoTrack != TrackCollection->end(); recoTrack++)
                {
                  reco::Track currentTrack = (*recoTrack);
                  double DeltaR = deltaR(genParticle->eta(),genParticle->phi(),recoTrack->eta(),recoTrack->phi());
                  if(DeltaR < 1) 
                    deltaRVsLxy->Fill(Lxy, DeltaR);
                  if(Match(currentGen, currentTrack).at(0) < matchDelRCut_ && Match(currentGen, currentTrack).at(1) < matchRelPtCut_)
                    matchedMapTracks.insert(pair<double, reco::Track>(Match(currentGen, currentTrack).at(0), currentTrack));
                }
                  if(matchedMapTracks.size())
                    {
                      toFill1D = true;
                      recoPt = matchedMapTracks.begin()->second.pt();   
  	              recoCharge = matchedMapTracks.begin()->second.charge();
                    }
              matchedMapTracks.clear(); 
           } 
          if(toFill1D)
            {
              recoMatchedGenMuFromStopLxy->Fill(Lxy); 
              recoEffLxy->Fill(Lxy); 
              double ptResolution = (recoCharge/recoPt - genParticle->charge()/genParticle->pt())*genParticle->pt()/genParticle->charge(); 
              ptRes->Fill(ptResolution);
            } 
       }
    }
}
double 
MuonValAnalyzer::LxyBeamSpot(double vx, double vy, double refx, double refy)
{
  return sqrt(pow(vx-refx,2) + pow(vy-refy, 2));
}

vector<double>
MuonValAnalyzer::Match(reco::GenParticle gen, reco::Track track)
{
  vector<double> matchedValues;
  double delRmatch = deltaR(gen.eta(),gen.phi(),track.eta(),track.phi());
  matchedValues.push_back(delRmatch);
  double relPtmatch = fabs((track.pt()-gen.pt())/gen.pt()); 
  matchedValues.push_back(relPtmatch);
  return matchedValues;
}


void
MuonValAnalyzer::endJob()
{
  recoEffLxy->Divide(genMuFromStopLxy) ;    
}

DEFINE_FWK_MODULE(MuonValAnalyzer);
