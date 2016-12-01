#pragma once

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "GraspMMIDataPlots.h"

namespace GraspMMI {

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

	private: 
		
		int TimebaseOffset;

	public:

		Form1(void) : TimebaseOffset( -16 )
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
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TextBox^  lastAbsoluteTimeTextBox;
	private: System::Windows::Forms::TextBox^  firstAbsoluteTimeTextBox;
	private: System::Windows::Forms::TrackBar^  spanSelector;
	private: System::Windows::Forms::HScrollBar^  scrollBar;


	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Panel^  hmdGraphPanel;

	private: System::Windows::Forms::Panel^  taskGraphPanel;
	private: System::Windows::Forms::TextBox^  hmdRightTimeLimit;
	private: System::Windows::Forms::TextBox^  hmdLeftTimeLimit;
	private: System::Windows::Forms::TextBox^  taskRightTimeLimit;
	private: System::Windows::Forms::TextBox^  taskLeftTimeLimit;

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
			this->lastAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->firstAbsoluteTimeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->spanSelector = (gcnew System::Windows::Forms::TrackBar());
			this->scrollBar = (gcnew System::Windows::Forms::HScrollBar());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->hmdRightTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->hmdLeftTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->hmdGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->taskRightTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->taskLeftTimeLimit = (gcnew System::Windows::Forms::TextBox());
			this->taskGraphPanel = (gcnew System::Windows::Forms::Panel());
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->lastAbsoluteTimeTextBox);
			this->groupBox1->Controls->Add(this->firstAbsoluteTimeTextBox);
			this->groupBox1->Controls->Add(this->spanSelector);
			this->groupBox1->Controls->Add(this->scrollBar);
			this->groupBox1->Location = System::Drawing::Point(4, 15);
			this->groupBox1->Margin = System::Windows::Forms::Padding(4);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Padding = System::Windows::Forms::Padding(4);
			this->groupBox1->Size = System::Drawing::Size(1180, 100);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			// 
			// lastAbsoluteTimeTextBox
			// 
			this->lastAbsoluteTimeTextBox->Location = System::Drawing::Point(1045, 68);
			this->lastAbsoluteTimeTextBox->Margin = System::Windows::Forms::Padding(4);
			this->lastAbsoluteTimeTextBox->Name = L"lastAbsoluteTimeTextBox";
			this->lastAbsoluteTimeTextBox->Size = System::Drawing::Size(100, 22);
			this->lastAbsoluteTimeTextBox->TabIndex = 3;
			this->lastAbsoluteTimeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// firstAbsoluteTimeTextBox
			// 
			this->firstAbsoluteTimeTextBox->Location = System::Drawing::Point(227, 68);
			this->firstAbsoluteTimeTextBox->Margin = System::Windows::Forms::Padding(4);
			this->firstAbsoluteTimeTextBox->Name = L"firstAbsoluteTimeTextBox";
			this->firstAbsoluteTimeTextBox->Size = System::Drawing::Size(100, 22);
			this->firstAbsoluteTimeTextBox->TabIndex = 2;
			this->firstAbsoluteTimeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// spanSelector
			// 
			this->spanSelector->LargeChange = 1;
			this->spanSelector->Location = System::Drawing::Point(12, 28);
			this->spanSelector->Margin = System::Windows::Forms::Padding(4);
			this->spanSelector->Maximum = 7;
			this->spanSelector->Name = L"spanSelector";
			this->spanSelector->Size = System::Drawing::Size(177, 53);
			this->spanSelector->TabIndex = 1;
			this->spanSelector->TickStyle = System::Windows::Forms::TickStyle::TopLeft;
			this->spanSelector->Value = 6;
			this->spanSelector->ValueChanged += gcnew System::EventHandler(this, &Form1::spanSelector_ValueChanged);
			// 
			// scrollBar
			// 
			this->scrollBar->LargeChange = 25;
			this->scrollBar->Location = System::Drawing::Point(208, 36);
			this->scrollBar->Name = L"scrollBar";
			this->scrollBar->Size = System::Drawing::Size(959, 23);
			this->scrollBar->TabIndex = 0;
			this->scrollBar->ValueChanged += gcnew System::EventHandler(this, &Form1::scrollBar_ValueChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->hmdRightTimeLimit);
			this->groupBox2->Controls->Add(this->hmdLeftTimeLimit);
			this->groupBox2->Controls->Add(this->hmdGraphPanel);
			this->groupBox2->Location = System::Drawing::Point(4, 120);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(1179, 330);
			this->groupBox2->TabIndex = 1;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Head";
			// 
			// hmdRightTimeLimit
			// 
			this->hmdRightTimeLimit->Location = System::Drawing::Point(1067, 299);
			this->hmdRightTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->hmdRightTimeLimit->Name = L"hmdRightTimeLimit";
			this->hmdRightTimeLimit->Size = System::Drawing::Size(100, 22);
			this->hmdRightTimeLimit->TabIndex = 4;
			this->hmdRightTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// hmdLeftTimeLimit
			// 
			this->hmdLeftTimeLimit->Location = System::Drawing::Point(15, 299);
			this->hmdLeftTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->hmdLeftTimeLimit->Name = L"hmdLeftTimeLimit";
			this->hmdLeftTimeLimit->Size = System::Drawing::Size(100, 22);
			this->hmdLeftTimeLimit->TabIndex = 3;
			this->hmdLeftTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// hmdGraphPanel
			// 
			this->hmdGraphPanel->Location = System::Drawing::Point(15, 27);
			this->hmdGraphPanel->Name = L"hmdGraphPanel";
			this->hmdGraphPanel->Size = System::Drawing::Size(1151, 265);
			this->hmdGraphPanel->TabIndex = 0;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->taskRightTimeLimit);
			this->groupBox3->Controls->Add(this->taskLeftTimeLimit);
			this->groupBox3->Controls->Add(this->taskGraphPanel);
			this->groupBox3->Location = System::Drawing::Point(4, 456);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(1179, 331);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Task History";
			// 
			// taskRightTimeLimit
			// 
			this->taskRightTimeLimit->Location = System::Drawing::Point(1067, 300);
			this->taskRightTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskRightTimeLimit->Name = L"taskRightTimeLimit";
			this->taskRightTimeLimit->Size = System::Drawing::Size(100, 22);
			this->taskRightTimeLimit->TabIndex = 6;
			this->taskRightTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// taskLeftTimeLimit
			// 
			this->taskLeftTimeLimit->Location = System::Drawing::Point(15, 300);
			this->taskLeftTimeLimit->Margin = System::Windows::Forms::Padding(4);
			this->taskLeftTimeLimit->Name = L"taskLeftTimeLimit";
			this->taskLeftTimeLimit->Size = System::Drawing::Size(100, 22);
			this->taskLeftTimeLimit->TabIndex = 5;
			this->taskLeftTimeLimit->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// taskGraphPanel
			// 
			this->taskGraphPanel->Location = System::Drawing::Point(14, 29);
			this->taskGraphPanel->Name = L"taskGraphPanel";
			this->taskGraphPanel->Size = System::Drawing::Size(1151, 265);
			this->taskGraphPanel->TabIndex = 1;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1188, 826);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"Form1";
			this->Text = L"GraspMMI Data Plots";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->spanSelector))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: void InitializeGraphics( void );
	private: void RefreshGraphics( void );
	private: void KillGraphics( void );
	private: void AdjustScrollSpan( void );
	private: void MoveToLatest( void );

	private: String^ CreateTimeString( double seconds ) {
			char label[256];
			int day_first = (int) floor(( seconds + TimebaseOffset )) / (24 * 60 * 60);
			int since_midnight = ((int) floor( seconds ) + TimebaseOffset ) % (24 * 60 * 60);
			int hour = since_midnight / (60 * 60);
			int minute = (since_midnight % (60 * 60)) / 60;
			int second = (since_midnight % 60);
			sprintf( label, "%02d:%02d:%02d GMT  ", hour, minute, second );
			return gcnew String( label );
		}

	private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {
			InitializeGraphics();
			AdjustScrollSpan();
		}
	private: System::Void spanSelector_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			AdjustScrollSpan();
			RefreshGraphics();
		 }
	private: System::Void scrollBar_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			RefreshGraphics();
		 }
	private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			 KillGraphics();
		 }
};
}

