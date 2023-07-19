import h5py
import os,sys
import uproot
import numpy as np
import optparse
import ROOT

parser = optparse.OptionParser()
parser.add_option('-i','--inputFile',   dest='inputFile', help='Specify the path to the input file.', default='')
parser.add_option('-o','--outputFile',   dest='outputFile', help='Specify the path to the output file.', default='')
parser.add_option('-s','--score',   dest='score', help='Specify the anomaly score to cut on.', default=0)
(arguments, args) = parser.parse_args()

if not arguments.inputFile or not arguments.outputFile:
  print("No input or output given! Aborting")
  sys.exit(0)

outputFile = ROOT.TFile(arguments.outputFile, "RECREATE")

inputFile = h5py.File(arguments.inputFile,'r+')

outputFile.cd()
mtHist = ROOT.TH1D("mT","mT",76, 1200, 5000)
scoreData = inputFile['data']["score"]
rTData = inputFile['data']["rT"]

#mtData = inputFile['data']["mT_jj"][np.logical_and(scoreData[:] >= float(arguments.score), rTData[:] > 0.1)]
#weightData = inputFile['data']["weight"][np.logical_and(scoreData[:] >= float(arguments.score), rTData[:] > 0.1)]
mtData = inputFile['data']["mT_jj"][scoreData[:] >= float(arguments.score)]
weightData = inputFile['data']["weight"][scoreData[:] >= float(arguments.score)]
for i in range(0, len(mtData)):
  mtHist.Fill(mtData[i], weightData[i])

outputFile.cd()
mtHist.Write()
inputFile.close()
outputFile.Close()
