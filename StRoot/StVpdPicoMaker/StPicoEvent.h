#ifndef ST_PICO_EVENT_H
#define ST_PICO_EVENT_H

#include "TObject.h"
#include "StThreeVectorD.hh"


class StPicoEvent : public TObject
{
public:
	StPicoEvent(){}
	~StPicoEvent(){}

	Int_t mxqTdcEast[16], mxqTdcWest[16];
	Int_t mxqAdcEast[16], mxqAdcWest[16];

	Int_t bbqTdcEast[16], bbqTdcWest[16];
	Int_t bbqAdcEast[16], bbqAdcWest[16];
	Int_t bbqEarliestEast, bbqEarliestWest;
	Int_t mxqEarliestEast, mxqEarliestWest;

	Int_t runId;

	Float_t vpdZ;

	StThreeVectorD vtx;

	Int_t grefmult, refmult;

	

	ClassDef( StPicoEvent, 1 )
};



#endif