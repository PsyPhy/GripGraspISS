#pragma once

namespace GraspGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for CmdLinePopup1
	/// </summary>
	public ref class CmdLinePopup1 : public System::Windows::Forms::Form
	{
	public:
		CmdLinePopup1(void)
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
		~CmdLinePopup1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  commandLineTextBox;
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
			this->commandLineTextBox = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// commandLineTextBox
			// 
			this->commandLineTextBox->Location = System::Drawing::Point(9, 12);
			this->commandLineTextBox->Multiline = true;
			this->commandLineTextBox->Name = L"commandLineTextBox";
			this->commandLineTextBox->ReadOnly = true;
			this->commandLineTextBox->Size = System::Drawing::Size(1348, 59);
			this->commandLineTextBox->TabIndex = 0;
			// 
			// CmdLinePopup1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1366, 85);
			this->Controls->Add(this->commandLineTextBox);
			this->Name = L"CmdLinePopup1";
			this->Text = L"GRASP Command";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
