#!/usr/bin/env python
import ROOT as r
import numpy as np
import math
import os

from array import array

#
#
#


USECOMPHITS = 1 # 0 means use recHits
HIT_FOLDER = "compHits" if USECOMPHITS else "recHits"
    
    #file used to train the data LUT
TRAININGFILE = "../data/%s/processedMatches_2016F.root"%HIT_FOLDER

    #file used to test the data LUT
TESTFILE     = "../data/%s/processedMatches_2017D-1.root"%HIT_FOLDER    

def printProgress(counter, entries):
    if((counter % 1000000) == 0) : print("Finished %0.2f%% of events"%(100.*counter/entries))

#Here we can select out if we want to look at only ME11, etc
def validEvent(event,st,ri):
    if((event.ST == st or st == 0) and (event.RI == ri or ri == 0)) : #0 means be indiscriminate
        return True
    return False

def getStats(nums, patt, cc):
    N = len(nums[patt][cc])
    mean = 1.*sum(nums[patt][cc])/float(N)
    rms = math.sqrt(1.*sum(1.*i*i for i in nums[patt][cc])/float(N) - mean**2)
    
    return mean, rms, N

def createLineFitLUT(filepath):
    print("\033[94m=== Reading Line Fit LUT ===\033[0m")
    
    linefitOffset = {} #will hold an entry for every possible pat / cc combination
    linefitSlope = {}
    linefitChi2 = {}
    linefitNDF = {}
    linefitOffsetErr = {}
    linefitSlopeErr = {}
    patList = [100,90,80,70,60]
    
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
            
    return linefitOffset, linefitSlope, linefitChi2, linefitNDF, linefitOffsetErr, linefitSlopeErr
       
def createDataLUT(chamber):
    print("\033[94m=== Creating Data LUT for %s===\033[0m"%(chamber[0]))
    #open file
    inF = r.TFile(TRAININGFILE)
    myT = inF.plotTree
    
    
    # dictionaries that calculate the total offset / slope for a given
    # pattern / cc combination
    offsetSums = {}
    slopeSums = {}
    offsetMeans = {}
    slopeMeans = {}
    offsetRMS = {}
    slopeRMS = {}
    N = {} #array of how many matching segments were found for each pattern/cc
    
    counter = 0
    entries = myT.GetEntries()
    for event in myT:
        printProgress(counter,entries)
        counter +=1
        if(event.ccId == -1): continue #ignore unmatched cc's
        
        if not validEvent(event, chamber[1], chamber[2]): continue
        
        #######  Now look at the event itself
        if not (event.patternId in offsetSums):
            offsetSums[event.patternId]  = {}
            slopeSums[event.patternId]   = {}
            offsetMeans[event.patternId] = {}
            slopeMeans[event.patternId]  = {}
            offsetRMS[event.patternId]   = {}
            slopeRMS[event.patternId]    = {}
            N[event.patternId]           = {}
            
        if not(event.ccId in offsetSums[event.patternId]):
            offsetSums[event.patternId][event.ccId] = []
            slopeSums[event.patternId][event.ccId] = []

            
        offsetSums[event.patternId][event.ccId].append(event.segmentX-event.patX)
        slopeSums[event.patternId][event.ccId].append(event.segmentdXdZ)
     
    # total pattern/ccs we have entries for
    totalPatterns = 0
    
    #now loop over all the patterns we found    
    for patt in offsetSums.keys(): 
        for cc in offsetSums[patt].keys():
            totalPatterns +=1
            offsetMeans[patt][cc], offsetRMS[patt][cc] ,N[patt][cc] = getStats(offsetSums, patt, cc)
            slopeMeans[patt][cc], slopeRMS[patt][cc], _ = getStats(slopeSums, patt, cc)
            
    inF.Close()
    
    print("Found Entries for %i / 20480 possible patterns"%totalPatterns)
    return offsetMeans, slopeMeans, offsetRMS, slopeRMS, N

