/*********************************************************************************/
/*                                                                               */
/*                            CodaRTnetDaemonTracker.h                           */
/*                                                                               */
/*********************************************************************************/

// Interface to the CodaRTnet hardware.

#pragma once

// Disable warnings about "unsafe" functions.
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>

#include "../Useful/fOutputDebugString.h"
#include "Trackers.h"
#include "CodaRTnetTracker.h"

namespace PsyPhy {

///
/// Yet another version of the CodaRTnetTracker.
/// This on connects to the GraspTrackerDaemon UDP stream to get the marker data.
///
class CodaRTnetDaemonTracker : public CodaRTnetTracker {

private:
		
	WSADATA	wsaData;
	SOCKET	daemonSocket;
	struct sockaddr_in daemonAddr;
	int daemonAddrLength;
	bool	acquiring;

	Timer	timer;

public:

	int nFramesPerUnit[MAX_UNITS];

	CodaRTnetDaemonTracker( void ) : daemonSocket(0) {
			daemonAddrLength = sizeof( daemonAddr );
	}
	virtual void  Initialize( const char *ini_filename = "CodaRTnet.ini" );
	virtual void StartAcquisition( float max_seconds );
	virtual bool GetAcquisitionState( void );
	virtual bool GetCurrentMarkerFrameUnit( MarkerFrame &frame, int selected_unit );
	virtual void StopAcquisition( void );
	virtual void AbortAcquisition( void );
	virtual int  Update( void );
	virtual void  Quit( void );
	virtual void  Shutdown( void );
	virtual void  Startup( void );
};


};
