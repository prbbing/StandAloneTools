#!/usr/bin/env python
# this script will automatically generate a copy of the cmsRun config(argv[1]) with a specific name (argv[2]) and change the nameo of the output file.

import sys
import os
import glob
import re
import datetime
import time

config = sys.argv[1]
name = sys.argv[2]
os.system('touch ' + config.split('.')[0] + '_' + str(name) + '.py')
newConfig = config.split('.')[0] + '_' + str(name) + '.py'
tmp = open(config, 'r')
new = open(newConfig,'r+w')
for line in tmp:
    if 'fileName' in line and 'fileNames' not in line:
        line = line.split('.')[0] + '.' + line.split('.')[1] + '_' + str(name) + '.' + line.split('.')[2]
        new.write(line)
    else:
        new.write(line)
tmp.close()
new.close()
os.system('nohup cmsRun ' + str(newConfig) + ' >& ' + 'cmsRun_' + str(newConfig).split('.')[0] + '.log &')