def runTest(chamber, dataOffset, dataSlope, dataN, linefitOffset, linefitSlope):  
    print("\033[94m=== Running Test on LUT for %s ===\033[0m"%(chamber[0])) 
     

    
    #array of all of the N threshold we should use, uses linefits for everything UNDER the threshold
    N_threshold      = array('d')
    rmsAtN_threshold = array('d')

    for N in range(1,15):
	N_threshold.append(N)
    N_threshold.append(20)
    N_threshold.append(30)
    N_threshold.append(50)
    N_threshold.append(100)
    N_threshold.append(500)
    N_threshold.append(1000)

    
    h_differences = []
    h_lineDiff    = []
    h_dataDiff    = []
    for i, N in enumerate(N_threshold):
        h_differences.append(r.TH1F("h_%i"%N,     "h_%i;     Segment - LUT [strips]; Segments"%N, 200,-1.,1.))
        h_lineDiff   .append(r.TH1F("h_line%i"%N, "h_line%i; Segment - LUT [strips]; Segments"%N, 200,-1.,1.))
        h_lineDiff[i].SetFillColor(r.kRed)
        h_dataDiff   .append(r.TH1F("h_data%i"%N, "h_data%i; Segment - LUT [strips]; Segments"%N, 200,-1.,1.))
        h_dataDiff[i].SetFillColor(r.kBlue)
    
    #open file
    inF = r.TFile(TESTFILE)
    myT = inF.plotTree
    
    #
    # make folder, if we don't have one, for the outputs
    #
    
    outputFolder = "../data/%s/%s"%(HIT_FOLDER, chamber[0])
    
    if not os.path.isdir(outputFolder):
        print("Making directory: %s"%outputFolder)
        os.system("mkdir %s"%outputFolder)
    
    #output file
    outFName = "%s/%s_LUT-Test.root"%(outputFolder,chamber[0])
    outF = r.TFile(outFName,"RECREATE")
    
    
    missedEvents = 0
    validEvents = 0
    entries = myT.GetEntries()
    for counter, event in enumerate(myT):
        printProgress(counter,entries)
        patt = event.patternId
        cc = event.ccId
        
        if(cc == -1): continue #ignore unmatched cc's
           
        #check if we should look at this event    
        if not validEvent(event, chamber[1], chamber[2]): continue
    
	validEvents += 1
        if not dataOffset.has_key(patt) or not dataOffset[patt].has_key(cc): 
            missedEvents += 1
            continue

        linefitDiff = event.segmentX - (linefitOffset[patt][cc] + event.patX)
        dataDiff    = event.segmentX -    (dataOffset[patt][cc] + event.patX)
    
        for i, N in enumerate(N_threshold):
            belowThreshold = dataN[patt][cc] < N
            h_differences[i].Fill(linefitDiff if belowThreshold else dataDiff)
            if belowThreshold:
                h_lineDiff[i].Fill(linefitDiff)
            else:
                h_dataDiff[i].Fill(dataDiff)
    
    for i, h in enumerate(h_differences):
        rmsAtN_threshold.append(h.GetRMS())
        h_differences[i].Write()
        h_stack = r.THStack(h_differences[i].GetName(), h_differences[i].GetName())
        h_stack.Add(h_lineDiff[i])
        h_stack.Add(h_dataDiff[i])
        c = r.TCanvas()
        c.cd()
        c.SetLogy()
        h_stack.Draw()
        
        leg = r.TLegend(0.03,0.05, 0.30, 0.25)
        leg.AddEntry(h_lineDiff[i], "Line Fit")
        leg.AddEntry(h_dataDiff[i], "Data")
        leg.Draw()
        
        #stats boxs
        c.Update()
        stack_stats = h_stack.GetHistogram().FindObject('stats')
        stack_stats.Draw()
        
        c.SaveAs("%s/%s-%i.pdf"%(outputFolder, chamber[0], N_threshold[i]))
        
        h_lineDiff[i].Write()
        h_dataDiff[i].Write()
    
    rmsVsNThreshold = r.TGraph(len(N_threshold),N_threshold,rmsAtN_threshold)
    rmsVsNThreshold.SetTitle("Test RMS vs LUT N_{t}")
    rmsVsNThreshold.GetXaxis().SetTitle("N_{t}")
    rmsVsNThreshold.GetYaxis().SetTitle("#sigma_{x} [strips]")
    rmsVsNThreshold.Write()
    
    print "Finished Writing data to ROOT file: %s"%outFName
    outF.Close()
    
    print("Finished running code: missed %i / %i = %f events"%(missedEvents,validEvents,1.*missedEvents/validEvents))
    
    return
  


#
#     Read in linear fit data
#    
linefitOffset, linefitSlope, _, _, _, _ = createLineFitLUT("../data/linearFits.txt")

#divide everything into chambers and run that way
chambers = []
#                name, st, ri
#chambers.append(["All-Chambers", 0, 0])
#chambers.append(["ME11B", 1,1])
chambers.append(["ME11A", 1,4])
#chambers.append(["ME12", 1,2])
#chambers.append(["ME13", 1,3])
#chambers.append(["ME21", 2,1])
#chambers.append(["ME22", 2,2])
#chambers.append(["ME31", 3,1])
#chambers.append(["ME32", 3,2])
#chambers.append(["ME41", 4,1])
#chambers.append(["ME42", 4,2])

for chamber in chambers:
    dataOffset, dataSlope, _, _, dataN = createDataLUT(chamber)
    runTest(chamber, dataOffset, dataSlope, dataN, linefitOffset, linefitSlope)
