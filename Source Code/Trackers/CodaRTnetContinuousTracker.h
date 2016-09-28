/*********************************************************************************/
/*                                                                               */
/*                           CodaRTnetContinuousTracker.h                        */
/*                                                                               */
/*********************************************************************************/

///
/// Another version of the CodaRTnetTracker that uses unbuffered acquisition.
///
#pragma once

// Disable warnings about "unsafe" functions.
#define _CRT_SECURE_NO_WARNINGS

#include "../Useful/fOutputDebugString.h"
#include "../Useful/Timers.h"
#include "CodaRTnetTracker.h"

namespace PsyPhy {


class CodaRTnetContinuousTracker : public CodaRTnetTracker {

protected:
		
	// The CODA continuously sends separate packets for each CODA unit. It is conceivable that
	// the two packets from a given time slice could be read on separate calls to Update().
	// So we keep track of the number of acquired frames for each CODA unit separately.
	// The common parameter nFrames is nevertheless maintained and set to the smaller of the
	// individual frame counters.
	unsigned int nFramesPerUnit[MAX_UNITS];

	Timer	acquisitionTimer;
	bool	acquiring;

public:

	CodaRTnetContinuousTracker( void ) : acquiring(false) {}
	virtual void Initialize( const char *ini_filename = "CodaRTnet.ini" );
	virtual void StartAcquisition( float duration );
	virtual bool GetAcquisitionState();
	virtual bool GetCurrentMarkerFrameUnit( MarkerFrame &frame, int selected_unit );
	virtual void StopAcquisition( void );
	virtual int  Update( void );

	virtual void StartContinuousAcquisition( void );
	virtual void StopContinuousAcquisition( void );

};

};
