import ROOT as r


useCompHits = 0 # 0 means use recHits
chamber = "All-Chambers"

#plot formatting
r.gStyle.SetOptStat(111110)
r.gStyle.SetPadTickX(1)
r.gStyle.SetPadTickY(1)
r.gStyle.SetStatFontSize(60)


if(useCompHits):
    folder = "compHits"
else:
    folder= "recHits"


inF = r.TFile("../data/%s/%s_resolutionPlots.root"%(folder, chamber))

patPos400 = inF.Get("patPos400")

htypes = ['patPos','patSlope']
patids = ['100','400','500','800','900']
hists = {htype:{patid:{} for patid in patids} for htype in htypes}
for htype in htypes:
    for patid in patids:
        hists[htype][patid] = inF.Get(htype+patid)
        
        
 # GET 3_WIDE PLOTS       
for htype in htypes:
    for patid in patids:
        c = r.TCanvas()
        c.cd()
        h = hists[htype][patid]
        h.SetTitle("Pattern %s"%(patid))
        h.GetXaxis().CenterTitle()
        h.GetYaxis().CenterTitle()
        range = h.GetXaxis().GetXmax() - h.GetXaxis().GetXmin()
        bins = h.GetNbinsX()
        
        if(htype == 'patPos') :
            h.GetYaxis().SetTitle("Segments / %0.2f [strips]"%(1.*range/bins))
        else:
            h.GetYaxis().SetTitle("Segments / %0.2f [strips/layer]"%(1.*range/bins))
        
        h.SetLineWidth(2)
        h.SetLineColor(9)
        h.Draw()
        r.gPad.Update()
        
        #stats box
        sbox = h.FindObject('stats')
        sbox.SetX1NDC(0.67)
        sbox.SetX2NDC(0.97)
        sbox.SetY1NDC(0.65)
        sbox.SetY2NDC(0.95)  
        
        c.SaveAs("%s.pdf"%(htype+patid))
        
        # GET ALL THE LEGACY PLOTS
ltypes = ['legacyPos', 'legacySlope']
lids = ['10','9','8','7','6','5','4','3','2']
lhists = {ltype:{patid:{} for patid in lids} for htype in ltypes}
for htype in ltypes:
    for patid in lids:
        hists[htype][patid] = inF.Get(htype+patid)
        
