#!/usr/bin/env python

import os
import sys

import ROOT as r

cores = 4 #amount of cores we will default to using 

if len(sys.argv) > 1: #second argument is number of cores
    cores = int(sys.argv[1])


useCompHits = 0 # 0 means use recHits
if(useCompHits):
    folder = "compHits"
else:
    folder= "recHits"
    
#get number of entries in tree
inF = r.TFile("../data/CSCDigiTree161031.root")
myT = inF.CSCDigiTree
entries = myT.GetEntries()

#temp
entries = 1003


split = entries / cores
mod = entries % cores


filepath = "../src/PatternFinder.cpp"
covered = 0
while(covered < entries):
    
    start = covered
    covered += split +mod if(covered + split+mod == entries) else split
    end = covered
    #start up a new thread
    print("Starting thread: %i - %i"%(start,end))
    os.system("qsub -N E%i -l h_data=1024M,time=00:05:00 '../sub/sh/subScript.sh %i %i' "%(start, start, end))
    #os.system("root -l -q ../src/PatternFinder.cpp++\(%i,%i\) &"%(start,end))

