/***************************************************************************/
/*                                                                         */
/*                               CodaRTnetTracker                          */
/*                                                                         */
/***************************************************************************/

#include "stdafx.h"

// A tracker that uses the CODA RTnet interface.

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"
#include "../VectorsMixin/VectorsMixin.h"

#include "CodaRTnetTracker.h"

using namespace PsyPhy;


// Starting up the CODA takes time. It would be nice if we could leave it
// in a running state after the first startup, to go faster on subsequent trials.
// Set this flag to force a shutdown before each start up for testing purposes.
#define ALWAYS_SHUTDOWN

// RTNet C++ includes
#define NO64BIT
#include "../Include/codaRTNetProtocolCPP/RTNetClient.h"
#include "../Include/codaRTNetProtocolCPP/DeviceOptionsAlignment.h"
#include "../Include/codaRTNetProtocolCPP/DeviceOptionsCodaMode.h"
#include "../Include/codaRTNetProtocolCPP/DeviceOptionsCodaPacketMode.h"
#include "../Include/codaRTNetProtocolCPP/DeviceInfoAlignment.h"
#include "../Include/codaRTNetProtocolCPP/PacketDecode3DResult.h"
#include "../Include/codaRTNetProtocolCPP/PacketDecode3DResultExt.h"
#include "../Include/codaRTNetProtocolCPP/PacketDecodeADC16.h"
#include "../Include/codaRTNetProtocolCPP/DeviceInfoUnitCoordSystem.h"

#define DEVICEID_ADC DEVICEID_GS16AIO

using namespace PsyPhy;
using namespace codaRTNet;

/***************************************************************************/

void CodaRTnetTracker::Initialize( void ) {
	
	try {
		
		// Decode the IP address string that has already been initialized as part of the class.
		unsigned int p, q, r, s;
		fOutputDebugString( "RTnet Server: %s   Port: %d  Configuration: %d\n", serverAddress, serverPort, codaConfig );
		sscanf( serverAddress, "%d.%d.%d.%d", &p, &q, &r, &s );
		
		// Connect to the server.
		cl.connect( (p << 24) + (q << 16) + (r << 8) + s, serverPort );
		
		// Check that the server has at least the expected number of configurations.
		cl.enumerateHWConfig(configs);
		// Print config names for debugging purposes.
		fOutputDebugString( "Found %u hardware configurations:\n", configs.dwNumConfig);
		char dest[1024];
		for(DWORD iconfig = 0; iconfig < configs.dwNumConfig; iconfig++) {
			wcstombs( dest, configs.config[iconfig].strName, sizeof( dest ) );
			fOutputDebugString( "  [%u] address: %08X  name: %s\n", 
				iconfig, configs.config[iconfig].dwAddressHandle, dest );
		}
		
		if (configs.dwNumConfig < (DWORD) codaConfig)
		{
			fOutputDebugString("ERROR: specified hardware configuration %d not available.\n", codaConfig);
			exit( -1 );
		}
		// Show which configuration we are trying to use.
		wcstombs( dest, configs.config[codaConfig].strName, sizeof( dest ) );
		fOutputDebugString( "RTnet Configuration: %s\n", dest );
		

		// It used to be that we could just run the startup commands even
		// if the system is already running. Now it seems that calling 
		// PrepareForAcquisition() if it has already been called causes problems.
		// So if we are running already and it is not the same configuration
		// then shut down and start again.
		DWORD	running_config = cl.getRunningHWConfig();

#ifdef ALWAYS_SHUTDOWN
		cl.stopAcq();
		OutputDebugString( "Shutting down ... " );
		cl.stopSystem();
		running_config = NULL;
		OutputDebugString( "OK.\n" );
#else
		if ( running_config != configs.config[codaConfig].dwAddressHandle ) {
			OutputDebugString( "*** Not running the right configuration! ***" );
			cl.stopSystem();
			running_config = NULL;
		}
#endif


		if ( !running_config ) {

			OutputDebugString( "Starting up Coda system ... " );

			// If system is already started, this does nothing.
			// Otherwise it will load the specified configuration.
			// We assume that the configuration constants defined here correspond
			//  to the actual configurations defined on the server..
			cl.startSystem( configs.config[codaConfig].dwAddressHandle );

			// Here we set the marker acquisition frequency, decimation and whether or
			//  not external sync is enabled. The values are set during class initialization.
			cl.setDeviceOptions( coda_mode );

			// This says that we want individual data from each coda.
			cl.setDeviceOptions( packet_mode );
			OutputDebugString( "OK.\n" );

		}

		// prepare for acquisition
		OutputDebugString( "cl.prepareForAcq() ... " );
		cl.prepareForAcq();
		OutputDebugString( "OK.\n" );

		// Find out how many Coda units are actually in use.
		// I don't really need the alignment information, but that structure
		// includes the number of Codas specified in the configuration that is in use.
		// So we do a bogus alignment.
		DeviceOptionsAlignment align(1, 1, 1, 1, 1);
		cl.setDeviceOptions( align );
		// Then this is what tells us how many units are there.
		DeviceInfoAlignment align_info;
		cl.getDeviceInfo( align_info );

		fOutputDebugString( "Number of connected CODA units: %d\n", nUnits = align_info.dev.dwNumUnits );

		// Create a data stream.
		// I know from experience that the port number (here 7000) has to be different
		// for each client that wants to access the RTnet server at the same time.
		// To run different applications simultaneously, I just define a different port
		// number for each one. But what if I want to run the same application on different
		// machines?
		// TODO: Figure out how the port number is supposed to be used.
		cl.createDataStream(stream, 7001);
		// ensure socket is up before starting acquisition
		codanet_sleep(50);
		
	}
	catch(NetworkException& exNet)
	{
		print_network_error( exNet );
		fOutputDebugString( "Caught (NetworkException& exNet)\n" );
		MessageBox( NULL, "DexRTnet() failed.\n(NetworkException& exNet)", "CodaRTnetTracker", MB_OK );
		exit( -1 );
	}
	catch(DeviceStatusArray& array)
	{
		print_devicestatusarray_errors(array);
		fOutputDebugString( "Caught (DeviceStatusArray& array)\n" );
		MessageBox( NULL, "DexRTnet() failed.\n(DeviceStatusArray& array)", "CodaRTnetTracker", MB_OK );
		exit( -1 );
	}
		
}

