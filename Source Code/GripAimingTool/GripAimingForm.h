#pragma once

#include "../VectorsMixin/VectorsMixin.h"
#include "../Useful/OpenGLUseful.h"

#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"
#include "../GraspVR/GraspGLObjects.h"

#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaRTnetDaemonTracker.h"
#include "../Trackers/CodaPoseTracker.h"

#include "../GraspMMIUtilities/GripGLObjects.h"

namespace GripAimingTool {

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
	public ref class GripAimingForm : public System::Windows::Forms::Form
	{
	public:
		GripAimingForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GripAimingForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::GroupBox^  boresightGroup0;
	private: System::Windows::Forms::Panel^  boresightPanel0;
	private: System::Windows::Forms::Panel^  manipulandumPanel0;
	private: System::Windows::Forms::Panel^  wristPanel0;
	private: System::Windows::Forms::Panel^  tabletPanel0;
	private: System::Windows::Forms::Panel^  distancePanel0;

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
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->boresightGroup0 = (gcnew System::Windows::Forms::GroupBox());
			this->boresightPanel0 = (gcnew System::Windows::Forms::Panel());
			this->distancePanel0 = (gcnew System::Windows::Forms::Panel());
			this->tabletPanel0 = (gcnew System::Windows::Forms::Panel());
			this->wristPanel0 = (gcnew System::Windows::Forms::Panel());
			this->manipulandumPanel0 = (gcnew System::Windows::Forms::Panel());
			this->groupBox1->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->boresightGroup0->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->groupBox6);
			this->groupBox1->Controls->Add(this->groupBox5);
			this->groupBox1->Controls->Add(this->groupBox4);
			this->groupBox1->Controls->Add(this->groupBox3);
			this->groupBox1->Controls->Add(this->boresightGroup0);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(12, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(710, 738);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Tracking Camera 1";
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->manipulandumPanel0);
			this->groupBox6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox6->Location = System::Drawing::Point(480, 577);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(224, 152);
			this->groupBox6->TabIndex = 2;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Manipulandum Markers";
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->wristPanel0);
			this->groupBox5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox5->Location = System::Drawing::Point(243, 577);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(224, 152);
			this->groupBox5->TabIndex = 2;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Wrist Markers";
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->tabletPanel0);
			this->groupBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox4->Location = System::Drawing::Point(8, 577);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(224, 152);
			this->groupBox4->TabIndex = 1;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Tablet Markers";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->distancePanel0);
			this->groupBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox3->Location = System::Drawing::Point(596, 34);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(107, 537);
			this->groupBox3->TabIndex = 1;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Distance";
			// 
			// boresightGroup0
			// 
			this->boresightGroup0->Controls->Add(this->boresightPanel0);
			this->boresightGroup0->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->boresightGroup0->Location = System::Drawing::Point(8, 34);
			this->boresightGroup0->Name = L"boresightGroup0";
			this->boresightGroup0->Size = System::Drawing::Size(582, 537);
			this->boresightGroup0->TabIndex = 0;
			this->boresightGroup0->TabStop = false;
			this->boresightGroup0->Text = L"Boresight";
			// 
			// boresightPanel0
			// 
			this->boresightPanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->boresightPanel0->Location = System::Drawing::Point(10, 26);
			this->boresightPanel0->Name = L"boresightPanel0";
			this->boresightPanel0->Size = System::Drawing::Size(562, 499);
			this->boresightPanel0->TabIndex = 0;
			// 
			// distancePanel0
			// 
			this->distancePanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->distancePanel0->Location = System::Drawing::Point(10, 26);
			this->distancePanel0->Name = L"distancePanel0";
			this->distancePanel0->Size = System::Drawing::Size(87, 498);
			this->distancePanel0->TabIndex = 0;
			// 
			// tabletPanel0
			// 
			this->tabletPanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabletPanel0->Location = System::Drawing::Point(8, 25);
			this->tabletPanel0->Name = L"tabletPanel0";
			this->tabletPanel0->Size = System::Drawing::Size(208, 120);
			this->tabletPanel0->TabIndex = 0;
			// 
			// wristPanel0
			// 
			this->wristPanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->wristPanel0->Location = System::Drawing::Point(8, 25);
			this->wristPanel0->Name = L"wristPanel0";
			this->wristPanel0->Size = System::Drawing::Size(208, 120);
			this->wristPanel0->TabIndex = 1;
			// 
			// manipulandumPanel0
			// 
			this->manipulandumPanel0->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->manipulandumPanel0->Location = System::Drawing::Point(8, 25);
			this->manipulandumPanel0->Name = L"manipulandumPanel0";
			this->manipulandumPanel0->Size = System::Drawing::Size(208, 120);
			this->manipulandumPanel0->TabIndex = 1;
			// 
			// GripAimingForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1450, 762);
			this->Controls->Add(this->groupBox1);
			this->Name = L"GripAimingForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Grip Aiming Tool";
			this->Shown += gcnew System::EventHandler(this, &GripAimingForm::GripAimingForm_Shown);
			this->groupBox1->ResumeLayout(false);
			this->groupBox6->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->boresightGroup0->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

