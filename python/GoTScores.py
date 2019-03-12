#############################################
#
#  I am impressed if you actually checking
#  this horseshit out
#  Send me the following code I'll buy you 
#  a coffee
#  "Giant Gummy Bear"
#  To use this tool: 
#     python GoTScores.py demoInput.txt
#
#############################################
import os,sys,re
from ROOT import *

if len(sys.argv) < 2:
  print "You are too lame to watch GoT."
  sys.exit(0)

playerInputString = sys.argv[1]

playerInputFile = open(playerInputString,"r")

dataArray = []

for line in playerInputFile:
  dataArray = line.rstrip('\n').split(',')

sumWeight = 0

for entry in range(1, len(dataArray)):
  sumWeight = sumWeight + TMath.Gaus(int(dataArray[entry]),int(dataArray[0]))

print "Total Weight is : " + str(sumWeight)

totalScore = 0

for entry in range(1, len(dataArray)):
  print "Player " + str(entry) + " has a score of: " + str(20*TMath.Gaus(int(dataArray[entry]),int(dataArray[0]))/sumWeight)