void CodaRTnetTracker::Shutdown( void ) {
		unsigned int p, q, r, s;
		sscanf( serverAddress, "%d.%d.%d.%d", &p, &q, &r, &s );
		cl.connect( (p << 24) + (q << 16) + (r << 8) + s, serverPort );
		cl.stopAcq();
		OutputDebugString( "Shutting down ... " );
		cl.stopSystem();
		OutputDebugString( "OK.\n" );
}

/***************************************************************************/

int CodaRTnetTracker::Update( void ) {
	// The Coda tracker does not need any periodic updating.
	return( 0 );
}

void CodaRTnetTracker::Quit( void ) {

#ifdef ALWAYS_SHUTDOWN
	OutputDebugString( "Shutting down ... " );
	cl.stopSystem();
#else
	OutputDebugString( "Quitting CodaRTnetTracker but leaving RTnet runnning ..." );
#endif
	OutputDebugString( "closing stream ... " );
	cl.closeDataStream( stream );
	OutputDebugString( "disconnecting ... " );
	cl.disconnect();
	OutputDebugString( "OK.\n" );

}

/*********************************************************************************/

void CodaRTnetTracker::StartAcquisition( float max_duration ) {
	
	overrun = false;
	cl.setAcqMaxTicks( DEVICEID_CX1, (DWORD) floor( max_duration * 200.0 ));
	OutputDebugString( "cl.startAcqContinuousBuffered()\n" );
	cl.startAcqContinuousBuffered();
	
}

bool CodaRTnetTracker::CheckAcquisitionOverrun( void ) { return overrun; }

void CodaRTnetTracker::AbortAcquisition( void ) {
	// Attempt to halt an ongoing aquisition. 
	// Does not care if it was actually acquiring or not.
	// Does not retrieve the data.
	OutputDebugString( "Aborting acquisition ..." );
	cl.stopAcq();
	OutputDebugString( "OK.\n" );
}

