#pragma once

#include <Windows.h>

#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"

#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../GraspVR/GraspGLObjects.h"

#include "../DexServices/DexServices.h"

// The following defines are used to indicate progress through different
// phases of the alignment process. These are sent as the sub-step indicator to 
// ground via DEX using DexServices.
#define STARTUP_VISIBILITY	10
#define VISIBILITY			15
#define SHUTDOWN_VISIBILITY 18
#define ANNUL_ALIGNMENT		19
#define STARTUP_INTRINSIC	20
#define ACQUIRE_INTRINSIC	25
#define SHUTDOWN_INTRINSIC	29
#define COMPUTE_ALIGNMENT	30
#define STARTUP_ALIGNED		40
#define ACQUIRE_ALIGNED		45
#define SHUTDOWN_ALIGNED	49
#define ALIGNMENT_ERROR		9999
#define ABORT_REQUESTED		8888

namespace AlignToRigidBodyGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace PsyPhy;
	using namespace Grasp;

	/// <summary>
	/// A windows Form that guides an operator through the alignment process.
	/// </summary>
	public ref class SingleObjectForm : public System::Windows::Forms::Form
	{

	private:

		DexServices	*dex;

	protected:

		bool									noCoda;
		bool									forceShow;

		PsyPhy::OpenGLWindow					*visibilityWindow1, *visibilityWindow2;
		PsyPhy::OpenGLWindow					*fovWindow1, *fovWindow2;
		PsyPhy::Viewpoint						*codaViewpoint, *objectViewpoint;
		Grasp::MarkerStructureGLObject			*alignmentObject1, *alignmentObject2;
		Grasp::MarkerStructureGLObject			*visibilityObject1, *visibilityObject2;
		PsyPhy::Assembly						*fovSweetSpot;

		PsyPhy::CodaRTnetDaemonTracker				*coda;
		Grasp::GraspGLObjects					*objects;

		String^ modelFile;
		String^ filenameRoot;

		double maxPositionError;

	protected: 

	private: System::Windows::Forms::TextBox^  instructionsTextBox2;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Panel^  fovPanel2;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Panel^  fovPanel1;
	private: System::Windows::Forms::Label^  busy;
	private: System::Windows::Forms::TextBox^  instructionsTextBox3;


	protected: 
		double maxOrientationError;

	public:
		SingleObjectForm( String ^model_file, String ^filename_root, Grasp::DexServices *dex, bool noCoda ) :
		  maxPositionError( 10.0 ),
			  maxOrientationError( 2.0 ),
			  forceShow( false )
		  {
			  InitializeComponent();
			  modelFile = model_file;
			  filenameRoot = filename_root;
			  this->dex = dex;
			  this->noCoda = noCoda;
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

		void Render ( void ) {

			// Update the 3D view of the alignment object.

			visibilityWindow2->Activate();
			visibilityWindow2->Clear( 0.10, 0.10, 0.30 );
			objectViewpoint->Apply( visibilityWindow2, CYCLOPS );
			visibilityObject2->Draw();
			visibilityWindow2->Swap();

			visibilityWindow1->Activate();
			visibilityWindow1->Clear( 0.10, 0.10, 0.30 );
			objectViewpoint->Apply( visibilityWindow1, CYCLOPS );
			visibilityObject1->Draw();
			visibilityWindow1->Swap();

			fovWindow2->Activate();
			fovWindow2->Clear( 0.0, 0.0, 0.0 );
			codaViewpoint->Apply( fovWindow2, CYCLOPS );
			alignmentObject2->Draw();
			fovSweetSpot->Draw();
			fovWindow2->Swap();

			fovWindow1->Activate();
			fovWindow1->Clear( 0.0, 0.0, 0.0 );
			codaViewpoint->Apply( fovWindow1, CYCLOPS );
			alignmentObject1->Draw();
			fovSweetSpot->Draw();
			fovWindow1->Swap();

		}

		// A timer to handle animations and screen refresh, and associated actions.
		static System::Windows::Forms::Timer^ refreshTimer;
		void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {

			TrackerPose pose;
			MarkerFrame codaFrame;
			CodaPoseTracker *poseTracker = new CodaPoseTracker( &codaFrame );

			// The name of the model file is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
			char *model_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( modelFile ).ToPointer();
			poseTracker->ReadModelMarkerPositions( model_file );
			System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( model_file ) );

			coda->GetCurrentMarkerFrameUnit( codaFrame, 0 );
			visibilityObject1->ShowVisibility( codaFrame );
			alignmentObject1->ShowVisibility( codaFrame );
			poseTracker->GetCurrentPose( pose );
			if ( pose.visible ) {
				alignmentObject1->SetPose( pose.pose );
				alignmentObject1->Enable();
			}
			else alignmentObject1->Disable();
			coda->GetCurrentMarkerFrameUnit( codaFrame, 1 );
			visibilityObject2->ShowVisibility( codaFrame );
			alignmentObject2->ShowVisibility( codaFrame );
			poseTracker->GetCurrentPose( pose );
			if ( pose.visible ) {
				alignmentObject2->SetPose( pose.pose );
				alignmentObject2->Enable();
			}
			else alignmentObject2->Disable();

			Render();

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
		System::Windows::Forms::Panel^  visibilityPanel2;
		System::Windows::Forms::GroupBox^	vrGroupBox1;
		System::Windows::Forms::Panel^  visibilityPanel1;
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
			this->visibilityPanel2 = (gcnew System::Windows::Forms::Panel());
			this->vrGroupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->visibilityPanel1 = (gcnew System::Windows::Forms::Panel());
			this->instructionsTextBox = (gcnew System::Windows::Forms::TextBox());
			this->instructionsTextBox2 = (gcnew System::Windows::Forms::TextBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->fovPanel2 = (gcnew System::Windows::Forms::Panel());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->fovPanel1 = (gcnew System::Windows::Forms::Panel());
			this->busy = (gcnew System::Windows::Forms::Label());
			this->instructionsTextBox3 = (gcnew System::Windows::Forms::TextBox());
			this->vrGroupBox2->SuspendLayout();
			this->vrGroupBox1->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// alignButton
			// 
			this->alignButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->alignButton->Location = System::Drawing::Point(641, 805);
			this->alignButton->Name = L"alignButton";
			this->alignButton->Size = System::Drawing::Size(159, 56);
			this->alignButton->TabIndex = 1;
			this->alignButton->Text = L"OK";
			this->alignButton->UseVisualStyleBackColor = true;
			this->alignButton->Click += gcnew System::EventHandler(this, &SingleObjectForm::alignButton_Click);
			// 
			// cancelButton
			// 
			this->cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->cancelButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cancelButton->Location = System::Drawing::Point(1181, 805);
			this->cancelButton->Name = L"cancelButton";
			this->cancelButton->Size = System::Drawing::Size(159, 56);
			this->cancelButton->TabIndex = 3;
			this->cancelButton->Text = L"Cancel";
			this->cancelButton->UseVisualStyleBackColor = true;
			this->cancelButton->Click += gcnew System::EventHandler(this, &SingleObjectForm::cancelButton_Click);
			// 
			// vrGroupBox2
			// 
			this->vrGroupBox2->Controls->Add(this->visibilityPanel2);
			this->vrGroupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->vrGroupBox2->Location = System::Drawing::Point(1000, 22);
			this->vrGroupBox2->Name = L"vrGroupBox2";
			this->vrGroupBox2->Size = System::Drawing::Size(520, 341);
			this->vrGroupBox2->TabIndex = 5;
			this->vrGroupBox2->TabStop = false;
			this->vrGroupBox2->Text = L"Tracker Camera 2";
			// 
			// visibilityPanel2
			// 
			this->visibilityPanel2->Location = System::Drawing::Point(10, 37);
			this->visibilityPanel2->Name = L"visibilityPanel2";
			this->visibilityPanel2->Size = System::Drawing::Size(500, 291);
			this->visibilityPanel2->TabIndex = 1;
			// 
			// vrGroupBox1
			// 
			this->vrGroupBox1->Controls->Add(this->visibilityPanel1);
			this->vrGroupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->vrGroupBox1->Location = System::Drawing::Point(460, 22);
			this->vrGroupBox1->Name = L"vrGroupBox1";
			this->vrGroupBox1->Size = System::Drawing::Size(520, 341);
			this->vrGroupBox1->TabIndex = 4;
			this->vrGroupBox1->TabStop = false;
			this->vrGroupBox1->Text = L"Tracker Camera 1";
			// 
			// visibilityPanel1
			// 
			this->visibilityPanel1->Location = System::Drawing::Point(10, 37);
			this->visibilityPanel1->Name = L"visibilityPanel1";
			this->visibilityPanel1->Size = System::Drawing::Size(500, 291);
			this->visibilityPanel1->TabIndex = 0;
			// 
			// instructionsTextBox
			// 
			this->instructionsTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->instructionsTextBox->Location = System::Drawing::Point(61, 59);
			this->instructionsTextBox->Multiline = true;
			this->instructionsTextBox->Name = L"instructionsTextBox";
			this->instructionsTextBox->Size = System::Drawing::Size(324, 291);
			this->instructionsTextBox->TabIndex = 6;
			// 
			// instructionsTextBox2
			// 
			this->instructionsTextBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->instructionsTextBox2->Location = System::Drawing::Point(61, 417);
			this->instructionsTextBox2->Multiline = true;
			this->instructionsTextBox2->Name = L"instructionsTextBox2";
			this->instructionsTextBox2->Size = System::Drawing::Size(324, 358);
			this->instructionsTextBox2->TabIndex = 13;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->fovPanel2);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(1000, 380);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(520, 409);
			this->groupBox1->TabIndex = 12;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Tracker Camera 2";
			// 
			// fovPanel2
			// 
			this->fovPanel2->Location = System::Drawing::Point(10, 37);
			this->fovPanel2->Name = L"fovPanel2";
			this->fovPanel2->Size = System::Drawing::Size(500, 358);
			this->fovPanel2->TabIndex = 1;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->fovPanel1);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox2->Location = System::Drawing::Point(460, 380);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(520, 409);
			this->groupBox2->TabIndex = 11;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Tracker Camera 1";
			// 
			// fovPanel1
			// 
			this->fovPanel1->Location = System::Drawing::Point(10, 37);
			this->fovPanel1->Name = L"fovPanel1";
			this->fovPanel1->Size = System::Drawing::Size(500, 358);
			this->fovPanel1->TabIndex = 0;
			// 
			// busy
			// 
			this->busy->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->busy->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 48, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->busy->ForeColor = System::Drawing::Color::Maroon;
			this->busy->Location = System::Drawing::Point(382, 346);
			this->busy->Name = L"busy";
			this->busy->Size = System::Drawing::Size(888, 184);
			this->busy->TabIndex = 15;
			this->busy->Text = L"Alignment in progress.\r\nPlease wait ...";
			this->busy->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// instructionsTextBox3
			// 
			this->instructionsTextBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->instructionsTextBox3->Location = System::Drawing::Point(52, 793);
			this->instructionsTextBox3->Multiline = true;
			this->instructionsTextBox3->Name = L"instructionsTextBox3";
			this->instructionsTextBox3->Size = System::Drawing::Size(333, 68);
			this->instructionsTextBox3->TabIndex = 16;
			this->instructionsTextBox3->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// SingleObjectForm
			// 
			this->AcceptButton = this->alignButton;
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->cancelButton;
			this->ClientSize = System::Drawing::Size(1652, 876);
			this->ControlBox = false;
			this->Controls->Add(this->instructionsTextBox3);
			this->Controls->Add(this->busy);
			this->Controls->Add(this->instructionsTextBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->instructionsTextBox);
			this->Controls->Add(this->vrGroupBox2);
			this->Controls->Add(this->vrGroupBox1);
			this->Controls->Add(this->cancelButton);
			this->Controls->Add(this->alignButton);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Location = System::Drawing::Point(20, 40);
			this->Name = L"SingleObjectForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"CODA Tracker Alignment";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &SingleObjectForm::Form1_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &SingleObjectForm::Form1_Shown);
			this->vrGroupBox2->ResumeLayout(false);
			this->vrGroupBox1->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {

				 // Create the OpenGLObjects that depict the marker array structure.
				 objects = new Grasp::GraspGLObjects();
				 char *model_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( modelFile ).ToPointer();
				 alignmentObject1 = objects->CreateChestMarkerStructure( model_file );
				 Sphere *sphere1 = new Sphere( 140.0 );
				 sphere1->SetColor( GREEN );	
				 sphere1->SetOffset( 0.0, 300.0, 0.0 );
				 alignmentObject1->AddComponent( sphere1 );
				 for ( int i = 0; i < alignmentObject1->components - 1; i++ ) {
					 alignmentObject1->component[i]->Disable();
				 }

				 alignmentObject2 = objects->CreateChestMarkerStructure( model_file );
				 Sphere *sphere2 = new Sphere( 140.0 );
				 sphere2->SetColor( GREEN );	
				 sphere2->SetOffset( 0.0, 300.0, 0.0 );
				 alignmentObject2->AddComponent( sphere2 );
				 for ( int i = 0; i < alignmentObject2->components - 1; i++ ) {
					 alignmentObject2->component[i]->Disable();
				 }

				 visibilityObject1 = objects->CreateChestMarkerStructure( model_file );
				 visibilityObject2 = objects->CreateChestMarkerStructure( model_file );
				 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( model_file ) );

				 // Create windows and viewpoints to show which markers are visible.
				 visibilityWindow1 = PsyPhy::CreateOpenGLWindowInForm( visibilityPanel1);
				 visibilityWindow2 = PsyPhy::CreateOpenGLWindowInForm( visibilityPanel2, visibilityWindow1->hRC );

				 // Create a viewpoint that looks at the origin from the negative Z axis.
				 // This is the cannonical viewpoint for an object at zero position and orientation.
				 objectViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
				 objectViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
				 objectViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

				 // Create windows and viewpoints to show what the CODA units are seeing.
				 fovWindow1 = PsyPhy::CreateOpenGLWindowInForm( fovPanel1 );
				 fovWindow2 = PsyPhy::CreateOpenGLWindowInForm( fovPanel2, fovWindow1->hRC );
				 fovSweetSpot = new Assembly();

				 Hole *hole = new Hole( 500.0,  2000.0, 2000.0 );
				 hole->SetColor( 0.15, 0.0, 0.0, 0.75 );	
				 hole->SetPosition( 0.0, 0.0, -100.0 );
				 fovSweetSpot->AddComponent( hole );

				 hole = new Hole( 100.0,  2000.0, 2000.0 );
				 hole->SetColor( 0.0, 0.0, 0.5, 0.5 );	
				 fovSweetSpot->AddComponent( hole );

				 Bar *bar = new Bar( 2000.0, 5.0, 5.0 );
				 bar->SetPosition( 0.0, -1000.0, 0.0 );
				 bar->SetOrientation( 0.0, 90.0, 0.0 );
				 bar->SetColor( GRAY );
				 fovSweetSpot->AddComponent( bar );
				 bar = new Bar( 2000.0, 5.0, 5.0 );
				 bar->SetPosition( 1000.0, 0.0, 0.0 );
				 bar->SetOrientation( 0.0, 0.0, 90.0 );
				 bar->SetColor( GRAY );
				 fovSweetSpot->AddComponent( bar );

				 fovSweetSpot->SetOrientation( 0.0, 90.0, 0.0 );
				 fovSweetSpot->SetPosition( 0.0, 1000.0, 0.0 );

				 Sphere *sphere = new Sphere( 100.0 );
				 sphere->SetColor( GREEN );	
				 sphere->SetOffset( 0.0, 300.0, 0.0 );
				 alignmentObject1->AddComponent( sphere );
				 alignmentObject2->AddComponent( sphere );

	
				 // Create a viewpoint that looks at the origin from the negative Y axis.
				 // This is where the CODAs are with respect to the workspace.
				 codaViewpoint = new Viewpoint( 6.0, 60.0, 10.0, 10000.0);
				 codaViewpoint->SetOrientation( 0.0, - 90.0, 0.0 );

				 // Initialize the state of the GL graphics engine.
				 // Some of this will be overridden by the object renderer.
				 glUsefulInitializeDefault();
				 glUsefulDefaultSpecularLighting( 0.75 );

				 // Show the Form as being inactive.
				 Enabled = false;
				 // Send a message to ground to show our progress.
				 dex->SendSubstep( STARTUP_VISIBILITY );

				 // Show a message while the tracker is initializing.
				 if ( noCoda ) instructionsTextBox->Text = instructionsTextBox->Text + " (noCoda debug mode)";
				 busy->Text = L"Tracker Initializing\r\nPlease wait ...";
				 busy->Visible = true;
				 // Make sure that the windows have refreshed  before starting up the CODA, 
				 // because it will take some time.
				 Refresh();
				 Application::DoEvents();
				 // Wait to make sure that the tracker is up and running so that we can kill it.
				 Sleep( 5000 );
				
				// Create the CODA tracker.
				coda = new CodaRTnetDaemonTracker();
				// Annul the previous alignment to get data in coordinates intrinsic to each CODA unit.
				// Send a message to ground to show our progress.
				dex->SendSubstep( ANNUL_ALIGNMENT );
				instructionsTextBox->Text = "[ Cancelling previous ... ]";
				Refresh();
				Application::DoEvents();
				char *tempfile = ".nullalignment.tmp";
				// Reset the alignment transformation.
				coda->AnnulAlignment( tempfile );
				DeleteFile( tempfile );
				
				// Start up the CODA tracker.
				coda->Initialize();
				 
				 // Send a message to ground to show our progress.
				 dex->SendSubstep( VISIBILITY );
				 // Hide the tracker message.
				 busy->Visible = false;
				 // Re-enable the Form as being inactive.
				 Enabled = true;
				 // Prompt for the required action.
				 instructionsTextBox->Text = "Check that all Chest Marker Support markers are visible to each Tracker Camera (all dots green).";
				 instructionsTextBox2->Text = "Adjust the orientation of the Tracker Cameras until the green ball is centered in the crosshairs for each Tracker Camera.";
				 instructionsTextBox3->Text = "When all conditions are met, press 'OK'.";

				 if ( !noCoda ) {
					 // Start a refresh time that will update the visibility of the LEDs in the GUI display.
					 CreateRefreshTimer( 300 );
					 StartRefreshTimer();
				 }
				 else Render();


			 }

	private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) { 
				 // Clear the task HK information to show that we are done.
				 dex->ResetTaskInfo();
				 if ( Environment::ExitCode == -2 ) {
					 dex->SendSubstep( ALIGNMENT_ERROR );
					 dex->SendTrackerStatus( TRACKER_ANOMALY ); 
				 }
			 }

	private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 // Send a message to ground to show our progress.
				 dex->SendSubstep( ABORT_REQUESTED );
				 System::Windows::Forms::DialogResult response;
				 response = MessageBox::Show( "Are you sure you want to exit without performing the alignment?", "AlignToRigidBodyGUI", MessageBoxButtons::YesNo );
				 if ( response == System::Windows::Forms::DialogResult::Yes ) {
					 if ( !noCoda ) {
						 coda->AbortAcquisition();
						 coda->Quit();
					 }
					 Environment::ExitCode = -1;
					 Close();
				 }
				 //Presumably, were were in the visibility check state and we are returning there.
				 dex->SendSubstep( VISIBILITY );
			 }


	private: System::Void alignButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 // Send a message to ground to show our progress.
				 dex->SendSubstep( SHUTDOWN_VISIBILITY );
				 // Take a picture of the setup as seen from the CODA.
				 dex->SnapPicture( "ALIGNMENT" );

				 StopRefreshTimer();

				 // Remove instruction.
				 instructionsTextBox->Text = "";
				 instructionsTextBox2->Text = "";
				 instructionsTextBox3->Text = "";
				 Refresh();
				 Application::DoEvents();
				 // Show a message while we are busy acquiring and computing the new alignment.
				 busy->Text = L"Alignment in Progress\r\nPlease wait ...";
				 busy->Visible = true;

				 // Show the buttons on the form as being inactive.
				 cancelButton->Enabled = false;
				 alignButton->Enabled = false;
				 Refresh();
				 Application::DoEvents();

				 if ( noCoda ) {
					 Sleep( 2000 );
					 Environment::ExitCode = 0;
					 Close();
					 return;
				 }

				 // Restart and acquire a short burst of marker data to be used to perform the alignment.
				 // Send a message to ground to show our progress.
				 dex->SendSubstep( STARTUP_INTRINSIC );
				 instructionsTextBox->Text = "[ Acquiring alignment data ... ]";
				 Sleep( 10 );
				 Refresh();
				 Application::DoEvents();

				 // Send a message to ground to show our progress.
				 dex->SendSubstep( ACQUIRE_INTRINSIC );

				 //// Get the pre-alignment transformation and make sure that it is null.
				 //Vector3 current_offset[MAX_UNITS];
				 //Matrix3x3 current_rotation[MAX_UNITS];
				 //coda->GetAlignment( current_offset, current_rotation );
				 //// If the offset is not zero, there was probably a problem trashing the alignment file on the CODA server.
				 //fAbortMessageOnCondition( 0.0 != coda->VectorNorm( current_offset[0] ) || 0.0 != coda->VectorNorm( current_offset[1] ), "AlignToRigidBody", "Alignment does not appear to have been nulled." );

				 fprintf( stderr, "Starting INTRINSIC acquisition ... " );
				 coda->StartAcquisition( 2.0 );
				 fprintf( stderr, "OK.\nAcquiring " );
				 // Just wait for the acquisition to finish.
				 while ( coda->GetAcquisitionState() ) {
					 Refresh();
					 Application::DoEvents();
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

				 // Use a CodaPoseTracker to compute the pose of the marker structure in the intrinsic frame of the CODA unit.
				 // We will then invert the pose to compute the transformation required by each unit.
				 // Send a message to ground to show our progress.
				 dex->SendSubstep( COMPUTE_ALIGNMENT );
				 instructionsTextBox->Text = "[ Computing transformations ... ]";
				 Refresh();
				 Application::DoEvents();
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
				 // Log the data acquired in aligned coordinates.
				 // The root for file names is passed as a String^. We need it as an ANSI string. Don't forget to free it aftwards.
				 String ^alignmentFilename = filenameRoot + ".alignment.dat";
				 char *alignment_file = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( alignmentFilename ).ToPointer();
				 // This does the real work.
				 coda->SetAlignmentFromPoses( poses, alignment_file );
				 System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( alignment_file ) );

				 // Restart and acquire a short burst of marker data to be used to verify the alignment.
				 // Send a message to ground to show our progress.
				 dex->SendSubstep( STARTUP_ALIGNED );
				 instructionsTextBox->Text = "[ Acquiring to confirm ... ]";
				 Refresh();
				 Application::DoEvents();

				 // Send a message to ground to show our progress.
				 dex->SendSubstep( ACQUIRE_ALIGNED );
				 fprintf( stderr, "Starting ALIGNED acquisition ... " );
				 coda->StartAcquisition( 5.0 );
				 fprintf( stderr, "OK.\nAcquiring " );
				 // Just wait for the acquisition to finish. 
				 while ( coda->GetAcquisitionState() ) {
					 fprintf( stderr, "." );
					 Application::DoEvents();
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
				 // Send a message to ground to show our progress.
				 dex->SendSubstep( SHUTDOWN_ALIGNED );

				 // Use a CodaPoseTracker to compute the pose of the marker structure in the newly aligned frame.
				 // We should get position zero and null orientation in each case.
				 char msg[25600] = "Alignment results: \n\n", line[256];
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
					 double orientation_error = codaPoseTracker->AngleBetweenOrientations( tracker_pose.pose.orientation, codaPoseTracker->nullQuaternion );
					 bool orientation_ok = orientation_error < maxOrientationError;
					 sprintf( line, "%d %s %6.3f %s\n", unit,  codaPoseTracker->qstr( tracker_pose.pose.orientation ), orientation_error, ( orientation_ok ? "OK" : "!!" ));
					 strcat( msg, line );
					 // Check that the alignment has been successful.
					 well_aligned &= ( position_ok & orientation_ok );
				 }
				 strcat( msg, "\n\nAre the serial numbers set correctly?\nAre the FTP login and password correct?" );
				 if ( !well_aligned || forceShow ) fMessageBox( MB_OK | MB_ICONEXCLAMATION , "AlignToRigidBodyGUI", msg );
				 if ( !well_aligned ) Environment::ExitCode = -2;
				 else Environment::ExitCode = 0;
				 // Here we quit, but we leave the daemon running.
				 coda->Quit();
				 // Close the form. 
				 Close();

			 }
	};
}

