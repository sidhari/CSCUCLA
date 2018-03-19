import ROOT as r


useCompHits = 0 # 0 means use recHits
chamber = "All-Chambers"
newPattColor = 9
legacyColor = 46
ccColor = 1


#plot formatting
r.gStyle.SetPadTickX(1)
r.gStyle.SetPadTickY(1)
r.gStyle.SetStatFontSize(60)
r.gStyle.SetPadLeftMargin(0.11)


if(useCompHits):
    folder = "compHits"
else:
    folder= "recHits"
    
path = "/Users/williamnash/CSCPatterns/img/"+ folder + "/fancyPlots/"


inF = r.TFile("../data/%s/%s_resolutionPlots.root"%(folder, chamber))

patPos400 = inF.Get("patPos400")

htypes = ['patPos','patSlope']
patids = ['100','400','500','800','900']
hists = {htype:{patid:{} for patid in patids} for htype in htypes}
cchists = {htype:{patid:{} for patid in patids} for htype in htypes}
for htype in htypes:
    for patid in patids:
        hists[htype][patid] = inF.Get(htype+patid)
        cchists[htype][patid] = inF.Get(htype+patid + "_fullCCRes")
        
        
 # GET 3_WIDE PLOTS  
for htype in htypes:
    for patid in patids:
        r.gStyle.SetOptStat(111110)
        c = r.TCanvas()
        c.cd()
        h = hists[htype][patid]
        h.SetTitle("New Pattern %s"%(patid))
        h.GetXaxis().CenterTitle()
        h.GetYaxis().CenterTitle()
        range = h.GetXaxis().GetXmax() - h.GetXaxis().GetXmin()
        bins = h.GetNbinsX()
        
        if(htype == 'patPos') :
            h.GetYaxis().SetTitle("Segments / %0.2f [strips]"%(1.*range/bins))
        else:
            h.GetYaxis().SetTitle("Segments / %0.2f [strips/layer]"%(1.*range/bins))
        
        h.SetLineWidth(2)
        h.SetLineColor(newPattColor)
        h.Draw()
        r.gPad.Update()
        
        #stats box
        sbox = h.FindObject('stats')
        sbox.SetX1NDC(0.67)
        sbox.SetX2NDC(0.97)
        sbox.SetY1NDC(0.65)
        sbox.SetY2NDC(0.95)
        sbox.SetTextColor(newPattColor)
        
        c.SaveAs(path+"%s.pdf"%(htype+patid))
        r.gStyle.SetOptStat(111111)
        
        h2 = cchists[htype][patid]
        if(htype == 'patPos'):
            h2.SetTitle("Pattern %s CC Position Resolution"%patid)
        else: 
            h2.SetTitle("Pattern %s CC Slope Resolution"%patid)
        h2.SetName("Comparator Code")
        h2.GetXaxis().CenterTitle()
        h2.GetYaxis().CenterTitle()
        h2.SetLineWidth(2)
        h2.SetLineColor(ccColor)
        if(htype == 'patPos') :
            h2.GetYaxis().SetTitle("Segments / %0.2f [strips]"%(1.*range/bins))
        else:
            h2.GetYaxis().SetTitle("Segments / %0.2f [strips/layer]"%(1.*range/bins))
        c2 = r.TCanvas()
        c2.cd()
        h2.Draw()
        r.gPad.Update()
        sbox2 = h2.FindObject('stats')
        sbox2.SetX1NDC(0.67)
        sbox2.SetX2NDC(0.97)
        sbox2.SetY1NDC(0.63)
        sbox2.SetY2NDC(0.93)
        sbox2.SetTextColor(ccColor)
        h.SetName("3-Wide Patterns")
        h.Draw("sames")
        r.gPad.Update()
        sbox3 = h.FindObject('stats')
        sbox3.SetX1NDC(0.67)
        sbox3.SetX2NDC(0.97)
        sbox3.SetY1NDC(0.33)
        sbox3.SetY2NDC(0.63)
        sbox3.SetTextColor(newPattColor)
        c2.SaveAs(path+"%sandCC_%s.pdf"%(htype, patid))
        

# GET ALL THE LEGACY PLOTS
r.gStyle.SetOptStat(111110)
ltypes = ['legacyPos', 'legacySlope']
lids = ['10','9','8','7','6','5','4','3','2']
lhists = {ltype:{patid:{} for patid in lids} for ltype in ltypes}
for htype in ltypes:
    for patid in lids:
        lhists[htype][patid] = inF.Get(htype+patid)
        