void CodaRTnetTracker::StopAcquisition( void ) {
	// stop acquisition if still in progress
	OutputDebugString( "Stop acquisition\n" );
	if ( cl.isAcqInProgress() ) cl.stopAcq();
	//* If the acquisition was already stopped, it means that the Coda
	//* stopped after reaching the defined number of cycles.
	//* Of course, it could also mean that an acquisition was not even started.
	else overrun = true;
	//* Find out how many ticks worth of data are stored for this device.
	//* Note that the routine says it gets the number of packets, but I believe
	//* that this is a misnomer, since it sends 3 packets per slice of data
	//* (one for each coda unit and one for the combined data).
	nFrames = cl.getAcqBufferNumPackets( cx1Device );
	if ( nFrames > MAX_FRAMES ) nFrames = MAX_FRAMES;

	int frm;
	int unit_count;
	int retry;
	
	int nChecksumErrors = 0;
	int nTimeouts = 0;
	int nUnexpectedPackets = 0;
	int nFailedFrames = 0;
	int nSuccessfullPackets = 0;
	
	fOutputDebugString( "\nStart retrieval of Marker data (%d frames).\n", nFrames );

	//* Loop through and get them all.
	for ( frm = 0; frm < nFrames; frm++ ) {

		//* Try to read the packets. Normally they should get here the first try.
		//* But theoretically, they could get lost or the could get corrupted. 
		//* So if we get a time out or checksum errors, we should try again.
		for ( retry = 0; retry < maxRetries; retry++ ) {
			try
			{
				//* Tell the server to send the specified packet.
				cl.requestAcqBufferPacket( cx1Device, frm);
			}
			catch(DeviceStatusArray&)
			{
				OutputDebugString( "Caught error from cl.requestAcqBufferPacket()\n" );
			}
			//* We are supposed to get nCoda packets per request.
			unit_count = 0;
			while ( unit_count < nUnits ) {
				// Time out means we did not get as many packets as expected.
				// So request them again for this time slice.
				if ( stream.receivePacket(packet, 50000) == CODANET_STREAMTIMEOUT ) {
					nTimeouts++;
					break;
				}
				if ( !packet.verifyCheckSum() ) {
					nChecksumErrors++;
					break;
				}
				else if ( !decode3D.decode( packet ) ) {
					nUnexpectedPackets++;
					break;
				}
				else {
					// Process packet.
					nSuccessfullPackets++;
					// find number of marker positions available
					DWORD nMarkers = decode3D.getNumMarkers();
					if ( nMarkers > MAX_MARKERS ) {
						fMessageBox(  MB_OK, "CodaRTnetTracker", "How many markers?!?!\n   %d > %d", nMarkers, MAX_MARKERS  );
						exit( -1 );
					}
					int   unit = decode3D.getPage();
					if ( unit >= nUnits ) {
						MessageBox( NULL, "Which unit?!?!", "CodaRTnetTracker", MB_OK );
						exit( -1 );
					}
					recordedMarkerFrames[unit][frm].time = decode3D.getTick() * cl.getDeviceTickSeconds( DEVICEID_CX1 );
					for ( DWORD mrk = 0; mrk < nMarkers; mrk++ ) {
						float *pos = decode3D.getPosition( mrk );
						for ( int i = 0; i < 3; i++ ) recordedMarkerFrames[unit][frm].marker[mrk].position[i] = pos[i];
						recordedMarkerFrames[unit][frm].marker[mrk].visibility = ( decode3D.getValid( mrk ) != 0 );
					}
					// Count the number of packets received for this frame.
					// There should be one per unit.
					unit_count++;
				}
			}
			// If we finished the loop normally that means we got all the packets for this frame.
			// If we got here via one of the break statements, then we should try again.
			if ( unit_count >= nUnits ) break;
		}
		if ( retry >= maxRetries ) {
			nFailedFrames++;
//			fOutputDebugString( "%08u\tfailed.\n", frm );
		}
	}
			
	if ( nFailedFrames || nTimeouts || nChecksumErrors || nUnexpectedPackets ) {
		char message[2048];
		sprintf( message, "Packet Errors:\n\nFrames: %d * %d = %d\nFailed Frames: %d\nTimeouts: %d\nChecksum Errors: %d\nUnexpected Packets: %d",
			nFrames, nUnits, nFrames * nUnits, nFailedFrames, nTimeouts, nChecksumErrors, nUnexpectedPackets );
		MessageBox( NULL, message, "RTnetAcquisition", MB_OK | MB_ICONEXCLAMATION );
	}
	else OutputDebugString( "Stop Retrieval\n" );

	// It appears that the system has to be prepared again for new acquisitions.
	// That's not documented anywhere, but it seems to be true.
	// TODO: Verify with Charnwood.
	//OutputDebugString( "OK.\ncl.prepareForAcq() ... " );
	//cl.prepareForAcq();
	//OutputDebugString( "OK.\n" );

}

