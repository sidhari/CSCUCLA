#include "CSCUCLA/CSCDigiTuples/include/FillCSCInfo.h"
#include "CSCUCLA/CSCDigiTuples/include/CSCHelper.h"

void FillEventInfo::fill(const edm::Event& iEvent){
  reset();
  EventNumber     = iEvent.id().event();
  RunNumber       = iEvent.id().run();
  LumiSection     = iEvent.eventAuxiliary().luminosityBlock();
  BXCrossing      = iEvent.eventAuxiliary().bunchCrossing();
}

/*
void FillRecHitInfo::fill(const CSCRecHit2DCollection& recHits){
  reset();
  for (CSCRecHit2DCollection::const_iterator hiti=recHits.begin(); hiti!=recHits.end(); hiti++)
  {
      DetId idd = (hiti)->geographicalId();
      CSCDetId hitID(idd.rawId());
      rh_id          .push_back(CSCHelper::chamberSerial(hitID));
      rh_lay         .push_back(CSCHelper::convertTo<size8>(hitID.layer(),"rh_lay"));
      rh_pos_x       .push_back(hiti->localPosition().x());
      rh_pos_y       .push_back(hiti->localPosition().y());
      rh_strip_1 .push_back(CSCHelper::convertTo<size8>(hiti->channels(0),"rh_strip_1"));
      rh_strip_2 .push_back(CSCHelper::convertTo<size8>(hiti->channels(1),"rh_strip_2"));
      rh_strip_3 .push_back(CSCHelper::convertTo<size8>(hiti->channels(2),"rh_strip_3"));
      rh_pos_strip   .push_back(hiti->positionWithinStrip());
      rh_n_wiregroups.push_back(CSCHelper::convertTo<size8>(hiti->nWireGroups(),"rh_n_wiregroups"));
      rh_wireGrp.push_back(CSCHelper::convertTo<size8>(hiti->hitWire(),"rh_wireGrp"));
  }
}

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

void FillSegmentInfo::fill(const CSCSegmentCollection& segments, const CSCRecHit2DCollection* recHits){
  reset();

  for(CSCSegmentCollection::const_iterator dSiter=segments.begin(); dSiter != segments.end(); dSiter++) {
    CSCDetId id  = (CSCDetId)(*dSiter).cscDetId();

    LocalPoint localPos = (*dSiter).localPosition();
    float segX     = localPos.x();
    float segY     = localPos.y();
    LocalVector segDir = (*dSiter).localDirection();
    AlgebraicSymMatrix covMatrix = (*dSiter).parametersError();
    const auto& segmentHits = dSiter->specificRecHits();

    segment_id   .push_back(CSCHelper::chamberSerial(id));
    segment_pos_x.push_back(segX);
    segment_pos_y.push_back(segY);
    segment_dxdz.push_back(segDir.x()/segDir.z());
    segment_dydz.push_back(segDir.y()/segDir.z());
    segment_cov_dxdz       .push_back(float(covMatrix[0][0]));
    segment_cov_dxdz_dydz  .push_back(float(covMatrix[0][1]));
    segment_cov_dxdz_x     .push_back(float(covMatrix[0][2]));
    segment_cov_dxdz_y     .push_back(float(covMatrix[0][3]));
    segment_cov_dydz       .push_back(float(covMatrix[1][1]));
    segment_cov_dydz_x     .push_back(float(covMatrix[1][2]));
    segment_cov_dydz_y     .push_back(float(covMatrix[1][3]));
    segment_cov_x          .push_back(float(covMatrix[2][2]));
    segment_cov_x_y        .push_back(float(covMatrix[2][3]));
    segment_cov_y          .push_back(float(covMatrix[3][3]));
    segment_chisq.push_back((*dSiter).chi2());
    segment_nHits.push_back(CSCHelper::convertTo<size8>(segmentHits.size()  ,"segment_nHits"));
    segment_recHitIdx_1 .push_back((recHits && segmentHits.size() > 0) ?findRecHitIdx(segmentHits[0],recHits) : 0);
    segment_recHitIdx_2 .push_back((recHits && segmentHits.size() > 1) ?findRecHitIdx(segmentHits[1],recHits) : 0);
    segment_recHitIdx_3 .push_back((recHits && segmentHits.size() > 2) ?findRecHitIdx(segmentHits[2],recHits) : 0);
    segment_recHitIdx_4 .push_back((recHits && segmentHits.size() > 3) ?findRecHitIdx(segmentHits[3],recHits) : 0);
    segment_recHitIdx_5 .push_back((recHits && segmentHits.size() > 4) ?findRecHitIdx(segmentHits[4],recHits) : 0);
    segment_recHitIdx_6 .push_back((recHits && segmentHits.size() > 5) ?findRecHitIdx(segmentHits[5],recHits) : 0);

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
