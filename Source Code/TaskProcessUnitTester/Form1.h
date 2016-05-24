#pragma once

#include <stdlib.h>

namespace TaskProcessUnitTester {

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
		Form1( array<System::String ^> ^args )
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			cmdTextBox->Text = "";
			for ( int i = 0; i < args->Length; i++ ) cmdTextBox->Text += args[i] + " ";
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



	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TextBox^  cmdTextBox;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Button^  exitMinusOneButton;
	private: System::Windows::Forms::Button^  exitZeroButton;



	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::TextBox^  altCodeTextBox;
	private: System::Windows::Forms::Button^  exitAlternateButton;


	private: System::Windows::Forms::Label^  label3;


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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->cmdTextBox = (gcnew System::Windows::Forms::TextBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->exitMinusOneButton = (gcnew System::Windows::Forms::Button());
			this->exitZeroButton = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->altCodeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->exitAlternateButton = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(47, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(643, 51);
			this->label1->TabIndex = 4;
			this->label1->Text = resources->GetString(L"label1.Text");
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->cmdTextBox);
			this->groupBox1->Location = System::Drawing::Point(26, 77);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(710, 166);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Command Line to be Executed";
			// 
			// cmdTextBox
			// 
			this->cmdTextBox->Location = System::Drawing::Point(24, 21);
			this->cmdTextBox->Multiline = true;
			this->cmdTextBox->Name = L"cmdTextBox";
			this->cmdTextBox->Size = System::Drawing::Size(662, 122);
			this->cmdTextBox->TabIndex = 4;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->exitMinusOneButton);
			this->groupBox2->Controls->Add(this->exitZeroButton);
			this->groupBox2->Location = System::Drawing::Point(26, 311);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(708, 90);
			this->groupBox2->TabIndex = 6;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Common Return Codes";
			// 
			// exitMinusOneButton
			// 
			this->exitMinusOneButton->Location = System::Drawing::Point(411, 27);
			this->exitMinusOneButton->Name = L"exitMinusOneButton";
			this->exitMinusOneButton->Size = System::Drawing::Size(191, 50);
			this->exitMinusOneButton->TabIndex = 4;
			this->exitMinusOneButton->Text = L"Error Return (code -1)";
			this->exitMinusOneButton->UseVisualStyleBackColor = true;
			this->exitMinusOneButton->Click += gcnew System::EventHandler(this, &Form1::exitMinusOneButton_Click);
			// 
			// exitZeroButton
			// 
			this->exitZeroButton->Location = System::Drawing::Point(107, 27);
			this->exitZeroButton->Name = L"exitZeroButton";
			this->exitZeroButton->Size = System::Drawing::Size(191, 50);
			this->exitZeroButton->TabIndex = 3;
			this->exitZeroButton->Text = L"Normal Return (code 0)";
			this->exitZeroButton->UseVisualStyleBackColor = true;
			this->exitZeroButton->Click += gcnew System::EventHandler(this, &Form1::exitZeroButton_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(47, 260);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(493, 34);
			this->label2->TabIndex = 7;
			this->label2->Text = L"The two most common return codes are 0 for normal exit and -1 for error exit.\r\nTh" 
				L"e GraspGUI scripts should handle at least these two cases.";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->altCodeTextBox);
			this->groupBox3->Controls->Add(this->exitAlternateButton);
			this->groupBox3->Location = System::Drawing::Point(26, 469);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(704, 90);
			this->groupBox3->TabIndex = 8;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Alternate Return Codes";
			// 
			// altCodeTextBox
			// 
			this->altCodeTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->altCodeTextBox->Location = System::Drawing::Point(408, 32);
			this->altCodeTextBox->Name = L"altCodeTextBox";
			this->altCodeTextBox->Size = System::Drawing::Size(191, 38);
			this->altCodeTextBox->TabIndex = 6;
			this->altCodeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->altCodeTextBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::altCodeTextBox_KeyPress);
			// 
			// exitAlternateButton
			// 
			this->exitAlternateButton->Location = System::Drawing::Point(104, 26);
			this->exitAlternateButton->Name = L"exitAlternateButton";
			this->exitAlternateButton->Size = System::Drawing::Size(191, 50);
			this->exitAlternateButton->TabIndex = 5;
			this->exitAlternateButton->Text = L"Alternate Code";
			this->exitAlternateButton->UseVisualStyleBackColor = true;
			this->exitAlternateButton->Click += gcnew System::EventHandler(this, &Form1::exitAlternateButton_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(47, 418);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(655, 34);
			this->label3->TabIndex = 9;
			this->label3->Text = L"If your external command can return other values you cant test the GUI behvior by" 
				L" manually  specifying\r\nan alternate return code to be sent to GraspGUI.";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(759, 572);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->label1);
			this->Name = L"Form1";
			this->Text = L"Grasp Process Simulator";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void exitZeroButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 exit( 0 );
			 }
	private: System::Void exitMinusOneButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 exit( -1 );
			 }
	private: System::Void exitAlternateButton_Click(System::Object^  sender, System::EventArgs^  e) {

				 try {
					 int number = Convert::ToInt32( altCodeTextBox->Text );
					 exit( number );
				 }
				 catch (FormatException ^e) {
					 MessageBox::Show( "Value is not an integer.", "TaskProcessUnitTester", MessageBoxButtons::OK );
				 }
			 }

private: System::Void altCodeTextBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
			 if ( e->KeyChar == (char) Keys::Return ) exitAlternateButton_Click( sender, e );
		 }
};
}

