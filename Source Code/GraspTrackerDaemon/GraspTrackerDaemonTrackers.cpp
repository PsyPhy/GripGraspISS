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
SOCKET sock;
struct sockaddr_in Sender_addr;

// A buffer to hold pose data.
struct {
	TrackerPose hmd;
	TrackerPose hand;
	TrackerPose chest;
} poseData[MAX_FRAMES];

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

	}

	// If we are not connected to actual codas, we simulate markers going in 
	// and out of view for debugging purposes. This should probably be integrated
	// into the CodaRTnetNullTracker.
	void Form1::FakeTheCodaData( void ) {
		static double angle = 0.0;
		int unit, mrk, id;
		for ( unit = 0; unit < coda->nUnits; unit++ ) {
			coda->recordedMarkerFrames[unit][0].time = (double) record.count / 200.0;
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

		trackers->hmdTracker->CopyTrackerPose( poseData[nPoseSamples].hmd, record.hmd );
		trackers->handTracker->CopyTrackerPose( poseData[nPoseSamples].hand, record.hand );
		trackers->chestTracker->CopyTrackerPose( poseData[nPoseSamples].chest, record.chest );

		if (recording) nPoseSamples = ( ++nPoseSamples ) % MAX_FRAMES;

		// Broadcast the record.
		for ( int prt = 0; prt < TRACKER_DAEMON_PORTS; prt++ ) { 
			Sender_addr.sin_family = AF_INET;
			Sender_addr.sin_port = htons( TRACKER_DAEMON_PORT + prt );
			Sender_addr.sin_addr.s_addr = inet_addr( TRACKER_BROADCAST_ADDRESS );

			if ( sendto( sock, (char *) &record, sizeof( record ), 0, (sockaddr *)&Sender_addr, sizeof(Sender_addr)) < 0)
			{
				int error_value = WSAGetLastError();
				closesocket( sock );
				fAbortMessage( "GraspTrackerDaemon", "Error on sendto (%d).", error_value );	
			}
		}

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
		timeTextBox1->Text = frame_time.ToString("F3");
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

	System::Void Form1::saveFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
			char *fn = (char*)(void*)Marshal::StringToHGlobalAnsi( saveFileDialog1->FileName ).ToPointer();
			trackers->codaTracker->WriteMarkerFile( fn );
			fMessageBox( MB_OK, "GraspTrackerDaemon", "Wrote %d samples to %s", trackers->codaTracker->nFrames, fn );
			char pose_fn[512] = "";
			strcat( pose_fn, fn );
			strcat( pose_fn, ".pse" );
			FILE *fp = fopen( pose_fn, "w" );
			for ( int i = 0; i < nPoseSamples; i++ ) {
				fprintf( fp, "%8d", i );
				fprintf( fp, " %0.3f %s %s", poseData[i].hmd.time, trackers->vstr( poseData[i].hmd.pose.position ), trackers->qstr( poseData[i].hmd.pose.orientation ) );
				fprintf( fp, " %0.3f %s %s", poseData[i].hand.time, trackers->vstr( poseData[i].hand.pose.position ), trackers->qstr( poseData[i].hand.pose.orientation ) );
				fprintf( fp, " %0.3f %s %s", poseData[i].chest.time, trackers->vstr( poseData[i].chest.pose.position ), trackers->qstr( poseData[i].chest.pose.orientation ) );
				fprintf( fp, " %0.3f %0.3f %0.3f", 
					trackers->ToDegrees( trackers->AngleBetween( poseData[i].hmd.pose.orientation,  poseData[i].hand.pose.orientation )),
					trackers->ToDegrees( trackers->AngleBetween( poseData[i].hand.pose.orientation,  poseData[i].chest.pose.orientation )),
					trackers->ToDegrees( trackers->AngleBetween( poseData[i].chest.pose.orientation,  poseData[i].hmd.pose.orientation ))
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