#include <TFile.h>
#include <TString.h>
#include <TH1F.h>
#include <TH2F.h>
#include <assert.h>
#include <iostream>
class HistGetter {
    public:
        void book1D(const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup) {
            addHist(name);
            TH1 * h = new TH1F(name,title,nbinsx,xlow,xup);
            h->Sumw2();
            hists.push_back( h );
        }
        void book1D(const char *name, const char *title, Int_t nbinsx, const Float_t *xbins) {
            addHist(name);
            TH1 * h = new TH1F(name,title,nbinsx,xbins);
            h->Sumw2();
            hists.push_back(h );
        }
        void book1D(const char *name, const char *title, Int_t nbinsx, const Double_t *xbins) {
            addHist(name);
            TH1 * h = new TH1F(name,title,nbinsx,xbins);
            h->Sumw2();
            hists.push_back(h );
        }

        void book2D(const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup){
            addHist2D(name);
            TH2 * h = new TH2F(name,title,nbinsx,xlow,xup,nbinsy,ylow,yup);
            h->Sumw2();
            hist2Ds.push_back( h );
        }
        void book2D(const char *name, const char *title, Int_t nbinsx, const Double_t *xbins, Int_t nbinsy, Double_t ylow, Double_t yup){
            addHist2D(name);
            TH2 * h = new TH2F(name,title,nbinsx,xbins,nbinsy,ylow,yup);
            h->Sumw2();
            hist2Ds.push_back( h );
        }
        void book2D(const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, const Double_t *ybins){
            addHist2D(name);
            TH2 * h = new TH2F(name,title,nbinsx,xlow,xup,nbinsy,ybins);
            h->Sumw2();
            hist2Ds.push_back( h );
        }
        void book2D(const char *name, const char *title, Int_t nbinsx, const Double_t *xbins, Int_t nbinsy, const Double_t *ybins){
            addHist2D(name);
            TH2 * h = new TH2F(name,title,nbinsx,xbins,nbinsy,ybins);
            h->Sumw2();
            hist2Ds.push_back( h );
        }
        void book2D(const char *name, const char *title, Int_t nbinsx, const Float_t *xbins, Int_t nbinsy, const Float_t *ybins){
            addHist2D(name);
            TH2 * h = new TH2F(name,title,nbinsx,xbins,nbinsy,ybins) ;
            h->Sumw2();
            hist2Ds.push_back( h );
        }

        TH1 * get1D(const char *name) {
            auto it = histsMap.find(name);
            if (it != histsMap.end())  return hists[it->second];
            throw std::invalid_argument(TString::Format("You did not book a 1D histogram with name: %s",name));
        }
        TH2 * get2D(const char *name) {
            auto it = hist2DsMap.find(name);
            if (it != hist2DsMap.end())  return hist2Ds[it->second];
            throw std::invalid_argument(TString::Format("You did not book a 2D histogram with name: %s",name));
        }

        TH1 * get1D(unsigned int iH) {
            if(iH >= hists.size()) throw std::invalid_argument(TString::Format("You don't have this many 1D histograms: %u",iH));
            return hists[iH];
        }
        TH2 * get2D(unsigned int iH) {
            if(iH >= hist2Ds.size()) throw std::invalid_argument(TString::Format("You don't have this many 2D histograms: %u",iH));
            return hist2Ds[iH];
        }

        unsigned int getN1D() const {return hists.size();}
        unsigned int getN2D() const {return hist2Ds.size();}

        void write(TString filename){
            TFile * file = TFile::Open(filename,"RECREATE");
            file->cd();
            for(auto * h : hists) h->Write();
            for(auto * h : hist2Ds) h->Write();
            file->Close();
            delete file;
        }

    private:

        void addHist(std::string name){
            if(histsMap.count(name)){
                throw std::invalid_argument("You already tried to book a 1D histogram with name: " + name);
            }
            histsMap[name] = hists.size();
        }
        void addHist2D(std::string name){
            if(hist2DsMap.count(name)){
                throw std::invalid_argument("You already tried to book a 2D histogram with name: " + name);
            }
            hist2DsMap[name] = hist2Ds.size();
        }

        std::vector<TH1*> hists;
        std::vector<TH2*> hist2Ds;

        std::map<std::string,unsigned int> histsMap;
        std::map<std::string,unsigned int> hist2DsMap;
};

