#ifndef ST_V0_PICO_DST_MAKER_H
#define ST_V0_PICO_DST_MAKER_H

// StRoot
#include "StMaker.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"


// ROOT
#include "TTree.h"
#include "TClonesArray.h"

// roobarb
#include "StRooBarb/XmlConfig.h"
#include "StRooBarb/ConfigPoint.h"
#include "StRooBarb/ConfigRange.h"

#include "StPicoEvent.h"


class StMuDstMaker;
class StMuDst;
class StMuEvent;
class StMuTrack;

class StVpdPicoDstMaker : public StMaker {
 public:
 	
	StVpdPicoDstMaker(const Char_t *name, const Char_t *outname);
	~StVpdPicoDstMaker();
	 
	Int_t  Init();
	Int_t  InitRun(int runnumber);
	Int_t  Make();
	Int_t  FinishRun(int runnumber);
	Int_t  Finish();

protected:

	bool keepEvent();
	virtual void passEventCut( string name );
	virtual void preEventCuts();
	virtual void postEventCuts();
	void analyzeEvent();

	bool keepTrack( StMuTrack * track);
	virtual void passTrackCut( string name ) {}
	virtual void preTrackCuts() {}
	virtual void postTrackCuts() {}
	void analyzeTrack( StMuTrack * track );
	
	jdb::XmlConfig * cfg;
	vector<int> triggers;
	

	StPicoEvent event;



	// Int_t nTofMatchedTracksA();

	void processMuDst();
	void bookNtuples();

	
	TTree *mTree;

	StMuDst *muDst;
	string mTupleFileName;
	TFile *mTupleFile;
	
	Bool_t  doPrintMemoryInfo; 		//! control debug memory data
	Bool_t  doPrintCpuInfo; 		//! control debug timing data

	virtual const char *GetCVS() const 
		{static const char cvs[]="Tag $Name:  $ $Id: StVpdPicoDstMaker.h, $ built "  __DATE__  "  "  __TIME__ ; return cvs;}

	ClassDef(StVpdPicoDstMaker,1)
};

#endif