// Retrieve the stored CX1 data from a specified unit.
int CodaRTnetTracker::RetrieveMarkerFramesUnit( MarkerFrame frames[], int max_frames, int unit ) {
	if ( unit >= nUnits ) {
		fMessageBox( MB_OK, "CodaRTnetTracker", "Specified unit out of range. \n   Specified: %d Units: %d", unit, nUnits );
		return( 0 );
	}
	//* Loop through and get them all.
	for ( int frm = 0; frm < nFrames; frm++ ) {
		CopyMarkerFrame( frames[frm], recordedMarkerFrames[unit][frm] );
	}
	return( nFrames );
}

// Retrieve the combined marker data.
// We do not actually acquire the combined data as computed by the Coda system.
// Rather, we compute a simple average of visible markers. 
int CodaRTnetTracker::RetrieveMarkerFrames( MarkerFrame frames[], int max_frames ) {
	//* Loop through and get them all.
	for ( int frm = 0; frm < nFrames; frm++ ) {
		frames[frm].time += recordedMarkerFrames[0][frm].time;
		for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
			CopyVector( frames[frm].marker[mrk].position, zeroVector );
			int count = 0;
			for ( int unit = 0; unit < nUnits; unit++ ) {
				if ( recordedMarkerFrames[unit][frm].marker[mrk].visibility ) {
					AddVectors( frames[frm].marker[mrk].position, frames[frm].marker[mrk].position, recordedMarkerFrames[unit][frm].marker[mrk].position );
					count++;
				}
			}
			if ( count > 0 ) {
				ScaleVector( frames[frm].marker[mrk].position, frames[frm].marker[mrk].position, 1.0 / (double) count );
				frames[frm].marker[mrk].visibility = true;
			}
			else {
				frames[frm].marker[mrk].position[X] = frames[frm].marker[mrk].position[Y] = frames[frm].marker[mrk].position[Z] = INVISIBLE;
				frames[frm].marker[mrk].visibility = false;
			}
		}
	}
	return( nFrames );
}


bool CodaRTnetTracker::GetCurrentMarkerFrameUnit( MarkerFrame &frame, int selected_unit ) {
	
	int unit_count;
	int mrk;
	
	int nChecksumErrors = 0;
	int nTimeouts = 0;
	int nUnexpectedPackets = 0;
	int nFailedFrames = 0;
	int nSuccessfullPackets = 0;

	bool status = false;

	// If the CODA is already acquiring, just read the latest data.
	// We assume that it is in buffered mode, so we have to ask for the latest frame.
	if ( cl.isAcqInProgress() ) {
		try
		{
			cl.monitorAcqBuffer();
		}
		catch( DeviceStatusArray& array )
		{
			OutputDebugString( "Caught ( DeviceStatusArray& array ) " );
			// If an error occured, output some debug info the debug screen.
			// Other than that, this is a fatal error so we exit.
			print_devicestatusarray_errors(array);
			OutputDebugString( "\n" );
			exit( -1 );
		}
	}
	// If not already acquiring, then request acquisition of a single frame.
	else {
		// request a frame from all devices
		try
		{
			cl.doSingleShotAcq();
		}
		catch(DeviceStatusArray& array)
		{
			print_devicestatusarray_errors(array);
			fOutputDebugString( "Caught (DeviceStatusArray& array)\n" );
			exit( -1 );
		}
	}
	
	// Set a counter to count the number of packets that we get from the request.
	// We are supposed to get nCoda packets per request.
	unit_count = 0;
	while ( unit_count < nUnits ) {

		// Time out means we did not get as many packets as expected.
		// So request them again for this time slice.
		if ( stream.receivePacket(packet, 50000) == CODANET_STREAMTIMEOUT ) {
			nTimeouts++;
			break;
		}

		// Check if the packet is corrupted.
		if ( !packet.verifyCheckSum() ) {
			nChecksumErrors++;
			break;
		}

		// Check if it is a 3D marker packet. It could conceivably  be a packet
		// from the ADC device, although we don't plan to use the CODA ADC at the moment.
		else if ( !decode3D.decode( packet ) ) {
			nUnexpectedPackets++;
			break;
		}

		// If we get this far, it is a valid marker packet.
		else {
			// Count the total number of valid packets..
			nSuccessfullPackets++;
			// find number of markers included in the packet.
			int n_markers = decode3D.getNumMarkers();

			// Single shots can return 56 marker positions, even if we are using
			// 200 Hz / 28 markers for continuous acquisition. Stay within bounds.
			if ( n_markers > MAX_MARKERS ) {
				n_markers = MAX_MARKERS;
			}
			
			// The 'page' number is used to say which CODA unit the packet belongs to.
			int   unit = decode3D.getPage();
			if ( unit >= nUnits ) {
				// I don't believe that we should ever get here, but who knows?
				MessageBox( NULL, "Which unit?!?!", "Dexterous", MB_OK );
				exit( -1 );
			}
			
			// Process the data from the selected unit only.
			if ( unit == selected_unit ) {

				// Compute the time from the tick counter in the packet and the tick duration.
				// Actually, I am not sure if the tick is defined on a single shot acquistion.
				frame.time = decode3D.getTick() * cl.getDeviceTickSeconds( DEVICEID_CX1 );

				// Get the marker data from the CODA packet.
				for ( mrk = 0; mrk < n_markers; mrk++ ) {
					float *pos = decode3D.getPosition( mrk );
					for ( int i = 0; i < 3; i++ ) frame.marker[mrk].position[i] = pos[i];
					frame.marker[mrk].visibility = ( decode3D.getValid( mrk ) != 0 );
				}

				// If the packet contains fewer markers than the nominal number for
				//  the apparatus, set the other markers to be out of sight..
				for ( mrk = mrk; mrk < nMarkers; mrk++ ) {
					float *pos = decode3D.getPosition( mrk );
					for ( int i = 0; i < 3; i++ ) frame.marker[mrk].position[i] =INVISIBLE;
					frame.marker[mrk].visibility = false;
				}

				// Signal that we got the data that we were seeking.
				status = true;
			}
			// Count the number of packets received for this frame.
			// There should be one per unit.
			unit_count++;
		}
	}

	return( status );
	
}

