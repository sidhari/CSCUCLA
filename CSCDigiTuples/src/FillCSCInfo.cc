#include "CSCUCLA/CSCDigiTuples/include/FillCSCInfo.h"
#include "CSCUCLA/CSCDigiTuples/include/CSCHelper.h"

#include "DataFormats/MuonReco/interface/Muon.h"

void TreeContainer::fill(){
	  tree->Fill();
	  for(auto& info: infos) info->reset();
}

void FillEventInfo::fill(const edm::Event& iEvent){
  EventNumber     = iEvent.id().event();
  RunNumber       = iEvent.id().run();
  LumiSection     = iEvent.eventAuxiliary().luminosityBlock();
  BXCrossing      = iEvent.eventAuxiliary().bunchCrossing();
}


void FillMuonInfo::fill(const reco::MuonCollection& muons){
	for(const auto& muon: muons){
		pt->push_back(muon.pt());
		eta->push_back(muon.eta());
		phi->push_back(muon.phi());
		q->push_back(muon.charge());
		isGlobal->push_back(muon.isGlobalMuon());
		isTracker->push_back(muon.isTrackerMuon());
	}
}

size16 FillSegmentInfo::findRecHitIdx(const CSCRecHit2D& hit, const CSCRecHit2DCollection* allRecHits){
  int idx = -1;
  int foundIDX = -1;
  for (CSCRecHit2DCollection::const_iterator hiti=allRecHits->begin(); hiti!=allRecHits->end(); hiti++)
  {
    idx++;
    if(!hit.sharesInput(&(*hiti),CSCRecHit2D::all)) continue;
    foundIDX = idx;
    break;
  }
  if(foundIDX < 0) throw std::invalid_argument("FillSegmentInfo::findRecHitIdx -> Could not find rechit");
  return CSCHelper::convertTo<size16,int>(foundIDX,"foundIDX");
}


void FillSegmentInfo::fill(std::vector<const CSCSegment*>& segments, const CSCGeometry* theCSC, int mu_index){
  for(auto dSiter=segments.begin(); dSiter != segments.end(); dSiter++) {
	 fill(**dSiter, theCSC, mu_index);

  }
}

void FillSegmentInfo::fill(const CSCSegment& segment, const CSCGeometry* theCSC, int mu_index){

	DetId detId  = segment.geographicalId();
	CSCDetId id(detId.rawId());

	const auto& segmentHits = segment.specificRecHits();
	//const auto& segmentHits = dSiter->specificRecHits();
	//inherited from C. Bravo - translates [cm] -> [strips]
	const CSCChamber *segChamber = theCSC->chamber(id);
	const CSCLayer *segLay3 = segChamber->layer(3);
	const CSCLayer *segLay4 = segChamber->layer(4);
	const CSCLayerGeometry *segLay3Geo = segLay3->geometry();
	LocalPoint segLPlayer = segLay3->toLocal(segChamber->toGlobal(segment.localPosition()));
	LocalVector segLVlayer = segLay3->toLocal(segChamber->toGlobal(segment.localDirection()));
	float scale = -1.0*segLPlayer.z()/segLVlayer.z();
	LocalVector tV = scale*segLVlayer;
	LocalPoint tP = segLPlayer + tV;
	float segStr = segLay3Geo->strip(tP);
	int strI = floor(segStr);
	float cm2strip = fabs( segLay3Geo->xOfStrip(strI,tP.y()) - segLay3Geo->xOfStrip(strI+1,tP.y()) );

	LocalPoint lzero(0.0,0.0,0.0);
	GlobalPoint lay4zero = segLay4->toGlobal(lzero);
	LocalPoint lay4zeroIn3 = segLay3->toLocal(lay4zero);
	float cm2lay = fabs(lay4zeroIn3.z());


	//fill everything
	mu_id->push_back(mu_index);
	ch_id->push_back(CSCHelper::serialize(id.station(), id.ring(), id.chamber(), id.endcap()));
	pos_x->push_back(segLay3Geo->strip(tP));
	pos_y->push_back(segment.localPosition().y());
	dxdz->push_back(segment.localDirection().x() / cm2strip / ( segment.localDirection().z() / cm2lay));
	dydz->push_back(segment.localDirection().y() / segment.localDirection().z());
	chisq->push_back(segment.chi2());
	nHits->push_back(CSCHelper::convertTo<size8>(segmentHits.size(), "segment_nHits"));

}



