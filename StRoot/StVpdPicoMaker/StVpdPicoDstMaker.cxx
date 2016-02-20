#include "StVpdPicoDstMaker.h"

//// StRoot
// StMuDst
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"

#include "StMemoryInfo.hh"
#include "StMessMgr.h"
#include "StTimer.hh"



#include "StBTofHeader.h"
#include "StBTofPidTraits.h"
#include "StBTofCollection.h"
#include "StEvent/StTriggerData.h"

ClassImp(StVpdPicoDstMaker);

/**
 * Processes the MuDst
 */
void StVpdPicoDstMaker::processMuDst() {
	//LOG_INFO << "StVpdPicoDstMaker::processMuDst()" << endm;

	StMuDstMaker *muDstMaker = (StMuDstMaker*) GetMaker( "MuDst" );

  	if( !muDstMaker ) {
		LOG_WARN << "WARNING: No MuDstMaker" << endm;
		return;
  	}
  	muDst = muDstMaker->muDst();
  	if(!muDst) {
		LOG_WARN << "WARNING: No MuDst" << endm;
		return;
  	}

	StTimer timer;

  	if (doPrintCpuInfo) 
  		timer.start();
  	if (doPrintMemoryInfo)
  		StMemoryInfo::instance()->snapshot();

 
 	// Event selection
  	if ( !keepEvent() )
  		return;

  	StMuEvent *muEvent = muDst->event();

	analyzeEvent();


	mTree->Fill();


	//- debug info`
	if (doPrintMemoryInfo) {
		StMemoryInfo::instance()->snapshot();
		StMemoryInfo::instance()->print();
	}
	if (doPrintCpuInfo) {
		timer.stop();
		LOG_INFO << "CPU time for StEventMaker::Make(): " << timer.elapsedTime() << " sec\n" << endm;
	}


}





/**
 * Called before main event cuts but after trigger selection
 */
void StVpdPicoDstMaker::preEventCuts(){
	//LOG_INFO << "StVpdPicoDstMaker::preEventCuts" << endm;
}

/**
 * Called after main event cuts
 */
void StVpdPicoDstMaker::postEventCuts(){
	//LOG_INFO << "StVpdPicoDstMaker::postEventCuts" << endm;
}

void StVpdPicoDstMaker::passEventCut( string name ){	
}

void StVpdPicoDstMaker::analyzeEvent(){

	StMuEvent *muEvent = muDst->event();
	const StTriggerData *td = muEvent->triggerData();
	

	for ( int iVPD = 0; iVPD < 16; iVPD ++ ){

		event.bbqAdcEast[ iVPD ] = td->vpdADC( east, iVPD+1 );
		event.bbqAdcWest[ iVPD ] = td->vpdADC( west, iVPD+1 );

		event.bbqTdcEast[ iVPD ] = td->vpdTDC( east, iVPD+1 );
		event.bbqTdcWest[ iVPD ] = td->vpdTDC( west, iVPD+1 );

		event.mxqAdcEast[ iVPD ] = td->vpdADCHighThr( east, iVPD+1 );
		event.mxqAdcWest[ iVPD ] = td->vpdADCHighThr( west, iVPD+1 );

		event.mxqTdcEast[ iVPD ] = td->vpdTDCHighThr( east, iVPD+1 );
		event.mxqTdcWest[ iVPD ] = td->vpdTDCHighThr( west, iVPD+1 );

	}

	event.bbqEarliestEast = td->vpdEarliestTDC( east );
	event.bbqEarliestWest = td->vpdEarliestTDC( west );

	event.mxqEarliestEast = td->vpdEarliestTDCHighThr( east );
	event.mxqEarliestWest = td->vpdEarliestTDCHighThr( west );


	event.grefmult = muEvent->grefmult();
	event.refmult = muEvent->refMult();

}



/**
 * Apply Event cuts
 * @return [description]
 */
