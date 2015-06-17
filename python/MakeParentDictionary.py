#!/usr/bin/env python
 
import os
import re
from optparse import OptionParser
from OSUT3Analysis.Configuration.processingUtilities import *
 
parser = OptionParser()
parser = set_commandline_arguments(parser)
 
parser.remove_option("-d")
parser.remove_option("-o")
 
parser.add_option("-d", "--dataset", dest="Dataset", default = "", help="Specify which dataset to run.")
parser.add_option("-o", "--output", dest="Output", default = "", help="Define the name of the output file")
 
(arguments, args) = parser.parse_args()
Dataset = arguments.Dataset
Output = arguments.Output
 
#fileList = os.popen('python das_client_new.py --query="file dataset=' + str(Dataset) + '" ' + '--instance prod/phys03 --limit 0').readlines()
fileList = os.popen('python das_client_new.py --query="file dataset=' + str(Dataset) + '" ' + '--instance prod/phys03 --limit 0').readlines()
 
os.system('touch ' + Output + '.py')
dicTmp = open(Output + '.py','r+w')
 
dicTmp.write('parentDic = {\n')
 
for file in fileList:
    parentList = os.popen('python das_client_new.py --query="parent file=' + str(file) + '" ' + '--instance prod/phys03 --limit 0').readlines()
    Parent = ''
    for parent in parentList:
        if len(parentList) == 1:
            Parent = Parent + '["root://cmsxrootd.fnal.gov/' + str(parent.strip('\n')) + '"]'
        elif parent == parentList[0]:
            Parent = Parent + '["root://cmsxrootd.fnal.gov/' + str(parent.strip('\n')) + '",'
        elif parent != parentList[len(parentList)-1]:
            Parent = Parent + '"root://cmsxrootd.fnal.gov/' + str(parent.strip('\n')) + '",'
        else:
            Parent = Parent + '"root://cmsxrootd.fnal.gov/' + str(parent.strip('\n')) + '"]'
    dicTmp.write('"' + 'root://cmsxrootd.fnal.gov/' + str(file.strip('\n')) + '" : ' + Parent + ',\n')
dicTmp.write('}')
dicTmp.close()
