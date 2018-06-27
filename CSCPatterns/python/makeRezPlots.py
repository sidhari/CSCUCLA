#!/usr/bin/env python
import ROOT as r
import numpy as np
import math as m

from array import array

useCompHits = 1 # 0 means use recHits

nbins = 200
hist_range  = 1.
coverage = 0.99 #look for bounds that give us 99% of the all entries in the histogram
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

#consider 10^-7 a frequency of zero
def getLogFrequency(frequency):
    logFreq = 0
    if (frequency != 0):
        logFreq = m.log10(frequency)
        if(logFreq <= -7.):
            print("Error, found frequency 10^%f reported as less than effective zero" % (logFreq))   
            sys.exit()     
    else:
        logFreq = -7.
    return logFreq

#for a given histogram, find the central bounds that give 99% of the events in the histogram
def findInterval(hist, coverage):
    entries = hist.GetEntries()
    if(entries is 0): return [0,0]
    low = hist.GetMinimum()
    high = hist.GetMaximum()
    
    threshold = (1.-coverage)/2.
    
    int = 0.
    for i in range(-1, hist.GetNbinsX()+1): #includes underflow and overflow
        int += hist.GetBinContent(i)/entries
        #print("int is : %f"%int)
        if int >= threshold :
            low = hist.GetBinCenter(i)
            break;
    
    #swap direction
    int = 0.
    for i  in range(-1, hist.GetNbinsX()+1)[::-1]:
        int += hist.GetBinContent(i)/entries
        #print("int is : %f"%int)
        if int >= threshold :
            high = hist.GetBinCenter(i)
            break;
    #print("returning [ %f, %f]"%(low,high))
    return [low,high]

def makeSegmentEfficiencyPlot(sortedPlots, chamberName, env):
    totalEntries = 0.
    #bad code
    for pat in sortedPlots:
        totalEntries += pat[0].GetEntries()
                
    x = array('d')
    y = array('d')
    
    sumCount = 0
    for pat in sortedPlots:
        thisCount = pat[0].GetEntries()
        sumCount += thisCount
        yP = 1.0 - (float(sumCount)/totalEntries)
        xP = float(thisCount)
        x.append(xP)
        y.append(yP)
        

    segmentGraph = r.TGraph(len(x), x,y)
    segmentGraph.SetTitle("%s - Pattern %i"%(chamberName, env))
    segmentGraph.GetXaxis().SetTitle("Minimum Number of Matches to a CC Used")
    segmentGraph.GetXaxis().CenterTitle()
    segmentGraph.GetYaxis().SetTitle("1-#epsilon")
    segmentGraph.GetYaxis().CenterTitle()
    c = r.TCanvas()
    c.SetLogx()
    c.SetTickx()
    c.SetLogy()
    c.SetTicky()
    segmentGraph.Draw("al")
    c.SaveAs("../img/%s/segmentEff_%s_%i.pdf"%(folder,chamberName, env))
    
    
    
    
    
