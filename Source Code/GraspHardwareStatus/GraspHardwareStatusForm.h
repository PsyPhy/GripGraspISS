#pragma once

#include <Windows.h>

#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../Trackers/CodaRTnetTracker.h"
#include "../Trackers/CodaPoseTracker.h"
#include "../GraspVR/GraspGLObjects.h"

// The following should be integrated elsewhere.
#include "../GraspGUIBellsAndWhistles/OpenGLWindowsInForms.h"

#include "../DexServices/DexServices.h"

#define BODIES 3
#define HMD 0
#define HAND 1
#define CHEST 2

#define CODAS  2

namespace GraspHardwareStatus {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	using namespace PsyPhy;
	using namespace Grasp;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{

	private:

		OpenGLWindow *hmdWindow0;
		OpenGLWindow *hmdWindow1;
		OpenGLWindow *handWindow0;
		OpenGLWindow *handWindow1;
		OpenGLWindow *chestWindow0;
	private: System::Windows::Forms::RichTextBox^  richTextBox1;

			 OpenGLWindow *chestWindow1;

	public:
		Form1(void)
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
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  chestGroupBox;
	private: System::Windows::Forms::Panel^  chestPanel1;
	private: System::Windows::Forms::Panel^  chestPanel0;
	private: System::Windows::Forms::GroupBox^  handGroupBox;
	private: System::Windows::Forms::Panel^  handPanel1;
	private: System::Windows::Forms::Panel^  handPanel0;
	private: System::Windows::Forms::GroupBox^  hmdGroupBox;
	private: System::Windows::Forms::Panel^  hmdPanel1;
	private: System::Windows::Forms::Panel^  hmdPanel0;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	protected: 

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->chestGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->chestPanel1 = (gcnew System::Windows::Forms::Panel());
			this->chestPanel0 = (gcnew System::Windows::Forms::Panel());
			this->handGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->handPanel1 = (gcnew System::Windows::Forms::Panel());
			this->handPanel0 = (gcnew System::Windows::Forms::Panel());
			this->hmdGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->hmdPanel1 = (gcnew System::Windows::Forms::Panel());
			this->hmdPanel0 = (gcnew System::Windows::Forms::Panel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->richTextBox1 = (gcnew System::Windows::Forms::RichTextBox());
			this->chestGroupBox->SuspendLayout();
			this->handGroupBox->SuspendLayout();
			this->hmdGroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// chestGroupBox
			// 
			this->chestGroupBox->Controls->Add(this->chestPanel1);
			this->chestGroupBox->Controls->Add(this->chestPanel0);
			this->chestGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->chestGroupBox->Location = System::Drawing::Point(12, 724);
			this->chestGroupBox->Name = L"chestGroupBox";
			this->chestGroupBox->Size = System::Drawing::Size(558, 256);
			this->chestGroupBox->TabIndex = 0;
			this->chestGroupBox->TabStop = false;
			this->chestGroupBox->Text = L"Chest Markers";
			// 
			// chestPanel1
			// 
			this->chestPanel1->Location = System::Drawing::Point(291, 33);
			this->chestPanel1->Name = L"chestPanel1";
			this->chestPanel1->Size = System::Drawing::Size(256, 205);
			this->chestPanel1->TabIndex = 2;
			// 
			// chestPanel0
			// 
			this->chestPanel0->Location = System::Drawing::Point(12, 33);
			this->chestPanel0->Name = L"chestPanel0";
			this->chestPanel0->Size = System::Drawing::Size(256, 205);
			this->chestPanel0->TabIndex = 1;
			// 
			// handGroupBox
			// 
			this->handGroupBox->Controls->Add(this->handPanel1);
			this->handGroupBox->Controls->Add(this->handPanel0);
			this->handGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->handGroupBox->Location = System::Drawing::Point(12, 450);
			this->handGroupBox->Name = L"handGroupBox";
			this->handGroupBox->Size = System::Drawing::Size(558, 256);
			this->handGroupBox->TabIndex = 1;
			this->handGroupBox->TabStop = false;
			this->handGroupBox->Text = L"Hand Markers";
			// 
			// handPanel1
			// 
			this->handPanel1->Location = System::Drawing::Point(291, 33);
			this->handPanel1->Name = L"handPanel1";
			this->handPanel1->Size = System::Drawing::Size(256, 205);
			this->handPanel1->TabIndex = 2;
			// 
			// handPanel0
			// 
			this->handPanel0->Location = System::Drawing::Point(12, 33);
			this->handPanel0->Name = L"handPanel0";
			this->handPanel0->Size = System::Drawing::Size(256, 205);
			this->handPanel0->TabIndex = 1;
			// 
			// hmdGroupBox
			// 
			this->hmdGroupBox->Controls->Add(this->hmdPanel1);
			this->hmdGroupBox->Controls->Add(this->hmdPanel0);
			this->hmdGroupBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->hmdGroupBox->Location = System::Drawing::Point(12, 176);
			this->hmdGroupBox->Name = L"hmdGroupBox";
			this->hmdGroupBox->Size = System::Drawing::Size(558, 256);
			this->hmdGroupBox->TabIndex = 2;
			this->hmdGroupBox->TabStop = false;
			this->hmdGroupBox->Text = L"HMD Markers";
			// 
			// hmdPanel1
			// 
			this->hmdPanel1->Location = System::Drawing::Point(291, 33);
			this->hmdPanel1->Name = L"hmdPanel1";
			this->hmdPanel1->Size = System::Drawing::Size(256, 205);
			this->hmdPanel1->TabIndex = 2;
			// 
			// hmdPanel0
			// 
			this->hmdPanel0->Location = System::Drawing::Point(12, 33);
			this->hmdPanel0->Name = L"hmdPanel0";
			this->hmdPanel0->Size = System::Drawing::Size(256, 205);
			this->hmdPanel0->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(45, 981);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(205, 25);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Tracker Camera Bar 1";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(323, 981);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(205, 25);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Tracker Camera Bar 2";
			// 
			// richTextBox1
			// 
			this->richTextBox1->Location = System::Drawing::Point(16, 22);
			this->richTextBox1->Name = L"richTextBox1";
			this->richTextBox1->Size = System::Drawing::Size(553, 128);
			this->richTextBox1->TabIndex = 5;
			this->richTextBox1->Text = resources->GetString(L"richTextBox1.Text");
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(582, 1015);
			this->Controls->Add(this->richTextBox1);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->hmdGroupBox);
			this->Controls->Add(this->handGroupBox);
			this->Controls->Add(this->chestGroupBox);
			this->Name = L"Form1";
			this->Text = L"GRASP Hardware Status";
			this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
			this->chestGroupBox->ResumeLayout(false);
			this->handGroupBox->ResumeLayout(false);
			this->hmdGroupBox->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {

				int bdy, unit;

				hmdWindow0 = PsyPhy::CreateOpenGLWindowInForm( hmdPanel0 );
				hmdWindow1 = PsyPhy::CreateOpenGLWindowInForm( hmdPanel1, hmdWindow0->hRC );
				handWindow0 = PsyPhy::CreateOpenGLWindowInForm( handPanel0,hmdWindow0->hRC );
				handWindow1 = PsyPhy::CreateOpenGLWindowInForm( handPanel1, hmdWindow0->hRC );
				chestWindow0 = PsyPhy::CreateOpenGLWindowInForm( chestPanel0, hmdWindow0->hRC );
				chestWindow1 = PsyPhy::CreateOpenGLWindowInForm( chestPanel1, hmdWindow0->hRC );

				 }
};

}

