#!/usr/bin/env python
import sys
import os
import re

exoConfigs = str(sys.argv[1])
outputSummary = str(sys.argv[2])

command = "git grep 'l1extra::L1' | cut -d: -f1 | sort -u > tmpFile"
os.system(command)
moduleNameList = []
file = open("tmpFile",'r')
for line in file:
    if '.cc' in str(line):
        moduleNameList.append(line.split('/')[-1].rstrip('.cc\n'))
for module in moduleNameList:
    os.system('grep "\'' + module + '\'" ' + exoConfigs + ' >> ' + outputSummary)
file.close()
os.system('rm tmpFile')

#Example: python checkEXOPaths.py exoHLTConfigs.py problematicSequence.txt
#exoHLTConfigs.py is the file got from dumpEXOPaths.py
