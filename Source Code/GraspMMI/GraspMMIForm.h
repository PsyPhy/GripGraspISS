#pragma once

#include "../GraspGUI/GraspDesktopForm.h"

namespace GraspGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	// This is used to map machine state values to strings for the Grasp.exe program. 
	// This has to be updated if the Grasp.exe state machine enum is updated.

	enum class GraspMachineState { 
		NullState = 0, 
		StartBlock = 10, 
		StartVtoV, StartVtoK, StartKtoK, StartVtoVK, StartVKtoVK, QuitVR, StartDemo,
		BlockCompleted = 19, 
		StartTrial = 20, 
		StraightenHead = 31, 
		AlignHead = 32, 
		PresentTarget = 33, 
		TiltHead = 34, 
		ObtainResponse = 35, 
		ProvideFeedback = 36, 
		TrialCompleted = 39, 
		TrialInterrupted = 40, 
		RAISE_HAND_TIMEOUT = 41, 
		LOWER_HAND_TIMEOUT, 
		RAISED_HAND_VIOLATION, 
		HAND_TOO_SOON, 
		ALIGN_HAND_TIMEOUT, 
		HEAD_ALIGNMENT_TIMEOUT, 
		HEAD_TILT_TIMEOUT, 
		HEAD_MISALIGNMENT, 
		RESPONSE_TIMEOUT,
		VRCompleted = 50, 
		Demo = 98, 
		ExitStateMachine = 99 };

	// This is used to map machine state values to strings for the AlignToRigidBodyGUI.exe program. 
	// This has to be updated if the AlignToRigidBodyGUI.exe state machine enum is updated.
	enum class AlignMachineState {
		STARTUP_VISIBILITY = 10,
		VISIBILITY = 15,
		SHUTDOWN_VISIBILITY =	18,
		ANNUL_ALIGNMENT	=		19,
		STARTUP_INTRINSIC =		20,
		ACQUIRE_INTRINSIC =		25,
		SHUTDOWN_INTRINSIC =	29,
		COMPUTE_ALIGNMENT =		30,
		STARTUP_ALIGNED	=		40,
		ACQUIRE_ALIGNED	=		45,
		SHUTDOWN_ALIGNED =		49,
		ALIGNMENT_ERROR	=		99, // Actually 9999, but that overflows the state field.
		ABORT_REQUESTED	=		88	// Actually 8888.
	};

	///
	/// GraspMMI is a descendent of GraspDesktop modified to be used as the ground MMI.
	/// 

	public ref class GraspMMI : public GraspDesktop
	{

	public:
		
		String^ packetRoot;

	protected:

		void ShowMarkerStatus( unsigned int tracker_status ) {

			Color color;

			// There is a bug in Grasp.exe such that TRACKER_ACQUIRING is not added to the marker visibilty digits.
			// If there is no high order digit, we assume that the system is acquiring and sending marker status.
			if ( tracker_status > 0 && tracker_status < TRACKER_CONNECTED ) tracker_status += TRACKER_ACQUIRING;

			// Set the background color of the marker visibility bars according to the state of the tracker.
			if ( tracker_status >= TRACKER_CONNECTED && tracker_status < TRACKER_PREPARED ) color = Color::LightBlue;
			else if ( tracker_status >= TRACKER_PREPARED && tracker_status < TRACKER_ACQUIRING ) color = Color::LightGreen;
			else if ( tracker_status >= TRACKER_ACQUIRING && tracker_status < TRACKER_ANOMALY ) color = SystemColors::Window;
			else if ( tracker_status == TRACKER_ANOMALY ) color = Color::Magenta;
			else color = Color::LightSlateGray;

			hmdVisibilityBar->BackColor = color;
			handVisibilityBar->BackColor = color;
			chestVisibilityBar->BackColor = color;

			if ( tracker_status < TRACKER_ANOMALY ) {
				hmdVisibilityBar->Value = tracker_status % 10;
				handVisibilityBar->Value = (tracker_status / 10) % 10;
				chestVisibilityBar->Value = (tracker_status / 100) % 10;
			}
			else {
					hmdVisibilityBar->Value = 0;
					handVisibilityBar->Value = 0;
					chestVisibilityBar->Value = 0;
			}

		}

		void NavigateTo( int subject_id, int protocol_id, int task_id, int step_id, int state, int status, int snapshots ) {

			static int previous_subject = -9999;
			static int previous_protocol = -9999;
			static int previous_task = -9999;
			static int previous_step = -9999;

			static int previous_status = -9999;
			static int previous_state = -9999;
			static enum { PGUNKNOWN, PGINSTRUCTION, PGREADY, PGEXECUTING, PGSUCCESS, PGFAILURE } previous_page = PGUNKNOWN;

			int i;

			// Make sure that these two items were not left visible.
			commandGroupBox->Visible = false;
			errorCodeNote->Visible = false;

			// Make selections in each menu.
			if ( subject_id != previous_subject ) {
				for ( i = 0; i < nSubjects; i++ ) {
					if ( subjectList[i]->number == subject_id ) break;
				}
				if ( i == nSubjects ) i = -1;
				subjectListBox->SelectedIndex = i;
				previous_subject = subject_id;
				Application::DoEvents();
			}

			if ( protocol_id != previous_protocol ) {
				for ( i = 0; i < nProtocols; i++ ) {
					if ( protocolList[i]->number == protocol_id ) break;
				}
				if ( i == nProtocols ) i = -1;
				protocolListBox->SelectedIndex = i;
				previous_protocol = protocol_id;
				Application::DoEvents();
			}

			if ( task_id != previous_task ) {
				for ( i = 0; i < nTasks; i++ ) {
					if ( taskList[i]->number == task_id ) break;
				}
				if ( i == nTasks ) i = -1;
				taskListBox->SelectedIndex = i;
				previous_task = task_id;
				Application::DoEvents();
				previous_step = -1;
			}
			if ( taskListBox->SelectedIndex >= 0 ) {
				for ( i = nSteps - 1; i >= 0; i-- ) {
					if ( stepList[i]->number == step_id ) break;
				}
				currentStep = i;
				if ( currentStep >= 0 && currentStep != previous_step ) {
					
					previous_step = currentStep;
					ShowStep();
					Sleep( 10 );


					previous_status = -9999;
					previous_state = -9999;
					previous_page = PGUNKNOWN;

				}

				if ( state != previous_state ) {
					// Hide all the navigation buttons by default.
					// The appropriate ones will then be rendered visible below.
					normalNavigationGroupBox->Visible = false;
					errorNavigationGroupBox->Visible = false;
					commandNavigationGroupBox->Visible = false;
					stepProgressGroupBox->Visible = false;
					commandGroupBox->Visible = false;
					errorCodeNote->Visible = false;
				}

				if ( stepList[currentStep]->type->StartsWith( "INSTRUCTION" ) ) {
					normalNavigationGroupBox->Visible = true;
				}
				else {
					if ( state >= STEP_READY_TO_EXECUTE && state < STEP_EXECUTING) {
						commandNavigationGroupBox->Visible = true;
						previous_page = PGREADY;
					}
					else if ( state >= STEP_EXECUTING && state < STEP_FINISHED_NORMAL ) {
						commandTextBox->Text = stepList[currentStep]->command;
						commandGroupBox->Visible = true;
						stepProgressGroupBox->Visible = true;
						int remaining = (state - STEP_EXECUTING) / 100;
						trialsRemainingTextBox->Text = remaining.ToString();
						int program_state = (state - STEP_EXECUTING) % 100;
						programStateTextBox->Text = program_state.ToString();
						// Translate the state machine state to a string and show it depending on what
						//  command is running. This part has to be customized depending on what executables
						//  can be called that use DexServices to communicate the current state.
						if ( stepList[currentStep]->command->Contains( "Grasp.exe" ) || stepList[currentStep]->command->Contains( "Grasp.bat" )) {
							Object^ o = Enum::Parse( GraspMachineState::typeid, program_state.ToString() );
							programStateEnumTextBox->Text = o->ToString();
						}
						else if ( stepList[currentStep]->command->Contains( "AlignToRigidBodyGUI.exe" ) ) {
							Object^ o = Enum::Parse( AlignMachineState::typeid, program_state.ToString() );
							programStateEnumTextBox->Text = o->ToString();
							trialsRemainingTextBox->Text = "n/a";
						}
						else programStateEnumTextBox->Text = "unknown";

						if ( previous_page != PGEXECUTING ) instructionViewer->Navigate( instructionsDirectory + stepList[currentStep]->running );
						previous_page = PGEXECUTING;
					}
					else if ( state >= STEP_FINISHED_NORMAL && state < STEP_FINISHED_ABNORMAL ) {
						commandTextBox->Text = stepList[currentStep]->command;
						commandGroupBox->Visible = true;
						normalNavigationGroupBox->Visible = true;
						int code = (state - STEP_FINISHED_NORMAL) % 100;
						if ( previous_page != PGSUCCESS ) instructionViewer->Navigate( instructionsDirectory + stepList[currentStep]->exit[ code ] );
						previous_page = PGSUCCESS;
					}
					else if ( state >= STEP_FINISHED_ABNORMAL ) {
						commandTextBox->Text = stepList[currentStep]->command;
						commandGroupBox->Visible = true;
						errorNavigationGroupBox->Visible = true;
						errorCodeNote->Visible = true;
						int code = (state - STEP_FINISHED_ABNORMAL) % 100;
						errorCodeTextBox->Text = code.ToString();
						if ( previous_page != PGFAILURE ) instructionViewer->Navigate( instructionsDirectory + stepList[currentStep]->exit[ code ] );
						previous_page = PGFAILURE;
					}
				}
				previous_state = state;
				previous_status = status;
				Refresh();
			}

			ShowMarkerStatus( status );
			snapshotsTextBox->Text = snapshots.ToString();
			int state_category = (state / 10000);
			scriptEngineTextBox->Text = state_category.ToString();

		}

		// In MMI mode there is no connection to the DEX services module, so we eliminate that from the opening and closing actions.
		virtual System::Void GraspDesktop_Shown(System::Object^  sender, System::EventArgs^  e) override {
			this->Text = L"GRASP MMI";
			this->Location = System::Drawing::Point(10, 10);
			this->navigatorGroupBox->Enabled = false;
			this->instructionsGroupBox->Enabled = false;
			this->dexStatusGroupBox->Visible = true;
			this->packetTimeTextBox->Visible = true;
			InitializeForm();
		}
		virtual System::Void GraspDesktop_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) override {
		}
		virtual void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) override {

			GripHealthAndStatusInfo hk;
			EPMTelemetryHeaderInfo header;

			// Stop the refresh timer, in case it takes us longer than 1 refresh cycle to complete the actions.
			StopRefreshTimer();

			// We need InteropServics in order to convert a String to a char *.
			using namespace System::Runtime::InteropServices;

			// Converts the String into a char *.
			// Don't forget to free it when exiting.
			char *filename_root = (char*)(void*)Marshal::StringToHGlobalAnsi( packetRoot ).ToPointer();

			// Get the latest HK packet from the cache.
			GetLastPacketHK( &header, &hk, filename_root );
			fOutputDebugString( "User: %d  Protocol: %3d  Task: %3d  Step: %2d  Engine: %8d  Tracker: %8d  Camera: %3d\n",
				 hk.user, hk.protocol, hk.task, hk.step, hk.scriptEngineStatusEnum, hk.motionTrackerStatusEnum, hk.crewCameraRate );
			
			// Show where the user is in the menus.
			NavigateTo( hk.user, hk.protocol, hk.task, hk.step, hk.scriptEngineStatusEnum, hk.motionTrackerStatusEnum, hk.crewCameraRate );

			// Show the time of the latest packet.
			int TimebaseOffset = 16;
			char label[256];
			double latest_packet_time = EPMtoSeconds( &header );
			int day_first = (int) floor(( latest_packet_time + TimebaseOffset )) / (24 * 60 * 60);
			int since_midnight = ((int) floor( latest_packet_time ) + TimebaseOffset ) % (24 * 60 * 60);
			int hour = since_midnight / (60 * 60);
			int minute = (since_midnight % (60 * 60)) / 60;
			int second = (since_midnight % 60);
			sprintf( label, "%02d:%02d:%02d GMT  ", hour, minute, second );
			packetTimeTextBox->Text = gcnew String( label );

			// Release the memory used to create the ANSI string.
			Marshal::FreeHGlobal( IntPtr( filename_root ) );
	
			// Restart the timer so that we get called again.
			StartRefreshTimer();
		}
		// In GUI mode, the gui will ask for confirmation if the user breaks the normal sequence.
		// In MMI mode, confirmation is given automatically.
		virtual bool VerifyInterruptStep( void ) override {
			return true;
		}

	};
}

