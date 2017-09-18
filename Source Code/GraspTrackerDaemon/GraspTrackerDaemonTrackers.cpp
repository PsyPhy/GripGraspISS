// GraspTrackerDaemonTrackers.cpp 
#include "stdafx.h"
#include "../GraspVR/GraspTrackers.h"
#include "../Trackers/NullPoseTracker.h"
#include "GraspTrackerDaemon.h"
#include "GraspTrackerDaemonForm.h"

using namespace PsyPhy;
using namespace Grasp;

//
// The items below have to be external to the Form because they are 'unmanaged'.
//

// A structured data buffer with fields for all of the tracker data.
GraspTrackerRecord record;
// Read the echo of the packet into this buffer.
GraspTrackerRecord echo;
int record_length;
SOCKET broadcast_socket;
struct sockaddr_in Sender_addr;

SOCKET receiver_socket;

// A buffer to hold pose data.
struct {
	TrackerPose hmd;
	TrackerPose hand;
	TrackerPose chest;
} poseData[MAX_FRAMES];

namespace GraspTrackerDaemon {

	void Form1::InitializeBroadcastSocket( void ) {
		// Initialize a socket to which we will broadcast the data.
		broadcast_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (broadcast_socket == INVALID_SOCKET) fAbortMessage( "GraspTrackerDaemon", "Error creating socket." );

		BOOL enabled = TRUE;
		if ( setsockopt( broadcast_socket, SOL_SOCKET, SO_BROADCAST, (char*)&enabled, sizeof(BOOL)) < 0 ) 
		{
			closesocket( broadcast_socket );
			fAbortMessage( "GraspTrackerDaemon", "Error setting broadcast options." );		
		}

	}

#ifdef RECEIVE_COMMANDS
	// This is not yet implemented fully so I comment it out.
	void Form1::InitializeReceiverSocket( void ) {
		// Initialize a socket that can receive commands from a client.

		struct addrinfo *result = NULL;
		struct addrinfo hints;
		int	iResult;

		ZeroMemory( &hints, sizeof(hints) );
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		iResult = getaddrinfo( NULL, TRACKER_DAEMON_COMMAND_PORT, &hints, &result );
		if ( iResult != 0 ) {
			fOutputDebugString("getaddrinfo() failed with error: %d\n", iResult);
			WSACleanup();
			return;
		}

		// Create a SOCKET for connecting to client
		receiver_socket = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
		if ( receiver_socket == INVALID_SOCKET ) {
			fOutputDebugString( "socket() failed with error: %ld\n", WSAGetLastError() );
			freeaddrinfo( result );
			WSACleanup();
			return;
		}

		// Setup the TCP listening socket
		iResult = bind( receiver_socket, result->ai_addr, (int)result->ai_addrlen );
		if (iResult == SOCKET_ERROR) {
			fOutputDebugString("bind() failed with error: %d\n", WSAGetLastError());
			freeaddrinfo( result );
			closesocket( receiver_socket );
			WSACleanup();
			return;
		}

		unsigned long nonblocking = 1;
		ioctlsocket( receiver_socket, FIONBIO, &nonblocking );
		// Listen until we get a connection.
		fOutputDebugString( "Listening for a client connection on port %s.\n", PROXY_DEX_PORT_STRING  );
		iResult = listen( receiver_socket, SOMAXCONN );
		if (iResult == SOCKET_ERROR) {
			fOutputDebugString("listen() failed with error: %d\n", WSAGetLastError());
			closesocket( receiver_socket );
			WSACleanup();
			return;
		}

	}
#endif

	void Form1::InitializeSockets( void ) {
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if ( iResult != 0 ) {
			fOutputDebugString( "WSAStartup failed with error: %d\n", iResult );
			return;
		}
		InitializeBroadcastSocket();
#ifdef RECEIVE_COMMANDS
		InitializeReceiverSocket();
#endif
	}