/****************************************************************************************/

bool CodaRTnetTracker::GetAcquisitionState( void ) {
	// I believe that this should work, but I did not test it with a CODA.
	return( cl.isAcqInProgress() );
}

int CodaRTnetTracker::GetNumberOfCodas( void ) {
	return( nUnits );
}

void CodaRTnetTracker::GetAlignment( Vector3 offset[MAX_UNITS], Matrix3x3 rotation[MAX_UNITS] ) {
	// Get what are the alignment transformations before doing the alignment.
	// This is just for debugging. Set a breakpoint to see the results.
	DeviceInfoUnitCoordSystem pre_xforms;
	cl.getDeviceInfo( pre_xforms );
	for ( int unit = 0; unit < nUnits; unit++ ) {
		for ( int i = 0; i < 3; i++ ) {
			for ( int j = 0; j < 3; j++ ) {
				rotation[unit][i][j] = pre_xforms.dev.Rt[unit].R[i*3+j];
			}
		}
		CopyVector( offset[unit], pre_xforms.dev.Rt[unit].t );
	}
}

int  CodaRTnetTracker::PerformAlignment( int origin, int x_negative, int x_positive, int xy_negative, int xy_positive, bool force_show ) {

	// Get what are the alignment transformations before doing the alignment.
	// This is just for debugging. Set a breakpoint to see the results.
	DeviceInfoUnitCoordSystem pre_xforms;
	cl.getDeviceInfo( pre_xforms );

	DeviceOptionsAlignment align( origin + 1, x_negative + 1, x_positive + 1, xy_negative + 1, xy_positive + 1);
	cl.setDeviceOptions( align );

	// Show what are the alignment transformations after doing the alignment.
	// This is just for debugging. Set a breakpoint to see the results.
	DeviceInfoUnitCoordSystem post_xforms;
	cl.getDeviceInfo( post_xforms );

	// retrieve information
	DeviceInfoAlignment info;
	cl.getDeviceInfo(info);

	// print alignment diagnostics
	DWORD marker_id_array[5] = { origin + 1, x_negative + 1, x_positive + 1, xy_negative + 1, xy_positive + 1 };
	int response;

	if ( info.dev.dwStatus != 0 ) response = print_alignment_status( marker_id_array, info, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION );
	else if ( force_show ) response = print_alignment_status( marker_id_array, info, MB_OK );
	else response = IDOK; 
	
	return( response );
}

