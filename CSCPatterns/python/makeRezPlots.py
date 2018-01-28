#!/usr/bin/env python
import ROOT as r
import numpy as np
import math as m

def decodeChamberCode(chamberCode):
    return 0

def printProgress(counter, entries):
    if((counter % 1000000) == 0) : print("Finished %0.2f%% of events"%(100.*counter/entries))

#Here we can select out if we want to look at only ME11, etc
def validEvent(event,st,ri):
    if((event.ST == st or st == 0) and (event.RI == ri or ri == 0)) : #0 means be indiscriminate
        return True
    return False


def createHists(chamber):
    print("=== Running over Chamber %s ==="%(chamber[0]))
    
    #open file
    inF = r.TFile("../data/plotTree.root")
    myT = inF.plotTree
    
    
    #init plots
    ccPosMeans = {}
    ccSlopeMeans = {}
    patternPosMeans = {}
    patternSlopeMeans = {}
    legacyPosMeans = {}
    legacySlopeMeans = {}
    
    print("Calculating Means...")
    counter = 0
    entries = myT.GetEntries()
    for event in myT:
        counter +=1
        printProgress(counter,entries)
            
        #check if we shuold look at this event    
        if not validEvent(event, chamber[1], chamber[2]): continue
        
        #fill stuff we dont have yet
        if not (event.envelopeId in patternPosMeans):
            patternPosMeans[event.envelopeId] = []
            patternSlopeMeans[event.envelopeId] = []
            ccPosMeans[event.envelopeId] = {}
            ccSlopeMeans[event.envelopeId] = {}
            
        if not (event.patternId in ccPosMeans[event.envelopeId]):
            ccPosMeans[event.envelopeId][event.patternId] = []
            ccSlopeMeans[event.envelopeId][event.patternId] = []
            
        if not (event.legacyLctId in legacyPosMeans) :
            legacyPosMeans[event.legacyLctId] = []
            legacySlopeMeans[event.legacyLctId] = []
            
        patternPosMeans[event.envelopeId].append(event.segmentX-event.patX)
        patternSlopeMeans[event.envelopeId].append(event.segmentdXdZ)
        ccPosMeans[event.envelopeId][event.patternId].append(event.segmentX-event.patX)
        ccSlopeMeans[event.envelopeId][event.patternId].append(event.segmentdXdZ)
        legacyPosMeans[event.legacyLctId].append(event.segmentX-event.legacyLctX)
        legacySlopeMeans[event.legacyLctId].append(event.segmentdXdZ)     
        
    
    print("Making Histograms...")
    
    pattPosPlots = {}
    pattSlopePlots = {}
    ccPosPlots = {}
    ccSlopePlots = {}
    legacyPosPlots = {}
    legacySlopePlots = {}
    
    
    #loop over tree, again
    counter = 0
    #calculate these once, so it runs faster
    calculatedCCPosMeans = {}
    calculatedCCSlopeMeans = {}
    calculatedPattPosMeans = {}
    calculatedPattSlopeMeans = {}
    calculatedlegacyPosMeans = {}
    calculatedlegacySlopeMeans = {}
    for event in myT:
        counter += 1
        printProgress(counter,entries)
        
        #check if we shuold look at this event    
        if not validEvent(event, chamber[1], chamber[2]): continue
        
        if not (event.envelopeId in calculatedPattPosMeans):
            calculatedPattPosMeans[event.envelopeId] = sum(patternPosMeans[event.envelopeId])/float(len(patternPosMeans[event.envelopeId]))
            calculatedPattSlopeMeans[event.envelopeId] = sum(patternSlopeMeans[event.envelopeId])/float(len(patternSlopeMeans[event.envelopeId]))
            pattPosPlots[event.envelopeId]= r.TH1D("patPos%i"%(event.envelopeId),
                                               "patPos%i;Position Difference [strips]; Events"%(event.envelopeId), 
                                               600,-1.,1.)
            pattSlopePlots[event.envelopeId] = r.TH1D("patSlope%i"%(event.envelopeId),
                                                      "patSlope%i;Slope; Events"%(event.envelopeId),
                                                      600,-1.,1.)
            calculatedCCPosMeans[event.envelopeId] = {}
            calculatedCCSlopeMeans[event.envelopeId] = {}
            ccPosPlots[event.envelopeId] = {}
            ccSlopePlots[event.envelopeId] = {}
        if not (event.patternId in calculatedCCPosMeans[event.envelopeId]):
            calculatedCCPosMeans[event.envelopeId][event.patternId] = sum(ccPosMeans[event.envelopeId][event.patternId])/float(len(ccPosMeans[event.envelopeId][event.patternId]))
            calculatedCCSlopeMeans[event.envelopeId][event.patternId] = sum(ccSlopeMeans[event.envelopeId][event.patternId])/float(len(ccSlopeMeans[event.envelopeId][event.patternId]))
            ccPosPlots[event.envelopeId][event.patternId] = r.TH1D("patPos%i_cc%i"%(event.envelopeId, event.patternId),
                                                                  "patPos%i_cc%i;Position Difference [strips]; Events"%(event.envelopeId, event.patternId),
                                                                  600,-1.,1.)
            ccSlopePlots[event.envelopeId][event.patternId] = r.TH1D("patSlope%i_cc%i"%(event.envelopeId, event.patternId),
                                                                  "patSlope%i_cc%i;Slope; Events"%(event.envelopeId, event.patternId),
                                                                  600,-1.,1.)
           
        if not (event.legacyLctId in calculatedlegacyPosMeans):
            calculatedlegacyPosMeans[event.legacyLctId] = sum(legacyPosMeans[event.legacyLctId])/float(len(legacyPosMeans[event.legacyLctId]))   
            calculatedlegacySlopeMeans[event.legacyLctId] = sum(legacySlopeMeans[event.legacyLctId])/float(len(legacySlopeMeans[event.legacyLctId]))
            legacyPosPlots[event.legacyLctId] = r.TH1D("legacyPos%i"%(event.legacyLctId),"legacyPos%i;Position Difference [strips]; Events"%(event.legacyLctId),600,-1.,1.)
            legacySlopePlots[event.legacyLctId] = r.TH1D("legacySlope%i"%(event.legacyLctId),"legacySlope%i;Slope; Events"%(event.legacyLctId),600,-1.,1.)
          
    
        pattPosPlots[event.envelopeId].Fill(event.segmentX-event.patX - calculatedPattPosMeans[event.envelopeId])
        pattSlopePlots[event.envelopeId].Fill(event.segmentdXdZ - calculatedPattSlopeMeans[event.envelopeId])
        ccPosPlots[event.envelopeId][event.patternId].Fill(event.segmentX-event.patX - calculatedCCPosMeans[event.envelopeId][event.patternId])
        ccSlopePlots[event.envelopeId][event.patternId].Fill(event.segmentdXdZ - calculatedCCSlopeMeans[event.envelopeId][event.patternId])
        legacyPosPlots[event.legacyLctId].Fill(event.segmentX-event.legacyLctX - calculatedlegacyPosMeans[event.legacyLctId])
        legacySlopePlots[event.legacyLctId].Fill(event.segmentdXdZ-calculatedlegacySlopeMeans[event.legacyLctId])
    
    
    
    
    print("Writing Histograms...")
    #center all the data
    outF = r.TFile("%s_resolutionPlots.root"%(chamber[0]),"RECREATE")
    
    cumulativePattPosResolution = r.TH1D("cumPatPosRes", "Cumulative Pattern Position Resolution; Position Difference [strips]; Events", 600,-1., 1.)
    cumulativePattSlopeResolution = r.TH1D("cumPatSlopeRes", "Cumulative Pattern Slope Resolution; Slope; Events", 600,-1., 1.)
    cumulativeCCPosResolution = r.TH1D("cumCCPosRes", "Cumulative CC Position Resolution; Position Difference [strips]; Events", 600,-1.,1.)
    cumulativeCCSlopeResolution = r.TH1D("cumCCSlopeRes", "Cumulative CC Slope Resolution; Slope; Events", 600,-1.,1.)
    cumulativeLegacyPosResolution = r.TH1D("cumLegPosRes","Cumulative Legacy Position Resolution; Position Difference [strips]; Events", 600, -1., 1.)
    cumulativeLegacySlopeResolution = r.TH1D("cumLegSlopeRes","Cumulative Legacy Slope Resolution; Slope; Events", 600, -1., 1.)

    
    for pat in pattPosPlots:
        cumulativePattPosResolution.Add(pattPosPlots[pat])
        cumulativePattSlopeResolution.Add(pattSlopePlots[pat])
        pattPosPlots[pat].Write()
        pattSlopePlots[pat].Write()
        

    ccFrequency = r.THStack("percentageStack","Frequency of CC for %s; Percentage of Pattern Matches; CC Count"%(chamber[0]))

    #HAVENT FINISHED WITH SLOPES UP TO HERE

    colors = [r.kBlue, r.kMagenta+1, r.kRed, r.kOrange+1, r.kBlack]
    ccounter = 0
    envOrdering = [900,800,500,400,100]
    for env in envOrdering:
        
        sortedccPosPlots = sorted( [[ccPosPlots[env][pat],pat] for pat in ccPosPlots[env]], key=lambda x: -1.*x[0].GetEntries())
        
        totalEntries = 0.
        #bad code
        for pat in sortedccPosPlots:
             totalEntries += pat[0].GetEntries()
        #print("For Pattern %i - CC - per - int"%(env))
        ccEnvFrequency  = r.TH1D("percentage_pat%i"%(env), "%s - Pattern %i using %i matches; Percentage; CC Count"%(chamber[0], env, totalEntries), 8,np.logspace(-6,2,9));
        ccEnvFrequency.SetLineColor(colors[ccounter])
        ccEnvFrequency.SetFillColor(colors[ccounter])
        ccounter +=1
        
        mcanvas = r.TCanvas("can","can",1)
        mcanvas.SetLogx()
        mcanvas.SetLogy()
        mcanvas.SetTickx()
        mcanvas.SetTicky()
        int = 0.
        for pat in sortedccPosPlots:
            int +=pat[0].GetEntries()
            #print("%s \t %i \t %0.3f \t %0.3f"%(pat[0].GetName(), pat[1], 100.*pat[0].GetEntries()/totalEntries, 100.*int/totalEntries))
            ccEnvFrequency.Fill(100.*pat[0].GetEntries()/totalEntries)
            #pat[0].Write()
        ccEnvFrequency.Draw()
        mcanvas.SaveAs("../img/patternFreq/%s-p%i.pdf"%(chamber[0], env))
        ccEnvFrequency.Write()
        ccFrequency.Add(ccEnvFrequency)
        
    ccFrequency.Write()
        
    
    for env in ccPosPlots:
        envFullPosRes = r.TH1D("patPos%i_fullCCRes"%(env),
                         "patPos%i_fullCCRes;Position Difference [strips]; Events"%(env),
                         600,-1.,1.)
        envFullSlopeRes = r.TH1D("patSlope%i_fullCCRes"%(env),
                         "patSlope%i_fullCCRes;Slope; Events"%(env),
                         600,-1.,1.)
        for pat in ccPosPlots[env]:
            envFullPosRes.Add(ccPosPlots[env][pat])
            envFullSlopeRes.Add(ccSlopePlots[env][pat])
            #ccPosPlots[env][pat].Write()
        cumulativeCCPosResolution.Add(envFullPosRes)
        cumulativeCCSlopeResolution.Add(envFullSlopeRes)
        envFullPosRes.Write()
        envFullSlopeRes.Write()
            
            
    for id in legacyPosPlots:
        cumulativeLegacyPosResolution.Add(legacyPosPlots[id])
        cumulativeLegacySlopeResolution.Add(legacySlopePlots[id])
        legacyPosPlots[id].Write()    
        legacySlopePlots[id].Write()
        
    cumulativePattPosResolution.Write()
    cumulativePattSlopeResolution.Write()
    cumulativeCCPosResolution.Write()
    cumulativeCCSlopeResolution.Write()
    cumulativeLegacyPosResolution.Write()
    cumulativeLegacySlopeResolution.Write()
    
     
    outF.Close()
    
    
    
#actually run the code, not particularly efficient
chambers = []
#                name, st, ri
#chambers.append(["All-Chambers", 0, 0])
chambers.append(["ME11B", 1,1])
#chambers.append(["ME11A", 1,4])
#chambers.append(["ME12", 1,2])
#chambers.append(["ME13", 1,3])
#chambers.append(["ME21", 2,1])
#chambers.append(["ME22", 2,2])
#chambers.append(["ME31", 3,1])
#chambers.append(["ME32", 3,2])
#chambers.append(["ME41", 4,1])
#chambers.append(["ME42", 4,2])

for chamber in chambers:
    createHists(chamber)
