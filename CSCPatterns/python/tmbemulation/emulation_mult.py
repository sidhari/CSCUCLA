import ROOT as r
import sys
sys.path.append("../")
import Plotter as p
 
f = r.TFile('../../dat/SingleMuon/zskim2018D/emulation.root')
can = p.Canvas(True,lumi='')
 
emu_h = f.Get('emulatedMultiplicity')
real_h = f.Get('realMultiplicity')
 
 
emu = p.Plot(emu_h,legName='#bf{Emulation}', legType='l',option='hist')
real = p.Plot(real_h,legName='#bf{Real}', legType='l',option='hist')
can.addMainPlot(emu, color=r.kBlue,addS=True)
can.addMainPlot(real, color=r.kBlack,addS=True)
#pt.GetXaxis().SetRangeUser(60.,120.)
can.makeLegend(pos='tl')

# r.gStyle.SetStatX(0.35)
# r.gStyle.SetStatY(0.85)
# r.gStyle.SetOptStat(111110)
# can.makeStatsBox(match)
   
can.cleanup('multiplicity.pdf', mode='BOB')#