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
    if(event.ST == st and event.RI == ri) : #me11b
        return True
    return False


def createHists(chamber):
    print("=== Running over Chamber %s ==="%(chamber[0]))
    
    #open file
    inF = r.TFile("../data/plotTree.root")
    myT = inF.plotTree
    
    
    #init plots
    patternMeans = {}
    envelopeMeans = {}
    legacyMeans = {}
    
    print("Calculating Means...")
    counter = 0
    entries = myT.GetEntries()
    for event in myT:
        counter +=1
        printProgress(counter,entries)
            
        #check if we shuold look at this event    
        if not validEvent(event, chamber[1], chamber[2]): continue
        
        #fill stuff we dont have yet
        if not (event.envelopeId in envelopeMeans):
            envelopeMeans[event.envelopeId] = []
            patternMeans[event.envelopeId] = {}
            
        if not (event.patternId in patternMeans[event.envelopeId]):
            patternMeans[event.envelopeId][event.patternId] = []
            
        if not (event.legacyLctId in legacyMeans) :
            legacyMeans[event.legacyLctId] = []
            
        envelopeMeans[event.envelopeId].append(event.segmentX-event.patX)
        patternMeans[event.envelopeId][event.patternId].append(event.segmentX-event.patX)
        legacyMeans[event.legacyLctId].append(event.segmentX-event.legacyLctX)
        
        
    
    print("Making Histograms...")
    
    envPlots = {}
    pattPlots = {}
    legacyPlots = {}
    
    
    #loop over tree, again
    counter = 0
    #calculate these once, so it runs faster
    calculatedPattMeans = {}
    calculatedEnvMeans = {}
    calculatedLegacyMeans = {}
    for event in myT:
        counter += 1
        printProgress(counter,entries)
        
        #check if we shuold look at this event    
        if not validEvent(event, chamber[1], chamber[2]): continue
        
        if not (event.envelopeId in calculatedEnvMeans):
            calculatedEnvMeans[event.envelopeId] = sum(envelopeMeans[event.envelopeId])/float(len(envelopeMeans[event.envelopeId]))
            envPlots[event.envelopeId]= r.TH1D("pat%i"%(event.envelopeId),
                                               "pat%i;Position Difference [strips]; Events"%(event.envelopeId), 
                                               600,-1.,1.)
            calculatedPattMeans[event.envelopeId] = {}
            pattPlots[event.envelopeId] = {}
        if not (event.patternId in calculatedPattMeans[event.envelopeId]):
            calculatedPattMeans[event.envelopeId][event.patternId] = sum(patternMeans[event.envelopeId][event.patternId])/float(len(patternMeans[event.envelopeId][event.patternId]))
            pattPlots[event.envelopeId][event.patternId] = r.TH1D("pat%i_cc%i"%(event.envelopeId, event.patternId),
                                                                  "pat%i_cc%i;Position Difference [strips]; Events"%(event.envelopeId, event.patternId),
                                                                  600,-1.,1.)
           
        if not (event.legacyLctId in calculatedLegacyMeans):
            calculatedLegacyMeans[event.legacyLctId] = sum(legacyMeans[event.legacyLctId])/float(len(legacyMeans[event.legacyLctId]))   
            legacyPlots[event.legacyLctId] = r.TH1D("legacy%i"%(event.legacyLctId),"legacy%i;Position Difference [strips]; Events"%(event.legacyLctId),600,-1.,1.)
          
    
        envPlots[event.envelopeId].Fill(event.segmentX-event.patX - calculatedEnvMeans[event.envelopeId])
        pattPlots[event.envelopeId][event.patternId].Fill(event.segmentX-event.patX - calculatedPattMeans[event.envelopeId][event.patternId])
        legacyPlots[event.legacyLctId].Fill(event.segmentX-event.legacyLctX - calculatedLegacyMeans[event.legacyLctId])
    
    
    
    
    print("Writing Histograms...")
    #center all the data
    outF = r.TFile("%s_resolutionPlots.root"%(chamber[0]),"RECREATE")
    
    cumulativeEnvResolution = r.TH1D("cumPatRes", "Cumulative Pattern Resolution; Position Difference [strips]; Events", 600,-1., 1.)
    cumulativePattResolution = r.TH1D("cumCCRes", "Cumulative CC Resolution; Position Difference [strips]; Events", 600,-1.,1.)
    cumulativeLegacyResolution = r.TH1D("cumLegRes","Cumulative Legacy Resolution; Position Difference [strips]; Events", 600, -1., 1.)
    
    for env in envPlots:
        cumulativeEnvResolution.Add(envPlots[env])
        envPlots[env].Write()
        

    ccFrequency = r.THStack("percentageStack","Frequency of CC for %s; Percentage of Pattern Matches; CC Count"%(chamber[0]))


    colors = [r.kBlue, r.kMagenta+1, r.kRed, r.kOrange+1, r.kBlack]
    ccounter = 0
    envOrdering = [900,800,500,400,100]
    for env in envOrdering:
        
        sortedPattPlots = sorted( [[pattPlots[env][pat],pat] for pat in pattPlots[env]], key=lambda x: -1.*x[0].GetEntries())
        
        totalEntries = 0.
        #bad code
        for pat in sortedPattPlots:
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
        for pat in sortedPattPlots:
            int +=pat[0].GetEntries()
            #print("%s \t %i \t %0.3f \t %0.3f"%(pat[0].GetName(), pat[1], 100.*pat[0].GetEntries()/totalEntries, 100.*int/totalEntries))
            ccEnvFrequency.Fill(100.*pat[0].GetEntries()/totalEntries)
            #pat[0].Write()
        ccEnvFrequency.Draw()
        mcanvas.SaveAs("../img/patternFreq/%s-p%i.pdf"%(chamber[0], env))
        ccEnvFrequency.Write()
        ccFrequency.Add(ccEnvFrequency)
        
    ccFrequency.Write()
        
    
    for env in pattPlots:
        envFullRes = r.TH1D("pat%i_fullCCRes"%(env),
                         "pat%i_fullCCRes;Position Difference [strips]; Events"%(env),
                         600,-1.,1.)
        for pat in pattPlots[env]:
            envFullRes.Add(pattPlots[env][pat])
            #pattPlots[env][pat].Write()
        cumulativePattResolution.Add(envFullRes)
        envFullRes.Write()
            
            
    for id in legacyPlots:
        cumulativeLegacyResolution.Add(legacyPlots[id])
        legacyPlots[id].Write()    
        
    cumulativeEnvResolution.Write()
    cumulativePattResolution.Write()
    cumulativeLegacyResolution.Write()
    
     
    outF.Close()
    
    
    
#actually run the code, not particularly efficient
chambers = []
#                name, st, ri
chambers.append(["ME11B", 1,1])
chambers.append(["ME11A", 1,4])
chambers.append(["ME12", 1,2])
chambers.append(["ME13", 1,3])
chambers.append(["ME21", 2,1])
chambers.append(["ME22", 2,2])
chambers.append(["ME31", 3,1])
chambers.append(["ME32", 3,2])
chambers.append(["ME41", 4,1])
chambers.append(["ME42", 4,2])

for chamber in chambers:
    createHists(chamber)
