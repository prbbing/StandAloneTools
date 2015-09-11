#include "IsolationCalculator.h"

using namespace std;
using namespace reco;
using namespace edm;

IsolationCalculator::IsolationCalculator (const edm::ParameterSet &cfg) :
New_ (cfg.getParameter<bool> ("New")), 
match_ (cfg.getParameter<double> ("match")), 
veto_ (cfg.getParameter<double> ("veto")), 
vertices_(consumes<vector<reco::Vertex>>(cfg.getParameter<edm::InputTag>("vertices")))
{
  TH1D::SetDefaultSumw2 ();  
  PFIso = fs->make<TH1D>("PFIso","PFIso",50,0,5);
  chargedHadronPtHist = fs->make<TH1D>("chargedHadronPt","chargedHadronPt",100,0,100);
  sumPUPtHist = fs->make<TH1D>("sumPUPt","sumPUPt",100,0,100);
  muonPVIndexHist = fs->make<TH1D>("muonPVIndex","muonPVIndex",10,0,10);
  pfCandidateToken_ = consumes<vector<pat::PackedCandidate>>(edm::InputTag("packedPFCandidates","", "PAT"));
  muonToken_ = consumes<vector<pat::Muon>>(edm::InputTag("slimmedMuons","","PAT"));
  genParticleToken_ = consumes<vector<reco::GenParticle>>(edm::InputTag("prunedGenParticles","","PAT"));
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
  Handle<vector<reco::GenParticle>> gens; 
  event.getByToken(genParticleToken_, gens);
  Handle<vector<reco::Vertex>> vertexCollection;
  event.getByToken(vertices_,vertexCollection);  
  vector<reco::Vertex> vertices;
  for (auto vtx = vertexCollection->begin(); vtx != vertexCollection->end(); vtx++)
    {
      vertices.push_back(*vtx);
    }
  if(muons.isValid())
    {
      for (auto muon = muons->begin(); muon != muons->end(); muon++)
        {
          if (!(muon->isPFMuon()&& muon->isGlobalMuon() && muon->pt() > 25))
            continue;
          bool genMatched = false;
          for(auto gen = gens->begin(); gen != gens->end(); gen++)
            {
              if(deltaR(muon->eta(),muon->phi(),gen->eta(),gen->phi()) < 0.5 && abs(gen->pdgId()) == 13 && gen->status() == 1)
                {
                  reco::GenParticle thisParticle = (*gen);
                  if(genMatching(thisParticle, match_, veto_))
                    genMatched = true;
                }
            } 
          if(!genMatched)
            continue;      
          double chargedHadronPt = 0;
          double puPt = 0;
          int muonPVIndex = 0;
          if(cands.isValid())
            { 
              for (auto cand = cands->begin(); cand != cands->end(); cand++) 
                {
                  if (abs(cand->pdgId()) == 13 && deltaR(muon->eta(),muon->phi(),cand->eta(),cand->phi()) < 0.01)
                    { 
                       pat::PackedCandidate thisPFCandidate = (*cand); 
                       muonPVIndex = chargedHadronVertex(vertices, thisPFCandidate);
                       muonPVIndexHist->Fill(double(muonPVIndex));
                    }
                }
              for (auto cand = cands->begin(); cand != cands->end(); cand++) 
                {
                  if((abs(cand->pdgId()) == 211 || abs(cand->pdgId()) == 321 || abs(cand->pdgId()) == 999211 || abs(cand->pdgId()) == 2212) && deltaR(muon->eta(),muon->phi(),cand->eta(),cand->phi()) < 0.4)
                    { 
                      pat::PackedCandidate thisPFCandidate = (*cand); 
                      int ivtx = chargedHadronVertex(vertices, thisPFCandidate);
                      if(ivtx == muonPVIndex)
                        chargedHadronPt = cand->pt() + chargedHadronPt;
                      else
                        puPt = cand->pt() + puPt;
                    }
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

bool
IsolationCalculator::genMatching(reco::GenParticle &genParticle, double selectId, double vetoId)
{
 bool matched = false;
  if(genParticle.mother())
    {
      int n = genParticle.numberOfMothers();
      for(int j = 0; j < n; ++ j)
        {
          reco::GenParticle *mother = (reco::GenParticle*) genParticle.mother(j);
          if(fabs(mother->pdgId()) == selectId)
            matched = true;
          else if(fabs(mother->pdgId()) == vetoId)
            {
              matched = false;
              break;
            }
          else
            matched = genMatching(*mother, selectId, vetoId);
        }
   }
  return matched;
}
int 
IsolationCalculator::chargedHadronVertex(const vector<reco::Vertex> &vertices, const pat::PackedCandidate &pfcand)
{
  int iVertex = -1;
  int vertexFlag = 0; 
  for(uint i = 0; i < vertices.size(); ++i)
    {
      if(pfcand.fromPV(int(i)) > vertexFlag)
        {
          vertexFlag = pfcand.fromPV(int(i));
          iVertex = int(i);
        }
    }
  return iVertex;
}