void  CodaRTnetTracker::AnnulAlignment( void ) {

	char command_line[10240];

	// Create a unique filename for a local temporary file.
	char filename[MAX_PATH];
	SYSTEMTIME st;
	GetSystemTime( &st );
	sprintf( filename, "%04d-%02d-%02d_%02dh.%02dm.%02ds_dummy_%s.tmp", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, codaAlignmentFilename );

	// Create a local copy of a null alignment file and then send it to the CODA server.
	FILE *fp = fopen( filename, "w" );
	fAbortMessageOnCondition( !fp, "CodaRTnetTracker", "Error opening alignment file %s for writing.", filename );
	fprintf( fp, "Dummy calibration file sent to clobber current alignment.\n" );
	fclose( fp );
	sprintf( command_line, "%sWinSCP.com /command \"open ftp://%s:%s@%s\" \"cd %s\" \"put %s %s\" \"exit\" ", 
		executablesPath, serverLogonID, serverPassword, serverAddress, codaCalDirectory, filename, codaAlignmentFilename );
	if ( 0 != system( command_line ) ) fAbortMessage( "CodaRTnetTracker", "Error executing FTP command:\n  %s", command_line );
	sprintf( command_line, "del %s",  filename );
	if ( 0 != system( command_line ) ) fAbortMessage( "CodaRTnetTracker", "Error executing FTP command:\n  %s", command_line );

}

// Given the pose of a reference object computed in the intrinsic reference frame of each CODA unit,
//  compute the alignment file that the CODA uses to align the units and send it to the server.
void  CodaRTnetTracker::SetAlignmentFromPoses( Pose pose[MAX_UNITS], const char *filename ) {

	char command_line[10240];
	Matrix3x3 rotation_matrix, transpose_matrix;
	Vector3 offset;

	// Create a unique filename for a local temporary file.
	// It can be overridden by the caller.
	char local_filename[MAX_PATH];
	if ( filename ) {
		strncpy( local_filename, filename, sizeof( local_filename ));
	}
	else {
		SYSTEMTIME st;
		GetSystemTime( &st );
		sprintf( local_filename, "%04d-%02d-%02d_%02dh.%02dm.%02ds_%s", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, codaAlignmentFilename );
	}

	// Open a file locally to accept the alignment information.
	FILE *fp = fopen( local_filename, "w" );
	fAbortMessageOnCondition( !fp, "CodaRTnetTracker", "Unable to open %s for writing.", local_filename );
	// This is an apparent header line.
	fprintf( fp, "[CODA System Alignment]\n" );
	// A block of parameters for each unit.
	for ( int unit = 0; unit < nUnits; unit++ ) {
		// The pose gives us the orientation as a quaternion. 
		// CODA wants it as a rotation matrix.
		QuaternionToMatrix( rotation_matrix, pose[unit].orientation );
		// The offset that the CODA appears to use is the computed from the measured offset
		// in intrinsic coordinates transformed by the inverse rotation (transpose).
		// NB The transpose may come from the fact that I use row vectors.
		// In any case, I came up with this by trial and error.
		TransposeMatrix( transpose_matrix, rotation_matrix );
		MultiplyVector( offset, pose[unit].position, transpose_matrix );
		// The serial numbers are recorded in the alignment file, so I need to put them here (I think).
		// But I don't know how to get the serial numbers from the CODA units that are connected.
		// So here I am hard coding the serial numbers from the GRIP science model. I will have to see
		//  what happens when we move to other hardware.
		fprintf( fp, "CX1SerialNumber%d=%s\n", unit, codaSerialNumber[unit] );
		// Note the negative signs for the offset. Again, trial and error.
		fprintf( fp, "Offset%d=%f,%f,%f\n", unit, - offset[X],  - offset[Y], - offset[Z] );
		fprintf( fp, "TransformX%d=%f,%f,%f\n", unit, rotation_matrix[X][X] , rotation_matrix[X][Y] , rotation_matrix[X][Z] );
		fprintf( fp, "TransformY%d=%f,%f,%f\n", unit, rotation_matrix[Y][X] , rotation_matrix[Y][Y] , rotation_matrix[Y][Z] );
		fprintf( fp, "TransformZ%d=%f,%f,%f\n", unit, rotation_matrix[Z][X] , rotation_matrix[Z][Y] , rotation_matrix[Z][Z] );
	}
	fclose( fp );
	// Here we send the new alignment file to the CODA server. 
	sprintf( command_line, "%sWinSCP.com /command \"open ftp://%s:%s@%s\" \"cd %s\" \"put %s %s\" \"exit\" ", 
		executablesPath, serverLogonID, serverPassword, serverAddress, codaCalDirectory, local_filename, codaAlignmentFilename );
	if ( 0 != system( command_line ) ) fAbortMessage( "SetAlignmentFromPoses", "Error executing ftp command:\n  %s", command_line );
	// We could clean up after ourselves by deleting the local file or moving it to a log location. 
	// I am leaving it in place. In GRASP I am going to copy it to a log file.
}	

