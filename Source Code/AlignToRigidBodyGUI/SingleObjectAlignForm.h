#pragma once

#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../GraspVR/GraspGLObjects.h"
// The following should be integrated elsewhere.
#include "../GraspGUIBellsAndWhistles/OpenGLWindowsInForms.h"

namespace AlignToRigidBodyGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace PsyPhy;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class SingleObjectForm : public System::Windows::Forms::Form
	{

	protected:

		bool									noCoda;

		PsyPhy::OpenGLWindow					*vrWindow1, *vrWindow2;
		PsyPhy::Viewpoint						*codaViewpoint;
		Grasp::MarkerStructureGLObject			*alignmentObject1;
		Grasp::MarkerStructureGLObject			*alignmentObject2;

		PsyPhy::CodaRTnetTracker				*coda;
		Grasp::GraspGLObjects					*objects;

		String^ modelFile;
		String^ filenameRoot;

		double maxPositionError;
		double maxOrientationError;

	public:
		SingleObjectForm( String ^model_file, String ^filename_root ) :
		  maxPositionError( 10.0 ),
		  maxOrientationError( 2.0 )
		{
			InitializeComponent();
			modelFile = model_file;
			filenameRoot = filename_root;
			noCoda = false;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SingleObjectForm()
		{
			if (components)
			{
				delete components;
			}
		}


		// A timer to handle animations and screen refresh, and associated actions.
		static System::Windows::Forms::Timer^ refreshTimer;
		void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
			static int count = 0;
			MarkerFrame localFrame;

			coda->GetCurrentMarkerFrameUnit( localFrame, 0 );
			alignmentObject1->ShowVisibility( localFrame );
			coda->GetCurrentMarkerFrameUnit( localFrame, 1 );
			alignmentObject2->ShowVisibility( localFrame );

			// Update the 3D view of the alignment object.
			vrWindow2->Activate();
			vrWindow2->Clear( 0.50, 0.50, 0.60 );
			codaViewpoint->Apply( vrWindow2, CYCLOPS );
			alignmentObject2->Draw();
			vrWindow2->Swap();

			vrWindow1->Activate();
			vrWindow1->Clear( 0.50, 0.50, 0.60 );
			codaViewpoint->Apply( vrWindow1, CYCLOPS );
			alignmentObject1->Draw();
			vrWindow1->Swap();

		}
		void CreateRefreshTimer( int interval ) {
			refreshTimer = gcnew( System::Windows::Forms::Timer );
			refreshTimer->Interval = interval;
			refreshTimer->Tick += gcnew EventHandler( this, &SingleObjectForm::OnTimerElapsed );
		}
		void StartRefreshTimer( void ) {
			refreshTimer->Start();
		}
		void StopRefreshTimer( void ) {
			refreshTimer->Stop();
		}		

	private:
		System::Windows::Forms::TextBox^  instructionsTextBox;
		System::Windows::Forms::GroupBox^	vrGroupBox2;
		System::Windows::Forms::Panel^		vrPanel2;
		System::Windows::Forms::GroupBox^	vrGroupBox1;
		System::Windows::Forms::Panel^		vrPanel1;
		System::Windows::Forms::Button^	alignButton;
		System::Windows::Forms::Button^	cancelButton;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->alignButton = (gcnew System::Windows::Forms::Button());
			this->cancelButton = (gcnew System::Windows::Forms::Button());
			this->vrGroupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->vrPanel2 = (gcnew System::Windows::Forms::Panel());
			this->vrGroupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->vrPanel1 = (gcnew System::Windows::Forms::Panel());
			this->instructionsTextBox = (gcnew System::Windows::Forms::TextBox());
			this->vrGroupBox2->SuspendLayout();
			this->vrGroupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// alignButton
			// 
			this->alignButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->alignButton->Location = System::Drawing::Point(351, 403);
			this->alignButton->Name = L"alignButton";
			this->alignButton->Size = System::Drawing::Size(159, 56);
			this->alignButton->TabIndex = 1;
			this->alignButton->Text = L"Align";
			this->alignButton->UseVisualStyleBackColor = true;
			this->alignButton->Click += gcnew System::EventHandler(this, &SingleObjectForm::alignButton_Click);
			// 
			// cancelButton
			// 
			this->cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->cancelButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cancelButton->Location = System::Drawing::Point(80, 403);
			this->cancelButton->Name = L"cancelButton";
			this->cancelButton->Size = System::Drawing::Size(159, 56);
			this->cancelButton->TabIndex = 3;
			this->cancelButton->Text = L"Cancel";
			this->cancelButton->UseVisualStyleBackColor = true;
			this->cancelButton->Click += gcnew System::EventHandler(this, &SingleObjectForm::cancelButton_Click);
			// 
			// vrGroupBox2
			// 
			this->vrGroupBox2->Controls->Add(this->vrPanel2);
			this->vrGroupBox2->Location = System::Drawing::Point(302, 174);
			this->vrGroupBox2->Name = L"vrGroupBox2";
			this->vrGroupBox2->Size = System::Drawing::Size(256, 223);
			this->vrGroupBox2->TabIndex = 5;
			this->vrGroupBox2->TabStop = false;
			this->vrGroupBox2->Text = L"Tracker Camera 2";
			// 
			// vrPanel2
			// 
			this->vrPanel2->Location = System::Drawing::Point(11, 28);
			this->vrPanel2->Name = L"vrPanel2";
			this->vrPanel2->Size = System::Drawing::Size(235, 186);
			this->vrPanel2->TabIndex = 1;
			// 
			// vrGroupBox1
			// 
			this->vrGroupBox1->Controls->Add(this->vrPanel1);
			this->vrGroupBox1->Location = System::Drawing::Point(31, 174);
			this->vrGroupBox1->Name = L"vrGroupBox1";
			this->vrGroupBox1->Size = System::Drawing::Size(256, 223);
			this->vrGroupBox1->TabIndex = 4;
			this->vrGroupBox1->TabStop = false;
			this->vrGroupBox1->Text = L"Tracker Camera 1";
			// 
			// vrPanel1
			// 
			this->vrPanel1->Location = System::Drawing::Point(11, 28);
			this->vrPanel1->Name = L"vrPanel1";
			this->vrPanel1->Size = System::Drawing::Size(235, 186);
			this->vrPanel1->TabIndex = 0;
			// 
			// instructionsTextBox
			// 
			this->instructionsTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->instructionsTextBox->Location = System::Drawing::Point(44, 25);
			this->instructionsTextBox->Multiline = true;
			this->instructionsTextBox->Name = L"instructionsTextBox";
			this->instructionsTextBox->Size = System::Drawing::Size(503, 126);
			this->instructionsTextBox->TabIndex = 6;
			this->instructionsTextBox->Text = L"Verify that all markers are visible in each Tracker Camera view and then press \'A" 
				L"lign\'.";
			// 
			// SingleObjectForm
			// 
			this->AcceptButton = this->alignButton;
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->cancelButton;
			this->ClientSize = System::Drawing::Size(570, 474);
			this->Controls->Add(this->instructionsTextBox);
			this->Controls->Add(this->vrGroupBox2);
			this->Controls->Add(this->vrGroupBox1);
			this->Controls->Add(this->cancelButton);
			this->Controls->Add(this->alignButton);
			this->Name = L"SingleObjectForm";
			this->Text = L"CODA Tracker Alignment";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &SingleObjectForm::Form1_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &SingleObjectForm::Form1_Shown);
			this->vrGroupBox2->ResumeLayout(false);
			this->vrGroupBox1->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 System::Windows::Forms::DialogResult response;
				 response = MessageBox::Show( "Are you sure you want to exit without performing the alignment?", "AlignToRigidBodyGUI", MessageBoxButtons::YesNo );
				 if ( response == System::Windows::Forms::DialogResult::Yes ) {
					 // coda->Quit();
					 Environment::ExitCode = -1;
					 Close();
				 }
			 }

	private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {

				 // Create windows and viewpoints to show what the CODA units are seeing.
				 vrWindow1 = PsyPhy::CreateOpenGLWindowInForm( vrPanel1);
				 vrWindow2 = PsyPhy::CreateOpenGLWindowInForm( vrPanel2, vrWindow1->hRC );

				 // Create the OpenGLObjects that depict the marker array structure.
				 objects = new Grasp::GraspGLObjects();
				 char *model_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( modelFile ).ToPointer();
				 alignmentObject1 = objects->CreateHandMarkerStructure( model_file );
				 alignmentObject2 = objects->CreateHandMarkerStructure( model_file );
				 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( model_file ) );

				 // Create a viewpoint that looks at the origin from the negative Z axis.
				 // This is where the CODAs are with respect to the workspace.
				 codaViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
				 codaViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
				 codaViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

				 // Initialize the state of the GL graphics engine.
				 // Some of this will be overridden by the object renderer.
				 glUsefulInitializeDefault();
				 glUsefulDefaultSpecularLighting( 0.75 );

				 // When debugging without the CODA, just quit.
				 if ( noCoda ) {
					 instructionsTextBox->Text = instructionsTextBox->Text + " (noCoda debug mode)";
					 return;
				 }

				 // Create and start up the CODA tracker.
				 coda = new CodaRTnetTracker();
				 coda->Initialize();
				 coda->StartAcquisition( 600.0 );

				 // Start a refresh time that will update the visibility of the LEDs in the GUI display.
				 CreateRefreshTimer( 300 );
				 StartRefreshTimer();

			 }

	private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) { }
	private: System::Void alignButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 // If in noCoda debug mode, signal to close the form and then return to skip initiating the CODA system.
				 if ( noCoda ) {
					 Close();
					 return;
				 }

				 // Show the Form as being inactive.
				 Enabled = false;

				 // We have to stop the CODA acquisiton, so we have to halt the update of the VR display.
				 StopRefreshTimer();
				 // Stop the ongoing acquisition and discard the recorded data.
				 coda->AbortAcquisition();	
				 // Unfortunately, we have to shutdown and restart to do a new acquisition.
				 coda->Quit();

				 // Annul the previous alignment to get data in coordinates intrinsic to each CODA unit.
				 coda->AnnulAlignment();

				 // Restart and acquire a short burst of marker data to be used to perform the alignment.
				 coda->Initialize();
				 fprintf( stderr, "Starting acquisition ... " );
				 coda->StartAcquisition( 2.0 );
				 fprintf( stderr, "OK.\nAcquiring " );
				 // Just wait for the acquisition to finish.
				 while ( coda->GetAcquisitionState() ) {
					 fprintf( stderr, "." );
					 Sleep( 20 );
				 }
				 coda->StopAcquisition();
				 fprintf( stderr, "OK.\n" );

				 // Log the data acquired in intrinsic coordinates.
				 // The root for file names is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
				 String ^rawFilename = filenameRoot + ".intrinsic.mrk";
				 char *raw_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( rawFilename ).ToPointer();
				 fprintf( stderr, "Writing marker positions acquired in intrinsic coordinates ... " );
				 coda->WriteMarkerFile( raw_file );
				 fprintf( stderr, "OK.\n" );
				 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( raw_file ) );
				 coda->Quit();

				 // Use a CodaPoseTracker to compute the pose of the marker structure in the intrinsic frame of the CODA unit.
				 // We will then invert the pose to compute the transformation required by each unit.
				 MarkerFrame avgFrame;
				 TrackerPose tracker_pose;
				 Pose poses[MAX_UNITS];
				 CodaPoseTracker *codaPoseTracker = new CodaPoseTracker( &avgFrame );
				 // The name of the model file is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
				 char *model_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( modelFile ).ToPointer();
				 codaPoseTracker->ReadModelMarkerPositions( model_file );
				 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( model_file ) );

				 for ( int unit = 0; unit < coda->nUnits; unit++ ) {
					 // Compute the average marker positions for the acquired sample.
					 coda->ComputeAverageMarkerFrame( avgFrame, coda->recordedMarkerFrames[unit], coda->nFrames );
					 // Compute the pose of the rigid body from the average acquired sample.
					 // This may seem strange that we are calling "GetCurrentPose()" when the CODA is not running, but
					 // codaPoseTracker assumes that the linked MarkerFrame has been filled and so it does not actually acquire anything.
					 // Instead, it computes the current pose from the average marker frame computed just above.
					 codaPoseTracker->GetCurrentPose( tracker_pose );
					 // We need to assemble the poses from each CODA unit into a single list to be passed to SetAlignmentFromPoses().
					 coda->CopyPose( poses[unit], tracker_pose.pose );
				 }
				 // This does the real work.
				 coda->SetAlignmentFromPoses( poses );

				 // Restart and acquire a short burst of marker data to be used to verify the alignment.
				 coda->Initialize();
				 fprintf( stderr, "Starting acquisition ... " );
				 coda->StartAcquisition( 3.0 );
				 fprintf( stderr, "OK.\nAcquiring " );
				 // Just wait for the acquisition to finish.
				 while ( coda->GetAcquisitionState() ) {
					 fprintf( stderr, "." );
					 Sleep( 20 );
				 }
				 coda->StopAcquisition();
				 fprintf( stderr, "OK.\n\n" );

				 // Log the data acquired in aligned coordinates.
				 // The root for file names is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
				 String ^alignedFilename = filenameRoot + ".aligned.mrk";
				 char *aligned_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( alignedFilename ).ToPointer();
				 fprintf( stderr, "Writing marker positions acquired in aligned coordinates ... " );
				 coda->WriteMarkerFile( aligned_file );
				 fprintf( stderr, "OK.\n" );
				 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( aligned_file ) );
				 coda->Quit();

				 // Use a CodaPoseTracker to compute the pose of the marker structure in the newly aligned frame.
				 // We should get position zero and null orientation in each case.
				 char msg[25600] = "Alignment results: \n", line[256];
				 bool well_aligned = true;
				 for ( int unit = 0; unit < coda->nUnits; unit++ ) {
					 // Compute the average marker positions for the acquired sample.
					 coda->ComputeAverageMarkerFrame( avgFrame, coda->recordedMarkerFrames[unit], coda->nFrames );
					 // Set the position of the model markers from the average acquired sample.
					 codaPoseTracker->GetCurrentPose( tracker_pose );
					 // Show the computed pose.
					 fOutputDebugString( "New Pose: %s %s\n", codaPoseTracker->vstr( tracker_pose.pose.position ), codaPoseTracker->qstr( tracker_pose.pose.orientation ) );
					 // Build a string with the poses, in case we need to display after an error.
					 double position_error = codaPoseTracker->VectorNorm( tracker_pose.pose.position );
					 bool position_ok = position_error < maxPositionError;
					 sprintf( line, "%d %s %6.3f %s\n", unit, codaPoseTracker->vstr( tracker_pose.pose.position ), position_error, ( position_ok ? "OK" : "!!" ));
					 strcat( msg, line );
					 double orientation_error = codaPoseTracker->AngleBetween( tracker_pose.pose.orientation, codaPoseTracker->nullQuaternion );
					 bool orientation_ok = orientation_error < maxOrientationError;
					 sprintf( line, "%d %s %6.3f %s\n", unit,  codaPoseTracker->qstr( tracker_pose.pose.orientation ), orientation_error, ( orientation_ok ? "OK" : "!!" ));
					 strcat( msg, line );
					 // Check that the alignment has been successful.
					 well_aligned &= ( position_ok & orientation_ok );
				 }
				 fAbortMessageOnCondition( !well_aligned, "AlignToRigidBodyGUI", msg );

				 // Close the form. 
				 Close();

			 }
};
}

