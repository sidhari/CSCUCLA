import ROOT as r
import sys
sys.path.append("../")
import Plotter as p
 
f = r.TFile('../../dat/SingleMuon/zskim2018D/emulation.root')
can = p.Canvas(lumi='')
 
emu_h = f.Get('emulationPattVsOffset')
#real_h = f.Get('realMultiplicity')
 
 
emu = p.Plot(emu_h,legName='', legType='l',option='colz')
#real = p.Plot(real_h,legName='#bf{Real}', legType='l',option='hist')
can.addMainPlot(emu, color=r.kBlue)
#can.addMainPlot(real, color=r.kBlack,addS=True)
#pt.GetXaxis().SetRangeUser(60.,120.)
#can.makeLegend(pos='tl')

# r.gStyle.SetStatX(0.35)
# r.gStyle.SetStatY(0.85)
# r.gStyle.SetOptStat(111110)
# can.makeStatsBox(match)

#r.gStyle.SetPadRightMargin(5)
can.finishCanvas('BOB')
r.gPad.SetRightMargin(0.2)
can.save('pattVsOffset.pdf')
can.deleteCanvas()
   
#can.cleanup('pattVsOffset.pdf', mode='BOB')#