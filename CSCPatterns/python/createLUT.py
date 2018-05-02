#!/usr/bin/env python
import ROOT as r

from array import array
from matplotlib.mlab import slopes

#
#
#

countCutoff = 100 #minimum amount of segments found to use its mean in place of linear fit
useCompHits = 1 # 0 means use recHits
folder = ""
if(useCompHits):
    folder = "compHits"
else:
    folder= "recHits"

def printProgress(counter, entries):
    if((counter % 1000000) == 0) : print("Finished %0.2f%% of events"%(100.*counter/entries))

#Here we can select out if we want to look at only ME11, etc
def validEvent(event,st,ri):
    if((event.ST == st or st == 0) and (event.RI == ri or ri == 0)) : #0 means be indiscriminate
        return True
    return False

def createLUT(filepath):
    
    linefitOffset = {} #will hold an entry for every possible pat / cc combination
    linefitSlope = {}
    linefitChi2 = {}
    linefitNDF = {}
    patList = [100,400,500,800,900]
    
    for pat in patList:
        linefitOffset[pat] = {}
        linefitSlope[pat] = {}
        linefitChi2[pat] = {}
        linefitNDF[pat] ={}
        
    
    
    with open(filepath, "r") as f:
        for l,line in enumerate(f):
            if (l == 0): continue #skip first line
            
            #should be pat, cc, offset, slope, chi2, ndf
            elements = line.strip('\n').split('\t')
            #print elements
            #print("pat: %i, cc: %i, off: %f, slope: %f"%(int(elements[0]), int(elements[1]), float(elements[2]), float(elements[3])))
            
            #
            # Some funky sign issues, slope is opposite the expected sign,
            # and offset is off by 0.5 strips, and need to convert to strips
            #
            
            linefitOffset[int(elements[0])][int(elements[1])] = 0.5*float(elements[2])
            linefitSlope[int(elements[0])][int(elements[1])] = -0.5*float(elements[3])
            linefitChi2[int(elements[0])][int(elements[1])] = float(elements[4])
            linefitNDF[int(elements[0])][int(elements[1])] = float(elements[5])
    return  linefitOffset, linefitSlope, linefitChi2, linefitNDF
    
    