	void Form1::Initialize( void ) {

		// Select which tracker to use to provide the marker data.
		// The flags are set when the Form is instantiated. Here we
		// do the actual work to create an instance of the appropriate tracker.
		if ( use_coda ) {
			coda = new CodaRTnetContinuousTracker();
			this->Text = this->Text + " (Coda RTnet Tracker)";
		}
		else if ( use_legacy ) {
			coda = new CodaLegacyPolledTracker();
			this->Text = this->Text + " (Coda Legacy Tracker)";
		}
		else {
			CodaRTnetNullTracker *null_tracker = new CodaRTnetNullTracker();
			null_tracker->fakeMovements = true;
			coda = null_tracker;
			this->Text = this->Text + " (Null Tracker)";
		}

		// We use GraspDexTrackers to provide the infrastructure for the pose trackers
		// for the HMD, hand and chest. But GraspDexTrackers also requires a tracker to
		// allow the subject to adjust the hand orientation in *-V protocols. So here
		// we create a null pose tracker just to fill in.
		PoseTracker *roll = new NullPoseTracker();
		// Create a set of Pose tracker so that poses can be computed here in the daemon.
		trackers = new GraspDexTrackers( coda, roll );
		trackers->Initialize();
		// Get the alignment transforms for the tracker units. These will be sent with
		// each record of marker data. The underlying assumption is that the alignment 
		// transformations will not change once the daemon is initialized. This may not
		// be a good assumption!!!
		coda->GetAlignmentTransforms( record.alignmentOffset, record.alignmentRotation );

	}

	void Form1::ReleaseCoda( void ) {
		trackers->Release();
	}

	void Form1::ProcessCodaInputs( void ) {

		record.nUnits = trackers->codaTracker->nUnits;
		record.count++;

		trackers->Update();
		// Fill the record with the marker data from the CODA system.
		for ( int unit = 0; unit < record.nUnits; unit++ ) {
			CopyMarkerFrame(  record.frame[unit], trackers->markerFrame[unit] );
		}

		// Get the poses from the CODA tracker.
		trackers->hmdTracker->GetCurrentPose( record.codaHmd );
		trackers->handTracker->GetCurrentPose( record.hand );
		trackers->chestTracker->GetCurrentPose( record.chest );

		CopyTrackerPose( poseData[nPoseSamples].hmd, record.codaHmd );
		CopyTrackerPose( poseData[nPoseSamples].hand, record.hand );
		CopyTrackerPose( poseData[nPoseSamples].chest, record.chest );

		if (recording) nPoseSamples = ( ++nPoseSamples ) % MAX_FRAMES;

		// Broadcast the record.
		for ( int prt = 0; prt < TRACKER_DAEMON_PORTS; prt++ ) { 
			int bytes;
			Sender_addr.sin_family = AF_INET;
			Sender_addr.sin_port = htons( TRACKER_DAEMON_PORT + prt );
			Sender_addr.sin_addr.s_addr = inet_addr( TRACKER_BROADCAST_ADDRESS );

			if ( ( bytes = sendto( broadcast_socket, (char *) &record, sizeof( record ), 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr))) < 0)
			{
				int error_value = WSAGetLastError();
				closesocket( broadcast_socket );
				fAbortMessage( "GraspTrackerDaemon", "Error on sendto (%d).", error_value );	
			}
		}

		// Send status to DEX.
		unsigned int tracker_status = trackers->GetTrackerStatus();
		dex->SendTrackerStatus( tracker_status );
		dex->AddTrackerSlice( record.codaHmd, record.hand, record.chest, trackers->markerFrame );