def createHists(chamber):
    print("=== Running over Chamber %s ==="%(chamber[0]))
    
    #open file
    #inF = r.TFile("../data/%s/processedMatches_1.root"%folder)
    inF = r.TFile("/uscms/home/wnash/eos/Charmonium/charmonium2017D/CLCTMatch-Full.root")
    myT = inF.plotTree
    
    #output file
    outF = r.TFile("../data/%s/%s_resolutionPlots.root"%(folder,chamber[0]),"RECREATE")
    
    
    #frequency plots, will be returned
    patList = [100,90,80,70,60]
    freakwencies = {}
    for pat in patList:
        freakwencies[pat] = np.zeros(4096) #all the possible comparator codes, initialize their frequency to zero
    
    
    #init plots
    ccPosMeans = {}
    ccSlopeMeans = {}
    patternPosMeans = {}
    patternSlopeMeans = {}
    legacyPosMeans = {}
    legacySlopeMeans = {}
    
    unshiftedLegacySlopePlots = {}
    unshiftedPosSlopePlots = {}
    
    print("Calculating Means...")
    counter = 0
    entries = myT.GetEntries()
    for event in myT:
        counter +=1
        printProgress(counter,entries)
            
        #check if we shuold look at this event    
        if not validEvent(event, chamber[1], chamber[2]): continue
        
        #fill stuff we dont have yet
        if not (event.patternId in patternPosMeans):
            patternPosMeans[event.patternId] = []
            patternSlopeMeans[event.patternId] = []
            ccPosMeans[event.patternId] = {}
            ccSlopeMeans[event.patternId] = {}
            unshiftedPosSlopePlots[event.patternId] = r.TH1D("unshifted_patSlope%i"%(event.patternId),
                                               "unshifted_patSlope%i;Slope [strips/layer]; Events"%(event.patternId), 
                                               nbins,-hist_range,hist_range)
            
        if not (event.ccId in ccPosMeans[event.patternId]):
            ccPosMeans[event.patternId][event.ccId] = []
            ccSlopeMeans[event.patternId][event.ccId] = []
            
        if not (event.legacyLctId in legacyPosMeans) :
            legacyPosMeans[event.legacyLctId] = []
            legacySlopeMeans[event.legacyLctId] = []
            unshiftedLegacySlopePlots[event.legacyLctId] = r.TH1D("unshifted_legSlope%i"%(event.legacyLctId),
                                               "unshifted_legSlope%i;Slope [strips/layer]; Events"%(event.legacyLctId), 
                                               nbins,-hist_range,hist_range)
            
        patternPosMeans[event.patternId].append(event.segmentX-event.patX)
        patternSlopeMeans[event.patternId].append(event.segmentdXdZ)
        ccPosMeans[event.patternId][event.ccId].append(event.segmentX-event.patX)
        ccSlopeMeans[event.patternId][event.ccId].append(event.segmentdXdZ)
        legacyPosMeans[event.legacyLctId].append(event.segmentX-event.legacyLctX)
        legacySlopeMeans[event.legacyLctId].append(event.segmentdXdZ)
        unshiftedPosSlopePlots[event.patternId].Fill(event.segmentdXdZ)
        unshiftedLegacySlopePlots[event.legacyLctId].Fill(event.segmentdXdZ)
        
        
    for env in unshiftedPosSlopePlots:
        unshiftedPosSlopePlots[env].Write()
        
    for leg in unshiftedLegacySlopePlots:
        unshiftedLegacySlopePlots[leg].Write()    
    
    print("Making Histograms...")
    
    
    pattPosPlots = {}
    pattSlopePlots = {}
    ccPosPlots = {}
    unshiftedccPosPlots = {}
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
        
        if not (event.patternId in calculatedPattPosMeans):
            calculatedPattPosMeans[event.patternId] = sum(patternPosMeans[event.patternId])/float(len(patternPosMeans[event.patternId]))
            #print("mean for %i is : %f"%(event.patternId, calculatedPattPosMeans[event.patternId]))
            calculatedPattSlopeMeans[event.patternId] = sum(patternSlopeMeans[event.patternId])/float(len(patternSlopeMeans[event.patternId]))
            pattPosPlots[event.patternId]= r.TH1D("patPos%i"%(event.patternId),
                                               "patPos%i;Position Difference [strips]; Events"%(event.patternId), 
                                               nbins,-hist_range,hist_range)
            pattSlopePlots[event.patternId] = r.TH1D("patSlope%i"%(event.patternId),
                                                      "patSlope%i;Slope [strips/layer]; Events"%(event.patternId),
                                                      nbins,-hist_range,hist_range)
            calculatedCCPosMeans[event.patternId] = {}
            calculatedCCSlopeMeans[event.patternId] = {}
            ccPosPlots[event.patternId] = {}
            unshiftedccPosPlots[event.patternId] = {}
            ccSlopePlots[event.patternId] = {}
        if not (event.ccId in calculatedCCPosMeans[event.patternId]):
            calculatedCCPosMeans[event.patternId][event.ccId] = sum(ccPosMeans[event.patternId][event.ccId])/float(len(ccPosMeans[event.patternId][event.ccId]))
            calculatedCCSlopeMeans[event.patternId][event.ccId] = sum(ccSlopeMeans[event.patternId][event.ccId])/float(len(ccSlopeMeans[event.patternId][event.ccId]))
            ccPosPlots[event.patternId][event.ccId] = r.TH1D("patPos%i_cc%i"%(event.patternId, event.ccId),
                                                                  "patPos%i_cc%i;Position Difference [strips]; Events"%(event.patternId, event.ccId),
                                                                  nbins,-hist_range,hist_range)
            unshiftedccPosPlots[event.patternId][event.ccId]  = r.TH1D("unshiftedPatPos%i_cc%i"%(event.patternId, event.ccId),
                                                                  "unshiftedPatPos%i_cc%i;Position Difference [strips]; Events"%(event.patternId, event.ccId),
                                                                  nbins,-hist_range,hist_range)
            ccSlopePlots[event.patternId][event.ccId] = r.TH1D("patSlope%i_cc%i"%(event.patternId, event.ccId),
                                                                  "patSlope%i_cc%i;Slope [strips/layer]; Events"%(event.patternId, event.ccId),
                                                                  nbins,-hist_range,hist_range)
           
        if not (event.legacyLctId in calculatedlegacyPosMeans):
            calculatedlegacyPosMeans[event.legacyLctId] = sum(legacyPosMeans[event.legacyLctId])/float(len(legacyPosMeans[event.legacyLctId]))   
            calculatedlegacySlopeMeans[event.legacyLctId] = sum(legacySlopeMeans[event.legacyLctId])/float(len(legacySlopeMeans[event.legacyLctId]))
            legacyPosPlots[event.legacyLctId] = r.TH1D("legacyPos%i"%(event.legacyLctId),"legacyPos%i;Position Difference [strips]; Events"%(event.legacyLctId),
                                                       nbins,-hist_range,hist_range)
            legacySlopePlots[event.legacyLctId] = r.TH1D("legacySlope%i"%(event.legacyLctId),"legacySlope%i;Slope [strips/layer]; Events"%(event.legacyLctId),
                                                         nbins,-hist_range,hist_range)
          
    
        pattPosPlots[event.patternId].Fill(event.segmentX-event.patX - calculatedPattPosMeans[event.patternId])
        pattSlopePlots[event.patternId].Fill(event.segmentdXdZ - calculatedPattSlopeMeans[event.patternId])
        ccPosPlots[event.patternId][event.ccId].Fill(event.segmentX-event.patX - calculatedCCPosMeans[event.patternId][event.ccId])
        unshiftedccPosPlots[event.patternId][event.ccId].Fill(event.segmentX-event.patX)
        ccSlopePlots[event.patternId][event.ccId].Fill(event.segmentdXdZ - calculatedCCSlopeMeans[event.patternId][event.ccId])
        legacyPosPlots[event.legacyLctId].Fill(event.segmentX-event.legacyLctX - calculatedlegacyPosMeans[event.legacyLctId])
        legacySlopePlots[event.legacyLctId].Fill(event.segmentdXdZ-calculatedlegacySlopeMeans[event.legacyLctId])
    
    
    
    
    print("Writing Histograms...")
    
    cumulativePattPosResolution = r.TH1D("cumPatPosRes", "Cumulative Pattern Position Resolution; Position Difference [strips]; Events", nbins,-hist_range,hist_range)
    cumulativePattSlopeResolution = r.TH1D("cumPatSlopeRes", "Cumulative Pattern Slope Resolution; Slope [strips/layer]; Events", nbins,-hist_range,hist_range)
    cumulativeCCPosResolution = r.TH1D("cumCCPosRes", "Cumulative CC Position Resolution; Position Difference [strips]; Events", nbins,-hist_range,hist_range)
    cumulativeCCSlopeResolution = r.TH1D("cumCCSlopeRes", "Cumulative CC Slope Resolution; Slope [strips/layer]; Events", nbins,-hist_range,hist_range)
    cumulativeLegacyPosResolution = r.TH1D("cumLegPosRes","Cumulative Legacy Position Resolution; Position Difference [strips]; Events", nbins,-hist_range,hist_range)
    cumulativeLegacySlopeResolution = r.TH1D("cumLegSlopeRes","Cumulative Legacy Slope Resolution; Slope [strips/layer]; Events", nbins,-hist_range,hist_range)

    
    for pat in pattPosPlots:
        cumulativePattPosResolution.Add(pattPosPlots[pat])
        cumulativePattSlopeResolution.Add(pattSlopePlots[pat])
        pattPosPlots[pat].Write()
        pattSlopePlots[pat].Write()
        

    ccFrequency = r.THStack("percentageStack","Frequency of CC for %s; Percentage of Pattern Matches; CC Count"%(chamber[0]))

    colors = [r.kBlue, r.kMagenta+1, r.kRed, r.kOrange+1, r.kBlack]
    ccounter = 0
    envOrdering = [90,80,70,60,100]
    for env in envOrdering:
        
        if not len(ccPosPlots[env]): continue
        
        # [ hist, pat id] - sorted from highest entry to lowest entry
        sortedccPosPlots = sorted( [[ccPosPlots[env][pat],pat] for pat in ccPosPlots[env]], key=lambda x: -1.*x[0].GetEntries())
        
        
        totalEntries = 0.
        #bad code
        for pat in sortedccPosPlots:
             totalEntries += pat[0].GetEntries()
             
             
        makeSegmentEfficiencyPlot(sortedccPosPlots,chamber[0], env)
             
        #real bad code lel
        for pat in sortedccPosPlots:
            freakwencies[env][pat[1]] = 1.*pat[0].GetEntries()/totalEntries
            
        
             
        #print("For Pattern %i - CC - per - int"%(env))
        ccEnvFrequency  = r.TH1D("percentage_pat%i"%(env), "%s - Pattern %i using %i matches; Percentage; CC Count"%(chamber[0], env, totalEntries), 8,np.logspace(-6,2,9));
        ccEnvFrequency.SetLineColor(colors[ccounter])
        ccEnvFrequency.SetFillColor(colors[ccounter])
        ccounter +=1
        
        ccCutOffNum = r.TH1D("cutoffNum_pat%i"%(env), 
                          "%s - Pattern %i; Amount of CCs; Efficiency"%(chamber[0],env),
                          len(sortedccPosPlots), 0, len(sortedccPosPlots)+1)
        ccCutOffDen = r.TH1D("cutoffDen_pat%i"%(env), 
                          "%s - Pattern %i; Amount of CCs; Efficiency"%(chamber[0],env),
                          len(sortedccPosPlots), 0, len(sortedccPosPlots)+1)
        
        int = 0.
        for bin in range(1,len(sortedccPosPlots)+1):
            #print sortedccPosPlots[bin-1][0].GetEntries()
            int += sortedccPosPlots[bin-1][0].GetEntries()
            ccCutOffNum.SetBinContent(bin, totalEntries - int)
            ccCutOffDen.SetBinContent(bin,totalEntries)
        
        mcanvas = r.TCanvas("can%i"%(env),"can%i"%(env),1)
        mcanvas.SetLogx()
        mcanvas.SetLogy()
        mcanvas.SetTickx()
        #mcanvas.SetGridy()
        mcanvas.SetTicky()    
            
        ccCutOff = r.TGraphAsymmErrors(ccCutOffNum,ccCutOffDen)
        ccCutOff.GetYaxis().SetTitle("1 - #epsilon")
        ccCutOff.GetYaxis().SetNdivisions(30)
        ccCutOff.GetYaxis().CenterTitle()
        ccCutOff.GetXaxis().SetTitle("Number of Comparator Codes Used")
        ccCutOff.GetXaxis().SetTitleOffset(1.2)
        ccCutOff.GetXaxis().CenterTitle()
        ccCutOff.GetXaxis().SetMoreLogLabels()
        ccCutOff.Draw()
        mcanvas.SaveAs("../img/%s/patternFreq/1-e_%s_p%i.pdf"%(folder, chamber[0], env))
        
        mcanvas.Clear()
        mcanvas.SetGridy(0)
        
        #make efficiency vs segment plot
        effVsSeg = r.TH1D("effVsSeg_pat%i"%env, "%s - Pattern %i; Segments found matched to a given CC; Comparator Codes"%(chamber[0],env),
                          60,np.logspace(0,6,61))
        
        for pat in sortedccPosPlots:
            effVsSeg.Fill(pat[0].GetEntries())
            
        effVsSeg.Draw()
        mcanvas.SaveAs("../img/%s/patternFreq/effVsSeg_%s_p%i.pdf"%(folder, chamber[0], env))
        
        mcanvas.Clear()
        mcanvas.SetGridy(0)
        

        int = 0.
        for pat in sortedccPosPlots:
            int += pat[0].GetEntries()
            #print("%s \t %i \t %0.3f \t %0.3f"%(pat[0].GetName(), pat[1], 100.*pat[0].GetEntries()/totalEntries, 100.*int/totalEntries))
            ccEnvFrequency.Fill(100.*pat[0].GetEntries()/totalEntries)
            #pat[0].Write()
        ccEnvFrequency.Draw()
        mcanvas.SaveAs("../img/%s/patternFreq/%s-p%i.pdf"%(folder, chamber[0], env))
        ccEnvFrequency.Write()
        ccFrequency.Add(ccEnvFrequency)
        
    ccFrequency.Write()


    for env in ccPosPlots:
        envFullPosRes = r.TH1D("patPos%i_fullCCRes"%(env),
                         "patPos%i_fullCCRes;Position Difference [strips]; Events"%(env),
                         nbins,-hist_range,hist_range)
        envFullSlopeRes = r.TH1D("patSlope%i_fullCCRes"%(env),
                         "patSlope%i_fullCCRes;Slope [strips/layer]; Events"%(env),
                         nbins,-hist_range,hist_range)
        for pat in ccPosPlots[env]:
            envFullPosRes.Add(ccPosPlots[env][pat])
            envFullSlopeRes.Add(ccSlopePlots[env][pat])
            ccPosPlots[env][pat].Write()
            unshiftedccPosPlots[env][pat].Write()
        cumulativeCCPosResolution.Add(envFullPosRes)
        cumulativeCCSlopeResolution.Add(envFullSlopeRes)
        envFullPosRes.Write()
        envFullSlopeRes.Write()
            
            
    for id in legacyPosPlots:
        cumulativeLegacyPosResolution.Add(legacyPosPlots[id])
        cumulativeLegacySlopeResolution.Add(legacySlopePlots[id])
        legacyPosPlots[id].Write()    
        legacySlopePlots[id].Write()
        
        
    legPosInt = findInterval(cumulativeLegacyPosResolution, coverage)
    legSlopeInt = findInterval(cumulativeLegacySlopeResolution, coverage)
    pattPosInt = findInterval(cumulativePattPosResolution, coverage)
    pattSlopeInt = findInterval(cumulativePattSlopeResolution, coverage)
    ccPosInt = findInterval(cumulativeCCPosResolution, coverage)
    ccSlopeInt = findInterval(cumulativeCCSlopeResolution, coverage)
    
    #Gross hack to print out intervals
    intFile.write("%s"%chamber[0])
    intFile.write("\t %f \t %f \t %f \t %f \t %f \t %f"%(legPosInt[0],legPosInt[1], pattPosInt[0], pattPosInt[1], ccPosInt[0], ccPosInt[1]))
    intFile.write("\t %f \t %f \t %f \t %f \t %f \t %f\n"%(legSlopeInt[0],legSlopeInt[1], pattSlopeInt[0], pattSlopeInt[1], ccSlopeInt[0], ccSlopeInt[1]))

    
    cumulativePattPosResolution.Write()
    cumulativePattSlopeResolution.Write()
    cumulativeCCPosResolution.Write()
    cumulativeCCSlopeResolution.Write()
    cumulativeLegacyPosResolution.Write()
    cumulativeLegacySlopeResolution.Write()
    
    outF.Close()   
    
    return freakwencies 
       
