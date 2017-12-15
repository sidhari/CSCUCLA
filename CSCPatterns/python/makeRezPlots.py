#!/bin/env python
import ROOT as r

#open file
inF = r.TFile("/home/bravo/move/plotTree.root")
myT = inF.plotTree

#init plots
rezPlots = {}

#Build full ID lookup
idT = r.TTree("idTree","Tree holding preproccessed id info")
idT.ReadFile("CSCDigiTree161031.ave")
idMean = {}
idNseg = {}
for event in idT:
    idMean[event.patID] = event.mean
    idNseg[event.patID] = event.Nseg
    pass

#draw stuff
posRes_h = r.TH1D("posRes_h","posRes_h",400,-2.0,2.0)
envPosRes_h = r.TH1D("envPosRes_h","envPosRes_h",400,-2.0,2.0)
outF = r.TFile("rezPlots.root","RECREATE")
Nev = myT.GetEntries()
Nproc = 0
for event in myT:
    Nproc += 1
    if Nproc%1000 == 0: print "Processing event %i of %i"%(Nproc,Nev)
    if event.RI != 1 or event.ST != 2: continue
    if not (event.patternId in idMean): 
        print "patId %i does not exist in ave file"%event.patternId
        continue
    if idNseg[event.patternId] < 10: continue
    #if event.segmentX < 10.0 or event.segmentX > 70.0: continue
    posRes_h.Fill(event.segmentX-event.patX-idMean[event.patternId])
    envPosRes_h.Fill(event.segmentX-event.patX)
    pass

posRes_h.Write()
envPosRes_h.Write()
outF.Close()
