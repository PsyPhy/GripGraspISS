/*********************************************************************************/
/*                                                                               */
/*                                  CodaRTnetTracker.h                           */
/*                                                                               */
/*********************************************************************************/

// Interface to the CodaRTnet hardware.

#pragma once

#include "Trackers.h"

// RTNet C++ includes
#define NO64BIT
#include "../Include/codaRTNetProtocolCPP/RTNetClient.h"
#include "../Include/codaRTNetProtocolCPP/DeviceOptionsAlignment.h"
#include "../Include/codaRTNetProtocolCPP/DeviceOptionsCodaPacketMode.h"
#include "../Include/codaRTNetProtocolCPP/DeviceOptionsCodaMode.h"
#include "../Include/codaRTNetProtocolCPP/DeviceInfoAlignment.h"
#include "../Include/codaRTNetProtocolCPP/PacketDecode3DResult.h"
#include "../Include/codaRTNetProtocolCPP/PacketDecode3DResultExt.h"
#include "../Include/codaRTNetProtocolCPP/PacketDecodeADC16.h"
#include "../Include/codaRTNetProtocolCPP/DeviceInfoUnitCoordSystem.h"

namespace PsyPhy {

class CodaRTnetTracker : public Tracker {

private:

	// Hardwire the server IP address and port.
	char *serverAddress;
	unsigned int serverPort;

	// Marker tracker device.
	const int cx1Device;	// Should be the CX1

	// How many tries to get a data packet before giving up.
	int maxRetries;

	// Flag to keep track of overrunning the acquisition time.
	bool overrun;

	// Helper function to print network connection error codes (client-side errors)
	void print_network_error(const codaRTNet::NetworkException& exNet);

	// Helper function to print device error codes (server-side errors)
	void print_devicestatusarray_errors(const codaRTNet::DeviceStatusArray& status);

	// Helper function to print system alignment status
	int print_alignment_status(const DWORD* marker_id_array, const codaRTNet::DeviceInfoAlignment& info);

	//* Generic data packet
	codaRTNet::RTNetworkPacket packet;

	// client connection object
	codaRTNet::RTNetClient cl;

	int codaConfig;
	codaRTNet::DeviceOptionsCodaMode coda_mode;
	codaRTNet::DeviceOptionsCodaPacketMode packet_mode;
	int packetsPerFrame;

	// decoder objects
	codaRTNet::PacketDecode3DResultExt decode3D;	// 3D measurements (CX1)
	codaRTNet::PacketDecodeADC16		decodeADC;		// 16-bit ADC measurements (GS16AIO)

	// Various objects
	codaRTNet::AutoDiscover	discover;


  // Holds information about the different configurations defined on the CODA system.
  // I am guessing that there will only be one. Ideally, though, one might define
  // three different configurations, one with both Coda units active, the other 
  // two with each of the Codas working in isolation.
	codaRTNet::HWConfigEnum	configs;
	codaRTNet::DataStream		stream;
	CODANET_HWCONFIG_DEVICEENABLE devices;

public:
	// Buffers to hold the data retrieved from the CODA units.
	// I am making them public so that the calling program can access them directly,
	// rather than going through RetrieveMarkerFramesUnit();
	MarkerFrame		recordedMarkerFrames[MAX_UNITS][MAX_FRAMES];

protected:

public:

	CodaRTnetTracker( void ) : 
		// Host address and UDP port for the Coda RTnet server.
		// serverAddress("192.168.5.61"), 
		serverAddress( "10.80.12.103" ),
		serverPort(10111), 

		// Marker acquistion rate (200Hz), down sampling (none) and external sync (no).
		coda_mode( CODANET_CODA_MODE_200, 1, false ), 

		// Request marker data from each Coda unit separately.
		packet_mode( CODANET_CODAPACKETMODE_SEPARATE_COORD ),	

		// Selects which of the configurations, as defined on the server, to be used.
		// This has to be set up as a cx1 only configuration on the server.
		// Someday I will think about how to allow for other devices as well, e.g. the ADC.
		codaConfig(0), 
		// A Coda RTnet configuration can include cx1 devices, ADC, force platforms, etc.
		// This is just a constant specifying the cx1 device.
		cx1Device(1),
		// This determines how many times we try to get a failed packet before giving up.
		maxRetries(5)
	{}

	void Initialize( void );
	int  Update( void );
	void StartAcquisition( float max_duration );
	void StopAcquisition( void );
	bool CheckAcquisitionOverrun( void );

	bool GetAcquisitionState( void );
	int  GetNumberOfCodas( void );

	int		RetrieveMarkerFramesUnit( MarkerFrame frames[], int max_frames, int unit );
	int		RetrieveMarkerFrames( MarkerFrame frames[], int max_frames );
	bool	GetCurrentMarkerFrameUnit( MarkerFrame &frame, int unit );

	// Need to add the following.
	int		PerformAlignment( int origin, int x_negative, int x_positive, int xy_negative, int xy_positive );
	void	GetUnitPlacement( int unit, Vector3 &pos, Quaternion &ori ) ;
	void	GetUnitTransform( int unit, Vector3 &offset, Matrix3x3 &rotation ) ;

};

};
