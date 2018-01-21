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

print("Calculating means...")
for event in myT:
    if not (event.envelopeId in newResMeans): 
        newResPlots[event.envelopeId] = {}
        newResMeans[event.envelopeId] = {}
    if not (event.patternId in newResPlots[event.envelopeId]): 
        newResPlots[event.envelopeId][event.patternId] = r.TH1D(
            "newRes%i_%i_h"%(event.envelopeId,event.patternId),
            "newRes%i_%i_h;position difference [strip];Number of Segments"%(event.envelopeId,event.patternId),
            600,-3.0,3.0)
        newResMeans[event.envelopeId][event.patternId] = []
        
    if not (event.legacyLctId in oldResPlots): 
        oldResPlots[event.legacyLctId] = r.TH1D(
        "oldRes%i_h"%(event.legacyLctId),
        "oldRes%i_h;position difference [strip];Number of Segments"%(event.legacyLctId),
        600,-3.0,3.0)
        oldResMeans[event.legacyLctId] = []
    
    newResMeans[event.envelopeId][event.patternId].append(event.segmentX-event.patX)    
    oldResMeans[event.legacyLctId].append(event.segmentX-event.legacyLctX)
    
    
            
    

print("Making histograms...")

fullOldResPlot = r.TH1D("fullLegacyResolution", "events;[strip]",600,-3.,3.)
fullEnvelopeResPlot = r.TH1D("fullEnvelopeResolution","event;[strip]",600,-3.,3.)
fullPatternResPlot = r.TH1D("fullPatternResolution", "event;[strip]", 600,-3.,3.)


#loop over tree
counter = 0
calculatedNewMeans = {}
calculatedOldMeans = {}
for event in myT:
    if not (event.envelopeId in calculatedNewMeans):
        calculatedNewMeans[event.envelopeId] = {}
    if not (event.patternId in calculatedNewMeans[event.envelopeId]):
        calculatedNewMeans[event.envelopeId][event.patternId] = sum(newResMeans[event.envelopeId][event.patternId])/float(len(newResMeans[event.envelopeId][event.patternId]))
       
    if not (event.legacyLctId in calculatedOldMeans):
        calculatedOldMeans[event.legacyLctId] =   sum(oldResMeans[event.legacyLctId])/float(len(oldResMeans[event.legacyLctId]))   
    #newMean = 0
    #if(len(newResMeans[event.envelopeId][event.patternId])) :
    #    newMean = sum(newResMeans[event.envelopeId][event.patternId])/float(len(newResMeans[event.envelopeId][event.patternId]))
        
    #oldMean = 0
    #if(len(oldResMeans[event.legacyLctId])) :
    #    oldMean = sum(oldResMeans[event.legacyLctId])/float(len(oldResMeans[event.legacyLctId]))
            
    counter = counter +1
    if((counter % 100000) == 0) : print("Finished %i events"%(counter))
    newResPlots[event.envelopeId][event.patternId].Fill(event.segmentX-event.patX - calculatedNewMeans[event.envelopeId][event.patternId])
    oldResPlots[event.legacyLctId].Fill(event.segmentX-event.legacyLctX- calculatedOldMeans[event.legacyLctId])
    fullOldResPlot.Fill(event.segmentX-event.legacyLctX- calculatedOldMeans[event.legacyLctId])
    #fullEnvelopeResPlot.Fill()
    fullPatternResPlot.Fill(event.segmentX-event.patX - calculatedNewMeans[event.envelopeId][event.patternId])
    pass


print("Writing Histograms...")
#center all the data
#newResMeans = {}
outF = r.TFile("rezPlots.root","RECREATE")
for env in newResPlots:
    #newResMeans[env] = {}
    for pat in newResPlots[env]:
        newResPlots[env][pat].Write()
        #rezMeans[env][pat] = rezPlots[env][pat].GetMean()
        pass
    pass

#oldResMeans = {}
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
            
            
        
    
