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

#### BEFORE NOV 28
#SAMPLEDIR       = "/uscms/home/wnash/eos/Charmonium/"
#TRAININGFOLDER = "charmonium2016F/"
#TESTFOLDER     = "charmonium2017D/"

#AFTER NOV 28
SAMPLEDIR       = "/uscms/home/wnash/eos/"
#TRAININGFOLDER = "charmonium2017C/"
TRAININGFOLDER = "Charmonium/charmonium2016F+2017BCEF/"
TESTFOLDER     = "SingleMuon/zskim2018D-redo/"
DATAFILE       = "CLCTMatch-Full.root"

LUTWRITEDIR = "../dat/" + TRAININGFOLDER
LUTROOTFILE = "LUT.root" #prefaced with chamber, e.g. ME11A-LUT.root
TESTWRITEDIR = LUTWRITEDIR+TESTFOLDER
    
    #file used to train the data LUT
TRAININGFILE = SAMPLEDIR+TRAININGFOLDER+DATAFILE

    #file used to test the data LUT
TESTFILE     = SAMPLEDIR+TESTFOLDER+DATAFILE  

BESTNTHRES = 10

common.makeDir(SAMPLEDIR, TRAININGFOLDER+TESTFOLDER)
#makeDir(SAMPLEDIR+)

#     #make directories
# if not os.path.isdir(SAMPLEDIR,LUTWRITEDIR):
#     print("Making directory: %s"%LUTWRITEDIR)
#     os.system("mkdir %s"%LUTWRITEDIR)
# 
# if not os.path.isdir(TESTWRITEDIR):
#     print("Making directory: %s"%TESTWRITEDIR)
#     os.system("mkdir %s"%TESTWRITEDIR)
#     
  
    

#TODO: move these to common
def printProgress(counter, entries):
    if((counter % 1000000) == 0) : print("Finished %0.2f%% of events"%(100.*counter/entries))

#Here we can select out if we want to look at only ME11, etc
def validEvent(event,st,ri):
    if((event.ST == st or st == 0) and (event.RI == ri or ri == 0)) : #0 means be indiscriminate
        return True
    return False


#TODO: Fix these two
def getStats(nums, patt, cc):
    N = len(nums[patt][cc])
    mean = 1.*sum(nums[patt][cc])/float(N)
    rms = math.sqrt(1.*sum(1.*i*i for i in nums[patt][cc])/float(N) - mean**2)
    
    return mean, rms, N

def getStats2(nums, patt):
    N = len(nums[patt])
    mean = 1.*sum(nums[patt])/float(N)
    rms = math.sqrt(1.*sum(1.*i*i for i in nums[patt])/float(N) - mean**2)
    
    return mean, rms, N



# writes a .lut file formatted as :
#    
# pattern (cc) ~ position slope nsegments (quality layers chi2)
#
# capable of writing LUTs for both the current patterns as well as the new 
# version designed to take advantage of them
def writeLUT(chamber, filepath, lut):
    print("\033[94m=== Writing Data LUT for %s===\033[0m"%(chamber))    
    print "Writing to file: %s"%filepath
    f = open(filepath,'w')
    
    for patt in lut.positions.keys(): 
        if not lut.isLegacy:
            for cc in lut.positions[patt].keys():
                #extra things we add to the line, if they exist, not great error checking!!
                extras = ' 0. 0 0.' if cc not in lut.quality[patt].keys() else ' %s %s %s'%(lut.quality[patt][cc], lut.layers[patt][cc], lut.chi2[patt][cc])
                
                outline = '%s %s ~ %s %s %s'%(patt, cc, lut.positions[patt][cc], lut.slopes[patt][cc], lut.nsegments[patt][cc])
                outline += extras
                f.write(outline+'\n')
        else: #we are making an LUT with just patterns, fill with fake cc
            outline = '%s ~ %s %s %s'%(patt,  lut.positions[patt], lut.slopes[patt], lut.nsegments[patt])
            f.write(outline+'\n')
            
    f.close()
    print "LUT Write complete!"
    return


 
         