#define DEX_CODA_UNITS 1
#define DEX_MARKERS 20


		OpenGLWindow	**boresightWindow;
		Viewpoint		*boresightViewpoint;

		OpenGLWindow	**tabletWindow;
		OpenGLWindow	**wristWindow;
		OpenGLWindow	**manipulandumWindow;
		Viewpoint		*objectViewpoint;
		Viewpoint		*tabletViewpoint;

		OpenGLWindow **distanceWindow;
		OrthoViewpoint *distanceViewpoint;
		Patch *lowerExclusionZone, *upperExclusionZone;
		Assembly	*distanceBars;

		static double	minDisplayDistance = 1000.0;
		static double	maxDisplayDistance = 5000.0;
		static double	minAcceptableDistance = 1800.0;
		static double	maxAcceptableDistance = 4000.0;

		Assembly		*markerSpheres;
		static double	markerRadius = 20.0;
		Sphere			*targetSphere;
		static double	targetRadius = 200.0;

		static int		firstManipulandum = 0;
		static int		lastManipulandum = 7;
		static int		firstTablet = 8;
		static int		lastTablet = 11;
		static int		firstWrist = 12;
		static int		lastWrist = 19;

		GripManipulandum	*manipulandum;
		GripTablet			*tablet;
		GripWrist			*wrist;

		static double	objectMarkerRadius = 3.0;
		static double	tabletMarkerRadius = 30.0;

	private: void SetLighting( void ) {

				 GLfloat fintensity = 0.75;
				 GLfloat ambient = fintensity;
				 GLfloat diffuse = fintensity;
				 GLfloat specular = fintensity;

				 // Light definition
				 GLfloat LightPosition[] = { 10000.0, 0.0, 0.0, 0.0 };
				 GLfloat LightAmbient[]  = { ambient, ambient, ambient, 1.0};
				 GLfloat LightDiffuse[]  = { diffuse, diffuse, diffuse, 1.0};
				 GLfloat LightSpecular[] = { 0.0, 0.0, 0.0, 1.0};

				 glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
				 glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
				 glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
				 glLightfv( GL_LIGHT0, GL_SPECULAR, LightSpecular );

				 glEnable( GL_LIGHT0 );

				 glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
				 glEnable( GL_BLEND );

				 glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
				 glClearColor( 0.0F, 0.0F, 0.0F, 1.0F );

			 }

	private: void EraseWindow( OpenGLWindow *window, double r, double g, double b, double transparency ) {
				 window->Activate();
				 window->Clear( r, g, b, transparency );
				 window->Swap();
			 }
	private: void EraseWindow( OpenGLWindow *window ) {
				 window->Activate();
				 window->Clear( 0.1, 0.1, 0.3, 1.0 );
				 window->Swap();
			 }

	private: void DrawDistance( OpenGLWindow *window ) {
				 window->Activate();
				 window->Clear( 0.65, 0.85, 0.65, 1.0 );
				 distanceViewpoint->Apply( window );
				 lowerExclusionZone->Draw();
				 upperExclusionZone->Draw();
				 distanceBars->Draw();
				 window->Swap();
			 }

	private: void DrawBoresight( OpenGLWindow *window ) {
				 window->Activate();
				 window->Clear( 0.1, 0.1, 0.3, 1.0 );
				 // Draw the other objects with the hopes of seeing specular reflections. 
				 boresightViewpoint->Apply( window );
				 SetLighting();
				 markerSpheres->Draw();
				 targetSphere->Draw();
				 window->Swap();
			 }

	private: void DrawObject( OpenGLWindow *window, Viewpoint *viewpoint, OpenGLObject *object ) {
				 window->Activate();
				 window->Clear( 0.1, 0.1, 0.3, 1.0 );
				 // Draw the other objects with the hopes of seeing specular reflections. 
				 viewpoint->Apply( window );
				 SetLighting();
				 object->Draw();
				 window->Swap();
			 }

	private: System::Void GripAimingForm_Shown(System::Object^  sender, System::EventArgs^  e) {

				 // Create the required OpenGLWindows each linked to a pane in the Form.
				 // Forms does not allow mixed managed and unmanaged arrays.
				 // So to have arrays of the following, they have to be 
				 // allocated at runtime. 
				 boresightWindow = CreateOpenGLWindowArray( DEX_CODA_UNITS );
				 boresightWindow[0] = CreateOpenGLWindowInForm( boresightPanel0 );
				 //boresightWindow[1] = CreateOpenGLWindowInForm( boresightPanel1, boresightWindow[0]->hRC );
				 boresightViewpoint =  new Viewpoint( 6.0, 70.0, 1000.0, 6000.0);
				 // When then CODA is in it's 0 orientation, it points along
				 // the positive Y axis. This attitude reflects that in the 
				 // representation of the CODA bar.
				 boresightViewpoint->SetAttitude( 0.0, - 90.0, 0.0 );
				 // The origin of the CODA intrinsic coordinate system is the 
				 // center of the A camera.
				 boresightViewpoint->SetOffset( 400.0, 0.0, 0.0 );

				 tabletWindow = CreateOpenGLWindowArray( DEX_CODA_UNITS );
				 tabletWindow[0] = CreateOpenGLWindowInForm( tabletPanel0, boresightWindow[0]->hRC );
				 //tabletWindow[1] = CreateOpenGLWindowInForm( tabletPanel1, tabletWindow[0]->hRC );
				 wristWindow = CreateOpenGLWindowArray( DEX_CODA_UNITS );
				 wristWindow[0] = CreateOpenGLWindowInForm( wristPanel0, boresightWindow[0]->hRC );
				 //wristWindow[1] = CreateOpenGLWindowInForm( wristPanel1, wristWindow[0]->hRC );
				 manipulandumWindow = CreateOpenGLWindowArray( DEX_CODA_UNITS );
				 manipulandumWindow[0] = CreateOpenGLWindowInForm( manipulandumPanel0, boresightWindow[0]->hRC );
				 //manipulandumWindow[1] = CreateOpenGLWindowInForm( manipulandumPanel1, manipulandumWindow[0]->hRC );

				 // Create objects for the distance display.
				 distanceWindow = CreateOpenGLWindowArray( DEX_CODA_UNITS );
				 distanceWindow[0] = CreateOpenGLWindowInForm( distancePanel0, boresightWindow[0]->hRC );
				 //distanceWindow[1] = CreateOpenGLWindowInForm( distancePanel1, distanceWindow[0]->hRC );
				 distanceViewpoint = new OrthoViewpoint( -1.0, 1.0, minDisplayDistance, maxDisplayDistance );
				 lowerExclusionZone = new Patch( 2.0, minAcceptableDistance - minDisplayDistance );
				 lowerExclusionZone->SetOffset( 0.0, ( minAcceptableDistance - minDisplayDistance ) / 2.0, 0.0 );
				 lowerExclusionZone->SetPosition( 0.0, minDisplayDistance, - 10.0 );
				 lowerExclusionZone->SetColor( 1.0, 0.5, 0.5, 0.05 );
				 upperExclusionZone = new Patch( 2.0, maxDisplayDistance - maxAcceptableDistance );
				 upperExclusionZone->SetOffset( 0.0, - ( maxDisplayDistance - maxAcceptableDistance ) / 2.0, 0.0 );
				 upperExclusionZone->SetPosition( 0.0, maxDisplayDistance, - 10.0 );
				 upperExclusionZone->SetColor( 1.0, 0.5, 0.5, 0.05 );
				 distanceBars = new Assembly();
				 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
					 Slab *bar = new Slab( 1.0, 30.0, 20.0 );
					 bar->SetPosition( 0.0, minDisplayDistance + ( mrk + 2)  * 100.0, 5.0 );
					 bar->SetAttitude( 0.0, 0.0, 0.0 );
					 distanceBars->AddComponent( bar );
				 }

				 markerSpheres = new Assembly();
				 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
					 Sphere *sphere = new Sphere( markerRadius );
					 sphere->SetPosition( mrk  * 100.0, minDisplayDistance + mrk  * 100.0, mrk * 100.0 );
					 sphere->SetColor( 1.0, 0.0, 1.0, 1.0 );
					 markerSpheres->AddComponent( sphere );
				 }
				 targetSphere = new Sphere( targetRadius );
				 targetSphere->SetColor( 0.2, 1.0, 0.2, 0.35 );
				 targetSphere->SetPosition( 0.0, minDisplayDistance + 9  * 100.0, 0.0 );
				 targetSphere->SetAttitude( 90.0, 0.0, 0.0 );

				 Sphere *objectMarker[DEX_MARKERS];
				 for ( int mrk = 0; mrk < DEX_MARKERS; mrk++ ) {
					 objectMarker[mrk] = new Sphere( objectMarkerRadius );
				 }



				 objectViewpoint = new Viewpoint( 6.0, 2.0, 10.0, 10000.0);
				 objectViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
				 objectViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

				 static GLfloat	manipulandumColor[] = { 0.5, 0.0, 0.5, 0.5 }; 
				 manipulandum = new GripManipulandum();
				 manipulandum->SetAttitude( 0.0, 0.0, - 90.0 );
				 for ( int mrk = firstManipulandum; mrk <= lastManipulandum; mrk++ ) {
					 markerSpheres->component[mrk]->SetColor( manipulandumColor );
					 distanceBars->component[mrk]->SetColor( manipulandumColor );
					 objectMarker[mrk]->SetColor( manipulandumColor );
					 manipulandum->AddComponent( objectMarker[mrk] );
				 }

				 objectMarker[firstManipulandum + 0]->SetPosition( 13.0, -20.0, -9.0 );
				 objectMarker[firstManipulandum + 1]->SetPosition( 13.0, -20.0,  9.0 );
				 objectMarker[firstManipulandum + 2]->SetPosition( 13.0,   0.0, -9.0 );
				 objectMarker[firstManipulandum + 3]->SetPosition( 13.0,   0.0,  9.0 );
				 objectMarker[firstManipulandum + 4]->SetPosition( 9.0,   20.0, -9.0 );
				 objectMarker[firstManipulandum + 5]->SetPosition( 9.0,   20.0,  9.0 );
				 objectMarker[firstManipulandum + 6]->SetPosition( -10.0, 25.0, -9.0 );
				 objectMarker[firstManipulandum + 7]->SetPosition( -10.0, 25.0,  9.0 );


				 tabletViewpoint = new Viewpoint( 6.0, 30.0, 10.0, 10000.0);
				 tabletViewpoint->SetPosition( 0.0, 0.0, - 2000.0 );
				 tabletViewpoint->SetOrientation( 0.0, 0.0, 180.0 );

				 static GLfloat	tabletColor[] = { 0.0, 1.0, 1.0, 0.5 }; 
				 tablet = new GripTablet();
				 tablet->SetOrientation( 0.0, 0.0, 180.0 );
				 tablet->SetPosition( 250.0, -200.0, 0.0 );
				 for ( int mrk = firstTablet; mrk <= lastTablet; mrk++ ) {
					 markerSpheres->component[mrk]->SetColor( tabletColor );
					 distanceBars->component[mrk]->SetColor( tabletColor );
					 objectMarker[mrk]->SetColor( tabletColor );
					 objectMarker[mrk]->SetRadius( tabletMarkerRadius );
					 tablet->AddComponent( objectMarker[mrk] );
				 }
				 objectMarker[firstTablet + 0]->SetPosition(     220.0,   -250.0,  25.0 );
				 objectMarker[firstTablet + 1]->SetPosition(   - 220.0,   -250.0,  25.0 );
				 objectMarker[firstTablet + 2]->SetPosition(   - 220.0,   -100.0, 600.0 );
				 objectMarker[firstTablet + 3]->SetPosition(   - 220.0,   -100.0, 100.0 );

				 static GLfloat	wristColor[] = { 0.75, 0.5, 0.0, 0.5 }; 
				 wrist = new GripWrist();
				 wrist->SetOrientation( 0.0, 90.0, 0.0 );
				 for ( int mrk = firstWrist; mrk <= lastWrist; mrk++ ) {
					 markerSpheres->component[mrk]->SetColor( wristColor );
					 distanceBars->component[mrk]->SetColor( wristColor );
					 objectMarker[mrk]->SetColor( wristColor );
					 wrist->AddComponent( objectMarker[mrk] );
				 }
				 objectMarker[firstWrist + 0]->SetPosition( -24.0, -20.0,  -20.0 );
				 objectMarker[firstWrist + 1]->SetPosition(  -8.0, -20.0,  -30.0 );
				 objectMarker[firstWrist + 2]->SetPosition(   8.0, -20.0,  -30.0 );
				 objectMarker[firstWrist + 3]->SetPosition(  24.0, -20.0,  -20.0 );
				 objectMarker[firstWrist + 4]->SetPosition( -18.0,  20.0,  -30.0 );
				 objectMarker[firstWrist + 5]->SetPosition(  -6.0,  20.0,  -30.0 );
				 objectMarker[firstWrist + 6]->SetPosition(   6.0,  20.0,  -30.0 );
				 objectMarker[firstWrist + 7]->SetPosition(  18.0,  20.0,  -30.0 );

				 for ( int unit = 0; unit < DEX_CODA_UNITS; unit++ ) {
					 EraseWindow( boresightWindow[unit] );
					 EraseWindow( tabletWindow[unit] );
					 EraseWindow( wristWindow[unit] );
					 EraseWindow( manipulandumWindow[unit] );
					 DrawDistance( distanceWindow[unit] );
					 DrawBoresight( boresightWindow[unit] );
					 DrawObject( manipulandumWindow[unit], objectViewpoint, manipulandum );
					 DrawObject( tabletWindow[unit], tabletViewpoint, tablet );
					 DrawObject( wristWindow[unit], objectViewpoint, wrist );
				 }



			 }
	};
}