void FillRecHitInfo::fill(const std::vector<CSCRecHit2D>& recHits, int mu_index){
  for (const auto& rechit : recHits){

      DetId detId = rechit.geographicalId();
      CSCDetId id(detId.rawId());

      int centerID = rechit.nStrips()/2;
      int centerStr = rechit.channels(centerID);

      float rhMaxBuf = -999.0;
      for(int tI = 0; tI < int(rechit.nTimeBins()); tI++)
      {
    	  if(rechit.adcs(centerID,tI) > rhMaxBuf) rhMaxBuf = rechit.adcs(centerID,tI);
      }

      mu_id->push_back(mu_index);
      ch_id->push_back(CSCHelper::serialize(id.station(), id.ring(), id.chamber(), id.endcap()));
      lay->push_back(CSCHelper::convertTo<size8>(id.layer(),"rh_lay"));
      pos_x->push_back(float(centerStr) + rechit.positionWithinStrip());
      pos_y->push_back(-1); //NOT IMPLEMENTED
      e->push_back(rechit.energyDepositedInLayer());
      max_adc->push_back(rhMaxBuf);
  }
}




void FillLCTInfo::fill(const CSCCorrelatedLCTDigiCollection& lcts){
  for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator chamber=lcts.begin(); chamber!=lcts.end(); chamber++)
  {
    CSCDetId id = (*chamber).first;
    const CSCCorrelatedLCTDigiCollection::Range& range =(*chamber).second;
    for(CSCCorrelatedLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
    {
      ch_id       ->push_back(CSCHelper::serialize(id.station(), id.ring(), id.chamber(), id.endcap()));
      quality     ->push_back(CSCHelper::convertTo<size8>(digiItr->getQuality(),"lct_quality"));
      pattern     ->push_back(CSCHelper::convertTo<size8>(digiItr->getPattern(),"lct_pattern"));
      bend        ->push_back(CSCHelper::convertTo<size8>(digiItr->getBend()   ,"lct_bend"));
      keyWireGroup->push_back(CSCHelper::convertTo<size8>(digiItr->getKeyWG()  ,"lct_keyWireGroup"));
      keyHalfStrip->push_back(CSCHelper::convertTo<size8>(digiItr->getStrip()  ,"lct_keyHalfStrip"));
      }
  }
}



void FillCLCTInfo::fill(const CSCCLCTDigiCollection& clcts) {
	for (CSCCLCTDigiCollection::DigiRangeIterator chamber = clcts.begin();
			chamber != clcts.end(); chamber++) {
		CSCDetId id = (*chamber).first;
		const CSCCLCTDigiCollection::Range& range = (*chamber).second;
		for (CSCCLCTDigiCollection::const_iterator digiItr = range.first;
				digiItr != range.second; ++digiItr) {

			ch_id->push_back(CSCHelper::serialize(id.station(), id.ring(), id.chamber(), id.endcap()));
			isValid->push_back(
				CSCHelper::convertTo<size8>(digiItr->isValid(),
						"clct_isvalid"));
			quality->push_back(
				CSCHelper::convertTo<size8>(digiItr->getQuality(),
						"clct_quality"));
			pattern->push_back(
				CSCHelper::convertTo<size8>(digiItr->getPattern(),
						"clct_pattern"));
			stripType->push_back(
				CSCHelper::convertTo<size8>(digiItr->getStripType(),
						"clct_stripType"));
			bend->push_back(
				CSCHelper::convertTo<size8>(digiItr->getBend(),
						"clct_bend"));
			halfStrip->push_back(
				CSCHelper::convertTo<size8>(digiItr->getStrip(),
						"clct_halfStrip"));
			CFEB->push_back(
				CSCHelper::convertTo<size8>(digiItr->getCFEB(),
						"clct_CFEB"));
			BX->push_back(
				CSCHelper::convertTo<size8>(digiItr->getBX(), "clct_BX"));
			trkNumber->push_back(
				CSCHelper::convertTo<size8>(digiItr->getTrknmb(),
						"clct_trkNumber"));
			keyStrip->push_back(
					CSCHelper::convertTo<size8>(digiItr->getKeyStrip(),
							"clct_keyStrip"));
		}

	}
}


