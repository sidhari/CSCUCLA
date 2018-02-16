#!/usr/bin/env python
import ROOT as r
import numpy as np
import math as m

useCompHits = 0 # 0 means use recHits


nbins = 1000
hist_range  = 2.
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

def createHists(chamber):
    print("=== Running over Chamber %s ==="%(chamber[0]))
    
    #open file
    inF = r.TFile("../data/%s/plotTree.root"%folder)
    myT = inF.plotTree
    
    #output file
    outF = r.TFile("../data/%s/%s_resolutionPlots.root"%(folder,chamber[0]),"RECREATE")
    
    
    #frequency plots, will be returned
    patList = [100,400,500,800,900]
    #freakwencies = {100:{},400:{},500:{},800:{},900:{}}
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
        if not (event.envelopeId in patternPosMeans):
            patternPosMeans[event.envelopeId] = []
            patternSlopeMeans[event.envelopeId] = []
            ccPosMeans[event.envelopeId] = {}
            ccSlopeMeans[event.envelopeId] = {}
            unshiftedPosSlopePlots[event.envelopeId] = r.TH1D("unshifted_patSlope%i"%(event.envelopeId),
                                               "unshifted_patSlope%i;Slope [strips/layer]; Events"%(event.envelopeId), 
                                               nbins,-hist_range,hist_range)
            
        if not (event.patternId in ccPosMeans[event.envelopeId]):
            ccPosMeans[event.envelopeId][event.patternId] = []
            ccSlopeMeans[event.envelopeId][event.patternId] = []
            
        if not (event.legacyLctId in legacyPosMeans) :
            legacyPosMeans[event.legacyLctId] = []
            legacySlopeMeans[event.legacyLctId] = []
            unshiftedLegacySlopePlots[event.legacyLctId] = r.TH1D("unshifted_legSlope%i"%(event.legacyLctId),
                                               "unshifted_legSlope%i;Slope [strips/layer]; Events"%(event.legacyLctId), 
                                               nbins,-hist_range,hist_range)
            
        patternPosMeans[event.envelopeId].append(event.segmentX-event.patX)
        patternSlopeMeans[event.envelopeId].append(event.segmentdXdZ)
        ccPosMeans[event.envelopeId][event.patternId].append(event.segmentX-event.patX)
        ccSlopeMeans[event.envelopeId][event.patternId].append(event.segmentdXdZ)
        legacyPosMeans[event.legacyLctId].append(event.segmentX-event.legacyLctX)
        legacySlopeMeans[event.legacyLctId].append(event.segmentdXdZ)
        unshiftedPosSlopePlots[event.envelopeId].Fill(event.segmentdXdZ)
        unshiftedLegacySlopePlots[event.legacyLctId].Fill(event.segmentdXdZ)
        
        
    for env in unshiftedPosSlopePlots:
        unshiftedPosSlopePlots[env].Write()
        
    for leg in unshiftedLegacySlopePlots:
        unshiftedLegacySlopePlots[leg].Write()    
    
    print("Making Histograms...")
    
    pattPosPlots = {}
    pattSlopePlots = {}
    ccPosPlots = {}
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
        
        if not (event.envelopeId in calculatedPattPosMeans):
            calculatedPattPosMeans[event.envelopeId] = sum(patternPosMeans[event.envelopeId])/float(len(patternPosMeans[event.envelopeId]))
            #print("mean for %i is : %f"%(event.envelopeId, calculatedPattPosMeans[event.envelopeId]))
            calculatedPattSlopeMeans[event.envelopeId] = sum(patternSlopeMeans[event.envelopeId])/float(len(patternSlopeMeans[event.envelopeId]))
            pattPosPlots[event.envelopeId]= r.TH1D("patPos%i"%(event.envelopeId),
                                               "patPos%i;Position Difference [strips]; Events"%(event.envelopeId), 
                                               nbins,-hist_range,hist_range)
            pattSlopePlots[event.envelopeId] = r.TH1D("patSlope%i"%(event.envelopeId),
                                                      "patSlope%i;Slope [strips/layer]; Events"%(event.envelopeId),
                                                      nbins,-hist_range,hist_range)
            calculatedCCPosMeans[event.envelopeId] = {}
            calculatedCCSlopeMeans[event.envelopeId] = {}
            ccPosPlots[event.envelopeId] = {}
            ccSlopePlots[event.envelopeId] = {}
        if not (event.patternId in calculatedCCPosMeans[event.envelopeId]):
            calculatedCCPosMeans[event.envelopeId][event.patternId] = sum(ccPosMeans[event.envelopeId][event.patternId])/float(len(ccPosMeans[event.envelopeId][event.patternId]))
            calculatedCCSlopeMeans[event.envelopeId][event.patternId] = sum(ccSlopeMeans[event.envelopeId][event.patternId])/float(len(ccSlopeMeans[event.envelopeId][event.patternId]))
            ccPosPlots[event.envelopeId][event.patternId] = r.TH1D("patPos%i_cc%i"%(event.envelopeId, event.patternId),
                                                                  "patPos%i_cc%i;Position Difference [strips]; Events"%(event.envelopeId, event.patternId),
                                                                  nbins,-hist_range,hist_range)
            ccSlopePlots[event.envelopeId][event.patternId] = r.TH1D("patSlope%i_cc%i"%(event.envelopeId, event.patternId),
                                                                  "patSlope%i_cc%i;Slope [strips/layer]; Events"%(event.envelopeId, event.patternId),
                                                                  nbins,-hist_range,hist_range)
           
        if not (event.legacyLctId in calculatedlegacyPosMeans):
            calculatedlegacyPosMeans[event.legacyLctId] = sum(legacyPosMeans[event.legacyLctId])/float(len(legacyPosMeans[event.legacyLctId]))   
            calculatedlegacySlopeMeans[event.legacyLctId] = sum(legacySlopeMeans[event.legacyLctId])/float(len(legacySlopeMeans[event.legacyLctId]))
            legacyPosPlots[event.legacyLctId] = r.TH1D("legacyPos%i"%(event.legacyLctId),"legacyPos%i;Position Difference [strips]; Events"%(event.legacyLctId),
                                                       nbins,-hist_range,hist_range)
            legacySlopePlots[event.legacyLctId] = r.TH1D("legacySlope%i"%(event.legacyLctId),"legacySlope%i;Slope [strips/layer]; Events"%(event.legacyLctId),
                                                         nbins,-hist_range,hist_range)
          
    
        pattPosPlots[event.envelopeId].Fill(event.segmentX-event.patX - calculatedPattPosMeans[event.envelopeId])
        pattSlopePlots[event.envelopeId].Fill(event.segmentdXdZ - calculatedPattSlopeMeans[event.envelopeId])
        ccPosPlots[event.envelopeId][event.patternId].Fill(event.segmentX-event.patX - calculatedCCPosMeans[event.envelopeId][event.patternId])
        ccSlopePlots[event.envelopeId][event.patternId].Fill(event.segmentdXdZ - calculatedCCSlopeMeans[event.envelopeId][event.patternId])
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
    envOrdering = [900,800,500,400,100]
    for env in envOrdering:
        
        # [ hist, pat id]
        sortedccPosPlots = sorted( [[ccPosPlots[env][pat],pat] for pat in ccPosPlots[env]], key=lambda x: -1.*x[0].GetEntries())
        
        totalEntries = 0.
        #bad code
        for pat in sortedccPosPlots:
             totalEntries += pat[0].GetEntries()
             
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
        
        
        
        int = 0.
        for pat in sortedccPosPlots:
            int +=pat[0].GetEntries()
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
            #ccPosPlots[env][pat].Write()
        cumulativeCCPosResolution.Add(envFullPosRes)
        cumulativeCCSlopeResolution.Add(envFullSlopeRes)
        envFullPosRes.Write()
        envFullSlopeRes.Write()
            
            
    for id in legacyPosPlots:
        cumulativeLegacyPosResolution.Add(legacyPosPlots[id])
        cumulativeLegacySlopeResolution.Add(legacySlopePlots[id])
        legacyPosPlots[id].Write()    
        legacySlopePlots[id].Write()
        
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
#chambers.append(["All-Chambers", 0, 0])
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

frequencies = []

for chamber in chambers:
    frequencies.append([chamber[0], createHists(chamber)])    

outF = r.TFile("../data/%s/fullAnalFrequency.root"%(folder),"RECREATE")
 
patList = [100,400,500,800,900]

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