/*********************************************************************************/

void CodaRTnetTracker::GetUnitTransform( int unit, Vector3 &offset, Matrix3x3 &rotation ) {

	DeviceInfoUnitCoordSystem coord;
	cl.getDeviceInfo( coord );

	CopyVector( offset, coord.dev.Rt[unit].t );
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			// If this seems backwards to you, it's because RTnet uses column vectors
			// and I use row vectors. So when RTnet does M * v, I need to do v * M'.
			rotation[i][j] = coord.dev.Rt[unit].R[j*3+i];
		}
	}

}

void CodaRTnetTracker::WriteMarkerFile( char *filename ) {
	FILE *fp = fopen( filename, "w" );
	fAbortMessageOnCondition( !fp, "Error opening %s for writing.", filename );
	fprintf( fp, "%s\n", filename );
	fprintf( fp, "Tracker Units: %d\n", nUnits );
	fprintf( fp, "Frame\tTime" );
	for ( int mrk = 0; mrk <nMarkers; mrk++ ) {
		for ( int unit = 0; unit < nUnits; unit++ ) {
			fprintf( fp, "\tM%02d.%1d.V\tM%02d.%1d.X\tM%02d.%1d.Y\tM%02d.%1d.Z", mrk, unit, mrk, unit, mrk, unit, mrk, unit  );
		}
	}
	fprintf( fp, "\n" );

	for ( int frm = 0; frm < nFrames; frm++ ) {
		fprintf( fp, "%05d %9.3f", frm, recordedMarkerFrames[0][frm].time );
		for ( int mrk = 0; mrk < nMarkers; mrk++ ) {
			for ( int unit = 0; unit < nUnits; unit++ ) {
				fprintf( fp, " %1d",  recordedMarkerFrames[unit][frm].marker[mrk].visibility );
				for ( int i = 0; i < 3; i++ ) fprintf( fp, " %9.3f",  recordedMarkerFrames[unit][frm].marker[mrk].position[i] );
			}
		}
		fprintf( fp, "\n" );
	}
	fclose( fp );
}


/****************************************************************************************/

// Helper Functions

// print_devicestatusarray_errors
// Helper function to print device errors (server-side errors)
// @param array Reference to DeviceStatusArray object thrown as an exception by RTNetClient
void CodaRTnetTracker::print_devicestatusarray_errors(const DeviceStatusArray& array)
{
	for (DWORD idev = 0; idev < array.numstatus; idev++)
	{
		if (array.status[idev].error)
		{
			fOutputDebugString( "DEVICE %u SUBSYSTEM %u ERROR: %u\n", 
				(DWORD)array.status[idev].deviceID, 
				(DWORD)array.status[idev].subsystemID,
				(DWORD)array.status[idev].error);
		}
	}
}

// print_network_error
// Helper function to network errors (client-side errors)
// @param exNet Network exception object thrown from RTNetClient or DataStream
void CodaRTnetTracker::print_network_error(const NetworkException& exNet)
{
	switch ( exNet.errorcode )
	{
	case CODANET_OK:
		fOutputDebugString( "Network error: CODANET_OK: NO ERROR CODE WAS PRODUCED ");
		break;
		
	case CODANET_SOCKETERROR_BROKEN:
		fOutputDebugString( "Network error: CODANET_SOCKETERROR_BROKEN ");
		break;
		
	case CODANET_SOCKETERROR_WINDOWSDLL:
		fOutputDebugString( "Network error: CODANET_SOCKETERROR_WINDOWSDLL ");
		break;
		
	case CODANET_SOCKETERROR_CREATE:
		fOutputDebugString( "Network error: CODANET_SOCKETERROR_CREATE ");
		break;
		
	case CODANET_SOCKETERROR_HOSTNAME:
		fOutputDebugString( "Network error: CODANET_SOCKETERROR_HOSTNAME ");
		break;
		
	case CODANET_SOCKETERROR_CONNECT:
		fOutputDebugString( "Network error: CODANET_SOCKETERROR_CONNECT ");
		break;
		
	case CODANET_SOCKETERROR_TCPOPTIONS:
		fOutputDebugString( "Network error: CODANET_SOCKETERROR_TCPOPTIONS ");
		break;
		
	case CODANET_CLIENTPROTOCOLERROR_BADNUMSTATUS:
		fOutputDebugString( "Network error: CODANET_CLIENTPROTOCOLERROR_BADNUMSTATUS ");
		break;
		
	case CODANET_SOCKETERROR_STREAMCREATE:
		fOutputDebugString( "Network error: CODANET_SOCKETERROR_STREAMCREATE ");
		break;
		
	case CODANET_SOCKETERROR_STREAMPORT:
		fOutputDebugString( "Network error: CODANET_SOCKETERROR_STREAMPORT ");
		break;
		
	case CODANET_CLIENTPROTOCOLERROR_TOOBIG:
		fOutputDebugString( "Network error: CODANET_CLIENTPROTOCOLERROR_TOOBIG ");
		break;
		
	default:
		fOutputDebugString( "Network error: UNKNOWN ERROR CODE %u ", codanet_getlasterror());
		break;
	}
	
}

