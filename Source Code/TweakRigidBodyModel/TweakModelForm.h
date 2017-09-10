#pragma once

#include <Windows.h>

#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"

#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"

namespace TweakRigidBodyModel {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace PsyPhy;

	/// <summary>
	/// Summary for TweakModelForm
	/// </summary>
	public ref class TweakModelForm : public System::Windows::Forms::Form
	{
	public:

		PsyPhy::OpenGLWindow		*originalFrontWindow, *originalSideWindow;
		PsyPhy::OpenGLWindow		*alignedFrontWindow, *alignedSideWindow;
		PsyPhy::OpenGLWindow		*tiltedFrontWindow, *tiltedSideWindow;
		PsyPhy::Viewpoint			*frontViewpoint, *sideViewpoint;
		CodaPoseTracker				*tracker;

		int upper_left;
		int upper_right;
		int lower_left;
		int lower_right;
		bool debug;	

		PsyPhy::Assembly		*original, *tilted, *aligned;
		char *inputFile;
		char *outputFile;
	private: System::Windows::Forms::Label^  directoryLabel;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  outputFilenameLabel;


	public: 
		double pitch;

	public: 

		Assembly *CreateObjectFromModel( CodaPoseTracker *tr ) {

			Assembly *body = new Assembly();
			for ( int mrk = 0; mrk < tr->nModelMarkers; mrk++ ) {
				Sphere *sphere = new Sphere( 10.0 );
				sphere->SetColor( BLUE );
				sphere->SetPosition( tr->modelMarker[mrk].position );
				body->AddComponent( sphere );
			}
			body->component[upper_left]->SetColor( GREEN );
			body->component[upper_right]->SetColor( RED );
			body->component[lower_left]->SetColor( CYAN );
			body->component[lower_right]->SetColor( MAGENTA );

			return( body );

		}

		TweakModelForm( char *body_file, double pitch, char *output_file ) :

		upper_left( 6 ),
			upper_right( 0 ),
			lower_left( 7 ),
			lower_right( 1 ),
			debug( false )

		{

			inputFile = body_file;
			this->pitch = pitch;
			outputFile = output_file;

			InitializeComponent();

			// Create PoseTracker objects to handle the models.
			tracker = new CodaPoseTracker( nullptr );
			tracker->ReadModelMarkerPositions( body_file );

			// Fake misalignment.
			if ( debug ) {
				Quaternion ptch, yaw, roll;
				tracker->SetQuaterniond( ptch, 12.0, tracker->iVector );
				tracker->SetQuaterniond( yaw, 6.0, tracker->jVector );
				tracker->SetQuaterniond( roll, 22.0, tracker->kVector );

				for ( int mrk = 0; mrk < tracker->nModelMarkers; mrk++ ) {
					Vector3 tilted_location;
					tracker->RotateVector( tilted_location, ptch, tracker->modelMarker[mrk].position );
					tracker->CopyVector(  tracker->modelMarker[mrk].position, tilted_location );
					tracker->RotateVector( tilted_location, yaw, tracker->modelMarker[mrk].position );
					tracker->CopyVector(  tracker->modelMarker[mrk].position, tilted_location );
					tracker->RotateVector( tilted_location, roll, tracker->modelMarker[mrk].position );
					tracker->CopyVector(  tracker->modelMarker[mrk].position, tilted_location );
				}
			}


			// Find the 4 markers farthest in depth.
			int z_sorted_list[MAX_MARKERS];
			int i, j;
			z_sorted_list[0] = 0;
			for ( i = 1; i < tracker->nModelMarkers; i++ ) {
				for ( j = i; j > 0; j-- ) {
					if ( tracker->modelMarker[i].position[Z] < tracker->modelMarker[z_sorted_list[j-1]].position[Z] ) break;
					z_sorted_list[j] = z_sorted_list[j-1];
				}
				z_sorted_list[j] = i;
			}
			fOutputDebugString( "Sorted in depth:\n" );
			for ( i = 0; i < tracker->nModelMarkers; i++ ) fOutputDebugString( "  %d %s\n", i, tracker->vstr( tracker->modelMarker[z_sorted_list[i]].position ) );
			// Now sort those markers left to right.
			int x_sorted_list[4];
			x_sorted_list[0] = z_sorted_list[0];
			for ( i = 1; i < 4; i++ ) {
				for ( j = i; j > 0; j-- ) {
					if ( tracker->modelMarker[z_sorted_list[i]].position[X]  > tracker->modelMarker[x_sorted_list[j-1]].position[X] ) break;
					x_sorted_list[j] = x_sorted_list[j-1];
				}
				x_sorted_list[j] = z_sorted_list[i];
			}
			fOutputDebugString( "Farthest 4 sorted left to right:\n" );
			for ( i = 0; i < 4; i++ ) fOutputDebugString( "%d %s\n", i, tracker->vstr( tracker->modelMarker[x_sorted_list[i]].position ) );

			if ( tracker->modelMarker[x_sorted_list[1]].position[Y] > tracker->modelMarker[x_sorted_list[0]].position[Y] ) {
				upper_left = x_sorted_list[1];
				lower_left =  x_sorted_list[0];
			}
			else {
				upper_left = x_sorted_list[0];
				lower_left =  x_sorted_list[1];
			}
			if ( tracker->modelMarker[x_sorted_list[3]].position[Y] > tracker->modelMarker[x_sorted_list[2]].position[Y] ) {
				upper_right = x_sorted_list[3];
				lower_right = x_sorted_list[2];
			}
			else {
				upper_right = x_sorted_list[2];
				lower_right =  x_sorted_list[3];
			}

			original = CreateObjectFromModel( tracker );

			// Compute a rotation that will align marker positions to a reference frame
			// defined by the farthest 4 markers. Z will be perpendicular to the plane defined by
			// the 4 markers and X will be defined by the horizontal edge of the marker array.
			Vector3 top, bottom, left, right;
			tracker->SubtractVectors( top, tracker->modelMarker[upper_right].position, tracker->modelMarker[upper_left].position );
			tracker->SubtractVectors( bottom, tracker->modelMarker[lower_right].position, tracker->modelMarker[lower_left].position );
			tracker->SubtractVectors( left, tracker->modelMarker[upper_right].position, tracker->modelMarker[lower_right].position );
			tracker->SubtractVectors( right, tracker->modelMarker[upper_left].position, tracker->modelMarker[lower_left].position );
			Matrix3x3 rotation, transpose, skewed;
			tracker->AddVectors( skewed[X], top, bottom );
			tracker->AddVectors( skewed[Y], left, right );
			tracker->ComputeCrossProduct( skewed[Z], skewed[Y], skewed[X] );
			tracker->OrthonormalizeMatrix( transpose, skewed );
			tracker->TransposeMatrix( rotation, transpose );
			for ( int mrk = 0; mrk < tracker->nModelMarkers; mrk++ ) {
				Vector3 tilted_location;
				tracker->MultiplyVector( tilted_location, tracker->modelMarker[mrk].position, rotation );
				tracker->CopyVector(  tracker->modelMarker[mrk].position, tilted_location );
			}
			aligned = CreateObjectFromModel( tracker );

			// Tilt the model up in yaw.
			Quaternion tilt;
			tracker->SetQuaterniond( tilt, pitch, tracker->iVector );
			for ( int mrk = 0; mrk < tracker->nModelMarkers; mrk++ ) {
				Vector3 tilted_location;
				tracker->RotateVector( tilted_location, tilt, tracker->modelMarker[mrk].position );
				tracker->CopyVector(  tracker->modelMarker[mrk].position, tilted_location );
			}
			tilted = CreateObjectFromModel( tracker );


		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TweakModelForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Panel^  originalSide;
	private: System::Windows::Forms::Panel^  originalFront;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Panel^  alignedSide;
	private: System::Windows::Forms::Panel^  alignedFront;
private: System::Windows::Forms::GroupBox^  tiltedGroupBox;

	private: System::Windows::Forms::Panel^  tiltedSide;
	private: System::Windows::Forms::Panel^  tiltedFront;
	private: System::Windows::Forms::TextBox^  sourceTextBox;
private: System::Windows::Forms::Label^  sourceFilenameLabel;


	private: System::Windows::Forms::Button^  cancelButton;
	private: System::Windows::Forms::Button^  acceptButton;
	private: System::Windows::Forms::GroupBox^  groupBox4;

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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->originalSide = (gcnew System::Windows::Forms::Panel());
			this->originalFront = (gcnew System::Windows::Forms::Panel());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->alignedSide = (gcnew System::Windows::Forms::Panel());
			this->alignedFront = (gcnew System::Windows::Forms::Panel());
			this->tiltedGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->tiltedSide = (gcnew System::Windows::Forms::Panel());
			this->tiltedFront = (gcnew System::Windows::Forms::Panel());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->outputFilenameLabel = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->directoryLabel = (gcnew System::Windows::Forms::Label());
			this->sourceTextBox = (gcnew System::Windows::Forms::TextBox());
			this->sourceFilenameLabel = (gcnew System::Windows::Forms::Label());
			this->cancelButton = (gcnew System::Windows::Forms::Button());
			this->acceptButton = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->tiltedGroupBox->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->originalSide);
			this->groupBox1->Controls->Add(this->originalFront);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(11, 20);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(477, 735);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Original";
			// 
			// originalSide
			// 
			this->originalSide->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->originalSide->Location = System::Drawing::Point(17, 374);
			this->originalSide->Name = L"originalSide";
			this->originalSide->Size = System::Drawing::Size(436, 334);
			this->originalSide->TabIndex = 1;
			// 
			// originalFront
			// 
			this->originalFront->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->originalFront->Location = System::Drawing::Point(17, 34);
			this->originalFront->Name = L"originalFront";
			this->originalFront->Size = System::Drawing::Size(436, 334);
			this->originalFront->TabIndex = 0;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->alignedSide);
			this->groupBox2->Controls->Add(this->alignedFront);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox2->Location = System::Drawing::Point(495, 20);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(477, 735);
			this->groupBox2->TabIndex = 1;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Aligned";
			// 
			// alignedSide
			// 
			this->alignedSide->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->alignedSide->Location = System::Drawing::Point(17, 374);
			this->alignedSide->Name = L"alignedSide";
			this->alignedSide->Size = System::Drawing::Size(436, 334);
			this->alignedSide->TabIndex = 1;
			// 
			// alignedFront
			// 
			this->alignedFront->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->alignedFront->Location = System::Drawing::Point(17, 34);
			this->alignedFront->Name = L"alignedFront";
			this->alignedFront->Size = System::Drawing::Size(436, 334);
			this->alignedFront->TabIndex = 0;
			// 
			// tiltedGroupBox
			// 
			this->tiltedGroupBox->Controls->Add(this->tiltedSide);
			this->tiltedGroupBox->Controls->Add(this->tiltedFront);
			this->tiltedGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tiltedGroupBox->Location = System::Drawing::Point(979, 20);
			this->tiltedGroupBox->Name = L"tiltedGroupBox";
			this->tiltedGroupBox->Size = System::Drawing::Size(477, 735);
			this->tiltedGroupBox->TabIndex = 2;
			this->tiltedGroupBox->TabStop = false;
			this->tiltedGroupBox->Text = L"Tilted";
			// 
			// tiltedSide
			// 
			this->tiltedSide->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->tiltedSide->Location = System::Drawing::Point(17, 374);
			this->tiltedSide->Name = L"tiltedSide";
			this->tiltedSide->Size = System::Drawing::Size(436, 334);
			this->tiltedSide->TabIndex = 1;
			// 
			// tiltedFront
			// 
			this->tiltedFront->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->tiltedFront->Location = System::Drawing::Point(17, 34);
			this->tiltedFront->Name = L"tiltedFront";
			this->tiltedFront->Size = System::Drawing::Size(436, 334);
			this->tiltedFront->TabIndex = 0;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->label3);
			this->groupBox4->Controls->Add(this->outputFilenameLabel);
			this->groupBox4->Controls->Add(this->label2);
			this->groupBox4->Controls->Add(this->label1);
			this->groupBox4->Controls->Add(this->directoryLabel);
			this->groupBox4->Controls->Add(this->sourceTextBox);
			this->groupBox4->Controls->Add(this->sourceFilenameLabel);
			this->groupBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox4->Location = System::Drawing::Point(1467, 20);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(394, 645);
			this->groupBox4->TabIndex = 3;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Source Body File";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(12, 602);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(64, 20);
			this->label3->TabIndex = 6;
			this->label3->Text = L"Output:";
			// 
			// outputFilenameLabel
			// 
			this->outputFilenameLabel->AutoSize = true;
			this->outputFilenameLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->outputFilenameLabel->Location = System::Drawing::Point(82, 602);
			this->outputFilenameLabel->Name = L"outputFilenameLabel";
			this->outputFilenameLabel->Size = System::Drawing::Size(53, 20);
			this->outputFilenameLabel->TabIndex = 5;
			this->outputFilenameLabel->Text = L"label1";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(12, 34);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(48, 20);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Path:";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(12, 175);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(50, 20);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Input:";
			// 
			// directoryLabel
			// 
			this->directoryLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->directoryLabel->Location = System::Drawing::Point(66, 34);
			this->directoryLabel->Name = L"directoryLabel";
			this->directoryLabel->Size = System::Drawing::Size(311, 130);
			this->directoryLabel->TabIndex = 2;
			this->directoryLabel->Text = L"label1";
			// 
			// sourceTextBox
			// 
			this->sourceTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->sourceTextBox->Location = System::Drawing::Point(12, 214);
			this->sourceTextBox->Multiline = true;
			this->sourceTextBox->Name = L"sourceTextBox";
			this->sourceTextBox->ReadOnly = true;
			this->sourceTextBox->Size = System::Drawing::Size(365, 370);
			this->sourceTextBox->TabIndex = 1;
			// 
			// sourceFilenameLabel
			// 
			this->sourceFilenameLabel->AutoSize = true;
			this->sourceFilenameLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->sourceFilenameLabel->Location = System::Drawing::Point(82, 175);
			this->sourceFilenameLabel->Name = L"sourceFilenameLabel";
			this->sourceFilenameLabel->Size = System::Drawing::Size(53, 20);
			this->sourceFilenameLabel->TabIndex = 0;
			this->sourceFilenameLabel->Text = L"label1";
			// 
			// cancelButton
			// 
			this->cancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->cancelButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cancelButton->Location = System::Drawing::Point(1507, 680);
			this->cancelButton->Name = L"cancelButton";
			this->cancelButton->Size = System::Drawing::Size(134, 65);
			this->cancelButton->TabIndex = 4;
			this->cancelButton->Text = L"Cancel";
			this->cancelButton->UseVisualStyleBackColor = true;
			this->cancelButton->Click += gcnew System::EventHandler(this, &TweakModelForm::cancelButton_Click);
			// 
			// acceptButton
			// 
			this->acceptButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->acceptButton->Location = System::Drawing::Point(1687, 680);
			this->acceptButton->Name = L"acceptButton";
			this->acceptButton->Size = System::Drawing::Size(134, 65);
			this->acceptButton->TabIndex = 5;
			this->acceptButton->Text = L"Accept";
			this->acceptButton->UseVisualStyleBackColor = true;
			this->acceptButton->Click += gcnew System::EventHandler(this, &TweakModelForm::acceptButton_Click);
			// 
			// TweakModelForm
			// 
			this->AcceptButton = this->acceptButton;
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->cancelButton;
			this->ClientSize = System::Drawing::Size(1878, 772);
			this->Controls->Add(this->acceptButton);
			this->Controls->Add(this->cancelButton);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->tiltedGroupBox);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Name = L"TweakModelForm";
			this->Text = L"Tweak Rigid Body Model";
			this->Shown += gcnew System::EventHandler(this, &TweakModelForm::TweakModelForm_Shown);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &TweakModelForm::TweakModelForm_Paint);
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->tiltedGroupBox->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 

		System::Void TweakModelForm_Shown(System::Object^  sender, System::EventArgs^  e) {

			// Show the source body file in the comments so that we see where it came from.
			char line[1024];
			GetCurrentDirectory( sizeof( line ), line );
			directoryLabel->Text = gcnew String( "\"" ) + gcnew String( line ) + gcnew String( "\"" );

			sourceFilenameLabel->Text = gcnew String( inputFile );
			outputFilenameLabel->Text = gcnew String( outputFile );

			tiltedGroupBox->Text = "Tilted by " + pitch.ToString() + "°";

			FILE *fp = fopen( inputFile, "r" );
			fAbortMessageOnCondition( ( NULL == fp ), "TweakRigidBodyModel", "Error opening %s for read.", inputFile );	
			sourceTextBox->Clear();
			while ( NULL != fgets( line, sizeof( line ), fp ) ) {
				sourceTextBox->AppendText( gcnew String( line ) );
			}
			fclose( fp );

			// Create windows and viewpoints to show the geometry.
			originalFrontWindow = PsyPhy::CreateOpenGLWindowInForm( originalFront );
			originalSideWindow = PsyPhy::CreateOpenGLWindowInForm( originalSide, originalFrontWindow->hRC );
			alignedFrontWindow = PsyPhy::CreateOpenGLWindowInForm( alignedFront, originalFrontWindow->hRC );
			alignedSideWindow = PsyPhy::CreateOpenGLWindowInForm( alignedSide, originalFrontWindow->hRC );
			tiltedFrontWindow = PsyPhy::CreateOpenGLWindowInForm( tiltedFront, originalFrontWindow->hRC );
			tiltedSideWindow = PsyPhy::CreateOpenGLWindowInForm( tiltedSide, originalFrontWindow->hRC );

			// Create a viewpoint that looks at the origin from the negative Z axis.
			// This is the cannonical viewpoint for an object at zero position and orientation.
			frontViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
			frontViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
			frontViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

			// Create a viewpoint that looks at the origin from the side.
			sideViewpoint = new Viewpoint( 6.0, 10.0, 10.0, 10000.0);
			sideViewpoint->SetPosition( 2000.0, 0.0, 0.0 );
			sideViewpoint->SetOrientation( 0.0, 0.0, - 90.0 );

			Render();

		}

		System::Void Render ( void ) {

			// Update the 3D view of the alignment object.

			originalFrontWindow->Activate();
			originalFrontWindow->Clear( 0.10, 0.10, 0.30 );
			frontViewpoint->Apply( originalFrontWindow, CYCLOPS );
			original->Draw();
			originalFrontWindow->Swap();

			originalSideWindow->Activate();
			originalSideWindow->Clear( 0.10, 0.10, 0.30 );
			sideViewpoint->Apply( originalSideWindow, CYCLOPS );
			original->Draw();
			originalSideWindow->Swap();

			alignedFrontWindow->Activate();
			alignedFrontWindow->Clear( 0.10, 0.10, 0.30 );
			frontViewpoint->Apply( alignedFrontWindow, CYCLOPS );
			aligned->Draw();
			alignedFrontWindow->Swap();

			alignedSideWindow->Activate();
			alignedSideWindow->Clear( 0.10, 0.10, 0.30 );
			sideViewpoint->Apply( alignedSideWindow, CYCLOPS );
			aligned->Draw();
			alignedSideWindow->Swap();

			tiltedFrontWindow->Activate();
			tiltedFrontWindow->Clear( 0.10, 0.10, 0.30 );
			frontViewpoint->Apply( tiltedFrontWindow, CYCLOPS );
			tilted->Draw();
			tiltedFrontWindow->Swap();

			tiltedSideWindow->Activate();
			tiltedSideWindow->Clear( 0.10, 0.10, 0.30 );
			sideViewpoint->Apply( tiltedSideWindow, CYCLOPS );
			tilted->Draw();
			tiltedSideWindow->Swap();



		}
	private: System::Void TweakModelForm_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
				 Render();
			 }
	private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 Close();
			 }
	private: System::Void acceptButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 // Output the new body file to stdout.
				 FILE *fout = fopen( outputFile, "w" );
				 fAbortMessageOnCondition( ( NULL == fout ), "TweakRigidBodyModel", "Error opening %s for read.", inputFile );	
				 tracker->WriteModelMarkerPositions( fout );
				 fprintf( fout, ";;;  from %s, then tilted in pitch by %.0f°\n", inputFile, pitch );
				 // Include the source body file in the comments so that we see where it came from.
				 fprintf( fout, "\n;;; Source file (%s): \n\n", inputFile );
				 FILE *fp = fopen( inputFile, "r" );
				 fAbortMessageOnCondition( ( NULL == fp ), "TweakRigidBodyModel", "Error opening %s for read.", inputFile );	
				 char line[1024];
				 while ( NULL != fgets( line, sizeof( line ), fp ) ) {
					 fprintf( fout, ";;;  %s", line );
				 }
				 fclose( fp );
				 fclose( fout );

				 Close();
			 }
	
};
}

