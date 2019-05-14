#!/usr/bin/env python
import ROOT as r
import sys
sys.path.append("../")
import Plotter as p

writePath ='~/Documents/Presentations/2018/181202-CCStatus/'

r.gSystem.Load('../../lib/LUTClasses_cpp')
r.gSystem.Load('../../lib/PatternFinderHelperFunctions_cpp')
r.gSystem.Load('../../lib/PatternFinderClasses_cpp')


lut = r.LUT("test", "../../dat/linearFits.lut");
lut.loadROOT("/afs/cern.ch/user/w/wnash/public/lutExample.root")


# Sort functionality for how lookup table is stored in memory
# p = probability for muon association after clct is found
# s = number of segments found
# c = number of clcts found
# l = number of layers in code
# m = average multiplicity within chamber
# x = chi^2 of fit
# e = energy (average momentum)
# k = key (pattern, code)
lut.sort("pcxl")


#Can iterate through LUT using
for [key, entry] in lut:
    #print entry.slope()
    if(entry._layers < 3): continue
    #code

# args = nclcts, nsegments, nlayers
lut.printPython(0,100)