def createDataLUT(chamber):
    print("\033[94m=== Creating Data LUT for %s===\033[0m"%(chamber[0]))
    #open file
    print "Using file: %s"%TRAININGFILE
    inF = r.TFile(TRAININGFILE)
    myT = inF.plotTree
    
    newLUT = common.LUT(False)
    legacyLUT = common.LUT(True)
    
    # dictionaries that calculate the total offset / slope for a given
    # pattern / cc combination
    newLCTPositionSums = {}
    newLCTSlopeSums = {}
    
    legacyLCTPositionSums = {}
    legacyLCTSlopeSums = {}
    
    
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
        if not (event.patternId in newLCTPositionSums):
            newLCTPositionSums[event.patternId]  = {}
            newLCTSlopeSums[event.patternId]   = {}
            
        if not(event.ccId in newLCTPositionSums[event.patternId]):
            newLCTPositionSums[event.patternId][event.ccId] = []
            newLCTSlopeSums[event.patternId][event.ccId] = []
            totalPatterns +=1

        if not(event.legacyLctId in legacyLCTPositionSums):
            legacyLCTPositionSums[event.legacyLctId] = []
            legacyLCTSlopeSums[event.legacyLctId] = []
            
        if not validSegments % 10000:
            ccCountVsSegmentsX.append(validSegments)
            ccCountVsSegmentsY.append(totalPatterns)

            
        newLCTPositionSums[event.patternId][event.ccId].append(float(event.segmentX)-float(event.patX))
        newLCTSlopeSums[event.patternId][event.ccId].append(float(event.segmentdXdZ))
        
        legacyLCTPositionSums[event.legacyLctId].append(float(event.segmentX) - float(event.legacyLctX))
        legacyLCTSlopeSums[event.legacyLctId].append(float(event.segmentdXdZ))
     
    
    #now loop over all the patterns we found    
    for patt in newLCTPositionSums.keys(): 
        for cc in newLCTPositionSums[patt].keys():
            position, _ ,nsegments = getStats(newLCTPositionSums, patt, cc)
            slope, _, _ = getStats(newLCTSlopeSums, patt, cc)
            newLUT.addEntry(patt, cc, position, slope, nsegments)
            
    for patt in legacyLCTPositionSums:
            position, _ ,nsegments = getStats2(legacyLCTPositionSums, patt)
            slope, _, _ = getStats2(legacyLCTSlopeSums, patt)
            legacyLUT.addLegacyEntry(patt, position, slope, nsegments)
            
    inF.Close()
    
    #TODO: Make it so we write the LUT to a file, so we don't ahve to calculate each time
    writeLUT(chamber[0], "%s%s.lut"%(LUTWRITEDIR, chamber[0]), newLUT)
    writeLUT(chamber[0], "%s%s-Legacy.lut"%(LUTWRITEDIR, chamber[0]), legacyLUT)

    outF = r.TFile(LUTWRITEDIR+chamber[0]+"-"+LUTROOTFILE,"RECREATE")
    g = r.TGraph(len(ccCountVsSegmentsX),ccCountVsSegmentsX,ccCountVsSegmentsY)
    g.Write()
    outF.Close()

    
    print("Found Entries for %i / 20480 possible patterns"%totalPatterns)
    return newLUT, legacyLUT

def getDataLUT(chamber):
    newlutfile    = LUTWRITEDIR+chamber[0]+'.lut'
    legacylutfile = LUTWRITEDIR+chamber[0]+'-Legacy.lut'
    
    if os.path.isfile(newlutfile) and os.path.isfile(legacylutfile):
        newlut = common.loadLUT(newlutfile)
        legacylut = common.loadLUT(legacylutfile)
        return newlut, legacylut
    else:
        return createDataLUT(chamber)


#TODO:
# write new function
# - should create an LUT, one with all line fits, one with all data differences, noting how many
#   segments were used
# - for different values of N, look at the RMS of the distribution obtained using linefits
#   compared to that using the data distribution
# - if things work like you expect, the width of the low N segments should be large in data 
#   for small N, but shrink (quadratically?) with more N
#   whereas the line fits should stay roughly the same as we move
# - Plot the distributions against each other, and find intersection, this is the best N

