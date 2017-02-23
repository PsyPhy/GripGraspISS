#pragma once
///
/// Module:	GripMMI
/// 
///	Author:					J. McIntyre, PsyPhy Consulting
/// Initial release:		18 December 2014
/// Modification History:	see https://github.com/PsyPhy/GripMMI
///
/// Copyright (c) 2014, 2015 PsyPhy Consulting
///

// Defines a dialog window to present 'about' information, including release info.

namespace GripMMI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for GripMMIAbout
	/// </summary>
	public ref class GripMMIAbout : public System::Windows::Forms::Form
	{
	public:
		GripMMIAbout( const char *version_string, const char *build_string, const char *script_string )
		{
			InitializeComponent();
			versionText->Text = gcnew String( "Source Release: " ) + gcnew String( version_string );
			buildText->Text =  gcnew String( "Build Info: " ) + gcnew String( build_string );
			scriptText->Text = gcnew String( "Scripts: " ) + gcnew String( script_string );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GripMMIAbout()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  versionText;
	protected: 

	private: System::Windows::Forms::Button^  aboutOkButton;
	private: System::Windows::Forms::PictureBox^  LogoPictureBox;
	private: System::Windows::Forms::Label^  copyrightText;
	private: System::Windows::Forms::Label^  buildText;
	private: System::Windows::Forms::Label^  scriptText;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GripMMIAbout::typeid));
			this->versionText = (gcnew System::Windows::Forms::Label());
			this->aboutOkButton = (gcnew System::Windows::Forms::Button());
			this->LogoPictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->copyrightText = (gcnew System::Windows::Forms::Label());
			this->buildText = (gcnew System::Windows::Forms::Label());
			this->scriptText = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LogoPictureBox))->BeginInit();
			this->SuspendLayout();
			// 
			// versionText
			// 
			this->versionText->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->versionText->Location = System::Drawing::Point(203, 12);
			this->versionText->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->versionText->Name = L"versionText";
			this->versionText->Size = System::Drawing::Size(612, 19);
			this->versionText->TabIndex = 0;
			this->versionText->Text = L"GripMMI V \?.\?";
			this->versionText->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// aboutOkButton
			// 
			this->aboutOkButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->aboutOkButton->Location = System::Drawing::Point(461, 148);
			this->aboutOkButton->Margin = System::Windows::Forms::Padding(4);
			this->aboutOkButton->Name = L"aboutOkButton";
			this->aboutOkButton->Size = System::Drawing::Size(96, 32);
			this->aboutOkButton->TabIndex = 5;
			this->aboutOkButton->Text = L"OK";
			this->aboutOkButton->UseVisualStyleBackColor = true;
			this->aboutOkButton->Click += gcnew System::EventHandler(this, &GripMMIAbout::aboutOkButton_Click);
			// 
			// LogoPictureBox
			// 
			this->LogoPictureBox->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"LogoPictureBox.Image")));
			this->LogoPictureBox->Location = System::Drawing::Point(13, 21);
			this->LogoPictureBox->Margin = System::Windows::Forms::Padding(4);
			this->LogoPictureBox->Name = L"LogoPictureBox";
			this->LogoPictureBox->Size = System::Drawing::Size(176, 162);
			this->LogoPictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->LogoPictureBox->TabIndex = 6;
			this->LogoPictureBox->TabStop = false;
			// 
			// copyrightText
			// 
			this->copyrightText->AutoSize = true;
			this->copyrightText->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->copyrightText->Location = System::Drawing::Point(424, 87);
			this->copyrightText->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->copyrightText->Name = L"copyrightText";
			this->copyrightText->Size = System::Drawing::Size(170, 51);
			this->copyrightText->TabIndex = 7;
			this->copyrightText->Text = L"Copyright (c) 2014 - 2017\r\nPsyPhy Consulting\r\nAll rights reserved.";
			this->copyrightText->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// buildText
			// 
			this->buildText->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 6, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->buildText->Location = System::Drawing::Point(203, 31);
			this->buildText->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->buildText->Name = L"buildText";
			this->buildText->Size = System::Drawing::Size(612, 19);
			this->buildText->TabIndex = 8;
			this->buildText->Text = L"<build info>";
			this->buildText->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// scriptText
			// 
			this->scriptText->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->scriptText->Location = System::Drawing::Point(203, 58);
			this->scriptText->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->scriptText->Name = L"scriptText";
			this->scriptText->Size = System::Drawing::Size(612, 19);
			this->scriptText->TabIndex = 9;
			this->scriptText->Text = L"GripMMI V \?.\?";
			this->scriptText->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// GripMMIAbout
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->ClientSize = System::Drawing::Size(828, 196);
			this->Controls->Add(this->scriptText);
			this->Controls->Add(this->copyrightText);
			this->Controls->Add(this->LogoPictureBox);
			this->Controls->Add(this->aboutOkButton);
			this->Controls->Add(this->versionText);
			this->Controls->Add(this->buildText);
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"GripMMIAbout";
			this->Text = L"GripMMIAbout";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->LogoPictureBox))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void aboutOkButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 this->Close();
			 }
};
}
