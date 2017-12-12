#!/bin/env python
import ROOT as r

#open file
inF = r.TFile("/uscms/home/wnash/nobackup/cscPat/ntuple/plotTree.root")
myT = inF.plotTree

#init plots
rezPlots = {}

#loop over tree
for event in myT:
    if not (event.envelopeId in rezPlots): rezPlots[event.envelopeId] = {}
    if not (event.patternId in rezPlots[event.envelopeId]): rezPlots[event.envelopeId][event.patternId] = r.TH1D(
            "rez%i_%i_h"%(event.envelopeId,event.patternId),
            "rez%i_%i_h;position difference [strip];Number of Segments"%(event.envelopeId,event.patternId),
            600,-3.0,3.0)
    rezPlots[event.envelopeId][event.patternId].Fill(event.segmentX-event.patX)
    pass

rezMeans = {}
#draw stuff
outF = r.TFile("rezPlots.root","RECREATE")
for env in rezPlots:
    rezMeans[env] = {}
    for pat in rezPlots[env]:
        rezPlots[env][pat].Write()
        rezMeans[env][pat] = rezPlots[env][pat].GetMean()
        pass
    pass

outF.Close()
