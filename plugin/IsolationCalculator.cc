#include "IsolationCalculator.h"

using namespace std;
using namespace reco;
using namespace edm;

IsolationCalculator::IsolationCalculator (const edm::ParameterSet &cfg) :
New_ (cfg.getParameter<bool> ("New")), 
match_ (cfg.getParameter<double> ("match")), 
veto_ (cfg.getParameter<double> ("veto")),
vertex_ (cfg.getParameter<edm::InputTag>("vertex")) 
{
  TH1D::SetDefaultSumw2 ();  
  PFIso = fs->make<TH1D>("PFIso","PFIso",500,0,5);
  chargedHadronPtHist = fs->make<TH1D>("chargedHadronPt","chargedHadronPt",100,0,100);
  sumPUPtHist = fs->make<TH1D>("sumPUPt","sumPUPt",100,0,100);
  muonPVIndexHist = fs->make<TH1D>("muonPVIndex","muonPVIndex",10,0,10);
  chargedHadronPVIndexHist = fs->make<TH1D>("chargedHadronPVIndex","chargedHadronPVIndex",30,0,30);
  pfCandidateToken_ = consumes<vector<pat::PackedCandidate>>(edm::InputTag("packedPFCandidates","", "PAT"));
  muonToken_ = consumes<vector<pat::Muon>>(edm::InputTag("slimmedMuons","","PAT"));
  //genParticleToken_ = consumes<vector<reco::GenParticle>>(edm::InputTag("prunedGenParticles","","PAT"));
}

IsolationCalculator::~IsolationCalculator ()
{
}

void
IsolationCalculator::analyze (const edm::Event &event, const edm::EventSetup &setup)
{
  Handle<vector<pat::PackedCandidate>> cands; 
  event.getByToken(pfCandidateToken_, cands);
  Handle<vector<pat::Muon>> muons; 
  event.getByToken(muonToken_, muons);
 // Handle<vector<reco::GenParticle>> gens; 
  //event.getByToken(genParticleToken_, gens);
  //Handle<vector<reco::Vertex>> vtxs;
  //int numVertex = 0;
  //if(event.getByLabel(vertex_, vtxs))
  //  {
  //    numVertex = vtxs->size();
  //  } 
  if(muons.isValid())
    {
      for (auto muon = muons->begin(); muon != muons->end(); muon++)
        {
          if (!(muon->isPFMuon()&& muon->isGlobalMuon() && muon->pt() > 0))
            continue;
          //bool genMatched = false;
          //for(auto gen = gens->begin(); gen != gens->end(); gen++)
         //   {
         //     if(deltaR(muon->eta(),muon->phi(),gen->eta(),gen->phi()) < 0.5 && abs(gen->pdgId()) == 13 && gen->status() == 1)
         //       {
          //        reco::GenParticle thisParticle = (*gen);
          //        if(genMatching(thisParticle, match_, veto_))
          //          genMatched = true;
          //      }
          //  }
          //if(!genMatched) 
          //  continue;
          double chargedHadronPt = 0;
          double puPt = 0;
          int muonPVIndex = 0;
          if(cands.isValid())
            { 
              for (auto cand = cands->begin(); cand != cands->end(); cand++) 
                {
                  if (abs(cand->pdgId()) == 13 && deltaR(muon->eta(),muon->phi(),cand->eta(),cand->phi()) < 0.001)
                    { 
                       pat::PackedCandidate muonPFCandidate = (*cand); 
                       muonPVIndex = cand->vertexRef().index();//chargedHadronVertex(numVertex, thisPFCandidate);
                       muonPVIndexHist->Fill(double(muonPVIndex));
                    }
                }
              if(muonPVIndex != 0)
                {
                  for (auto cand = cands->begin(); cand != cands->end(); cand++) 
                    {
                      if((abs(cand->pdgId()) == 13 || abs(cand->pdgId()) == 11 || abs(cand->pdgId()) == 211 || abs(cand->pdgId()) == 321 || abs(cand->pdgId()) == 999211 || abs(cand->pdgId()) == 2212) && deltaR(muon->eta(),muon->phi(),cand->eta(),cand->phi()) <= 0.4)
                      //if(abs(cand->pdgId()) == 211 && deltaR(muon->eta(),muon->phi(),cand->eta(),cand->phi()) < 0.4)
                        { 
                          pat::PackedCandidate thatPFCandidate = (*cand); 
                          int ivtx = cand->vertexRef().index(); //chargedHadronVertex(numVertex, thatPFCandidate);
                          chargedHadronPVIndexHist -> Fill(double(ivtx)); 
                          if(ivtx == muonPVIndex)
                            { 
                              if(deltaR(muon->eta(),muon->phi(),cand->eta(),cand->phi()) > 0.001)
                                chargedHadronPt = cand->pt() + chargedHadronPt;
                            }
                          else if(cand->pt() >= 0.5 && deltaR(muon->eta(),muon->phi(),cand->eta(),cand->phi()) > 0.01)
                            puPt = cand->pt() + puPt;
                        }
                    }
                 double muonIsoNew = (chargedHadronPt + max(0.0,muon->pfIsolationR04().sumNeutralHadronEt + muon->pfIsolationR04().sumPhotonEt - 0.5*puPt))/muon->pt(); 
                 double muonIsoDefault = (muon->pfIsolationR04().sumChargedHadronPt + max(0.0,muon->pfIsolationR04().sumNeutralHadronEt + muon->pfIsolationR04().sumPhotonEt - 0.5*muon->pfIsolationR04().sumPUPt))/muon->pt(); 
                if(New_)
                  { 
                    PFIso->Fill(muonIsoNew);
                    chargedHadronPtHist->Fill(chargedHadronPt);
                    sumPUPtHist->Fill(puPt);
                  }
                else
                  {
                    PFIso->Fill(muonIsoDefault);
                    chargedHadronPtHist->Fill(muon->pfIsolationR04().sumChargedHadronPt);
                    sumPUPtHist->Fill(muon->pfIsolationR04().sumPUPt);
                  }
             }
          } 
      }  
  }
}

//bool
//IsolationCalculator::genMatching(reco::GenParticle &genParticle, double selectId, double vetoId)
//{
// bool matched = false;
//  if(genParticle.mother())
//    {
//      int n = genParticle.numberOfMothers();
//      for(int j = 0; j < n; ++ j)
//        {
//          reco::GenParticle *mother = (reco::GenParticle*) genParticle.mother(j);
//          if(fabs(mother->pdgId()) == selectId)
//            matched = true;
//          else if(fabs(mother->pdgId()) == vetoId)
//            {
//             matched = false;
//             break;
//           }
//          else
//            matched = genMatching(*mother, selectId, vetoId);
//        }
//   }
//  return matched;
//}

//int 
//IsolationCalculator::chargedHadronVertex(int numVertex, const pat::PackedCandidate &pfcand)
//{
//  int iVertex = -1;
//  int vertexFlag = -1; 
//  for(int i = 0; i < numVertex; ++ i)
//    {
//      if(!pfcand.vertexRef().isNull())
//        {
//          if(pfcand.fromPV(i) > vertexFlag)
//            { 
//              vertexFlag = pfcand.fromPV(i);
//              iVertex = i;
//            }
//        }
//    }
//  return iVertex;
//}

DEFINE_FWK_MODULE(IsolationCalculator);
