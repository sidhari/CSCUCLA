#!/usr/bin/env python
import ROOT as r
import numpy as np
import math
import os
import common

from array import array

#
#
#


SAMPLEDIR       = "/uscms/home/wnash/eos/Charmonium/"
#TRAININGFOLDER = "charmonium2017C/"
TRAININGFOLDER = "charmonium2017C/"
TESTFOLDER     = "charmonium2017D/"
DATAFILE       = "CLCTMatch-Full.root"

LUTWRITEDIR = "../data/" + TRAININGFOLDER
LUTFILE = "LUT.root" #prefaced with chamber, e.g. ME11A-LUT.root
TESTWRITEDIR = LUTWRITEDIR+TESTFOLDER
    
    #file used to train the data LUT
TRAININGFILE = SAMPLEDIR+TRAININGFOLDER+DATAFILE

    #file used to test the data LUT
TESTFILE     = SAMPLEDIR+TESTFOLDER+DATAFILE  

    #make directories
if not os.path.isdir(LUTWRITEDIR):
    print("Making directory: %s"%LUTWRITEDIR)
    os.system("mkdir %s"%LUTWRITEDIR)

if not os.path.isdir(TESTWRITEDIR):
    print("Making directory: %s"%TESTWRITEDIR)
    os.system("mkdir %s"%TESTWRITEDIR)


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

def createDataLUT(chamber):
    print("\033[94m=== Creating Data LUT for %s===\033[0m"%(chamber[0]))
    #open file
    print "Using file: %s"%TRAININGFILE
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
    
    
    # for unique cc count vs segments ran over plot
    ccCountVsSegmentsY         = array('d')
    ccCountVsSegmentsX         = array('d')

    # total pattern/ccs we have entries for
    totalPatterns = 0
    #valid segments we have looked at
    validSegments = 0
    
    counter = 0
    entries = myT.GetEntries()
    for event in myT:
        printProgress(counter,entries)
        counter +=1
        if(event.ccId == -1): continue #ignore unmatched cc's
        
        if not validEvent(event, chamber[1], chamber[2]): continue
        validSegments+=1
        
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
            totalPatterns +=1

        if not validSegments % 10000:
            ccCountVsSegmentsX.append(validSegments)
            ccCountVsSegmentsY.append(totalPatterns)

            
        offsetSums[event.patternId][event.ccId].append(event.segmentX-event.patX)
        slopeSums[event.patternId][event.ccId].append(event.segmentdXdZ)
     
    
    #now loop over all the patterns we found    
    for patt in offsetSums.keys(): 
        for cc in offsetSums[patt].keys():
            offsetMeans[patt][cc], offsetRMS[patt][cc] ,N[patt][cc] = getStats(offsetSums, patt, cc)
            slopeMeans[patt][cc], slopeRMS[patt][cc], _ = getStats(slopeSums, patt, cc)
            
    inF.Close()
    
    #TODO: Make it so we write the LUT to a file, so we don't ahve to calculate each time
    print("\033[94m=== Writing Data LUT for %s===\033[0m"%(chamber[0]))
    outF = r.TFile(LUTWRITEDIR+chamber[0]+"-"+LUTFILE,"RECREATE")
    g = r.TGraph(len(ccCountVsSegmentsX),ccCountVsSegmentsX,ccCountVsSegmentsY)
    g.Write()
    outF.Close()

    
    print("Found Entries for %i / 20480 possible patterns"%totalPatterns)
    return offsetMeans, slopeMeans, offsetRMS, slopeRMS, N


#TODO:
# write new function
# - should create an LUT, one with all line fits, one with all data differences, noting how many
#  segments were used
# - for different values of N, look at the RMS of the distribution obtained using linefits
#   compared to that using the data distribution
# - if things work like you expect, the width of the low N segments should be large in data 
#   for small N, but shrink (quadratically?) with more N
#   whereas the line fits should stay roughly the same as we move
# - Plot the distributions against each other, and find intersection, this is the best N

