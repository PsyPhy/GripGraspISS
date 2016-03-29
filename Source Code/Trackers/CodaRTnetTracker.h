/*********************************************************************************/
/*                                                                               */
/*                                  CodaRTnetTracker.h                           */
/*                                                                               */
/*********************************************************************************/

// Interface to the CodaRTnet hardware.

#pragma once

#include "Trackers.h"

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
	void print_network_error(const NetworkException& exNet);

	// Helper function to print device error codes (server-side errors)
	void print_devicestatusarray_errors(const DeviceStatusArray& status);

	// Helper function to print system alignment status
	int print_alignment_status(const DWORD* marker_id_array, const DeviceInfoAlignment& info);

	//* Generic data packet
	RTNetworkPacket packet;

	// client connection object
	RTNetClient cl;

	int codaConfig;
	DeviceOptionsCodaMode mode;
	DeviceOptionsCodaPacketMode packet_mode;

	// decoder objects
	PacketDecode3DResultExt decode3D;	// 3D measurements (CX1)
	PacketDecodeADC16		decodeADC;		// 16-bit ADC measurements (GS16AIO)

	// Various objects
	AutoDiscover	discover;


  // Holds information about the different configurations defined on the CODA system.
  // I am guessing that there will only be one. Ideally, though, one might define
  // three different configurations, one with both Coda units active, the other 
  // two with each of the Codas working in isolation.
	HWConfigEnum	configs;
	DataStream		stream;
	CODANET_HWCONFIG_DEVICEENABLE devices;

	MarkerFrame		recordedMarkerFrames[MAX_UNITS][MAX_FRAMES];

protected:

public:

	CodaRTnetTracker( void ) : 
		// Host address and UDP port for the Coda RTnet server.
		serverAddress("192.168.1.1"), 
		serverPort(10111), 
		// Marker acquistion rate (200Hz), down sampling (none) and external sync (no).
		mode( CODANET_CODA_MODE_200, 1, false ), 
		// Request marker data from each Coda unit separately, and the combined data.
		packet_mode( CODANET_CODAPACKETMODE_SEPARATE_AND_COMBINED_COORD ),	
		// Use the first Coda configuration in the list.
		// This has to be set up as a cx1 only configuration on the server.
		codaConfig(1), 
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

	int		RetrieveMarkerFrames( MarkerFrame frames[], int max_frames, int unit );
	bool	GetCurrentMarkerFrame( MarkerFrame &frame );

	// Need to add the following.
	int		PerformAlignment( int origin, int x_negative, int x_positive, int xy_negative, int xy_positive );
	void	GetUnitPlacement( int unit, Vector3 &pos, Quaternion &ori ) ;
	void	GetUnitTransform( int unit, Vector3 &offset, Matrix3x3 &rotation ) ;

};


