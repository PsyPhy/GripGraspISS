// GraspTrackerDaemonTrackers.cpp 
#include "stdafx.h"
#include "../GraspVR/GraspTrackers.h"
#include "../Trackers/NullPoseTracker.h"
#include "GraspTrackerDaemon.h"

#include "Form1.h"

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
SOCKET sock;
struct sockaddr_in Sender_addr;

namespace GraspTrackerDaemon {

	void Form1::InitializeSocket( void ) {
		// Initialize a socket to which we will broadcast the data.
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == INVALID_SOCKET) fAbortMessage( "GraspTrackerDaemon", "Error creating socket." );

		BOOL enabled = TRUE;
		if ( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char*)&enabled, sizeof(BOOL)) < 0 ) 
		{
			closesocket(sock);
			fAbortMessage( "GraspTrackerDaemon", "Error setting broadcast options." );		
		}

		Sender_addr.sin_family = AF_INET;
		Sender_addr.sin_port = htons( TRACKER_DAEMON_PORT );
		Sender_addr.sin_addr.s_addr = inet_addr( TRACKER_BROADCAST_ADDRESS );
	}

	// If we are not connected to actual codas, we simulate markers going in 
	// and out of view for debugging purposes. This should probably be integrated
	// into the CodaRTnetNullTracker.
	void Form1::FakeTheCodaData( void ) {
		static double angle = 0.0;
		int unit, mrk, id;
		for ( unit = 0; unit < coda->nUnits; unit++ ) {
			for ( mrk = 0; mrk < trackers->hmdCodaPoseTracker[unit]->nModelMarkers; mrk++ ) {
				id = trackers->hmdCodaPoseTracker[unit]->modelMarker[mrk].id;
				vm->CopyVector( coda->recordedMarkerFrames[unit][0].marker[id].position, 
					trackers->hmdCodaPoseTracker[unit]->modelMarker[mrk].position );
				coda->recordedMarkerFrames[unit][0].marker[id].visibility = true;
			}
			for ( mrk = 0; mrk < trackers->handCodaPoseTracker[unit]->nModelMarkers; mrk++ ) {
				id = trackers->handCodaPoseTracker[unit]->modelMarker[mrk].id;
				vm->CopyVector( coda->recordedMarkerFrames[unit][0].marker[id].position, 
					trackers->handCodaPoseTracker[unit]->modelMarker[mrk].position );
				coda->recordedMarkerFrames[unit][0].marker[id].position[Z] -= 300.0;
				coda->recordedMarkerFrames[unit][0].marker[id].position[Y] -= 50.0;
				coda->recordedMarkerFrames[unit][0].marker[id].position[X] += 100.0 + 200.0 * sin( angle );
				coda->recordedMarkerFrames[unit][0].marker[id].visibility = true;
			}
			for ( mrk = 0; mrk < trackers->chestCodaPoseTracker[unit]->nModelMarkers; mrk++ ) {
				id = trackers->chestCodaPoseTracker[unit]->modelMarker[mrk].id;
				vm->CopyVector( coda->recordedMarkerFrames[unit][0].marker[id].position, 
					trackers->chestCodaPoseTracker[unit]->modelMarker[mrk].position );
				coda->recordedMarkerFrames[unit][0].marker[id].visibility = true;
			}
			// The following code causes intermittent occlusions of the simulated markers.
			for ( int unit = 0; unit < record.nUnits; unit++ ) {
				int mrk;
				record.frame[unit].time = (double) record.count / 200.0;
				for ( mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
					if ( ( (mrk + record.count / 10 ) / 8 ) % 6 == unit ) coda->recordedMarkerFrames[unit][0].marker[mrk].visibility = false;
					else coda->recordedMarkerFrames[unit][0].marker[mrk].visibility = true;
				}
			}
		}
		angle += 0.05;
	}

	void Form1::InitializeCoda( void ) {

		if ( use_coda ) coda = new CodaRTnetContinuousTracker();
		else coda = new CodaRTnetNullTracker();

		PoseTracker *roll = new NullPoseTracker();

		trackers = new GraspDexTrackers( coda, roll );
		trackers->Initialize();
		for ( int unit = 0; unit < trackers->codaTracker->nUnits; unit++ ) {
			trackers->codaTracker->GetAlignment( record.alignmentOffset, record.alignmentRotation );
		}
		if ( !use_coda ) FakeTheCodaData();
	}

	void Form1::ReleaseCoda( void ) {
			trackers->Release();
	}

	void Form1::ProcessCodaInputs( void ) {

		record.nUnits = trackers->codaTracker->nUnits;
		record.count++;
		if ( !use_coda ) FakeTheCodaData();

		trackers->Update();
		// Fill the record with the marker data from the CODA system.
		for ( int unit = 0; unit < record.nUnits; unit++ ) {
			trackers->codaTracker->CopyMarkerFrame(  record.frame[unit], trackers->markerFrame[unit] );
		}
		// Get the poses.
		trackers->hmdTracker->GetCurrentPose( record.hmd );
		trackers->handTracker->GetCurrentPose( record.hand );
		trackers->chestTracker->GetCurrentPose( record.chest );

		// Broadcast the record.
		if ( sendto( sock, (char *) &record, sizeof( record ), 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr)) < 0)
		{
			int error_value = WSAGetLastError();
			closesocket( sock );
			fAbortMessage( "GraspTrackerDaemon", "Error on sendto (%d).", error_value );		
		}

		// Update the screen display.
		String ^line;
		int unit = 0;
		double frame_time = record.frame[unit].time;
		timeTextBox->Text = frame_time.ToString("F3");
		line = " ";
		for ( int mrk = 0; mrk < 24; mrk++ ) {
			if ( record.frame[unit].marker[mrk].visibility ) line += "O";
			else line += ".";
			if ( ((mrk+1) % 8) == 0 ) line += "  ";
		}
		visibilityTextBox0->Text = line;
		unit = 1;
		line = " ";
		for ( int mrk = 0; mrk < 24; mrk++ ) {
			if ( record.frame[unit].marker[mrk].visibility ) line += "O";
			else line += ".";
			if ( ((mrk+1) % 8) == 0 ) line += "  ";
		}
		visibilityTextBox1->Text = line;

		if ( record.hmd.visible ) {
			char str[256];
			sprintf( str, "%s %s", trackers->vstr( record.hmd.pose.position ), trackers->qstr( record.hmd.pose.orientation ) );
			hmdPoseTextBox->Text = gcnew String( str );
		}
		else hmdPoseTextBox->Text = "                         (obscured)";

		if ( record.hand.visible ) {
			char str[256];
			sprintf( str, "%s %s", trackers->vstr( record.hand.pose.position ), trackers->qstr( record.hand.pose.orientation ) );
			handPoseTextBox->Text = gcnew String( str );
		}
		else handPoseTextBox->Text = "                        (obscured)";

		if ( record.chest.visible ) {
			char str[256];
			sprintf( str, "%s %s", trackers->vstr( record.chest.pose.position ), trackers->qstr( record.chest.pose.orientation ) );
			chestPoseTextBox->Text = gcnew String( str );
		}
		else chestPoseTextBox->Text = "                       (obscured)";

	}

};