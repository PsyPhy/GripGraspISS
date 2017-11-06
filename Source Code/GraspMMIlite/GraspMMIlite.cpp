// GraspMMIlite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>

#include "..\Grip\GripPackets.h"
#include "..\GraspMMIUtilities\GraspMMIUtilities.h"

#include "..\VectorsMixin\VectorsMixin.h"
#include "..\Useful\fMessageBox.h"
#include "..\Useful\fOutputDebugString.h"

#include "..\Grasp\Grasp.h"
#include "..\GraspGUI\GraspGUI.h"
#include "..\AlignToRigidBodyGUI\AligntoRigidBodyGUI.h"

EPMTelemetryPacket			hkPacket;
EPMTelemetryPacket			rtPacket;
EPMTelemetryHeaderInfo		epmHeader;
GripHealthAndStatusInfo		hkInfo;
GraspMMI::GraspRealtimeDataInfo		rtInfo;

// A local instance of VectorsMixin to perform vector and quaternion operations.
static PsyPhy::VectorsMixin vm;


// Default path for packet storage is the current directory.
const char *packetCacheFilenameRoot = ".\\GraspPackets";

// Buffers to hold the path to the packet caches.
char rtPacketCacheFilePath[1024];
char hkPacketCacheFilePath[1024];

int main( int argc, char *argv[] )
{

	BOOL new_hk;
	BOOL new_rt;

	int i;

	if ( argc < 2 ) printf( "Using default root path for cache files: %s\n", packetCacheFilenameRoot );
	else {
		packetCacheFilenameRoot = argv[1];
		printf( "Using command-line root path for cache files: %s\n", packetCacheFilenameRoot );
	}

	// Construct the paths to the cache files where we expect to read the incoming packets.
	CreateGripPacketCacheFilename( hkPacketCacheFilePath, sizeof( hkPacketCacheFilePath ), GRIP_HK_BULK_PACKET, packetCacheFilenameRoot );
	CreateGripPacketCacheFilename( rtPacketCacheFilePath, sizeof( rtPacketCacheFilePath ), GRIP_RT_SCIENCE_PACKET, packetCacheFilenameRoot );

	// Attempt to open the packet cache to read the accumulated packets.
	// If it is not immediately available, try for a few seconds then query the user.
	// The user can choose to continue to wait or cancel program execution.
	int retry_count;
	int return_code;
	int  fid;
	int packets_read = 0;
	char *filename = rtPacketCacheFilePath;
	int bytes_read;	do {
		for ( retry_count = 0; retry_count  < MAX_OPEN_CACHE_RETRIES; retry_count ++ ) {
			// If open succeeds, it will return zero. So if zero return, break from retry loop.
			return_code = _sopen_s( &fid, filename, _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IWRITE | _S_IREAD  );
			if ( return_code == 0 ) break;
			// Wait a second before trying again.
			Sleep( RETRY_PAUSE );
		}
		// If return_code is zero, file is open, so break out of loop and continue.
		if ( return_code == 0 ) break;
		// If return_code is non-zero, we are here because the retry count has been reached without opening the file.
		// Ask the user if they want to keep on trying or abort.
		else {
			int mb_answer = fMessageBox( MB_RETRYCANCEL, "GraspMMIlite", "Error opening %s for binary read.\nContinue trying?", filename );
			if ( mb_answer == IDCANCEL ) exit( ERROR_CACHE_NOT_FOUND ); // User chose to abort.
		}
	} while ( true ); // Keep trying until success or until user cancels.

	// Read in all of the data packets in the file.
	packets_read = 0;
	while ( rtPacketLengthInBytes == (bytes_read = _read( fid, &rtPacket, rtPacketLengthInBytes )) ) {
		packets_read++;
		if ( bytes_read < 0 ) {
			fMessageBox( MB_OK, "GraspMMIlite", "Error reading from %s.", filename );
			exit( -1 );
		}
		// Check that it is a valid GRIP packet. It would be strange if it was not.
		ExtractEPMTelemetryHeaderInfo( &epmHeader, &rtPacket );
		if ( epmHeader.epmSyncMarker != EPM_TELEMETRY_SYNC_VALUE || epmHeader.TMIdentifier != GRIP_RT_ID ) {
			fMessageBox( MB_OK, "GraspMMIlite", "Unrecognized packet from %s.", filename );
			exit( -1 );
		}
		// Show some of the realtime science data.
		ExtractGraspRealtimeDataInfo( rtInfo, rtPacket );
		for ( int i = 0; i < GRASP_RT_SLICES_PER_PACKET; i++ ) {
			if ( strcmp( (char *) rtInfo.dataSlice[i].clientData, "NULL" ) ) {
				printf( "\nFill Time: %8.3f Frame Time: %8.3f    Client Time: %8.3f Client Tag: %s", 
					rtInfo.dataSlice[i].fillTime,  
					rtInfo.dataSlice[i].codaFrame.time,
					rtInfo.dataSlice[i].clientTime,
					rtInfo.dataSlice[i].clientData );

				if ( !strcmp( "GRASP", (char *) rtInfo.dataSlice[i].clientData ) ) {
					Grasp::GraspClientData *grasp = (Grasp::GraspClientData *)  rtInfo.dataSlice[i].clientData;
					printf( "  %d %6s %08x", grasp->currentTrial, Grasp::GraspParadigmString[grasp->paradigm], grasp->currentState );
					Pose	pose;
					char	*object;
					int		visible;
//					vm.CopyPose( pose, grasp->headPose ); object = "Head"; visible = rtInfo.dataSlice[i].HMD.visible;
//					vm.CopyPose( pose, grasp->handPose ); object = "Hand"; visible = rtInfo.dataSlice[i].hand.visible;
					vm.CopyPose( pose, grasp->chestPose ); object = "Chest"; visible = rtInfo.dataSlice[i].chest.visible;
					printf( "  %5s: %d %s %s", 
						object, visible, vm.vstr( pose.position ), vm.qstr( pose.orientation ) );
				}

				if ( !strcmp( "GRSPGUI", (char *) rtInfo.dataSlice[i].clientData ) ) {
					GraspGUI::GraspActionSlice *action = (GraspGUI::GraspActionSlice *) rtInfo.dataSlice[i].clientData;
					for (int j = 0; j < GUI_ITEMS_IN_SLICE; j++ ) {
						printf( "  %03d.%03d.%05d", action->record[j].task, action->record[j].step, action->record[j].code );
					}
				}
			
				if ( !strcmp( "ALIGN", (char *) rtInfo.dataSlice[i].clientData ) ) {
					AlignToRigidBodyGUI::AlignClientBuffer *align = (AlignToRigidBodyGUI::AlignClientBuffer *) rtInfo.dataSlice[i].clientData;
					printf( " %4s", ( align->prePost == PRE ? "PRE" : "POST" ) );
					for ( int unit = 0; unit < MAX_UNITS; unit++ ) {
						printf( "   Coda %d: %s %s", unit,
							vm.vstr( align->offsets[unit], "<%+6.1f %+6.1f %+6.1f>" ), 
							vm.mstr( align->rotations[unit], "[%+4.3f %+4.3f %+4.3f | %+4.3f %+4.3f %+4.3f | %+4.3f %+4.3f %+4.3f]" ) );
					}
				}
			}
		}

		if ( 0 == ( packets_read % 1000 ) ) {
			printf( "\nPress a key to continue ..." );
			fflush( stdout );
			int c = _getch();
			if ( ESCAPE == c ) exit( 0 );
		}
	}
	// Finished reading. Close the file and check for errors.
	return_code = _close( fid );

	if ( return_code ) {
		fMessageBox( MB_OK, "GraspMMIlite", "Error closing %s after binary read.\nError code: %s", filename, return_code );
		exit( return_code );
	}

	printf( "\n\nPress any key to exit ..." );
	fflush( stdout );
	_getch();

	return 0;
}

