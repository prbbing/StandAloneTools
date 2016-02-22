#!/usr/bin/env python
import sys
import os
import re

filePath = str(sys.argv[1])
targetFile = str(sys.argv[2])

command = "hltGetConfiguration /dev/CMSSW_8_0_0/GRun --full --offline --no-output --mc --unprescale --process TEST --globaltag auto:run2_mc_GRun --paths \""

file = open(filePath,'r')
for line in file:
    command = command + str(line).rstrip('\n') + ','
finalCommand = command.rstrip(',') + '" > ' + targetFile
os.system(finalCommand)

#Example: python dumpEXOPaths.py exoHLTList.txt exoHLTConfigs.py 
#exoHLTList.txt will have something like:
#HLT_DoubleMu33NoFiltersNoVtx_v*
#HLT_DoubleMu38NoFiltersNoVtx_v*
#HLT_Mu38NoFiltersNoVtx_Photon38_CaloIdL_v*
#HLT_Mu42NoFiltersNoVtx_Photon42_CaloIdL_v*
#....