def runTest(chamber, newLUT, legacyLUT, linefitLUT):  
    print("\033[94m=== Running Test on LUT for %s ===\033[0m"%(chamber[0])) 
    print "Testing with: %s"%TESTFILE

    #array of the values of N we will look at, for each value, we look at the rms 
    # using entries in the LUT that were created using exactly the value in the array
    
    nThresholds = array('d')
    RMSatN = array('d')
    
    for N in range(1,10):
        nThresholds.append(N)
    nThresholds.append(15)
    nThresholds.append(20)
    nThresholds.append(30)
    nThresholds.append(50)
    nThresholds.append(100)
    nThresholds.append(500)
    nThresholds.append(1000)
    nThresholds.append(10000)
    nThresholds.append(100000)
    nThresholds.append(1000000)
    
    #set up different histograms    
    
    
    #plots i want 
    #   : data-segment, using khs if there is no entry in LUT  put in N-t = -1 ??
    #   : old pattern - segment with same requirement ^
    #   : plot using only line fits (h_lineOnly)
    #   : 
        
    h_newLUT_PosDiff = r.TH1F("h_newLUT-PosDiff", "h_newLUT-PosDiff; Segment - LUT [strips]; Segments", 200, -1.,1.)
    h_legLUT_PosDiff = r.TH1F("h_legLUT-PosDiff", "h_legLUT-PosDiff; Segment - LUT [strips]; Segments", 200, -1.,1.)
    h_linLUT_PosDiff = r.TH1F("h_linLUT-PosDiff", "h_linLUT-PosDiff; Segment - LUT [strips]; Segments", 200, -1.,1.)
    h_bestLUT_PosDiff = r.TH1F("h_bestLUT-PosDiff", "h_bestLUT-PosDiff; Segment - LUT [strips]; Segments", 200, -1.,1.)
    
    h_newLUT_SlopeDiff = r.TH1F("h_newLUT-SlopeDiff", "h_newLUT-SlopeDiff; Segment - LUT [strips/layer]; Segments", 200, -1.,1.)
    h_legLUT_SlopeDiff = r.TH1F("h_legLUT-SlopeDiff", "h_legLUT-SlopeDiff; Segment - LUT [strips/layer]; Segments", 200, -1.,1.)
    h_linLUT_SlopeDiff = r.TH1F("h_linLUT-SlopeDiff", "h_linLUT-SlopeDiff; Segment - LUT [strips/layer]; Segments", 200, -1.,1.)
    h_bestLUT_SlopeDiff = r.TH1F("h_bestLUT-SlopeDiff", "h_bestLUT-SlopeDiff; Segment - LUT [strips/layer]; Segments", 200, -1.,1.)
    
    h_legLUTPosDiff_Split = {}
    for patt in legacyLUT.positions.keys():
        h_legLUTPosDiff_Split[patt] = r.TH1F("h_legLUT-PosDiff-%i"%patt, "h_legLUT-PosDiff-%i; Segment - LUT [strips]; Segments"%patt, 200, -1.,1.)
     
    h_newLUTPosDiff_Split = {}  
    for patt in newLUT.positions.keys():
        h_newLUTPosDiff_Split[patt]= {} 
        for cc in newLUT.positions[patt].keys():
               h_newLUTPosDiff_Split[patt][cc] = r.TH1F("h_newLUT-PosDiff-%i-%i"%(patt,cc), "h_newLUT-PosDiff-%i-%i; Segment - LUT [strips]; Segments"%(patt,cc), 200, -1.,1.)  
    
    h_chi2s       = {}
     
    for i in range(3,7):
        h_chi2s[i] = r.TH1F("h_chi2-%ilays"%i, "h_chi2-%ilays; #chi^2; Segments"%i,100, 0.,10.)
         
    h_differences = []
#     h_lineDiff    = []
#     h_dataDiff    = []
# 
    for i, N in enumerate(nThresholds):
        h_differences.append(r.TH1F("h_diff%i"%N, "h_diff%i; Segment - LUT [strips]; Segments"%N, 200,-1.,1.))
#         h_lineDiff   .append(r.TH1F("h_line%i"%N, "h_line%i; Segment - LUT [strips]; Segments"%N, 200,-1.,1.))
#         h_lineDiff[i].SetFillColor(r.kRed)
#         h_dataDiff   .append(r.TH1F("h_data%i"%N, "h_data%i; Segment - LUT [strips]; Segments"%N, 200,-1.,1.))
#         h_dataDiff[i].SetFillColor(r.kBlue)    
    
        
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
        legacyId = event.legacyLctId
        
        if(cc == -1): continue #ignore unmatched cc's
           
        #check if we should look at this event    
        if not validEvent(event, chamber[1], chamber[2]): continue
        validEvents += 1
        
        
        newPosDiff = float(event.segmentX) - float(event.patX) #default to no information
        legPosDiff = float(event.segmentX) - float(event.patX)
        linPosDiff = float(event.segmentX) - float(event.legacyLctX)
        bestPosDiff = float(event.segmentX) - float(event.patX)
        
        newSlopeDiff = event.segmentdXdZ
        legSlopeDiff = event.segmentdXdZ
        linSlopeDiff = event.segmentdXdZ  
        bestSlopeDiff = event.segmentdXdZ  
        
        if newLUT.positions.has_key(patt) and newLUT.positions[patt].has_key(cc):
            newPosDiff   = float(event.segmentX) - (newLUT.positions[patt][cc] + float(event.patX))
            newSlopeDiff = event.segmentdXdZ - newLUT.slopes[patt][cc]
            h_newLUTPosDiff_Split[patt][cc].Fill(newPosDiff)
        
        if linefitLUT.positions.has_key(patt) and linefitLUT.positions[patt].has_key(cc):
            linPosDiff   = float(event.segmentX) - (linefitLUT.positions[patt][cc] + float(event.patX))  
            linSlopeDiff = event.segmentdXdZ - linefitLUT.slopes[patt][cc]
            h_chi2s[linefitLUT.layers[patt][cc]].Fill(linefitLUT.chi2[patt][cc])      
        
        if legacyLUT.positions.has_key(legacyId):
            legPosDiff = float(event.segmentX) - (legacyLUT.positions[legacyId] + float(event.legacyLctX))
            legSlopeDiff = event.segmentdXdZ - legacyLUT.slopes[legacyId]
            h_legLUTPosDiff_Split[legacyId].Fill(legPosDiff)
            
            
        if newLUT.positions.has_key(patt) and newLUT.positions[patt].has_key(cc) and newLUT.nsegments > BESTNTHRES:
            bestPosDiff = float(event.segmentX) - (newLUT.positions[patt][cc] + float(event.patX))
            bestSlopeDiff = event.segmentdXdZ - newLUT.slopes[patt][cc]
        else:
            bestPosDiff = float(event.segmentX) - (linefitLUT.positions[patt][cc] + float(event.patX)) 
            bestSlopeDiff = event.segmentdXdZ - linefitLUT.slopes[patt][cc]
            
        
        for i, N in enumerate(nThresholds):   
            if newLUT.nsegments.has_key(patt) and newLUT.nsegments[patt].has_key(cc) and newLUT.nsegments[patt][cc]  > N:
                h_differences[i].Fill(newPosDiff)
            else:
                h_differences[i].Fill(linPosDiff)
        
        h_newLUT_PosDiff.Fill(newPosDiff)
        h_linLUT_PosDiff.Fill(linPosDiff)
        h_legLUT_PosDiff.Fill(legPosDiff)
        h_bestLUT_PosDiff.Fill(bestPosDiff)
        
        h_newLUT_SlopeDiff.Fill(newSlopeDiff)
        h_linLUT_SlopeDiff.Fill(linSlopeDiff)
        h_legLUT_SlopeDiff.Fill(legSlopeDiff)
        h_bestLUT_SlopeDiff.Fill(bestSlopeDiff)
  

        
        
    
        if not newLUT.positions.has_key(patt) or not newLUT.positions[patt].has_key(cc): 
             missedEvents += 1
