#pragma once
#include <string.h>
#include <stdio.h>
#include "../GraspMMIUtilities/GraspMMIUtilities.h"

namespace GraspMMIHistory {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for GraspMMIHistoryStartup
	/// </summary>
	public ref class GraspMMIHistoryStartup : public System::Windows::Forms::Form
	{

	private:
		String^ packetBufferPathRoot;
		System::Windows::Forms::Timer^	timer;

	public:
		GraspMMIHistoryStartup( String^ rootPath )
		{
			packetBufferPathRoot = rootPath;
			InitializeComponent();
			char hkCacheFilename[1024];
			char *root = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( packetBufferPathRoot ).ToPointer();
			CreateGripPacketCacheFilename( hkCacheFilename, sizeof( hkCacheFilename ), GRIP_HK_BULK_PACKET, root );
			hkCacheFilenameText->Text = gcnew String( hkCacheFilename );
			System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( root ) );
			// Create Refresh Timer
			timer = gcnew System::Windows::Forms::Timer();
			timer->Interval = 1000;
			timer->Tick += gcnew EventHandler( this, &GraspMMIHistoryStartup::OnTimerElapsed );
			timer->Start();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GraspMMIHistoryStartup()
		{
			if (components)
			{
				delete components;
			}
		}

		void GraspMMIHistoryStartup::OnTimerElapsed( System::Object^ source, System::EventArgs ^ e ) {
			RefreshPrompt();
		}
		System::Void GraspMMIHistoryStartup_Shown(System::Object^  sender, System::EventArgs^  e) {
			RefreshPrompt();
		}	
		void GraspMMIHistoryStartup::RefreshPrompt( void ) {

			char hkCacheFilename[1024];
			char *root = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( packetBufferPathRoot ).ToPointer();
			CreateGripPacketCacheFilename( hkCacheFilename, sizeof( hkCacheFilename ), GRIP_HK_BULK_PACKET, root );
			hkCacheFilenameText->Text = gcnew String( hkCacheFilename );
			System::Runtime::InteropServices::Marshal::FreeHGlobal( IntPtr( root ) );

			int housekeeping_missing = _access( hkCacheFilename, 00 );

			// If HK packets are here, we exit this dialog with OK status, which will allow the program to continue.
			// If the user closes the window by the cancel button or close button, exit status will be Cancel 
			//  and the application will exit.
			if ( housekeeping_missing == 0 ) {
				this->DialogResult = System::Windows::Forms::DialogResult::OK;
				this->Close();
			}
		}

	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  hkCacheFilenameText;

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
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->hkCacheFilenameText = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button1->Location = System::Drawing::Point(173, 121);
			this->button1->Margin = System::Windows::Forms::Padding(2);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(112, 32);
			this->button1->TabIndex = 7;
			this->button1->Text = L"Cancel";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(11, 9);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(309, 24);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Waiting for initial data packets ...";
			// 
			// hkCacheFilenameText
			// 
			this->hkCacheFilenameText->AutoSize = true;
			this->hkCacheFilenameText->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->hkCacheFilenameText->Location = System::Drawing::Point(63, 67);
			this->hkCacheFilenameText->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->hkCacheFilenameText->Name = L"hkCacheFilenameText";
			this->hkCacheFilenameText->Size = System::Drawing::Size(51, 20);
			this->hkCacheFilenameText->TabIndex = 5;
			this->hkCacheFilenameText->Text = L"label2";
			// 
			// GraspMMIHistoryStartup
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(452, 166);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->hkCacheFilenameText);
			this->Name = L"GraspMMIHistoryStartup";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GraspMMIHistoryStartup";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