Bool_t StVpdPicoDstMaker::keepEvent(){
	//LOG_INFO << "StVpdPicoDstMaker::keepEvent" << endm;

	passEventCut( "All");

	// The Pre event cuts hook
	preEventCuts();
	
	StMuEvent *muEvent = muDst->event();
	if ( !muEvent )
		return false;

	passEventCut( "MuDstEvent" );

	// run Id
	event.runId = muEvent->runId();
	

	// Test for trigger
	Bool_t isTrigger = false;
	for ( int t : triggers ){
		isTrigger = isTrigger || muEvent->triggerIdCollection().nominal().isTrigger( t );
	}

	if (triggers.size() == 0)
		isTrigger = true;

	if ( !isTrigger )
		return false;
	passEventCut( "Trigger" );


	if( !muDst->primaryVertex() ) {
		LOG_DEBUG << "No Primary Vertex" << endm;
		return false;
	}
	passEventCut( "VertexExists" );

	event.vtx = muDst->primaryVertex()->position();
	double pX = event.vtx.x();
	double pY = event.vtx.y();
	double pZ = event.vtx.z();

	StBTofHeader* tofHeader = muDst->btofHeader();
	event.vpdZ = tofHeader->vpdVz();

	// if ( fabs( event.vtx.z() - event.vpdZ ) > 3.0 )
	// 	return false;

	// passEventCut( "vZ");

	// if ( TMath::Sqrt( pX*pX + pY*pY ) > 1.0 )
	// 	return false;
	// passEventCut( "vR");
	
	return true;

}


/**
 * Creates the Maker
 */
StVpdPicoDstMaker::StVpdPicoDstMaker( const Char_t *name="rcpSkimmer", const Char_t *outname="rcp.skim.root") : StMaker(name) {
	// the output filename
	mTupleFileName = outname;

	// default debug options
	doPrintMemoryInfo = kFALSE;
	doPrintCpuInfo    = kFALSE;

	// load up the cuts from an xml config
	cfg = NULL;
	cfg = new jdb::XmlConfig( "./config.xml" );

	triggers = cfg->getIntVector( "Triggers" );

}

/**
 * Destructor
 */
StVpdPicoDstMaker::~StVpdPicoDstMaker( ){ 
	LOG_INFO << "StVpdPicoDstMaker::~StVpdPicoDstMaker( )" << endm;

	if ( NULL != cfg )
		delete cfg;
	cfg = NULL;
	LOG_INFO << "StVpdPicoDstMaker::~StVpdPicoDstMaker( )" << endm;
}

/**
 * Initialise the Maker
 */
Int_t StVpdPicoDstMaker::Init( ){

	bookNtuples();

	return kStOK;
}


Int_t StVpdPicoDstMaker::InitRun( int runnumber ) {

	return kStOK;
}

Int_t StVpdPicoDstMaker::FinishRun( int runnumber ) {
  	return kStOK;
}

/// write and close the ntuple file
Int_t StVpdPicoDstMaker::Finish() {
	LOG_INFO << "Int_t StVpdPicoDstMaker::Finish()" << endm;
	if ( (string)"" != mTupleFileName && mTupleFile ){
		
		mTupleFile->Write();
		mTupleFile->Close();

		LOG_INFO  << "StVpdPicoDstMaker::Finish() ntuple file " << mTupleFileName  << " closed." << endm;

	}

	return kStOK;
}


/**
 * Called for each event 
 */
Int_t StVpdPicoDstMaker::Make(){

  processMuDst();

  return kStOK;
}

/**
 * Initialize Tree Structure
 */
void StVpdPicoDstMaker::bookNtuples(){

	mTupleFile = new TFile(mTupleFileName.c_str(), "RECREATE");
	mTupleFile->SetCompressionLevel( 9 ); // maximum compression
	mTree = new TTree("vpdPico","V0 Pico Dst");

	mTree->Branch( "event", &event, 32000, 99 );

	
	return;
}