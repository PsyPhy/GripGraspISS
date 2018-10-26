// This is the main DLL file.

#include "stdafx.h"
#include "..\DexServices\TMdata.h"

#include "../Trackers/Trackers.h"
#include "..\Grasp\Grasp.h"
#include "..\GraspGUI\GraspGUI.h"
#include "..\AlignToRigidBodyGUI\AligntoRigidBodyGUI.h"

#include "GraspMMIUtilities.h"

using namespace GraspMMI;
using namespace Grasp;
using namespace PsyPhy;

///
/// Extract the individual slices of data from a Grasp Realtime telemetry packet.
/// NB It is up to the caller to ensure that the array of slices has enough room 
/// to hold all the slices (GRASP_RT_SLICES_PER_PACKET) in the packet.
///

namespace GraspMMI {

	bool LoadGraspCodaAlignment ( GraspRealtimeDataSlice *slice, const char *filename ) {

		// Read a file containing the CODA alignment transformations stored by GRASP.
		static VectorsMixin vm;

		FILE *fp = fopen( filename, "r" );
		if ( !fp ) {
			fMessageBox( MB_OK, "GraspMMIUtilities", "Error opening GRASP alignment %s for read.", filename );
			return false;
		}
		else {
			char	line[1024];
			int		items;

			PsyPhy::Matrix3x3	r, inv;
			PsyPhy::Vector3		p;
			char field[1024];
			fOutputDebugString( "File %s open for reading.\n", filename );

			// Skip the header.
			fgets( line, sizeof( line ), fp );

			// Serial Number
			fgets( field, sizeof(field), fp );
			// Offset
			items = fscanf( fp, "Offset0=%lf,%lf,%lf\n", &p[X], &p[Y], &p[Z] );
			if ( items < 3 ) return false;
			// Rotation
			items = fscanf( fp, "TransformX0=%lf,%lf,%lf\n", &r[X][X], &r[X][Y], &r[X][Z] );
			if ( items < 3 ) return false;
			items = fscanf( fp, "TransformY0=%lf,%lf,%lf\n", &r[Y][X], &r[Y][Y], &r[Y][Z] );
			if ( items < 3 ) return false;
			items = fscanf( fp, "TransformZ0=%lf,%lf,%lf\n", &r[Z][X], &r[Z][Y], &r[Z][Z] );
			if ( items < 3 ) return false;

			vm.InvertMatrix( inv, r );
			vm.CopyVector( slice->alignmentOffset[0], p );
			vm.CopyMatrix( slice->alignmentRotation[0], r );


			// Serial Number
			fgets( field, sizeof(field), fp );
			// Offset
			items = fscanf( fp, "Offset1=%lf,%lf,%lf\n", &p[X], &p[Y], &p[Z] );
			if ( items < 3 ) return false;
			// Rotation
			items = fscanf( fp, "TransformX1=%lf,%lf,%lf\n", &r[X][X], &r[X][Y], &r[X][Z] );
			if ( items < 3 ) return false;
			items = fscanf( fp, "TransformY1=%lf,%lf,%lf\n", &r[Y][X], &r[Y][Y], &r[Y][Z] );
			if ( items < 3 ) return false;
			items = fscanf( fp, "TransformZ1=%lf,%lf,%lf\n", &r[Z][X], &r[Z][Y], &r[Z][Z] );
			if ( items < 3 ) return false;

			vm.InvertMatrix( inv, r );
			vm.CopyVector( slice->alignmentOffset[1], p );
			vm.CopyMatrix( slice->alignmentRotation[1], inv );

			slice->clientType = GraspRealtimeDataSlice::ALIGNPOST;

			fclose( fp );
			return true;
		}
	}


