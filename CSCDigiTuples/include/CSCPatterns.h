// user include files

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCCLCTDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCWireDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCStripDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCDDUStatusDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCDMBStatusDigiCollection.h>
#include <DataFormats/CSCDigi/interface/CSCTMBStatusDigiCollection.h>
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCChamber.h"
#include "Geometry/CSCGeometry/interface/CSCLayer.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "RecoMuon/TrackingTools/interface/MuonServiceProxy.h"
#include "RecoMuon/TrackingTools/interface/MuonSegmentMatcher.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenuFwd.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "CSCUCLA/CSCDigiTuples/include/MuonQualityCuts.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"

#include "DataFormats/CSCRecHit/interface/CSCSegment.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "TFile.h"
#include "TTree.h"
#include "TProfile2D.h"
#include "TNtuple.h"
#include "TLorentzVector.h"
#include <memory>
#include <vector>
#include <math.h>
#include "TH1F.h"

using namespace std;
using namespace edm;
using namespace reco;


class CSCPatterns : public edm::EDAnalyzer {
    public:
        explicit CSCPatterns(const edm::ParameterSet&);
        ~CSCPatterns();


    private:
        virtual void beginJob() ;
        virtual void analyze(const edm::Event&, const edm::EventSetup&);
        virtual void endJob() ;

        // ----------member data ---------------------------

        int chamberSerial( CSCDetId id );
        double FindAnodeTime(std::vector<CSCRecHit2D>::const_iterator  hiti,  const edm::Handle<CSCWireDigiCollection> cscWireDigi, double local_t0);

    private:

        edm::EDGetTokenT<reco::MuonCollection> mc_token;
        edm::EDGetTokenT<CSCWireDigiCollection> wd_token;
        edm::EDGetTokenT<CSCStripDigiCollection> sd_token;
        edm::EDGetTokenT<CSCALCTDigiCollection> ad_token;
        edm::EDGetTokenT<CSCCLCTDigiCollection> cd_token;
        edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> ld_token;
        edm::EDGetTokenT<CSCComparatorDigiCollection> cod_token;
        edm::EDGetTokenT<reco::BeamSpot> obs_token;
        edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> csctflct_token;
        edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> emtflct_token;
        edm::EDGetTokenT<CSCDDUStatusDigiCollection> ddu_token;
        edm::EDGetTokenT<CSCDMBStatusDigiCollection> dmb_token;
        edm::EDGetTokenT<CSCTMBStatusDigiCollection> tmb_token;

        const CSCGeometry *theCSC;
        MuonServiceProxy *theService;
        MuonSegmentMatcher *theMatcher;
        MuonQualityCuts *muonQualityCuts;
        double minPt;
        edm::InputTag CSCSegmentTags_;

        int evN;

        int Event_EventNumber;
        int Event_RunNumber;
        int Event_LumiSection;
        int Event_BXCrossing;

        bool ss;
        bool os;
        double Pt;
        double eta;
        double phi;
        int q;

        //Segment data
        int Nseg;
        vector<int> segEc;
        vector<int> segSt;
        vector<int> segRi;
        vector<int> segCh;
        vector<float> segX;
        vector<float> segY;
        vector<float> segdXdZ;
        vector<float> segdYdZ;

        //RecHit data
        vector<int> rhId;
        vector<int> rhLay;
        vector<float> rhPos;
        vector<float> rhE;
        vector<float> rhMax;

        //LCT data
        vector<int> lctId;
        vector<vector<int>> lctQ;
        vector<vector<int>> lctPat;
        vector<vector<int>> lctKWG;
        vector<vector<int>> lctKHS;
        vector<vector<int>> lctBend;
        vector<vector<int>> lctBX;

        //csctfLCT data
        vector<int> csctflctId;
        vector<vector<int>> csctflctQ;
        vector<vector<int>> csctflctPat;
        vector<vector<int>> csctflctKWG;
        vector<vector<int>> csctflctKHS;
        vector<vector<int>> csctflctBend;
        vector<vector<int>> csctflctBX;

        //emtfLCT data
        vector<int> emtflctId;
        vector<vector<int>> emtflctQ;
        vector<vector<int>> emtflctPat;
        vector<vector<int>> emtflctKWG;
        vector<vector<int>> emtflctKHS;
        vector<vector<int>> emtflctBend;
        vector<vector<int>> emtflctBX;

        //CLCT data
        vector<int> clctId;
        vector<vector<int>> clctQ;
        vector<vector<int>> clctPat;
        vector<vector<int>> clctKHS;
        vector<vector<int>> clctCFEB;
        vector<vector<int>> clctBend;
        vector<vector<int>> clctBX;
        vector<vector<int>> clctFBX;

        //ALCT data
        vector<int> alctId;
        vector<vector<int>> alctQ;
        vector<vector<int>> alctKWG;
        vector<vector<int>> alctAc;
        vector<vector<int>> alctPB;
        vector<vector<int>> alctBX;
        vector<vector<int>> alctFBX;

        //Comparator data
        vector<int> compId;
        vector<int> compLay;
        vector<vector<int>> compStr;
        vector<vector<int>> compHS;
        vector<vector<vector<int>>> compTimeOn;

        //Wire Data
        vector<int> wireId;
        vector<int> wireLay;
        vector<vector<int>> wireGrp;
        vector<vector<vector<int> > > wireTimeOn;

        //Strip Data
        vector<int> stripId;
        vector<int> stripLay;
        vector<vector<int>> strip;
        vector<vector<vector<int>>> stripADCs;

        //DDU Status Data
        vector<int> dduId;
        vector<vector<int>> dduHeader;
        vector<vector<int>> dduTrailer;

        //DMB Status Data
        vector<int> dmbId;
        vector<vector<int>> dmbHeader;
        vector<vector<int>> dmbTrailer;

        //TMB Status Data
        vector<int> tmbId;
        vector<vector<int>> tmbHeader;
        vector<vector<int>> tmbTrailer;

        // double anodeTime;
        //    double stripTime;
        //    double wireTime;
        string filename;
        edm::EDGetTokenT<CSCSegmentCollection> allSegmentsCSCToken;

        TTree *tree;
        TH1F * hist;
        TH1F * ptmuon;
        TH1F * ptmu1;
        TH1F * ptmu2;
        TH1F * dimuonMos;
        TH1F * dimuonMss;
        TH1F * dimuon3M;
        TH1F * dimuonMos_1GS;
        TH1F * dimuon3M_1GS;
        TH1F * dimuonMos_1Gl;
        TH1F * dimuon3M_1Gl;
        TH1F * dimuonMos_2Gl;
        TH1F * dimuon3M_2Gl;
        TH1F * dimuonMos_1SA;
        TH1F * dimuon3M_1SA;
        TH1F * dimuonMos_2SA;
        TH1F * dimuon3M_2SA;
        TH1F * etamuon;
        TH1F * ptsamuon;
        TH1F * Nmuon_h;
        TH1F * NSelectedMuons_h;
        TH1F * NSegments_h_Test;
        TH1F * NSegments_h;
        TH1F * chambernumber;
        TH1F* Events_h;
        TH1F* OSorSS_h;
        TH1F* inMass_h;
        TFile *file;

};

