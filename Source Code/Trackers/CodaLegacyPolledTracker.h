/*********************************************************************************/
/*                                                                               */
/*                           CodaLegacyContinuousTracker.h                        */
/*                                                                               */
/*********************************************************************************/

///
/// A tracker that uses the legacy CODA SDK to perform unbuffered acquisition.
///
#pragma once

// Disable warnings about "unsafe" functions.
#define _CRT_SECURE_NO_WARNINGS

#include "../Useful/fOutputDebugString.h"
#include "../Useful/Timers.h"
#include "../Include/codasys.h"
#include "../CodaLegacySDK/CodaUtilities.h"
#include "Trackers.h"

#define C
namespace PsyPhy {


	class CodaLegacyPolledTracker : public Tracker {

	protected:

		Timer	acquisitionTimer;
		bool	acquiring;
		bool	overrun;

		// Real-time marker data
		// Defines storage for one Coda frame.
		CODA_FRAME_DATA_STRUCT coda_data_frame;
		codaBYTE bInView[ CODA_MAX_MARKERS ];
		codaFLOAT fPosition[ CODA_MAX_MARKERS * 3 ];

	public:

		// Holds the CodaRTnet server IP address and port.
		char serverAddress[64];
		unsigned int serverPort;


		CodaLegacyPolledTracker( void ) : acquiring(false) {

			// Host address and UDP port for the Coda RTnet server.
			// The default addresss is for the RTnet server on DEX via the ETD port.
			// This can be overidden in the .ini file.
			strncpy( serverAddress, "10.80.12.103", sizeof( serverAddress ) );
			serverPort = 10111;

		}
		virtual void Initialize( const char *ini_filename = "CodaSDK.ini" );
		virtual void StartAcquisition( float duration );
		virtual bool GetAcquisitionState();
		virtual bool CheckAcquisitionOverrun( void );
		virtual bool GetCurrentMarkerFrameUnit( MarkerFrame &frame, int selected_unit );
		virtual void StopAcquisition( void );
		virtual int  Update( void );
		virtual void Quit( void );

		virtual void StartContinuousAcquisition( void );
		virtual void StopContinuousAcquisition( void );

		virtual void GetUnitTransform( int unit, Vector3 &offset, Matrix3x3 &rotation );
		virtual void SetUnitTransform( int unit, Vector3 &offset, Matrix3x3 &rotation );

		// Provide the means to read a .ini file to set configuration parameters.
		// This is defined here as static because its address is sent as a callback to a parsing routine.
		static int iniHandler( void *which_instance, const char* section, const char* name, const char* value ) {
			CodaLegacyPolledTracker *instance = (CodaLegacyPolledTracker *) which_instance;
			if ( !strcmp( section, "CodaSDK" ) && !strcmp( name, "CodaServerHost" )) strncpy( instance->serverAddress, value, sizeof( instance->serverAddress ));
			if ( !strcmp( section, "CodaSDK" ) && !strcmp( name, "CodaServerPort" )) instance->serverPort = atoi( value );

			return 1;

		}


	};

};
