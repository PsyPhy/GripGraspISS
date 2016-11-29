// GraspTelemetryDaemon.cpp : main project file.

#include "stdafx.h"

#include "Form1.h"
#include "../DexServices/TMdata.h"
#include "../DexServices/DexServices.h"
#include "../../../GripMMI/GripSourceCode/Grip/GripPackets.h"
#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

using namespace GraspTelemetryDaemon;
using namespace Grasp;
using namespace PsyPhy;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
	f32 fillTime;
	u32 globalCount;
	u32 objectStateBits;
	PsyPhy::TrackerPose	HMD;
	PsyPhy::TrackerPose	codaHMD;
	PsyPhy::TrackerPose hand;
	PsyPhy::TrackerPose chest;
	PsyPhy::TrackerPose	mouse;
} GraspRealtimeDataSlice;

#define MAX_SLICES	(24*60*60*10)
GraspRealtimeDataSlice graspDataSlice[MAX_SLICES];

int GetGraspRT( GraspRealtimeDataSlice grasp_data_slice[], int max_slices, char *filename_root );

void ExtractGraspRealtimeDataInfo( GraspRealtimeDataSlice grasp_data_slice[], EPMTelemetryPacket &packet ) {

	u8 *buffer = packet.sections.rawData;
	u32 p = 0;
	
	// Packets that are sent to the GraspASW include a special sync word and packet type.
	// But these do not get put into the data payload that is included in the DEX telemetry packet.
	// I just leave this here as a reminder of the difference between what is sent to and from DEX.
	// u32 sync;
	// u16 pktype;
	//p = extractTM( sync, buffer, p );
	//p = extractTM( pktype, buffer, p );

	// Each packet includes a small amount of data about the packet itself, then the actual data.
	// Here we extract the packet info, but it does not go into the data arrays.
	u32 acquisition_id;
	u32 packet_counter;
	p = extractTM( acquisition_id, buffer, p );
	p = extractTM( packet_counter, buffer, p );

	// Extract the slices.
	// NB We assume that the array of slices has enough room to hold all the slices in the packet.
	for ( int i = 0; i < GRASP_RT_SLICES_PER_PACKET; i++ ) {
		p = extractTM( grasp_data_slice[i].fillTime, buffer, p );
		p = extractTM( grasp_data_slice[i].globalCount, buffer, p );
		p = extractTM( grasp_data_slice[i].objectStateBits, buffer, p );
		p = extractTM( grasp_data_slice[i].HMD, buffer, p );
		p = extractTM( grasp_data_slice[i].codaHMD, buffer, p );
		p = extractTM( grasp_data_slice[i].hand, buffer, p );
		p = extractTM( grasp_data_slice[i].chest, buffer, p );
		p = extractTM( grasp_data_slice[i].mouse, buffer, p );

		// GraspServices was supposed to put a copy of the 2 marker frames into the packet.
		// But I believe that there was an error in the code and that only 1 byte was inserted for each frame.
		bool mistake;
		p = extractTM( mistake, buffer, p );
		p = extractTM( mistake, buffer, p );

	}

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

	double previous_packet_timestamp;
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
	// But if we do fail to open the file, just signal the error and exit the hard way.
	if ( fid < 0 ) {
			fMessageBox( MB_OK, "GripMMI", "Error opening packet file %s.\n\n", filename );
			exit( -1 );
	}

	// Prepare for reading in packets. This is used to calculate the elapsed time between two packets.
	// By setting it to zero here, the first packet read will be signaled as having arrived after a long delay.
	previous_packet_timestamp = 0.0;

	// Read in all of the data packets in the file.
	// Be careful not to overrun the data buffers.
	packets_read = 0;
	n_slices = 0;
	while ( n_slices < max_slices ) {

		// Attempt to read next packet. Any error is terminal.
		bytes_read = _read( fid, &packet, rtPacketLengthInBytes );
		if ( bytes_read < 0 ) {
			fMessageBox( MB_OK, "GripMMI", "Error reading from %s.\n\n", filename );
			exit( -1 );
		}

		// If the number of bytes read is less than the expected number
		//  we are at the end of the file and should break out of the loop.
		if ( rtPacketLengthInBytes != bytes_read ) break;

		// We have a valid packet.
		packets_read++;

		// Check that it is a valid GRIP packet. It would be strange if it was not.
		ExtractEPMTelemetryHeaderInfo( &epmHeader, &packet );
		if ( epmHeader.epmSyncMarker != EPM_TELEMETRY_SYNC_VALUE || epmHeader.TMIdentifier != GRIP_RT_ID ) {
			fMessageBox( MB_OK, "GripMMIlite", "Unrecognized packet from %s.\n\n%s", filename );
			exit( -1 );
		}
		
		// If there has been a break in the arrival of the packets, insert
		//  a blank frame into the data buffer. This will cause breaks in
		//  the traces in the data graphs.
		if ( ( EPMtoSeconds( &epmHeader ) - previous_packet_timestamp ) > PACKET_STREAM_BREAK_THRESHOLD ) {
			// Subsampling in graphs will be used when the data record is very long.
			// Insert enough points so that we see the break even if we are sub-sampling in the graphs.
			for ( int count = 0; count < PACKET_STREAM_BREAK_INSERT_SAMPLES && n_slices < max_slices; count++ ) {
				grasp_data_slice[n_slices].fillTime = MISSING_DOUBLE;
				n_slices++;
			}
		}
		previous_packet_timestamp = EPMtoSeconds( &epmHeader );

		// Each packet is a set of slices. Extract each one.
		if ( n_slices < max_slices - GRASP_RT_SLICES_PER_PACKET ) {
			ExtractGraspRealtimeDataInfo( &grasp_data_slice[n_slices], packet );
			n_slices += GRASP_RT_SLICES_PER_PACKET;
		}

	}
	// Finished reading. Close the file and check for errors.
	return_code = _close( fid );
	if ( return_code ) {
		fMessageBox( MB_OK, "GripMMI", "Error closing %s after binary read.\nError code: %s\n\n", filename, return_code );
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
	
	return ( n_slices );
}
[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Read the cached packets.
	// This is here temporarily just for testing.
	GetGraspRT( graspDataSlice, MAX_SLICES, "GraspPackets" );

	// Create the main window and run it
	Application::Run(gcnew Form1());
	return 0;
}
