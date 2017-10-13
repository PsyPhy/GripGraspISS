#pragma once

namespace GraspGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for HistoryForm
	/// </summary>
	public ref class HistoryForm : public System::Windows::Forms::Form
	{
	public:
		HistoryForm(void)
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
		~HistoryForm()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::TextBox^  historyTextBox;
	protected: System::Windows::Forms::Button^  historyCloseButton;
	private: System::Windows::Forms::Label^  label1;
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
			this->historyTextBox = (gcnew System::Windows::Forms::TextBox());
			this->historyCloseButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// historyTextBox
			// 
			this->historyTextBox->Font = (gcnew System::Drawing::Font(L"Consolas", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->historyTextBox->Location = System::Drawing::Point(13, 47);
			this->historyTextBox->Multiline = true;
			this->historyTextBox->Name = L"historyTextBox";
			this->historyTextBox->ReadOnly = true;
			this->historyTextBox->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->historyTextBox->Size = System::Drawing::Size(1035, 543);
			this->historyTextBox->TabIndex = 1;
			this->historyTextBox->Text = L"2017.10.13 11:38:17 | 006 | 200 | 232 | 001 | Completed abnormally (code -3)  | 2" 
				L"017.10.13 11:38:25";
			// 
			// historyCloseButton
			// 
			this->historyCloseButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->historyCloseButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->historyCloseButton->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->historyCloseButton->Location = System::Drawing::Point(920, 597);
			this->historyCloseButton->Margin = System::Windows::Forms::Padding(4);
			this->historyCloseButton->Name = L"historyCloseButton";
			this->historyCloseButton->Size = System::Drawing::Size(127, 44);
			this->historyCloseButton->TabIndex = 23;
			this->historyCloseButton->Text = L"Close";
			this->historyCloseButton->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label1->Font = (gcnew System::Drawing::Font(L"Consolas", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(17, 18);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(1009, 29);
			this->label1->TabIndex = 24;
			this->label1->Text = L"       Start        | Usr | Ssn | Tsk | Stp |        Completion Status        |  " 
				L"       End";
			// 
			// HistoryForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->historyCloseButton;
			this->ClientSize = System::Drawing::Size(1060, 654);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->historyCloseButton);
			this->Controls->Add(this->historyTextBox);
			this->Name = L"HistoryForm";
			this->Text = L"GRASP Task History";
			this->Shown += gcnew System::EventHandler(this, &HistoryForm::HistoryForm_Shown);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void HistoryForm_Shown(System::Object^  sender, System::EventArgs^  e) {
				 historyTextBox->TextBoxBase::ScrollToCaret();
			 }
	};
}