for htype in ltypes:
    for patid in lids:
        c = r.TCanvas()
        c.cd()
        h = lhists[htype][patid]
        h.SetTitle("Legacy Pattern %s"%(patid))
        h.GetXaxis().CenterTitle()
        h.GetYaxis().CenterTitle()
        range = h.GetXaxis().GetXmax() - h.GetXaxis().GetXmin()
        bins = h.GetNbinsX()
        
        if(htype == 'legacyPos') :
            h.GetYaxis().SetTitle("Segments / %0.2f [strips]"%(1.*range/bins))
        else:
            h.GetYaxis().SetTitle("Segments / %0.2f [strips/layer]"%(1.*range/bins))
        
        h.SetLineWidth(2)
        h.SetLineColor(legacyColor)
        h.Draw()
        r.gPad.Update()
        
        #stats box
        sbox = h.FindObject('stats')
        sbox.SetX1NDC(0.67)
        sbox.SetX2NDC(0.97)
        sbox.SetY1NDC(0.65)
        sbox.SetY2NDC(0.95) 
        sbox.SetTextColor(legacyColor) 
        
        c.SaveAs(path+"%s.pdf"%(htype+patid))
        
        
# COMBINED DISTRIBUTIONS
r.gStyle.SetOptStat(111111)

ctypes = ['CC', 'Leg']
disttypes = ['Slope', 'Pos']

chists = {ctype:{dist:{} for dist in disttypes} for ctype in ctypes}

for ctype in ctypes:
    for dist in disttypes:
        chists[ctype][dist] = inF.Get("cum"+ctype+dist+"Res")


   
for dist in disttypes:
    c = r.TCanvas()
    c.cd() 
    split = 0.2 #how far apart stat boxes should be     
    for ctype in ctypes:
           h = chists[ctype][dist]
           h.GetXaxis().CenterTitle()
           h.GetYaxis().CenterTitle()
           h.SetLineWidth(2)
           range = h.GetXaxis().GetXmax() - h.GetXaxis().GetXmin()
           bins = h.GetNbinsX()
           if(dist == 'Pos'):
               h.GetYaxis().SetTitle("Segments / %0.2f [strips]"%(1.*range/bins))
           else:
               h.GetYaxis().SetTitle("Segments / %0.2f [strips/layer]"%(1.*range/bins))
            
           h.SetLineWidth(2)
           color = 0
           
           if(ctype == "CC"):
               #if(dist == 'Pos'):
               #    h.SetTitle("Full Position Resolution")
               #else:
               #    h.SetTitle("Full Slope Resolution")
               h.SetTitle("")
               h.SetName("Comparator Code")
               color = ccColor
               h.SetLineColor(ccColor)
               h.SetStats(1)
               h.Draw()
           elif(ctype == "Leg"):
               h.SetLineColor(legacyColor)
               color = legacyColor
               h.SetName("Current Patterns")
               h.Draw("sames")
           #else:
           #    h.SetLineColor(newPattColor)
           #    color = newPattColor
           #    h.SetName("3-Wide Patterns")
           #    h.Draw("sames")
           r.gPad.Update()
           sbox = h.FindObject('stats')
           sbox.SetX1NDC(0.67)
           sbox.SetX2NDC(0.97)
           sbox.SetY1NDC(0.25+split)
           sbox.SetY2NDC(0.45+split)
           sbox.SetTextColor(color)
           split += 0.2
           
    
    c.SaveAs(path+"full_%s.pdf"%(dist))
    

c = r.TCanvas()
c.cd()
h2 = inF.Get("cumLegPosRes")
h2.GetXaxis().CenterTitle()
h2.GetYaxis().CenterTitle()
h2.GetYaxis().SetTitle("Segments / %0.2f [strips]"%(1.*range/bins))
h2.SetLineWidth(2)
h2.SetLineColor(legacyColor)
h2.SetName("Legacy Patterns")
h2.SetTitle("")
h2.Draw()

split = 0
r.gPad.Update()
sbox2 = h2.FindObject('stats')
sbox2.SetX1NDC(0.67)
sbox2.SetX2NDC(0.97)
sbox2.SetY1NDC(0.45+split)
sbox2.SetY2NDC(0.65+split)
split += 0.2
sbox2.SetTextColor(legacyColor)


h1 = inF.Get("cumPatPosRes")
h1.GetXaxis().CenterTitle()
range = h1.GetXaxis().GetXmax() - h1.GetXaxis().GetXmin()
bins = h1.GetNbinsX()
h1.GetYaxis().SetTitle("Segments / %0.2f [strips]"%(1.*range/bins))
h1.GetYaxis().CenterTitle()
h1.SetLineWidth(2)
h1.SetLineColor(newPattColor)
h1.SetName("3-Wide Patterns")
h1.Draw("sames")

r.gPad.Update()
sbox = h1.FindObject('stats')
sbox.SetX1NDC(0.67)
sbox.SetX2NDC(0.97)
sbox.SetY1NDC(0.45+split)
sbox.SetY2NDC(0.65+split)
sbox.SetTextColor(newPattColor)





c.SaveAs(path+"pattAndLegacy.pdf")   