		// Update the screen display.
		String ^line;
		int unit = 0;
		double frame_time = record.frame[unit].time;
		timeTextBox0->Text = frame_time.ToString("F3");
		line = " ";
		for ( int mrk = 0; mrk < 24; mrk++ ) {
			if ( record.frame[unit].marker[mrk].visibility ) line += "O";
			else line += ".";
			if ( ((mrk+1) % 8) == 0 ) line += "  ";
		}
		visibilityTextBox0->Text = line;
		unit = 1;
		frame_time = record.frame[unit].time;
		// timeTextBox1->Text = frame_time.ToString("F3");
		timeTextBox1->Text = record.count.ToString();
		line = " ";
		for ( int mrk = 0; mrk < 24; mrk++ ) {
			if ( record.frame[unit].marker[mrk].visibility ) line += "O";
			else line += ".";
			if ( ((mrk+1) % 8) == 0 ) line += "  ";
		}
		visibilityTextBox1->Text = line;
		trackerStatusTextBox->Text = tracker_status.ToString();

		if ( record.codaHmd.visible ) {
			char str[256];
			sprintf( str, "%s %s", trackers->vstr( record.codaHmd.pose.position ), trackers->qstr( record.codaHmd.pose.orientation ) );
			hmdPoseTextBox->Text = gcnew String( str );
		}
		else hmdPoseTextBox->Text = "                         (obscured)";

		if ( record.hand.visible ) {
			char str[256];
			sprintf( str, "%s %s", trackers->vstr( record.hand.pose.position ), trackers->qstr( record.hand.pose.orientation ) );
			handPoseTextBox->Text = gcnew String( str );
		}
		else handPoseTextBox->Text = "                         (obscured)";

		if ( record.chest.visible ) {
			char str[256];
			sprintf( str, "%s %s", trackers->vstr( record.chest.pose.position ), trackers->qstr( record.chest.pose.orientation ) );
			chestPoseTextBox->Text = gcnew String( str );
		}
		else chestPoseTextBox->Text = "                         (obscured)";

	}

	System::Void Form1::saveFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
		char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( saveFileDialog1->FileName ).ToPointer();
		trackers->codaTracker->WriteMarkerFile( fn );
		fMessageBox( MB_OK, "GraspTrackerDaemon", "Wrote %d samples to %s", trackers->codaTracker->nFrames, fn );
		char pose_fn[512] = "";
		strcat( pose_fn, fn );
		strcat( pose_fn, ".pse" );
		FILE *fp = fopen( pose_fn, "w" );
		for ( unsigned int i = 0; i < nPoseSamples; i++ ) {
			fprintf( fp, "%8d", i );
			fprintf( fp, " %0.3f %s %s", poseData[i].hmd.time, trackers->vstr( poseData[i].hmd.pose.position ), trackers->qstr( poseData[i].hmd.pose.orientation ) );
			fprintf( fp, " %0.3f %s %s", poseData[i].hand.time, trackers->vstr( poseData[i].hand.pose.position ), trackers->qstr( poseData[i].hand.pose.orientation ) );
			fprintf( fp, " %0.3f %s %s", poseData[i].chest.time, trackers->vstr( poseData[i].chest.pose.position ), trackers->qstr( poseData[i].chest.pose.orientation ) );
			fprintf( fp, " %0.3f %0.3f %0.3f", 
				trackers->ToDegrees( trackers->AngleBetweenOrientations( poseData[i].hmd.pose.orientation,  poseData[i].hand.pose.orientation )),
				trackers->ToDegrees( trackers->AngleBetweenOrientations( poseData[i].hand.pose.orientation,  poseData[i].chest.pose.orientation )),
				trackers->ToDegrees( trackers->AngleBetweenOrientations( poseData[i].chest.pose.orientation,  poseData[i].hmd.pose.orientation ))
				);
			fprintf( fp, " %0.3f %0.3f %0.3f", 
				trackers->ToDegrees( trackers->RotationAngle( poseData[i].hmd.pose.orientation )),
				trackers->ToDegrees( trackers->RotationAngle( poseData[i].hand.pose.orientation )),
				trackers->ToDegrees( trackers->RotationAngle( poseData[i].chest.pose.orientation ))
				);
			fprintf( fp, "\n" );
		}
		fclose( fp );
		fMessageBox( MB_OK, "GraspTrackerDaemon", "Wrote %d samples to %s", nPoseSamples, pose_fn );
		Marshal::FreeHGlobal( IntPtr(fn) );
	}

};