	void ComputeGraspRTDerivedValues( GraspRealtimeDataSlice *slice ) {

		static VectorsMixin vm;

		// Compute some pertinent angles from the orientation information.
		if ( slice->HMD.visible ) {
			slice->hmdRotationAngle = vm.ToDegrees( vm.RotationAngle( slice->HMD.pose.orientation ) );
			slice->hmdRollAngle = vm.ToDegrees( vm.RollAngle( slice->HMD.pose.orientation ) );
		}
		else {
			vm.CopyPose( slice->HMD.pose, vm.missingPose );
			slice->hmdRotationAngle = MISSING_DOUBLE;
			slice->hmdRollAngle = MISSING_DOUBLE;
		}
		if ( slice->hand.visible ) {
			slice->handRotationAngle = vm.ToDegrees( vm.RotationAngle( slice->hand.pose.orientation ) );
			slice->handRollAngle = vm.ToDegrees( vm.RollAngle( slice->hand.pose.orientation ) );
		}
		else {
			vm.CopyPose( slice->hand.pose, vm.missingPose );
			slice->handRotationAngle = MISSING_DOUBLE;
			slice->handRollAngle = MISSING_DOUBLE;
		}
		if ( slice->chest.visible ) {
			slice->chestRotationAngle = vm.ToDegrees( vm.RotationAngle( slice->chest.pose.orientation ) );
			slice->chestRollAngle = vm.ToDegrees( vm.RollAngle( slice->chest.pose.orientation ) );
		}
		else {
			vm.CopyPose( slice->chest.pose, vm.missingPose );
			slice->chestRotationAngle = MISSING_DOUBLE;
			slice->chestRollAngle = MISSING_DOUBLE;
		}
		if ( slice->chest.visible && slice->HMD.visible ) {
			Vector3 delta;
			vm.SubtractVectors( delta, slice->HMD.pose.position, slice->chest.pose.position );
			slice->torsoRollAngle = vm.ToDegrees( atan2( - delta[X], delta[Y] ) );
		}
		else {
			slice->torsoRollAngle = MISSING_DOUBLE;
		}

	}

	u32 ExtractGraspRealtimeDataSliceContent( GraspRealtimeDataSlice *slice, u8 *buffer, u32 p ) {

		static VectorsMixin vm;
		for ( int i = 0; i < GRASP_RT_SLICES_PER_PACKET; i++ ) {

			p = extractTM( slice[i].fillTime, buffer, p );
			p = extractTM( slice[i].globalCount, buffer, p );
			p = extractTM( slice[i].HMD, buffer, p );
			p = extractTM( slice[i].hand, buffer, p );
			p = extractTM( slice[i].chest, buffer, p );

			p = extractTM( slice[i].codaUnit, buffer, p );
			p = extractTM( slice[i].codaFrame[ slice[i].codaUnit ], buffer, p );

			p = extractTM( slice[i].clientTime, buffer, p );
			p = extractTM( slice[i].clientData, sizeof( slice[i].clientData ), buffer, p );

			// It is convenient to compute some derived values here.
			ComputeGraspRTDerivedValues( &slice[i] );

			slice[i].clientType = GraspRealtimeDataSlice::NONE;
			if ( !strcmp( "GRASP", (char *) slice[i].clientData ) ) {
				Grasp::GraspClientData *grasp = (Grasp::GraspClientData *)  &slice[i].clientData;
				slice[i].clientType = GraspRealtimeDataSlice::GRASP;
				vm.CopyPose( slice[i].headPose.pose, grasp->headPose ); 
				vm.CopyPose( slice[i].handPose.pose, grasp->handPose ); 
				vm.CopyPose( slice[i].chestPose.pose, grasp->chestPose ); 
				vm.CopyQuaternion( slice[i].rollQuaternion, grasp->rollQuaternion ); 
				slice[i].targetOrientation =  (double) grasp->targetOrientationD / 100.0;
				slice[i].enableBits = grasp->enableBits;
				slice[i].spinnerBits = grasp->spinnerBits;
			}

			if ( !strcmp( "GRSPGUI", (char *) slice[i].clientData ) ) {
				GraspGUI::GraspActionSlice *action = (GraspGUI::GraspActionSlice *) &slice[i].clientData;
				//for (int j = 0; j < GUI_ITEMS_IN_SLICE; j++ ) {
				//	printf( "  %03d.%03d.%05d", action->record[j].task, action->record[j].step, action->record[j].code );
				//}
			}

			if ( !strcmp( "ALIGN", (char *) slice[i].clientData ) ) {
				AlignToRigidBodyGUI::AlignClientBuffer *align = (AlignToRigidBodyGUI::AlignClientBuffer *) &slice[i].clientData;
				if ( align->prePost == PRE ) slice[i].clientType = GraspRealtimeDataSlice::ALIGNPRE;
				else  slice[i].clientType = GraspRealtimeDataSlice::ALIGNPOST;
				for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
					vm.CopyVector( slice[i].alignmentOffset[unit], align->offsets[unit] );
					vm.CopyMatrix( slice[i].alignmentRotation[unit], align->rotations[unit] );
				}
			}

		}

