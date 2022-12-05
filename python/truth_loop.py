# run this from an environment that has asetup AnalysisBase,blahblah
import ROOT
if(not ROOT.xAOD.Init().isSuccess()): print('Failed xAOD.Init()')

f = ROOT.TFile('DAOD_TRUTH1.truth1.root')
t = ROOT.xAOD.MakeTransientTree(f, 'CollectionTree')
o = ROOT.TFile("out.root","RECREATE")
hist = ROOT.TH1D("frac","frac",20,0,1)
o.cd()

def find_truth_xds():
    print(t.GetEntries())
    for i in range(1000):
    #for i in range(2):
        numInv = 0
        num = 0
        t.GetEntry(i)
        # Get truth vertex container
        truth_vertices = t.TruthVertices
        # Loop over every vertex
        for j in range(len(truth_vertices)):
            #print("truth vertex: " + str(j))
            for k in range(0, truth_vertices.at(j).incomingParticleLinks().size()):
              #print("incoming particle: " + str(k))
              pdgId = truth_vertices.at(j).incomingParticleLinks().at(k).pdgId()
              #print("pdgid: " + str(pdgId))
              #if 1: 
              if abs(pdgId) in [4900111, 4900113, 4900211, 4900213]:
                if truth_vertices.at(j).outgoingParticleLinks().at(0).pdgId() !=  truth_vertices.at(j).incomingParticleLinks().at(k).pdgId():
                  #print(pdgId)
                  for l in range(0, truth_vertices.at(j).outgoingParticleLinks().size()):
                    #print("outgoing particle: " + str(l))
                    #print("pdgid: " + str(truth_vertices.at(j).outgoingParticleLinks().at(l).pdgId()))
                    if abs(truth_vertices.at(j).outgoingParticleLinks().at(l).pdgId()) == 51 or abs(truth_vertices.at(j).outgoingParticleLinks().at(l).pdgId()) == 53:
                      numInv = numInv + 1.0
                    else:
                      num = num + 1.0
        print(float(numInv)/(numInv + num))
        hist.Fill(float(numInv)/(numInv + num))

find_truth_xds()
hist.Write()
o.Close()
