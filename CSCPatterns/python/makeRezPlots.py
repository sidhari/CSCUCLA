#!/usr/bin/env python
import ROOT as r


#def encodeChamberCode

def decodeChamberCode(chamberCode):
    return 0

#Here we can select out if we want to look at only ME11, etc
def validEvent(event):
    if(event.ST == 1 and event.RI == 1) : #me11b
        return True
    return False


def createHists(chamberCode):
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
        if((counter % 100000) == 0) : print("Finished %0.2f percent of events"%(100.*counter/entries))
            
        #check if we shuold look at this event    
        if not validEvent(event): continue
        
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
        if((counter % 100000) == 0) : print("Finished %0.2f percent of events"%(100.*counter/entries))
        
        #check if we shuold look at this event    
        if not validEvent(event): continue
        
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
    outF = r.TFile("resolutionPlots.root","RECREATE")
    
    cumulativeEnvResolution = r.TH1D("cumPatRes", "Cumulative Pattern Resolution; Position Difference [strips]; Events", 600,-1., 1.)
    cumulativePattResolution = r.TH1D("cumCCRes", "Cumulative CC Resolution; Position Difference [strips]; Events", 600,-1.,1.)
    cumulativeLegacyResolution = r.TH1D("cumLegRes","Cumulative Legacy Resolution; Position Difference [strips]; Events", 600, -1., 1.)
    
    for env in envPlots:
        cumulativeEnvResolution.Add(envPlots[env])
        envPlots[env].Write()
        
        
        ##TO VERIFY
    for env in pattPlots:
        sortedPattPlots = sorted( [pattPlots[env][pat] for pat in pattPlots[env]], key=lambda x: -1.*x.GetEntries() )
        
        totalEntries = 0.
        #bad code
        for pat in sortedPattPlots:
             totalEntries += pat.GetEntries()
        print("For Envelope %i - per - int"%(env))
        int = 0.
        for pat in sortedPattPlots:
            int +=pat.GetEntries()
            print("%s \t %0.3f \t %0.3f"%(pat.GetName(), 100.*pat.GetEntries()/totalEntries, 100.*int/totalEntries))
            pat.Write()
        
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
    
    
    
#actually run the code, TODO for-loop for all possible chamber code to create everything...
createHists(0)
