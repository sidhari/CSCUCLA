#!/usr/bin/env python
import ROOT as r
import sys
sys.path.append("../")
import Plotter as p
import common

writePath ='~/Documents/Presentations/2018/181213-EMTF/'

f = r.TFile('/home/wnash/workspace/CSCUCLA/CSCPatterns/dat/SingleMuon/zskim2018D/CSCDigiTree-Full.root')
if not hasattr(f, 'IsOpen'):
    print "can't open file"
    exit()

can = p.Canvas(True,lumi='')
# 
# pt_h = r.TH1F('pt','pt; Muon pT [GeV]; Muons / 1 GeV', 500, 0, 150)
# 
# myT = f.CSCDigiTree
# entries = myT.GetEntries()
# for counter, event in enumerate(myT):
#     #if counter == 10000: break
#     common.printProgress(counter,entries)
#     pt_h.Fill(event.Pt)
    
pt = p.Plot('h_selectedMuonsPt',f,legType='', option='hist')
pt.GetYaxis().SetTitle('Muons / 1 GeV')
pt.legName='#splitline{#splitline{#bf{Entries}: %i}{#bf{Mean}: %3.3f}}{#bf{OFlow}: %i}'%(pt.GetEntries(),pt.GetMean(), pt.GetBinContent(pt.GetNbinsX()+1))
can.addMainPlot(pt)

leg = can.makeLegend(pos='tr')
leg.moveLegend(X=-0.15)

can.cleanup(writePath+"ptZ.pdf", mode='BOB')