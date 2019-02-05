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
#include <DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h>
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
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"

#include "DataFormats/CSCRecHit/interface/CSCSegment.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CSCUCLA/CSCDigiTuples/include/FillCSCInfo.h"
#include "CSCUCLA/CSCDigiTuples/include/CSCHelper.h"

//temp
#include "Geometry/EcalAlgo/interface/EcalEndcapGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"


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

class  CSCPatternExtractor : public edm::EDAnalyzer {
    public:
        explicit CSCPatternExtractor(const edm::ParameterSet&);
        ~CSCPatternExtractor();


    private:
        virtual void beginJob();
        virtual void analyze(const edm::Event&, const edm::EventSetup&);
        virtual void endJob();


        const reco::MuonCollection (*selectMuons)(const reco::MuonCollection& m,const reco::Vertex& vtx, TreeContainer& t);

        //TODO: could come up with a nicer way to pass arguments, static definition is tricky...
        static const reco::MuonCollection selectResonanceMuons(const reco::MuonCollection& m, const reco::Vertex& vtx, TreeContainer& t, const float minPt, const float resMass, const float massWindow);
        static const reco::MuonCollection selectSingleMuMuons(const reco::MuonCollection& m, const reco::Vertex& vtx, TreeContainer& t);
        static const reco::MuonCollection selectJPsiMuons(const reco::MuonCollection& m, const reco::Vertex& vtx, TreeContainer& t);
        static const reco::MuonCollection selectStandaloneMuons(const reco::MuonCollection& m, const reco::Vertex& vtx, TreeContainer& t);
        static const reco::MuonCollection selectDisplacedMuons(const reco::MuonCollection& m,const reco::Vertex& vtx, TreeContainer& t);

        vector<const CSCSegment*> matchCSC(const reco::Track& muon, edm::Handle<CSCSegmentCollection> allSegmentsCSC);
        bool cscTightMatch;

    private:

        edm::EDGetTokenT<reco::MuonCollection> mc_token;
        edm::EDGetTokenT<CSCWireDigiCollection> wd_token;
        edm::EDGetTokenT<CSCStripDigiCollection> sd_token;
        edm::EDGetTokenT<CSCALCTDigiCollection> ad_token;
        edm::EDGetTokenT<CSCCLCTDigiCollection> cd_token;
        edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> ld_token;
        edm::EDGetTokenT<CSCComparatorDigiCollection> cod_token;
        edm::EDGetTokenT<reco::BeamSpot> obs_token;
        edm::EDGetTokenT<reco::VertexCollection> vtx_token;
        edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> csctflct_token;
        edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> emtflct_token;
        edm::EDGetTokenT<CSCDDUStatusDigiCollection> ddu_token;
        edm::EDGetTokenT<CSCDMBStatusDigiCollection> dmb_token;
        edm::EDGetTokenT<CSCTMBStatusDigiCollection> tmb_token;
        edm::EDGetTokenT<CSCRecHit2DCollection> rh_token;
        edm::EDGetTokenT<vector<reco::PFCandidate>> pf_token;
        //gen
        edm::EDGetTokenT<vector<reco::GenParticle>> gen_token;
        edm::EDGetTokenT<vector<PSimHit>> sim_token;
        edm::EDGetTokenT<vector<PCaloHit>> eeCalo_token;
        edm::EDGetTokenT<vector<PCaloHit>> hCalo_token;

        const CSCGeometry *theCSC;
        const EcalEndcapGeometry* theEcal;
        const HcalGeometry* theHcal;

        MuonServiceProxy *theService;
        MuonQualityCuts *muonQualityCuts;
        string selection;
        edm::InputTag CSCSegmentTags_;


        TreeContainer tree;


        FillEventInfo eventInfo;
        FillMuonInfo muonInfo;
        FillSegmentInfo segmentInfo;
        FillRecHitInfo recHitInfo;
        FillLCTInfo lctInfo;
        FillCLCTInfo clctInfo;
        FillCompInfo compInfo;
        FillPFInfo pfInfo;
        //gen
        FillGenParticleInfo genInfo;
        FillSimHitsInfo  simHitInfo;
        FillCaloHitsInfo eeCaloInfo;
        FillCaloHitsInfo hCaloInfo;

        edm::EDGetTokenT<CSCSegmentCollection> allSegmentsCSCToken;

        //should be member of CSCRecHit2D, but...
        static bool areEqual(const CSCRecHit2D& rh1, const CSCRecHit2D& rh2);


};

