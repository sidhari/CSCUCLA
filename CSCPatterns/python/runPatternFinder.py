#!/usr/bin/env python

import os
import sys

import ROOT as r

blocksize = 10000 #warning, this is also defined in PatternConstants.h TODO: reduce duplication in C and python code
ram = "1024M"

#recompile executable
print("Compiling executable...")
os.system("make -C../src")

    
#get number of entries in tree
inF = r.TFile("../data/CSCDigiTree-2016F.root")
myT = inF.CSCDigiTree
entries = myT.GetEntries()

split = entries / blocksize
mod = entries % blocksize


jobs = split if not mod else split+1
print("Entries = %i, splitting into %i job(s) of size %i"%(entries,jobs, blocksize))


# -V = Exports environment variables
# -N = Names the job
# -l = Resource allocation
# -t = Job array 
#
# More here: https://www.ccn.ucla.edu/wiki/index.php/Hoffman2:Submitting_Jobs
#
os.system("qsub -V -N pattFinder -l h_data=%s,time=00:05:00 -t 1:%i:1 ../jobs/sh/subScript.sh"%(ram, jobs))

