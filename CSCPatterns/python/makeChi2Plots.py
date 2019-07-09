#!/usr/bin/env python
import ROOT as r
import common
from array import array


r.gSystem.Load('../lib/PatternFinderHelperFunctions_cpp')
r.gSystem.Load('../lib/LUTClasses_cpp')

#make lut
lut = r.LUT("linefits","../data/linearFits.lut")
lut.makeFinal()

hists = {}

nlayers = [3,4,5,6]

for layer in nlayers:
     hists[layer] = r.TH1D("%ilayers-quality"%layer, "%ilayers-quality; #chi^{2}; Comparator Codes"%layer,100,0,10.)


#file = r. TFile('../data/Charmonium/charmonium2016F+2017BCEF/CLCTMatch-Full.root')
file = r. TFile('../data/Charmonium/charmonium2016F+2017BCEF/CLCTMatch-Full.root')
tree = file.plotTree

entries = tree.GetEntries()
for i, event in enumerate(tree):
    common.printProgress(i,entries)
    if(event.ccId == -1): continue
    
    key = r.LUTKey(event.patternId, event.ccId)
    
    
    entry = r.MakeNullPointer(r.LUTEntry())
    if lut.getEntry(key, entry):
        exit()




newLUTs = r.DetectorLUTs()
legacyLUTs = r.DetectorLUTs()

#if r.makeLUT(tree, newLUTs, legacyLUTs) :
#    exit()

print "gottem'd"




'''

class code:
    def __init__(self, patt_, cc_, chi2_):
        self.patt = patt_
        self.cc   = cc_
        self.chi2 = chi2_

#load the pattern drawer class
if r.gSystem.Load('../src/patternDrawer_cpp'):
    print "Can't find pattern drawer library"



lineLUT = common.loadLUT("../data/linearFits.lut")

qualityDivisions = 16 #4 bits


hists = {}

nlayers = [2,3,4,5,6]

#create a dictionary containing all the chi2 we see, seperated by chamber
chi2Entries = {}
for layer in nlayers:
    chi2Entries[layer] = array('d')
    
codes = {}
for layer in nlayers:
    codes[layer] = []

for patt in lineLUT.chi2.keys():
    for cc in lineLUT.chi2[patt].keys():
        chi2 = lineLUT.chi2[patt][cc]
        layers  = lineLUT.layers[patt][cc]
        #if not layers in hists:
        #    hists[layers] = r.TH1D("%i"%layers,"%i; #chi^{2};Comparator Codes"%layers,100,0,10.)
        #hists[layers].Fill(chi2)
        #chi2Entries[layers].append(chi2)
        codes[layers].append(code(patt, cc, chi2))
        
   
   
#sorting   
for layer in chi2Entries:
    codes[layer] = sorted(codes[layer], key=lambda code : code.chi2)
#    print "Printing layer %i"%layer
   #chi2Entries[layer] = sorted(chi2Entries[layer])
 #   for i,code in enumerate(codes[layer]):
  #      print code.chi2
   #     if i > 200: break
        
#    for i, chi2 in enumerate(chi2Entries[layer]):
#        print chi2
#        if i > 500: break







 
outF = r.TFile("../data/chi2Distributions.root", "RECREATE")      

#find samples of each code that falls within a given quality
samples = {}
        
#calculate intevals
for layer in nlayers:
    print "Looping through layer: %i"%layer
    if layer ==2: continue
    #samples[layer] = {} 
    colorhists ={}
    for i in range(0,qualityDivisions):
        colorhists[i] = r.TH1D("%ilayers-quality%i"%(layer, i), "%ilayers-quality%i; #chi^{2}; Comparator Codes"%(layer,i),100,0,10.)
        colorhists[i].SetFillColor(common.colors[i])
    histkey = 0 # current histogram we are looking at
    
    chi2Cuts = array('d')
    entries = len(codes[layer])

    for i in range(0, entries):
        if i ==0:
            print "Best Pattern"
            r.patternDrawer(codes[layer][i].patt, codes[layer][i].cc)
            
        #print "i: %i - integral: %i"%(i,integral)
        
        if i > ((histkey+1)*entries/qualityDivisions):
            print "-- histkey - %i i = %i , chi2 = %f, entries = %i"%((histkey+1), i, codes[layer][i].chi2,entries)
            r.patternDrawer(codes[layer][i].patt, codes[layer][i].cc)
            chi2Cuts.append(codes[layer][i].chi2)
            histkey+=1
        
        #integral += hists[layer].GetBinContent(i)
        colorhists[histkey].Fill(codes[layer][i].chi2)
        #print "entries = %i i = %i quality = %i"%(entries, i, qualityDivisions)



    #print "layer: %i - printing chi2Cuts"%layer
    #for i in range(0,len(chi2Cuts)):
        #print chi2Cuts[i]
      
      
#     #find samples of each code that falls within a given quality
#     samples = {}  
#     for i in range(0,len(chi2Cuts)):
#         for patt in lineLUT.chi2.keys():
#             if patt != 100: continue #only sample with pattern 100
#             for cc in lineLUT.chi2[patt].keys():
#                 
#                 if lineLUT.layers[patt][cc] != layer: continue
#                 chi2 = lineLUT.chi2[patt][cc]
#                 
#                 lowBound = 0 if i == 0 else chi2Cuts[i-1]
#                 highBound = chi2Cuts[i]
#                 #print "chi2 = %f - - lowBound = %f, highBound = %f"%(chi2, lowBound,highBound)
#                 
#                 if (not samples.has_key(chi2Cuts[i])) and chi2 >= lowBound and chi2 < highBound:
#                     samples[chi2Cuts[i]] = [patt, cc]
#  

                   
#     #print "Printing cc samples for %i layers:" %layer
#     for i in range(0, len(chi2Cuts)):
#         key = chi2Cuts[i]
#         #print key 
#         #print samples[key]
#                     
#     c = r.TCanvas()
#     t = r.TLegend(0.80,0.37, 0.97, 0.95)
#     stack = r.THStack("%i"%(layer), "%i"%(layer))
#     for key in range(0,qualityDivisions):
#         print "div = %i, entries = %i"%(layer, colorhists[layer].GetEntries())
#         stack.Add(colorhists[key])
# #         if key == 0:
# #             colorhists[key].SetStats(False)
# #             ymax = 0
# #             #if layer == 6: ymax = 250.
# #             #if layer == 5: ymax = 500.
# #             #if layer == 4: ymax = 400.
# #             #if layer == 3: ymax = 700.
# #              
# #              
# #             #colorhists[key].GetYaxis().SetRangeUser(0,ymax)
# #             colorhists[key].SetMaximum(ymax)
# #             colorhists[key].Draw()
# #             colorhists[key].SetMaximum(ymax)
# #              
# #         else:
# #             colorhists[key].Draw("same")
#         t.AddEntry(colorhists[layer], "Quality %i"%key)
#         colorhists[layer].Write()
#     stack.Draw()
#     t.Draw()
#     c.Write() 
# 
# outF.Close()


'''