void FillCompInfo::fill(const CSCComparatorDigiCollection& comps){
	//cout << "filling comp hits..." <<endl;
  for (CSCComparatorDigiCollection::DigiRangeIterator chamber=comps.begin(); chamber!=comps.end(); chamber++)
  {
    CSCDetId id = (*chamber).first;
	  //cout << id << endl;
    const CSCComparatorDigiCollection::Range& range =(*chamber).second;
    for(CSCComparatorDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
    {
    	//cout << "test" << endl;
			ch_id->push_back(CSCHelper::serialize(id.station(), id.ring(), id.chamber(), id.endcap()));
			lay->push_back(
				CSCHelper::convertTo<size8>(id.layer(), "comp_lay"));
			strip->push_back(
				CSCHelper::convertTo<size8>((*digiItr).getStrip(),
						"comp_strip"));
			halfStrip->push_back(
				CSCHelper::convertTo<size8>((*digiItr).getComparator(),
						"comp_comp"));
			nTimeOn->push_back(
					CSCHelper::convertTo<size8>((*digiItr).getTimeBinsOn().size(),
							"comp_nTimeOn"));
			unsigned int bestTimeBin = 0;
			for(const auto& time : (*digiItr).getTimeBinsOn()){
				bestTimeBin = time;
				if(time < 10 && time > 5) {
					break;
				}
			}
			bestTime->push_back(CSCHelper::convertTo<size8>(bestTimeBin, "comp_bestTime"));

		}
	}

}


/*
void FillStripInfo::fill(const CSCStripDigiCollection& strips){
  reset();
  for (CSCStripDigiCollection::DigiRangeIterator dSDiter=strips.begin(); dSDiter!=strips.end(); dSDiter++) {
    CSCDetId id = (CSCDetId)(*dSDiter).first;

    std::vector<CSCStripDigi>::const_iterator stripIter = (*dSDiter).second.first;
    std::vector<CSCStripDigi>::const_iterator lStrip = (*dSDiter).second.second;
    for( ; stripIter != lStrip; ++stripIter) {
      std::vector<int> myADCVals = stripIter->getADCCounts();
      bool thisStripFired = false;
      float thisPedestal = 0.5*(float)(myADCVals[0]+myADCVals[1]);
      float threshold = 13.3 ;
      float diff = 0.;
      for (unsigned int iCount = 0; iCount < myADCVals.size(); iCount++) {
        diff = (float)myADCVals[iCount]-thisPedestal;
        if (diff > threshold) { thisStripFired = true; }
      }
      if(!thisStripFired) continue;

      strip_id.push_back(CSCHelper::chamberSerial(id));
      strip_lay.push_back(CSCHelper::convertTo<size8>(id.layer(),"strip_lay"));
      strip_number.push_back(CSCHelper::convertTo<size8>(stripIter->getStrip(),"strip_number"));
    }
  } // end strip loop
}

void FillCompInfo::fill(const CSCComparatorDigiCollection& comps){
  reset();
  for (CSCComparatorDigiCollection::DigiRangeIterator chamber=comps.begin(); chamber!=comps.end(); chamber++)
  {
    CSCDetId id = (*chamber).first;

    const CSCComparatorDigiCollection::Range& range =(*chamber).second;
    for(CSCComparatorDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
    {
      comp_id    .push_back(CSCHelper::chamberSerial(id));
      comp_lay   .push_back(CSCHelper::convertTo<size8>(id.layer(),"comp_lay"));
      comp_strip .push_back(CSCHelper::convertTo<size8>((*digiItr).getStrip(),"comp_strip"));
      comp_comp  .push_back(CSCHelper::convertTo<size8>((*digiItr).getComparator(),"comp_comp"));
    }
  }

}

void FillWireInfo::fill(const CSCWireDigiCollection& wires){
  reset();
  for (CSCWireDigiCollection::DigiRangeIterator chamber=wires.begin(); chamber!=wires.end(); chamber++)
  {
    CSCDetId id = (*chamber).first;
    int layer = id.layer();
      const CSCWireDigiCollection::Range& range =(*chamber).second;
      for(CSCWireDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
      {
        wire_id  .push_back(CSCHelper::chamberSerial(id));
        wire_lay .push_back(CSCHelper::convertTo<size8>(layer,"wire_lay"));
        wire_grp .push_back(CSCHelper::convertTo<size8>((*digiItr).getWireGroup(),"wire_grp"));
        wire_time.push_back(CSCHelper::convertTo<size8>((*digiItr).getTimeBin(),"wire_time"));
        wire_bx.push_back(CSCHelper::convertTo<int>((*digiItr).getWireGroupBX(),"wire_bx"));
      }
  }


}




void FillLCTInfo::fill(const CSCCorrelatedLCTDigiCollection& lcts){
  reset();
  for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator chamber=lcts.begin(); chamber!=lcts.end(); chamber++)
  {
    CSCDetId id = (*chamber).first;
    const CSCCorrelatedLCTDigiCollection::Range& range =(*chamber).second;
    for(CSCCorrelatedLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
    {
      lct_id          .push_back(CSCHelper::chamberSerial(id));
      lct_quality     .push_back(CSCHelper::convertTo<size8>(digiItr->getQuality(),"lct_quality"));
      lct_pattern     .push_back(CSCHelper::convertTo<size8>(digiItr->getPattern(),"lct_pattern"));
      lct_bend        .push_back(CSCHelper::convertTo<size8>(digiItr->getBend()   ,"lct_bend"));
      lct_keyWireGroup.push_back(CSCHelper::convertTo<size8>(digiItr->getKeyWG()  ,"lct_keyWireGroup"));
      lct_keyHalfStrip.push_back(CSCHelper::convertTo<size8>(digiItr->getStrip()  ,"lct_keyHalfStrip"));
      }
  }
}


void FillCLCTInfo::fill(const CSCCLCTDigiCollection& clcts){
  reset();

  for(CSCCLCTDigiCollection::DigiRangeIterator chamber=clcts.begin(); chamber != clcts.end(); chamber++) {
    CSCDetId id = (*chamber).first;
    const CSCCLCTDigiCollection::Range& range =(*chamber).second;
    for(CSCCLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
    {

      clct_id         .push_back(CSCHelper::chamberSerial(id));
      clct_isvalid    .push_back(CSCHelper::convertTo<size8>(digiItr->isValid()  ,"clct_isvalid"  ));
      clct_quality    .push_back(CSCHelper::convertTo<size8>(digiItr->getQuality()  ,"clct_quality"  ));
      clct_pattern    .push_back(CSCHelper::convertTo<size8>(digiItr->getPattern()  ,"clct_pattern"  ));
      clct_stripType  .push_back(CSCHelper::convertTo<size8>(digiItr->getStripType()  ,"clct_stripType"));
      clct_bend       .push_back(CSCHelper::convertTo<size8>(digiItr->getBend()  ,"clct_bend"     ));
      clct_halfStrip  .push_back(CSCHelper::convertTo<size8>(digiItr->getStrip()  ,"clct_halfStrip"));
      clct_CFEB       .push_back(CSCHelper::convertTo<size8>(digiItr->getCFEB()  ,"clct_CFEB"     ));
      clct_BX         .push_back(CSCHelper::convertTo<size8>(digiItr->getBX()  ,"clct_BX"       ));
      clct_trkNumber  .push_back(CSCHelper::convertTo<size8>(digiItr->getTrknmb()  ,"clct_trkNumber"));
      clct_keyStrip   .push_back(CSCHelper::convertTo<size8>(digiItr->getKeyStrip()  ,"clct_keyStrip" ));
    }

}
}


void FillALCTInfo::fill(const CSCALCTDigiCollection& alcts){
  reset();

  for(CSCALCTDigiCollection::DigiRangeIterator chamber=alcts.begin(); chamber != alcts.end(); chamber++) {
    CSCDetId id = (*chamber).first;
    const CSCALCTDigiCollection::Range& range =(*chamber).second;
    for(CSCALCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
    {

      alct_id         .push_back(CSCHelper::chamberSerial(id));
      alct_isvalid    .push_back(CSCHelper::convertTo<size8>(digiItr->isValid()  ,"alct_isvalid"  ));
      alct_quality    .push_back(CSCHelper::convertTo<size8>(digiItr->getQuality()  ,"alct_quality"  ));
      alct_accel      .push_back(CSCHelper::convertTo<size8>(digiItr->getAccelerator()  ,"alct_accel"  ));
      alct_collB      .push_back(CSCHelper::convertTo<size8>(digiItr->getCollisionB()  ,"alct_collB"  ));
      alct_wireGroup  .push_back(CSCHelper::convertTo<size8>(digiItr->getKeyWG()  ,"alct_wireGroup"));
      alct_BX         .push_back(CSCHelper::convertTo<size8>(digiItr->getBX()  ,"alct_BX"       ));
      alct_trkNumber  .push_back(CSCHelper::convertTo<size8>(digiItr->getTrknmb()  ,"alct_trkNumber"));
    }

}
}
*/
