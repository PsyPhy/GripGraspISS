#pragma once

#include <Windows.h>

#include "../OculusInterface/OculusInterface.h"

namespace OculusStartupCheck {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
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

	protected: 
	private: System::Windows::Forms::Button^  closeButton;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Panel^  panel1;

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
			this->closeButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// closeButton
			// 
			this->closeButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->closeButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->closeButton->Location = System::Drawing::Point(837, 540);
			this->closeButton->Name = L"closeButton";
			this->closeButton->Size = System::Drawing::Size(136, 55);
			this->closeButton->TabIndex = 1;
			this->closeButton->Text = L"Close";
			this->closeButton->UseVisualStyleBackColor = true;
			this->closeButton->Click += gcnew System::EventHandler(this, &Form1::closeButton_Click);
			// 
			// label1
			// 
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(25, 521);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(791, 115);
			this->label1->TabIndex = 2;
			this->label1->Text = resources->GetString(L"label1.Text");
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->panel1);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(18, 14);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(954, 481);
			this->groupBox1->TabIndex = 3;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"VR Mirror";
			// 
			// panel1
			// 
			this->panel1->Location = System::Drawing::Point(12, 39);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(927, 429);
			this->panel1->TabIndex = 0;
			this->panel1->Click += gcnew System::EventHandler(this, &Form1::panel1_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->closeButton;
			this->ClientSize = System::Drawing::Size(998, 645);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->closeButton);
			this->Name = L"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Perspectives VR Check";
			this->TopMost = true;
			this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void closeButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 Close();
			 }

	private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {
				 panel1_Click( sender, e );
			 }
	private: System::Void panel1_Click(System::Object^  sender, System::EventArgs^  e) {
				 HWND parent = static_cast<HWND>( panel1->Handle.ToPointer() );
				 char cmd[1024];
				 sprintf( cmd, "Start /min \"VR Test\" Executables\\PsyPhyOculusDemo.exe --parent=%d", parent );
				 system( cmd );
			 }
	};
}