// print_alignment_status
// Helper function to show alignment status
// @param marker_id_array 5-element array containing one-based marker identities for alignment markers
// @param info Alignment info retrieved from server
int CodaRTnetTracker::print_alignment_status( const DWORD* marker_id_array,  const DeviceInfoAlignment& info, DWORD MB )
{

	char message[10240] = "";
	char line[1024];
	// print alignment status value
	strcpy( message, "Alignment result: ");
	switch (info.dev.dwStatus)
	{
	case 0:
		strcat( message, "success");
		break;
	case CODANET_ALIGNMENTERROR_SYSTEM:
		strcat( message,  "system error");
		break;
	case CODANET_ALIGNMENTERROR_ALREADY_ACQUIRING:
		strcat( message,  "already acquiring (is another program running?");
		break;
	case CODANET_ALIGNMENTERROR_OCCLUSIONS:
		strcat( message,  "occlusions");
		break;
	case CODANET_ALIGNMENTERROR_XTOOCLOSE:
		strcat( message,  "x-axis markers too close");
		break;
	case CODANET_ALIGNMENTERROR_XYTOOCLOSE:
		strcat( message,  "xy-plane markers too close");
		break;
	case CODANET_ALIGNMENTERROR_NOTPERP:
		strcat( message,  "marked axes not sufficiently perpendicular");
		break;
	default:
		sprintf( line, "unknown alignment status error code %d", info.dev.dwStatus);
		strcat( message, line );
		break;
	}
	strcat( message, "\n");
	
	// number of CX1 units
	DWORD nunits = info.dev.dwNumUnits;
	
	// frame count
	DWORD nframes = info.dev.dwNumFrames;
	
	// print visibility information
	for (DWORD icoda = 0; icoda < nunits; icoda++)
	{
		// index of Codamotion CX1 unit
		sprintf( line, "Coda %d\n", icoda+1);
		strcat( message, line );
		
		// data from each marker
		for (DWORD imarker = 0; imarker < 5; imarker++)
		{
			// actual marker identity
			DWORD marker_identity = marker_id_array[imarker];
			
			for (DWORD icam = 0; icam < 3; icam++)
			{
				// label for this marker
				switch (imarker)
				{
				case 0:
					strcat( message, "Org");
					break;
				case 1:
					strcat( message,  "X0  ");
					break;
				case 2:
					strcat( message, "X1  ");
					break;
				case 3:
					strcat( message, "XY0");
					break;
				case 4:
					strcat( message, "XY1");
					break;
				}
				
				// print marker identity
				sprintf( line, " (Marker %02d) ", marker_identity);
				strcat( message, line );
				
				// camera letter for each coda camera
				switch (icam)
				{
				case 0:
					strcat( message, "A");
					break;
				case 1:
					strcat( message, "B");
					break;
				case 2:
					strcat( message, "C");
					break;
				}
				
				// space
				strcat( message, ": ");
				
				// print visibility graph for frames of data
				// show a 1 for visible and _ for occluded
				// (show alternate frames only to save screen space)
				for (DWORD iframe = 0; iframe < nframes; iframe+=2)
				{
					BYTE flag = info.dev.camera_flag[3*nframes*5*icoda + 3*nframes*imarker + 3*iframe + icam];
					if (flag <= 10)
						strcat( message, "-");
					else
						strcat( message, "1");
				}
				
				// new line
				strcat( message, "\n");
			}
		}
	}

	int response = MessageBox( NULL, message, "Coda RTnet Alignment", MB );
	return( response );

}