def runTest(chamber):
    print("=== Running over Chamber %s ==="%(chamber[0]))
   
    #open file
    inF = r.TFile("../data/%s/processedMatches_FULLSET.root"%folder)
    myT = inF.plotTree
    
     #output file
    outF = r.TFile("../data/%s/%s_LUT.root"%(folder,chamber[0]),"RECREATE")
    
    # dictionaries that calculate the total offset / slope for a given
    # pattern / cc combination
    offsetSums = {}
    slopeSums = {}
    
    #
    # Read in linear fit data
    #
    
    linefitOffset, linefitSlope, linefitChi2, linefitNDF = createLUT("../data/linearFits.txt")

    #
    # Make some plots
    #

    h_linearOffsetDiff = r.TH1D("h_linOffsetDiff", "h_linOffsetDiff; Fit - Segment [strips]; Segments", 500, -2, 2)
    h_linearSlopeDiff = r.TH1D("h_linSlopeDiff", "h_linSlopeDiff; Fit - Segment [strips/layer]; Segments", 500, -2, 2)
    
    #
    # Iterate through the tree
    #
    
    counter = 0
    entries = myT.GetEntries()
    for event in myT:
        printProgress(counter,entries)
        counter +=1
        if(event.patternId == -1): continue #ignore unmatched cc's
           
        #check if we should look at this event    
        if not validEvent(event, chamber[1], chamber[2]): continue
        
        #######  Now look at the event itself
        if not (event.envelopeId in offsetSums):
            offsetSums[event.envelopeId] = {}
            slopeSums[event.envelopeId] = {}
            
        if not(event.patternId in offsetSums[event.envelopeId]):
            offsetSums[event.envelopeId][event.patternId] = []
            slopeSums[event.envelopeId][event.patternId] = []
            
        offsetSums[event.envelopeId][event.patternId].append(event.segmentX-event.patX)
        slopeSums[event.envelopeId][event.patternId].append(event.segmentdXdZ)
        #print("lOff %f\t sOff %f"%(linefitOffset[event.envelopeId][event.patternId] + 0.5*event.patX,0.5*event.segmentX))
        
        h_linearOffsetDiff.Fill(linefitOffset[event.envelopeId][event.patternId] + event.patX- event.segmentX)
        h_linearSlopeDiff.Fill(linefitSlope[event.envelopeId][event.patternId] - event.segmentdXdZ)


    #
    # Now redo resolution plots
    #
    
    avgOffsetCutoffPlots = []
    avgSlopeCutoffPlots = []
    linOffsetCutoffPlots = []
    linSlopeCutoffPlots = []
    
    
    #minimum amount of segments found to use its mean in place of linear fit
    cutoffs = [0,1,2,3,4,5,10, 25, 50, 100, 500, 1000, 2000, 200000000]
    for cutoff in cutoffs:
        avgOffsetCutoffPlots.append(r.TH1D("h_avgOffsetCutoff%i"%cutoff, "h_avgOffsetCutoff%i; Fit - Segment[strips]; Segments"%cutoff, 200, -2, 2))
        avgSlopeCutoffPlots.append(r.TH1D("h_avgSlopeCutoff%i"%cutoff, "h_avgSlopeCutoff%i; Fit - Segment[strips/layer]; Segments"%cutoff, 200, -2,2))
        linOffsetCutoffPlots.append(r.TH1D("h_linOffsetCutoff%i"%cutoff, "h_linOffsetCutoff%i; Fit - Segment[strips]; Segments"%cutoff, 200, -2, 2))
        linSlopeCutoffPlots.append(r.TH1D("h_linSlopeCutoff%i"%cutoff, "h_linSlopeCutoff%i; Fit - Segment[strips/layer]; Segments"%cutoff, 200, -2,2))
    
 
    #
    # Iterate through the tree again, with the averages...
    #
    
    print("Now redoing resolution plots...")
    counter = 0
    for event in myT:
        printProgress(counter,entries)
        counter += 1
        if(event.patternId == -1): continue #ignore unmatched cc's
        
        #check if we should look at this event    
        if not validEvent(event, chamber[1], chamber[2]): continue
        
        
        patt = event.envelopeId
        cc = event.patternId
        
        if (patt != 100): continue
        
        offset = 1.*sum(offsetSums[patt][cc])/float(len(offsetSums[patt][cc]))
        slope = 1.*sum(slopeSums[patt][cc])/float(len(slopeSums[patt][cc]))
        
        for i,cutoff in enumerate(cutoffs):
            
            #check how many events we have in distribution
            if(len(offsetSums[patt][cc]) == cutoff):
                avgOffsetCutoffPlots[i].Fill(offset + event.patX - event.segmentX)
                avgSlopeCutoffPlots[i].Fill(slope - event.segmentdXdZ)
                offset = linefitOffset[patt][cc] + 0.25
                slope = linefitSlope[patt][cc]
                linOffsetCutoffPlots[i].Fill(offset + event.patX - event.segmentX)
                linSlopeCutoffPlots[i].Fill(slope - event.segmentdXdZ)
        
    #c = r.TCanvas("c_pos", "c_pos", 1200, 800)
    #c.cd()    

    for i in range(len(cutoffs)):
       #avgOffsetCutoffPlots[i].SetLineColor(i+20)
       #(avgOffsetCutoffPlots[i].Draw() if i == 0 else avgOffsetCutoffPlots[i].Draw("same"))    
       avgOffsetCutoffPlots[i].Write()
       linOffsetCutoffPlots[i].Write()
         
       
    h_linearOffsetDiff.Write()
    h_linearSlopeDiff.Write()
    
    #c.BuildLegend()
    #c.Write()
    
    #c2 = r.TCanvas("c_slope", "c_slope", 1200, 800)
    #c2.cd()   
    
    for i in range(len(cutoffs)):
        #avgSlopeCutoffPlots[i].SetLineColor(i+20)
        #(avgSlopeCutoffPlots[i].Draw() if i == 0 else avgSlopeCutoffPlots[i].Draw("same"))
        avgSlopeCutoffPlots[i].Write()
        linSlopeCutoffPlots[i].Write()
    
    #c2.BuildLegend()
    #c2.Write()
    
    outF.Close()
    

#divide everything into chambers and run that way
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
    runTest(chamber)