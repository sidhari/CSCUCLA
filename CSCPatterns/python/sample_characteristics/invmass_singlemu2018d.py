#!/usr/bin/env python
import ROOT as r
import sys
sys.path.append("../")
import Plotter as p

writePath ='~/Documents/Presentations/2018/181213-EMTF/'

f = r.TFile('/home/wnash/workspace/CSCUCLA/CSCPatterns/dat/SingleMuon/zskim2018D/CSCDigiTree-Full.root')
if not hasattr(f, 'IsOpen'):
    print "can't open file"
    exit()

can = p.Canvas(lumi='')

invMass = p.Plot('h_osInvMass',f, legType='', option='hist')
invMass.legName='#bf{Entries}: %i'%invMass.GetEntries()
invMass.GetYaxis().SetTitle("Dimuons / 0.2 GeV")
invMass.GetXaxis().SetTitle("m_{#mu#mu} [GeV]")

can.addMainPlot(invMass)

leg = can.makeLegend(pos='tl')

can.cleanup(writePath+"invmassZ.pdf", mode='BOB')