#actually run the code, not particularly efficient
chambers = []
#                name, st, ri
chambers.append(["All-Chambers", 0, 0])
#chambers.append(["ME11B", 1,1])
#chambers.append(["ME11A", 1,4])
#chambers.append(["ME12", 1,2])
#chambers.append(["ME13", 1,3])
#chambers.append(["ME21", 2,1])
#chambers.append(["ME22", 2,2])
#chambers.append(["ME31", 3,1])
#chambers.append(["ME32", 3,2])
#chambers.append(["ME41", 4,1])
#chambers.append(["ME42", 4,2])


#interval file
intFile = open("intervals_%s.txt"%folder, "w")
intFile.write("Chamber \t legLow[strips] \t legHigh[strips] \t 3low[strips] \t 3high[strips] \t clow[strips] \t chigh[strips]")
intFile.write("\t legLow[strips/layer] \t legHigh[strips/layer] \t 3low[strips/layer] \t 3high[strips/layer] \t clow[strips/layer] \t chigh[strips/layer]\n")

frequencies = []

for chamber in chambers:
    frequencies.append([chamber[0], createHists(chamber)])   
    
intFile.close() 

outF = r.TFile("../data/%s/fullAnalFrequency.root"%(folder),"RECREATE")
 
patList = [100,70,60,80,90]

