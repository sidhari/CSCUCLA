#!/usr/bin/env python
import ROOT as r
import numpy as np
import math

from array import array

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

def getStats(nums, patt, cc):
    mean = 1.*sum(nums[patt][cc])/float(len(nums[patt][cc]))
    rms = math.sqrt(1.*sum(1.*i*i for i in nums[patt][cc])/float(len(nums[patt][cc])) - mean**2)
    
    return mean, rms

def createLUT(filepath):
    
    linefitOffset = {} #will hold an entry for every possible pat / cc combination
    linefitSlope = {}
    linefitChi2 = {}
    linefitNDF = {}
    linefitOffsetErr = {}
    linefitSlopeErr = {}
    patList = [100,400,500,800,900]
    
    for pat in patList:
        linefitOffset[pat] = {}
        linefitSlope[pat] = {}
        linefitChi2[pat] = {}
        linefitNDF[pat] ={}
        linefitOffsetErr[pat] = {}
        linefitSlopeErr[pat] = {}
    
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
            
            linefitOffset[int(elements[0])][int(elements[1])] = 0.5*float(elements[2])  + 0.25 #offset between the two
            linefitSlope[int(elements[0])][int(elements[1])] = -0.5*float(elements[3])
            linefitChi2[int(elements[0])][int(elements[1])] = float(elements[4])
            linefitNDF[int(elements[0])][int(elements[1])] = float(elements[5])
            linefitOffsetErr[int(elements[0])][int(elements[1])] = 0.5*float(elements[6])
            linefitSlopeErr[int(elements[0])][int(elements[1])] = 0.5*float(elements[7])
            
    return  linefitOffset, linefitSlope, linefitChi2, linefitNDF, linefitOffsetErr, linefitSlopeErr
    
    
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
    offsetMeans = {}
    slopeMeans = {}
    offsetRMS = {}
    slopeRMS = {}
    
    #
    # Read in linear fit data
    #
    
    linefitOffset, linefitSlope, linefitChi2, linefitNDF, linefitOffsetErr, linefitSlopeErr = createLUT("../data/linearFits.txt")

    #
    # Make some plots
    #

    h_linearOffsetDiff = r.TH1D("h_linOffsetDiff", "h_linOffsetDiff; Fit - Avg [strips]; Pattern/CCs", 500, -1, 1)
    h_linearOffsetDiffOverRMS = r.TH1D("h_linOffsetDiffOverRMS", "h_linOffsetDiffOverRMS; Fit - Avg ; Pattern/CCs", 500, -5, 5)
    h_linearSlopeDiff = r.TH1D("h_linSlopeDiff", "h_linSlopeDiff; Fit - Avg [strips/layer]; Pattern/CCs", 500, -1, 1)
    h_linearSlopeDiffOverRMS = r.TH1D("h_linSlopeDiffOverRMS", "h_linSlopeDiffOverRMS; Fit - Avg; Pattern/CCs", 500, -5, 5) 
    h_N = r.TH1D("h_N", "h_N; N; Pattern-CCs", 100,0,100)
    h_N_3lay = r.TH1D("h_N_3lay", "h_N_3lay; N; Pattern-CCs", 100,0,100)
    h_N_4lay = r.TH1D("h_N_4lay", "h_N_4lay; N; Pattern-CCs", 100,0,100)
    h_N_5lay = r.TH1D("h_N_5lay", "h_N_5lay; N; Pattern-CCs", 100,0,100)
    h_N_6lay = r.TH1D("h_N_6lay", "h_N_6lay; N; Pattern-CCs", 100,0,100)
    
    logBins = 5
    h_UncertaintyVsN = [r.TH2D("h_offsetUncertaintyVsN","h_offsetUncertaintyVsN; N; Offset Uncertainty [strips]", logBins,np.logspace(0,0+logBins,logBins+1),30, 0,0.3), \
                        r.TH2D("h_slopeUncertaintyVsN","h_slopeUncertaintyVsN; N; Slope Uncertainty [strips/layer]", logBins,np.logspace(0,0+logBins,logBins+1),30, 0,0.3)]
    
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
            offsetMeans[event.envelopeId] = {}
            slopeMeans[event.envelopeId] = {}
            offsetRMS[event.envelopeId] = {}
            slopeRMS[event.envelopeId] = {}
            
        if not(event.patternId in offsetSums[event.envelopeId]):
            offsetSums[event.envelopeId][event.patternId] = []
            slopeSums[event.envelopeId][event.patternId] = []

            
        offsetSums[event.envelopeId][event.patternId].append(event.segmentX-event.patX)
        slopeSums[event.envelopeId][event.patternId].append(event.segmentdXdZ)
        #print("lOff %f\t sOff %f"%(linefitOffset[event.envelopeId][event.patternId] + 0.5*event.patX,0.5*event.segmentX))


    #
    # Calculate the means, rms, etc, also look at distribution compare to averaging method
    #
    
    Nvec = array('d')
    #pos, slope
    avgRMS_vec = [array('d'), array('d')]
    linRMS_vec = [array('d'), array('d')]

    for patt in offsetSums.keys(): 
        for cc in offsetSums[patt].keys():
            
            N = len(offsetSums[patt][cc])
            offsetMeans[patt][cc], offsetRMS[patt][cc] = getStats(offsetSums, patt, cc)
            slopeMeans[patt][cc], slopeRMS[patt][cc] = getStats(slopeSums, patt, cc)
            
            if N > 1: #RMS is 0 for combinations with just one segment
                Nvec.append(N)
                avgOffsetErr = offsetRMS[patt][cc]/math.sqrt(N)
                avgRMS_vec[0].append(avgOffsetErr)
                linRMS_vec[0].append(linefitOffsetErr[patt][cc])
                h_UncertaintyVsN[0].Fill(N,avgOffsetErr)
                
                avgSlopeErr = slopeRMS[patt][cc]/math.sqrt(N)
                avgRMS_vec[1].append(avgSlopeErr)
                linRMS_vec[1].append(linefitSlopeErr[patt][cc])            
                h_UncertaintyVsN[1].Fill(N,avgSlopeErr)
                
                
                #solving the equation sigma /sqrt(N) = deltaB -> N = sigma^2/ deltaB^2
                
                approxN = offsetRMS[patt][cc]**2 / linefitOffsetErr[patt][cc]**2
                h_N.Fill(approxN)
                
                if(approxN > 100):
                    print("sigma = %3.4f, db = %3.4f, actualN = %i, patt = %i, cc = %i"%(offsetRMS[patt][cc],linefitOffsetErr[patt][cc],N, patt, cc))
                
                layers =linefitNDF[patt][cc] +2 
                if(layers == 3): h_N_3lay.Fill(approxN)
                if(layers == 4): h_N_4lay.Fill(approxN)
                if(layers == 5): h_N_5lay.Fill(approxN)
                if(layers == 6): h_N_6lay.Fill(approxN)
                
            
            if N > 1 and N < 10 :
                h_linearOffsetDiff.Fill(linefitOffset[patt][cc] - offsetMeans[patt][cc])
                if (offsetRMS[patt][cc] != 0): h_linearOffsetDiffOverRMS.Fill((linefitOffset[patt][cc] - offsetMeans[patt][cc])/offsetRMS[patt][cc])
                h_linearSlopeDiff.Fill(linefitSlope[patt][cc] - slopeMeans[patt][cc])  
                if(slopeRMS[patt][cc] != 0): h_linearSlopeDiffOverRMS.Fill((linefitSlope[patt][cc] - slopeMeans[patt][cc])/slopeRMS[patt][cc]) 
                #if(linefitOffset[patt][cc] - offsetMeans[patt][cc] > 0.1): print("line = %f, avg = %f, diff = %f, patt = %i, cc = %i, count = %i"%(linefitOffset[patt][cc],offsetMeans[patt][cc],linefitOffset[patt][cc] - offsetMeans[patt][cc],patt, cc,len(offsetSums[patt][cc])))
                
                avgErr = offsetRMS[patt][cc]/math.sqrt(N)
                #print("POS: avgRMS = %3.4f, linRMS = %3.4f, count = %i"%(avgErr,linefitOffsetErr[patt][cc], N))
                #print("POS: avgRMS = %3.4f, linRMS = %3.4f -- SLOPE: avgRMS = %3.4f, linRMS = %3.4f, count = %i"%(offsetRMS[patt][cc], linefitOffsetErr[patt][cc], slopeRMS[patt][cc],linefitSlopeErr[patt][cc],len(offsetSums[patt][cc])))
    
    
    #
    # Make plots looping over pos, slope
    #
    
    for i in range(len(avgRMS_vec)):
        avgG = r.TGraph(len(Nvec), Nvec , avgRMS_vec[i])
        
        avgG.SetTitle("Average RMS vs Match Counts")
        avgG.GetXaxis().SetTitle("Match Counts")
        avgG.SetMarkerStyle(20)
        avgG.SetMarkerSize(1)
        if (i == 0) : avgG.GetYaxis().SetTitle("RMS [strips]")
        else : avgG.GetYaxis().SetTitle("RMS [strips/layer]")
        
        
        linG = r.TGraph(len(Nvec), Nvec, linRMS_vec[i])
        linG.SetTitle("Line Fit RMS vs Match Counts")
        linG.GetXaxis().SetTitle("Match Counts")
        linG.SetMarkerStyle(20)
        linG.SetMarkerColor(9)
        linG.SetMarkerSize(1)
        if(i == 0) : linG.GetYaxis().SetTitle("RMS [strips]")
        else : linG.GetYaxis().SetTitle("RMS [strips/layer]")
        
        c = r.TCanvas()
        c.cd()
        c.SetLogx()
        # fit to sigma / sqrt(N), uncertainty on mean of a Gaussian
        avgFit = r.TF1("avgFit", "[0]/sqrt(x)", 2, 10000)
        avgFit.SetLineWidth(5)
        #fit the RMS of the line fits vs how many counts show up for that combination, assume a straight line
        #lineFit = r.TF1("lineFit", "[0] + [1]*x",2, 10000)
        lineFit = r.TF1("lineFit", "[0]",2, 10000)
        lineFit.SetLineColor(r.kBlack)
        lineFit.SetLineWidth(5)
        
        avgG.Fit("avgFit")
        linG.Fit("lineFit")
        h_UncertaintyVsN[i].Draw("colz")
        avgFit.Draw("same")
        lineFit.Draw("same")
        c.Update()
        c.Write()
    
    

    #
    # Now redo resolution plots
    #
    
    avgOffsetCutoffPlots = []
    avgSlopeCutoffPlots = []
    linOffsetCutoffPlots = []
    linSlopeCutoffPlots = []
    
    
    #minimum amount of segments found to use its mean in place of linear fit
    #cutoffs = [0,1,2,3,4,5,10, 25, 50, 100, 500, 1000, 2000, 200000000]
    cutoffs = [0,1,2,3,4,5,10, 25, 50, 100]
    for cutoff in cutoffs:
        avgOffsetCutoffPlots.append(r.TH1D("h_avgOffsetCutoff%i"%cutoff, "h_avgOffsetCutoff%i; Fit - Segment[strips]; Segments"%cutoff, 200, -2, 2))
        avgSlopeCutoffPlots.append(r.TH1D("h_avgSlopeCutoff%i"%cutoff, "h_avgSlopeCutoff%i; Fit - Segment[strips/layer]; Segments"%cutoff, 200, -2,2))
        linOffsetCutoffPlots.append(r.TH1D("h_linOffsetCutoff%i"%cutoff, "h_linOffsetCutoff%i; Fit - Segment[strips]; Segments"%cutoff, 200, -2, 2))
        linSlopeCutoffPlots.append(r.TH1D("h_linSlopeCutoff%i"%cutoff, "h_linSlopeCutoff%i; Fit - Segment[strips/layer]; Segments"%cutoff, 200, -2,2))
    
 
  
         
       
    h_linearOffsetDiff.Write()
    h_linearOffsetDiffOverRMS.Write()
    h_linearSlopeDiff.Write()
    h_linearSlopeDiffOverRMS.Write()
    h_N.Write()
    h_N_3lay.Write()
    h_N_4lay.Write()
    h_N_5lay.Write()
    h_N_6lay.Write()
    h_UncertaintyVsN[0].Write()
    h_UncertaintyVsN[1].Write()
    

    
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