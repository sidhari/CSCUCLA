#!/bin/env python
import ROOT as r

#open file
inF = r.TFile("/home/bravo/move/plotTree.root")
myT = inF.plotTree

#init plots
rezPlots = {}

#loop over tree
Nev = myT.GetEntries()
Nproc = 0
for event in myT:
    Nproc += 1
    if Nproc%1000 == 0: print "Processing event %i of %i"%(Nproc,Nev)
    if event.RI != 1 or event.ST != 2: continue
    #if event.segmentX < 10.0 or event.segmentX > 70.0: continue
    if not (event.envelopeId in rezPlots): rezPlots[event.envelopeId] = {}
    if not (event.patternId in rezPlots[event.envelopeId]): rezPlots[event.envelopeId][event.patternId] = r.TH1D(
            "rez%i_%i_h"%(event.envelopeId,event.patternId),
            "rez%i_%i_h;position difference [strip];Number of Segments"%(event.envelopeId,event.patternId),
            600,-3.0,3.0)
    rezPlots[event.envelopeId][event.patternId].Fill(event.segmentX-event.patX)
    pass

#draw stuff
posRes_h = r.TH1D("posRes_h","posRes_h",400,-2.0,2.0)
envPosRes_h = r.TH1D("envPosRes_h","envPosRes_h",400,-2.0,2.0)
outF = r.TFile("rezPlots.root","RECREATE")
Nproc = 0
for event in myT:
    Nproc += 1
    if Nproc%1000 == 0: print "Processing event %i of %i"%(Nproc,Nev)
    if event.RI != 1 or event.ST != 2: continue
    #if event.segmentX < 10.0 or event.segmentX > 70.0: continue
    posRes_h.Fill(event.segmentX-event.patX-rezPlots[event.envelopeId][event.patternId].GetMean())
    envPosRes_h.Fill(event.segmentX-event.patX)
    pass

posRes_h.Write()
envPosRes_h.Write()
outF.Close()
