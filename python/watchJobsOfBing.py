#!/usr/bin/env python
import sys
import os
import glob
import re
import time
import fcntl
Tag = ""

for i in range(0,10000):
        if "0 jobs; 0 completed, 0 removed, 0 idle, 0 running, 0 held, 0 suspended" not in Tag:
            condorReadout = os.popen("condor_q bing").readlines()
            for line in condorReadout:
                if "suspended" in line:
                    print line
                    Tag = str(line)
            time.sleep(10)
        else:
                 break