		return( p );

	}

	void ExtractGraspRealtimeDataSlices( GraspRealtimeDataSlice *slice, EPMTelemetryPacket &packet ) {

		u8 *buffer = packet.sections.rawData;
		u32 p = 0;

		// Each packet includes a small amount of data about the packet itself, then the actual data.
		// Here we extract the packet info, but it does not go into the data arrays.
		unsigned int acquisitionID;
		unsigned int packetCount;

		p = extractTM( acquisitionID, buffer, p );
		p = extractTM( packetCount, buffer, p );

		// Extract the slices.
		p = ExtractGraspRealtimeDataSliceContent( slice, buffer, p );

	}

	void ExtractGraspRealtimeDataInfo( GraspRealtimeDataInfo &info, EPMTelemetryPacket &packet ) {

		u8 *buffer = packet.sections.rawData;
		u32 p = 0;

		// Each packet includes a small amount of data about the packet itself, then the actual data.
		p = extractTM( info.acquisitionID, buffer, p );
		p = extractTM( info.packetCount, buffer, p );
		// Extract the slices.
		p = ExtractGraspRealtimeDataSliceContent( info.dataSlice, buffer, p );

	}

	///
	/// Read realtime GRASP science data packets into a local buffer.
	///

	int GetGraspRT( GraspRealtimeDataSlice grasp_data_slice[], int max_slices, char *filename_root ) {

		// Keep track of the last packet TM counter from previous call.
		// This is how we know if new data has arrived.
		static unsigned short previousTMCounter = 0;

		// Keep track of whether we have already seen that the buffers are full.
		static BOOL buffers_full_alert = FALSE;

		// Buffers and structures to hold data from the real time science packets.
		EPMTelemetryPacket		packet;
		EPMTelemetryHeaderInfo	epmHeader;

		// Will hold the filename (path) of the packet file.
		char filename[MAX_PATHLENGTH];
		// Will hold the pointer to the open packet file.
		int  fid;

		// Various local counters and flags.
		int bytes_read;
		int packets_read;
		int return_code;
		int n_slices;

		// Create the path to the realtime science packet file, based on the root and the packet type.
		// The global variable 'packetBufferPathRoot' has been initialized elsewhere.
		CreateGripPacketCacheFilename( filename, sizeof( filename ), GRIP_RT_SCIENCE_PACKET, filename_root );	

		// Attempt to open the packet cache to read the accumulated packets.
		// If it is not immediately available, keep trying for a few seconds.
		for ( int retry_count = 0; retry_count  < MAX_OPEN_CACHE_RETRIES; retry_count ++ ) {
			// Try to open the packet cache file.
			fid = _sopen( filename, _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IWRITE | _S_IREAD  );
			// If open succeeds, it will return zero. So if zero return, break from retry loop.
			if ( fid >= 0 ) break;
			// Wait a second before trying again.
			Sleep( RETRY_PAUSE );
		}
		// If fid is negative, file is not open. This should not happen, because GripMMIStartup should verify 
		// the availability of files containing packets before the GripMMIDesktop form is executed.
		// But if we do fail to open the file, fake a single frame of data with everything missing.
		if ( fid < 0 ) {
			//fMessageBox( MB_OK, "GripMMI", "Error opening packet file %s.\n\n", filename );
			//exit( -1 );
			n_slices = 0;
			grasp_data_slice[n_slices].fillTime = MISSING_FLOAT;
			grasp_data_slice[n_slices].absoluteTime = MISSING_DOUBLE;
			n_slices++;
			return( n_slices );
		}

		// Prepare for reading in packets. This is used to calculate the elapsed time between two packets.
		// By setting it to a really high number here (100 years into the future) we are sure that the 
		//  calculated difference between this value and the first frame will be negative, as if there was no gap.
		double previous_packet_timestamp = 100.0 * 365.0 * 24.0 * 60.0 * 60.0;

		// Similarly, we look for resets of the fill time for each slice, and when the latest fill time is less
		// than the previous fill time, we reset the reference time to the time that the packet was received.
		// By setting the previous fill time to a really large number, the reference time will be established 
		// by the packet time of the first packet and slice.
		double previous_fill_time = 100.0 * 365.0 * 24.0 * 60.0 * 60.0;
		double absolute_time_reference = 0.0;

		// Read in all of the data packets in the file.
		// Be careful not to overrun the data buffers.
		packets_read = 0;
		n_slices = 0;
		while ( n_slices < max_slices ) {

			// Attempt to read next packet. Any error is terminal.
			bytes_read = _read( fid, &packet, rtPacketLengthInBytes );
			if ( bytes_read < 0 ) {
				fMessageBox( MB_OK, "GraspMMI", "Error reading from %s.\n\n", filename );
				exit( -1 );
			}

			// If the number of bytes read is less than the expected number
			//  we are at the end of the file and should break out of the loop.
			if ( rtPacketLengthInBytes != bytes_read ) break;

			// We have a valid packet.
			packets_read++;

			// Check that it is a valid GRIP packet. It would be strange if it was not, because we are supposed
			//  to be reading a cache file that has been filled by DexGroundMonitorClient with only GRIP RT packets.
			ExtractEPMTelemetryHeaderInfo( &epmHeader, &packet );
			if ( epmHeader.epmSyncMarker != EPM_TELEMETRY_SYNC_VALUE || epmHeader.TMIdentifier != GRIP_RT_ID ) {
				// fMessageBox( MB_OK, "GripMMIlite", "Unrecognized packet from %s.\n\n%s", filename );
				return( -1 );
			}

			// If there has been a break in the arrival of the packets, insert
			//  a blank frame into the data buffer. This will cause breaks in
			//  the traces in the data graphs.
			if ( ( EPMtoSeconds( &epmHeader ) - previous_packet_timestamp ) > PACKET_STREAM_BREAK_THRESHOLD ) {
				// Subsampling in graphs will be used when the data record is very long.
				// Insert enough points so that we see the break even if we are sub-sampling in the graphs.
				for ( int count = 0; count < PACKET_STREAM_BREAK_INSERT_SAMPLES && n_slices < max_slices; count++ ) {
					grasp_data_slice[n_slices].fillTime = MISSING_FLOAT;
					grasp_data_slice[n_slices].absoluteTime = MISSING_DOUBLE;
					grasp_data_slice[n_slices].HMD.visible = false;
					grasp_data_slice[n_slices].hand.visible = false;
					grasp_data_slice[n_slices].chest.visible = false;
					grasp_data_slice[n_slices].codaUnit = MISSING_CHAR;
					for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
						for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) grasp_data_slice[n_slices].codaFrame[unit].marker[mrk].visibility = false;
					}
					n_slices++;
				}
			}

			// Each packet is a set of slices. Extract each one.
			if ( n_slices < max_slices - GRASP_RT_SLICES_PER_PACKET ) {
				ExtractGraspRealtimeDataSlices( &grasp_data_slice[n_slices],  packet );
#if 0
				// We need to map each data slice to an absolute point in time.
				// Each packet has a timestamp generated by EPM with respect to our absolute time reference.
				// But it does not give the time of each individual slice within the packet.
				// Each slice in a packet includes the fillTime, which is the time that the data in the packet
				// was obtained with respect to the start of an acquisition. But each time that Grasp.exe is started
				// the fillTime timer starts from zero. We equate the absoluteTime of the first packet with the 
				// fillTime of the last slice in that packet, and use that reference to calculate the absoluteTime of
				// each subsequent slice in the same packet and in each subsequent packet. If the fillTime decreases
				// from one packet to the next, we know that a new acquistion was started and we remap the fillTime of 
				// that slice to the timestamp of the packet.
				double fill_time = grasp_data_slice[n_slices + GRASP_RT_SLICES_PER_PACKET - 1].fillTime;
				double fill_time_increment = fill_time - previous_fill_time;
				if ( fill_time_increment < 0.0 || ( ( EPMtoSeconds( &epmHeader ) - previous_packet_timestamp ) > PACKET_STREAM_BREAK_THRESHOLD ) ) {
					absolute_time_reference = EPMtoSeconds( &epmHeader ) - fill_time;
					fOutputDebugString( "Slice: %d Header: %f  fillTime: %f  reference: %f\n", n_slices, EPMtoSeconds( &epmHeader ), fill_time, absolute_time_reference );
				}
				for ( int s = 0; s < GRASP_RT_SLICES_PER_PACKET; s++ ) {
					grasp_data_slice[n_slices + s].absoluteTime = grasp_data_slice[n_slices + s].fillTime  + absolute_time_reference;
					fOutputDebugString( "  Slice: %d Header: %.3f  fillTime: %8.3f  reference: %.3f  absoluteTime: %.3f\n", 
						n_slices + s, EPMtoSeconds( &epmHeader ), grasp_data_slice[n_slices + s].fillTime, 
						absolute_time_reference, grasp_data_slice[n_slices + s].absoluteTime );
				}
#else 
				// In this alternate version, we just give the last slice the same time as the EPM packet time and
				//  then compute the time of the other packets based on differences in fill itmes.  This may be 
				//	somewhat less accurate, but the above method causes problems if there is skew between the 
				//  clock that timestamps the EPM packets and the fillTime clock on the GRASP machine.
				// The effect here is that time can sometimes move backwards.
				for ( int s = 0; s < GRASP_RT_SLICES_PER_PACKET; s++ ) {
					grasp_data_slice[n_slices + s].absoluteTime = EPMtoSeconds( &epmHeader )
						- ( grasp_data_slice[n_slices + GRASP_RT_SLICES_PER_PACKET - 1].fillTime - grasp_data_slice[n_slices + s].fillTime );
					//fOutputDebugString( "  Slice: %4d Header: %.3f  fillTime: %8.3f absoluteTime: %.3f  delta: %.3f\n", 
					//	n_slices + s, EPMtoSeconds( &epmHeader ), grasp_data_slice[n_slices + s].fillTime, 
					//	grasp_data_slice[n_slices + s].absoluteTime,
					//	( n_slices > 0 ? grasp_data_slice[n_slices + s].absoluteTime - grasp_data_slice[n_slices + s - 1].absoluteTime : 0.0 ));
				}
#endif
			}
			previous_packet_timestamp = EPMtoSeconds( &epmHeader );
			previous_fill_time = grasp_data_slice[n_slices + GRASP_RT_SLICES_PER_PACKET - 1].fillTime;
			n_slices += GRASP_RT_SLICES_PER_PACKET;

		}
		// Finished reading. Close the file and check for errors.
		return_code = _close( fid );
		if ( return_code ) {
			fMessageBox( MB_OK, "GraspMMI", "Error closing %s after binary read.\nError code: %s\n\n", filename, return_code );
			exit( return_code );
		}
		fOutputDebugString( "Acquired Data Slices (max %d): %d\n", max_slices, n_slices );
		if ( n_slices >= max_slices ) {
			char filename2[MAX_PATHLENGTH]; 
			CreateGripPacketCacheFilename( filename2, sizeof( filename ), GRIP_HK_BULK_PACKET, filename_root );
			fMessageBox( MB_OK | MB_ICONERROR, "DexServices", 
				"Internal buffers are full.\n\nYou can continue plotting existing data.\nTracking of script progress will also continue.\n\nTo resume following new data transmissions:\n\n1) Halt all processes.\n2) Rename or move:\n      %s\n      %s\n3) Restart using RunGraspMMI.bat.",
				filename, filename2 );
			// Signal for the next call that we have already reached the limit of the buffers.
			buffers_full_alert = true;
		}

		// Each realtime slice has only one marker frame, corresponding to one or the other coda.
		// If the preceding slice contains the other frame, we copy it here.
		// Otherwise we set all of the markers in the missing frame to be occluded.
		for ( int i = 0; i < n_slices; i++ ) {
			for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
				if ( unit != grasp_data_slice[i].codaUnit ) {
					if ( ( i > 0 ) && ( grasp_data_slice[i-1].codaUnit == unit ) ) CopyMarkerFrame( grasp_data_slice[i].codaFrame[unit], grasp_data_slice[i-1].codaFrame[unit] );
					else OccludeMarkerFrame( grasp_data_slice[i].codaFrame[unit] );
				}
			}
		}

		return ( n_slices );
	}

	/// Read housekeeping cache and create a trace of the tasks that have been performed.
	/// The path to the cache file is presumed to be set in global variable 'packetBufferPathRoot'.

	int GetHousekeepingTrace( GraspHousekeepingSlice *trace, int max_slices, char *filename_root ) {

		static int count = 0;

		int  fid;
		int packets_read = 0;
		int bytes_read;
		int return_code;
		unsigned long bit = 0;
		int retry_count;

		EPMTelemetryPacket		packet;
		EPMTelemetryHeaderInfo	epm_header;
		GripHealthAndStatusInfo hk;

		char filename[MAX_PATHLENGTH];

		// Create the path to the housekeeping packet file, based on the root and the packet type.
		CreateGripPacketCacheFilename( filename, sizeof( filename ), GRIP_HK_BULK_PACKET, filename_root );

		// Attempt to open the packet cache to read the accumulated packets.
		// If it is not immediately available, try for a few seconds.
		for ( retry_count = 0; retry_count  < MAX_OPEN_CACHE_RETRIES; retry_count ++ ) {
			// Try to open the packet cache file.
			fid = _open( filename, _O_RDONLY | _O_BINARY, _S_IWRITE | _S_IREAD  );
			// If open succeeds, it will return zero. So if zero return, break from retry loop.
			if ( fid >= 0 ) break;
			// Wait a second before trying again.
			Sleep( RETRY_PAUSE );
		}
		// If fid is negative, file is not open. This should not happen, because GripMMIStartup should verify 
		// the availability of files containing packets before the GripMMIDesktop form is executed.
		// So if we do fail to open the file, signal the error and exit.
		if ( fid < 0 ) {
			fMessageBox( MB_OK, "GraspMMI", "Error reading from %s.", filename );
			exit( -1 );
		}

		// Prepare for reading in packets. This is used to calculate the elapsed time between two packets.
		// By setting it to a really high number here (100 years into the future) we are sure that the 
		//  calculated difference between this value and the first frame will be negative, as if there was no gap.
		double previous_packet_timestamp = 100.0 * 365.0 * 24.0 * 60.0 * 60.0;

		// Read in all of the data packets in the file.
		packets_read = 0;
		while ( 1 ) {
			bytes_read = _read( fid, &packet, hkPacketLengthInBytes );
			// Return less than zero means read error.
			if ( bytes_read < 0 ) {
				fMessageBox( MB_OK, "GraspMMI", "Error reading from %s.", filename );
				exit( -1 );
			}
			// Return less than expected number of bytes means we have read all packets.
			if ( bytes_read < hkPacketLengthInBytes ) break;

			// Check that it is a valid GRIP packet. It would be strange if it was not.
			ExtractEPMTelemetryHeaderInfo( &epm_header, &packet );
			if ( epm_header.epmSyncMarker != EPM_TELEMETRY_SYNC_VALUE || epm_header.TMIdentifier != GRIP_HK_ID ) {
				fMessageBox( MB_OK, "GraspMMI", "Unrecognized packet from %s.", filename );
				exit( -1 );
			}
			// If there has been a break in the arrival of the packets, insert
			//  a blank frame into the data buffer. This will cause breaks in
			//  the traces in the data graphs.
			if ( ( EPMtoSeconds( &epm_header ) - previous_packet_timestamp ) > PACKET_STREAM_BREAK_THRESHOLD ) {
				trace[packets_read].absoluteTime = MISSING_DOUBLE;
				trace[packets_read].stepID = MISSING_DOUBLE;
				trace[packets_read].taskID = MISSING_DOUBLE;
				trace[packets_read].protocolID = MISSING_DOUBLE;
				trace[packets_read].userID = MISSING_DOUBLE;
				trace[packets_read].scriptEngine = MISSING_INT;
				packets_read++;
				if ( packets_read >= max_slices ) break;
			}

			// Extract the interesting info in proper byte order.
			ExtractGripHealthAndStatusInfo( &hk, &packet );
			// Fill in the different components of the housekeeping history trace.
			trace[packets_read].absoluteTime = EPMtoSeconds( &epm_header );
			if ( hk.step == 0 && hk.task == 0 && hk.protocol == 0 && hk.user == 0 ) {
				trace[packets_read].stepID = MISSING_DOUBLE;
				trace[packets_read].taskID = MISSING_DOUBLE;
				trace[packets_read].protocolID = MISSING_DOUBLE;
				trace[packets_read].userID = MISSING_DOUBLE;
				trace[packets_read].scriptEngine = MISSING_INT;
			}
			else {
				trace[packets_read].stepID = hk.step;
				trace[packets_read].taskID = hk.task;
				trace[packets_read].protocolID = hk.protocol;
				trace[packets_read].userID = hk.user;
				trace[packets_read].scriptEngine = hk.scriptEngineStatusEnum;
			}
			for ( int bdy = 0; bdy < MARKER_STRUCTURES; bdy++ ) {
				if ( hk.motionTrackerStatusEnum < TRACKER_ANOMALY ) {
					int visible = (hk.motionTrackerStatusEnum / (int) pow( 10.0, MARKER_STRUCTURES - bdy - 1 )) % 10;
					trace[packets_read].visibleMarkers[bdy] = visible;
				}
				else trace[packets_read].visibleMarkers[bdy] = MISSING_DOUBLE;
			}
			previous_packet_timestamp = EPMtoSeconds( &epm_header );
			packets_read++;
			if ( packets_read >= max_slices ) break;
		}
		// Finished reading. Close the file and check for errors.
		return_code = _close( fid );
		if ( return_code ) {
			fMessageBox( MB_OK, "GraspMMI", "Error closing %s after binary read.\nError code: %s", filename, return_code );
			exit( return_code );
		}

		return( packets_read );
	}

}