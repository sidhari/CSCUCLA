#!/usr/bin/env python
import ROOT as r

#open file
#inF = r.TFile("/uscms/home/wnash/nobackup/cscPat/ntuple/plotTree.root")
inF = r.TFile("../data/plotTree.root")
myT = inF.plotTree


#init plots
newResPlots = {}
oldResPlots = {}
#and means
newResMeans = {}
oldResMeans = {}

counter = 0
print("Calculating means...")
for event in myT:
    counter = counter +1
    if((counter % 100000) == 0) : print("Finished %i events"%(counter))
    
    if not (event.envelopeId in newResMeans): 
        newResPlots[event.envelopeId] = {}
        newResMeans[event.envelopeId] = {}
    if not (event.patternId in newResPlots[event.envelopeId]): 
        newResPlots[event.envelopeId][event.patternId] = r.TH1D(
            "newRes%i_%i_h"%(event.envelopeId,event.patternId),
            "newRes%i_%i_h;position difference [strip];Number of Segments"%(event.envelopeId,event.patternId),
            600,-2.0,2.0)
        newResMeans[event.envelopeId][event.patternId] = []
        
    if not (event.legacyLctId in oldResPlots): 
        oldResPlots[event.legacyLctId] = r.TH1D(
        "oldRes%i_h"%(event.legacyLctId),
        "oldRes%i_h;position difference [strip];Number of Segments"%(event.legacyLctId),
        600,-2.0,2.0)
        oldResMeans[event.legacyLctId] = []
    
    newResMeans[event.envelopeId][event.patternId].append(event.segmentX-event.patX)    
    oldResMeans[event.legacyLctId].append(event.segmentX-event.legacyLctX)
    
    
print("Making histograms...")

fullOldResPlot = r.TH1D("fullLegacyResolution", "events;[strip]",600,-2.,2.)
fullEnvelopeResPlot = r.TH1D("fullEnvelopeResolution","events;[strip]",600,-2.,2.)
fullPatternResPlot = r.TH1D("fullPatternResolution", "events;[strip]", 600,-2.,2.)


#loop over tree
counter = 0
calculatedNewPattMeans = {}
calculatedNewEnvMeans = {}
calculatedOldMeans = {}
for event in myT:
    if not (event.envelopeId in calculatedNewPattMeans):
        calculatedNewPattMeans[event.envelopeId] = {}
    if not (event.patternId in calculatedNewPattMeans[event.envelopeId]):
        calculatedNewPattMeans[event.envelopeId][event.patternId] = sum(newResMeans[event.envelopeId][event.patternId])/float(len(newResMeans[event.envelopeId][event.patternId]))
 
       
    if not (event.legacyLctId in calculatedOldMeans):
        calculatedOldMeans[event.legacyLctId] = sum(oldResMeans[event.legacyLctId])/float(len(oldResMeans[event.legacyLctId]))   
      
    counter = counter +1
    if((counter % 100000) == 0) : print("Finished %i events"%(counter))
    newResPlots[event.envelopeId][event.patternId].Fill(event.segmentX-event.patX - calculatedNewPattMeans[event.envelopeId][event.patternId])
    oldResPlots[event.legacyLctId].Fill(event.segmentX-event.legacyLctX- calculatedOldMeans[event.legacyLctId])
    fullOldResPlot.Fill(event.segmentX-event.legacyLctX- calculatedOldMeans[event.legacyLctId])
    #fullEnvelopeResPlot.Fill()
    fullPatternResPlot.Fill(event.segmentX-event.patX - calculatedNewPattMeans[event.envelopeId][event.patternId])
    pass


print("Writing Histograms...")
#center all the data
outF = r.TFile("rezPlots.root","RECREATE")
for env in newResPlots:
    for pat in newResPlots[env]:
        newResPlots[env][pat].Write()

        pass
    pass


for id in oldResPlots:
    oldResPlots[id].Write()
    
fullOldResPlot.Write()
fullPatternResPlot.Write()
 
#print("Processing data with Mean")   
#for event in myT:
#    #print("Finished %f of pre-processing"%)
#    if not (event.envelopeId in newResPlots): newResPlots[event.envelopeId] = {}
#    if not (event.patternId in newResPlots[event.envelopeId]): newResPlots[event.envelopeId][event.patternId] = r.TH1D(
#            "newRes%i_%i_h"%(event.envelopeId,event.patternId),
#            "newRes%i_%i_h;position difference [strip];Number of Segments"%(event.envelopeId,event.patternId),
#            600,-3.0,3.0)
#    if not (event.legacyLctId in oldResPlots): oldResPlots[event.legacyLctId] = r.TH1D(
#        "oldRes%i_h"%(event.legacyLctId),
#        "oldRes%i_h;position difference [strip];Number of Segments"%(event.legacyLctId),
#        600,-3.0,3.0)
#        
#        
#    newResPlots[event.envelopeId][event.patternId].Fill(event.segmentX-event.patX)
#    oldResPlots[event.legacyLctId].Fill(event.segmentX-event.legacyLctX)
#    pass    


outF.Close()



#plots the patternId resolution for all the patterns which are a subset of envelopeId, as well as the envelopeId resolution
#def showPlot(envelopeId, patternId) :
#    for event in myT:
        #only care about one envelope here
#        if not(event.envelopeId == envelopeId) : 
#            continue
        
#        if(event.patternId == patternId) :
            
            
        
    