#         
#         ccLayers = int(linefitNDF[patt][cc]+2)
#         
#         h_chi2s[ccLayers-3].Fill(linefitChi2[patt][cc])
#         linefitDiff = event.segmentX - (linefitOffset[patt][cc] + event.patX)
#         dataDiff    = event.segmentX -    (dataOffset[patt][cc] + event.patX)
#     
#         for i, N in enumerate(nThresholds):   
#             if dataN[patt][cc]  == N:
#                 h_lineDiff[i].Fill(linefitDiff)
#                 h_dataDiff[i].Fill(dataDiff)
#             elif i and dataN[patt][cc] > nThresholds[i-1] and dataN[patt][cc] < N:
#                 h_lineDiff[i].Fill(linefitDiff)
#                 h_dataDiff[i].Fill(dataDiff)
#                 
#     
#         
#     for h in h_chi2s:
#         h.Write()
#     
#     g_LineRMSatN = r.TGraph(len(nThresholds),nThresholds,LineRMSatN)
#     g_LineRMSatN.GetXaxis().SetTitle("N_{t}")
#     g_LineRMSatN.GetYaxis().SetTitle("#sigma_{x} [strips]")
#     g_LineRMSatN.Write()
#     



    h_newLUT_PosDiff.Write()
    h_legLUT_PosDiff.Write()
    h_linLUT_PosDiff.Write()
    h_bestLUT_PosDiff.Write()
    
    h_newLUT_SlopeDiff.Write()
    h_legLUT_SlopeDiff.Write()
    h_linLUT_SlopeDiff.Write()
    h_bestLUT_SlopeDiff.Write()
     
    for i, h in enumerate(h_differences):
        RMSatN.append(h_differences[i].GetRMS())
        h_differences[i].Write()
        
        
    g_RMSatN = r.TGraph(len(nThresholds),nThresholds,RMSatN)
    g_RMSatN.GetXaxis().SetTitle("N_{t}")
    g_RMSatN.GetYaxis().SetTitle("#sigma_{x} [strips]")    
    g_RMSatN.Write()
    
    for i in h_chi2s:
         h_chi2s[i].Write()
    
    for patt in h_legLUTPosDiff_Split.keys():
        h_legLUTPosDiff_Split[patt].Write()
        
    for patt in h_newLUTPosDiff_Split.keys():
        for cc in h_newLUTPosDiff_Split[patt].keys():
            h_newLUTPosDiff_Split[patt][cc].Write()
    
    print "Finished Writing data to ROOT file: %s"%outFName

    outF.Close()

    
    print("Finished running code: missed %i / %i = %f events"%(missedEvents,validEvents,1.*missedEvents/validEvents))
    
    return 
  


#
#     Read in linear fit data
#    
linefitLUT = common.loadLUT("../dat/linearFits.lut")




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
    newLUT, legacyLUT = getDataLUT(chamber)
    runTest(chamber, newLUT, legacyLUT, linefitLUT)

