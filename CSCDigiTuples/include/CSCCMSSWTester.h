/*
 * CSCCMSSWTester.h
 *
 *  Created on: Jul 22, 2019
 *      Author: wnash
 */

//#ifndef CSCDIGITUPLES_INCLUDE_CSCCMSSWTESTER_H_
//#define CSCDIGITUPLES_INCLUDE_CSCCMSSWTESTER_H_

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//#include "CSCUCLA/CSCDigiTuples/include/CSCHelper.h"
//#include "CSCUCLA/CSCDigiTuples/include/FillCSCInfo.h"


class CSCCMSSWTester : public edm::EDAnalyzer {
public:
	explicit CSCCMSSWTester(const edm::ParameterSet&);
	~CSCCMSSWTester();
private:
    virtual void beginJob();
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob();

    /* TODO
    edm::EDGetTokenT<CSCCLCTDigiCollection> cd_token;
    edm::EDGetTokenT<CSCComparatorDigiCollection> cod_token;

    TreeContainer tree;

    FillCLCTInfo clctInfo;
    FillCompInfo compInfo;
    */
};


//#endif /* CSCDIGITUPLES_INCLUDE_CSCCMSSWTESTER_H_ */