def runTest(chamber, dataOffset, dataSlope, dataN, linefitOffset, linefitSlope, linefitChi2, linefitNDF):  
    print("\033[94m=== Running Test on LUT for %s ===\033[0m"%(chamber[0])) 
    print "Testing with: %s"%TESTFILE


    #array of the values of N we will look at, for each value, we look at the rms 
    # using entries in the LUT that were created using exactly the value in the array
    
    nSegmentsUsedInLUTCreation = array('d')
    DataRMSatN = array('d')
    LineRMSatN = array('d')
    
    for N in range(1,10):
        nSegmentsUsedInLUTCreation.append(N)
    nSegmentsUsedInLUTCreation.append(15)
    nSegmentsUsedInLUTCreation.append(20)
    nSegmentsUsedInLUTCreation.append(30)
    nSegmentsUsedInLUTCreation.append(50)
    nSegmentsUsedInLUTCreation.append(100)
    nSegmentsUsedInLUTCreation.append(500)
    nSegmentsUsedInLUTCreation.append(1000)
    nSegmentsUsedInLUTCreation.append(10000)
    nSegmentsUsedInLUTCreation.append(100000)
    
    #set up different histograms    
        
    h_chi2s       = []
    
    
    for i in range(3,7):
        h_chi2s.append(r.TH1F("h_chi2-%ilays"%i, "h_chi2-%ilays; #chi^2; Segments"%i,100, 0.,5.))
        
    h_differences = []
    h_lineDiff    = []
    h_dataDiff    = []


    for i, N in enumerate(nSegmentsUsedInLUTCreation):
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
    

    outputFolder = "%s/%s"%(TESTWRITEDIR, chamber[0])
    
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
        
        ccLayers = int(linefitNDF[patt][cc]+2)
        
        h_chi2s[ccLayers-3].Fill(linefitChi2[patt][cc])
        linefitDiff = event.segmentX - (linefitOffset[patt][cc] + event.patX)
        dataDiff    = event.segmentX -    (dataOffset[patt][cc] + event.patX)
    
        for i, N in enumerate(nSegmentsUsedInLUTCreation):   
            if dataN[patt][cc]  == N:
                h_lineDiff[i].Fill(linefitDiff)
                h_dataDiff[i].Fill(dataDiff)
            elif i and dataN[patt][cc] > nSegmentsUsedInLUTCreation[i-1] and dataN[patt][cc] < N:
                h_lineDiff[i].Fill(linefitDiff)
                h_dataDiff[i].Fill(dataDiff)
                
    
    for i, h in enumerate(h_lineDiff):
        LineRMSatN.append(h_lineDiff[i].GetRMS())
        DataRMSatN.append(h_dataDiff[i].GetRMS())
        
        h_lineDiff[i].Write()
        h_dataDiff[i].Write()
        
    for h in h_chi2s:
        h.Write()
    
    g_LineRMSatN = r.TGraph(len(nSegmentsUsedInLUTCreation),nSegmentsUsedInLUTCreation,LineRMSatN)
    g_LineRMSatN.GetXaxis().SetTitle("N_{t}")
    g_LineRMSatN.GetYaxis().SetTitle("#sigma_{x} [strips]")
    g_LineRMSatN.Write()
    
    g_DataRMSatN = r.TGraph(len(nSegmentsUsedInLUTCreation),nSegmentsUsedInLUTCreation,DataRMSatN)
    g_DataRMSatN.GetXaxis().SetTitle("N_{t}")
    g_DataRMSatN.GetYaxis().SetTitle("#sigma_{x} [strips]")    
    g_DataRMSatN.Write()
    
    print "Finished Writing data to ROOT file: %s"%outFName

    outF.Close()

    
    print("Finished running code: missed %i / %i = %f events"%(missedEvents,validEvents,1.*missedEvents/validEvents))
    
    return 
  


#
#     Read in linear fit data
#    
linefitOffset, linefitSlope, linefitChi2, linefitNDF, _, _ = common.createLineFitLUT("../data/linearFits.txt")




#divide everything into chambers and run that way
chambers = []
#                name, st, ri
#chambers.append(["All-Chambers", 0, 0])
#chambers.append(["ME11B", 1,1])
#chambers.append(["ME11A", 1,4])
chambers.append(["ME12", 1,2])
#chambers.append(["ME13", 1,3])
#chambers.append(["ME21", 2,1])
#chambers.append(["ME22", 2,2])
#chambers.append(["ME31", 3,1])
#chambers.append(["ME32", 3,2])
#chambers.append(["ME41", 4,1])
#chambers.append(["ME42", 4,2])


for chamber in chambers:
    dataOffset, dataSlope, _, _, dataN = createDataLUT(chamber)
    runTest(chamber, dataOffset, dataSlope, dataN, linefitOffset, linefitSlope, linefitChi2, linefitNDF)

