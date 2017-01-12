#pragma once
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "../GraspMMIUtilities/GraspMMIUtilities.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\Displays.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\Views.h"
#include "..\..\..\GripMMI\GripSourceCode\PsyPhy2dGraphicsLib\Layouts.h"
namespace GraspMMI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for GraspMMIDataPlotsStartup
	/// </summary>
	public ref class GraspMMIDataPlotsStartup : public System::Windows::Forms::Form
	{

	private:
		String^ packetBufferPathRoot;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  button1;
			 System::Windows::Forms::Timer^	timer;

	public:
		GraspMMIDataPlotsStartup( String^ root )
		{
			packetBufferPathRoot = root;
			InitializeComponent();
			// Create Refresh Timer
			timer = gcnew System::Windows::Forms::Timer();
			timer->Interval = 1000;
			timer->Tick += gcnew EventHandler( this, &GraspMMI::GraspMMIDataPlotsStartup::OnTimerElapsed );
			timer->Start();
		}

	protected:
		~GraspMMIDataPlotsStartup()
		{
			if (components)
			{
				delete components;
			}
		}


	protected:

	void GraspMMIDataPlotsStartup::OnTimerElapsed( System::Object^ source, System::EventArgs ^ e ) {
		RefreshPrompt();
	}
	System::Void GraspMMIDataPlotsStartup_Shown(System::Object^  sender, System::EventArgs^  e) {
		RefreshPrompt();
	}	
	
	void GraspMMIDataPlotsStartup::RefreshPrompt( void ) {

		char *root = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( packetBufferPathRoot ).ToPointer();

		char rtCacheFilename[1024];
		CreateGripPacketCacheFilename( rtCacheFilename, sizeof( rtCacheFilename ), GRIP_RT_SCIENCE_PACKET, root );
		rtCacheFilenameText->Text = gcnew String( rtCacheFilename );

		char hkCacheFilename[1024];
		CreateGripPacketCacheFilename( hkCacheFilename, sizeof( hkCacheFilename ), GRIP_HK_BULK_PACKET, root );
		hkCacheFilenameText->Text = gcnew String( hkCacheFilename );

		System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( root ) );

		Refresh();

		int science_missing = _access( rtCacheFilename, 00 );
		int housekeeping_missing = _access( hkCacheFilename, 00 );

		// If HK packets are here, we exit this dialog with OK status, which will allow the program to continue.
		// If the user closes the window by the cancel button or close button, exit status will be Cancel 
		//  and the application will exit.
		if ( housekeeping_missing == 0 ) {
			this->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->Close();
		}
	}

	private: System::Windows::Forms::Label^  rtCacheFilenameText;
	private: System::Windows::Forms::Label^  hkCacheFilenameText;
	protected: 

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
			this->rtCacheFilenameText = (gcnew System::Windows::Forms::Label());
			this->hkCacheFilenameText = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// rtCacheFilenameText
			// 
			this->rtCacheFilenameText->AutoSize = true;
			this->rtCacheFilenameText->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->rtCacheFilenameText->Location = System::Drawing::Point(74, 53);
			this->rtCacheFilenameText->Name = L"rtCacheFilenameText";
			this->rtCacheFilenameText->Size = System::Drawing::Size(64, 25);
			this->rtCacheFilenameText->TabIndex = 0;
			this->rtCacheFilenameText->Text = L"label1";
			// 
			// hkCacheFilenameText
			// 
			this->hkCacheFilenameText->AutoSize = true;
			this->hkCacheFilenameText->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->hkCacheFilenameText->Location = System::Drawing::Point(74, 84);
			this->hkCacheFilenameText->Name = L"hkCacheFilenameText";
			this->hkCacheFilenameText->Size = System::Drawing::Size(64, 25);
			this->hkCacheFilenameText->TabIndex = 1;
			this->hkCacheFilenameText->Text = L"label2";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(6, 4);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(390, 29);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Waiting for initial data packets ...";
			// 
			// button1
			// 
			this->button1->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button1->Location = System::Drawing::Point(222, 142);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(150, 40);
			this->button1->TabIndex = 3;
			this->button1->Text = L"Cancel";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// GraspMMIDataPlotsStartup
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->button1;
			this->ClientSize = System::Drawing::Size(595, 194);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->hkCacheFilenameText);
			this->Controls->Add(this->rtCacheFilenameText);
			this->Name = L"GraspMMIDataPlotsStartup";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GraspMMI Data Plots Startup";
			this->Shown += gcnew System::EventHandler(this, &GraspMMIDataPlotsStartup::GraspMMIDataPlotsStartup_Shown);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

};
}