#iterate over all combinations of chambers
for chamber1 in range(0,len(frequencies)):
    for chamber2 in range(chamber1+1, len(frequencies)):
        for pat in patList:
        
            freqPlot1D = r.TH1D("%s-%s-1DlogFreq%i"%(frequencies[chamber1][0],frequencies[chamber2][0],pat),
                              "%s vs %s (Pattern %i)"%(frequencies[chamber1][0],frequencies[chamber2][0], pat),
                               200, -3., 3.)  
            freqPlot1D.GetXaxis().SetTitle("log_{10}(f_{%s}/f_{%s})"%(frequencies[chamber1][0],frequencies[chamber2][0]))
            freqPlot1D.GetXaxis().CenterTitle()
            
            freqPlot1D.GetYaxis().SetTitle("Counts")
            
            freqPlot2D = r.TH2D("%s-%s-2DlogFreq%i"%(frequencies[chamber1][0],frequencies[chamber2][0],pat),
                                "%s vs %s (Pattern %i); log_{10}(f_{%s}); log_{10}(f_{%s})"%(frequencies[chamber1][0],frequencies[chamber2][0],pat,
                                                                               frequencies[chamber1][0],frequencies[chamber2][0]),
                                           200, -8., 0.,
                                           200, -8., 0.)
                                
            
            for cc in range(0,len(frequencies[chamber1][1][pat])): #0-4096
 
                chamber1Fill = getLogFrequency(frequencies[chamber1][1][pat][cc])
                chamber2Fill = getLogFrequency(frequencies[chamber2][1][pat][cc])     
                
                
                ### THESE ARE HEAVILY EFFECTED BY FAKE ZERO RATES
                freqPlot1D.Fill(chamber1Fill-chamber2Fill)
                freqPlot2D.Fill(chamber1Fill,chamber2Fill)
                
            freqPlot1D.Write()
            freqPlot2D.Write()
              
outF.Close()

