import ROOT as r
import Plotter as p


#writePath ='~/Documents/Presentations/2018/181026-3LayerEff/'
# 
# f = r.TFile('../data/SingleMuon/zskim2018D-full/CLCTLayerAnalysis-Full.root')
# print f
# if not hasattr(f, 'IsOpen'):
#     print "can't open file"
#     exit()

r.gSystem.Load('../lib/LUTClasses_cpp')
r.gSystem.Load('../lib/PatternFinderHelperFunctions_cpp')
r.gSystem.Load('../lib/PatternFinderClasses_cpp')


lut = r.LUT("test", "../dat/linearFits.lut")
lut.loadROOT('/home/wnash/workspace/CSCUCLA/CSCPatterns/dat/SingleMuon/zskim2018D/BayesAnal-1Star.root')
#lut.loadROOT('../temp.root')

lut.printPython(1)