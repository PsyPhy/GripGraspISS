#pragma once

namespace WaitAndGo {

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
	private: System::Windows::Forms::Label^  message;
	private: System::Windows::Forms::Button^  OKbutton;
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
			this->message = (gcnew System::Windows::Forms::Label());
			this->OKbutton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// message
			// 
			this->message->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->message->Location = System::Drawing::Point(37, 29);
			this->message->Name = L"message";
			this->message->Size = System::Drawing::Size(490, 64);
			this->message->TabIndex = 0;
			this->message->Text = L"Waiting for Oculus to load ...";
			this->message->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->message->UseWaitCursor = true;
			// 
			// OKbutton
			// 
			this->OKbutton->Enabled = false;
			this->OKbutton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->OKbutton->Location = System::Drawing::Point(188, 130);
			this->OKbutton->Name = L"OKbutton";
			this->OKbutton->Size = System::Drawing::Size(189, 50);
			this->OKbutton->TabIndex = 1;
			this->OKbutton->Text = L"OK";
			this->OKbutton->UseVisualStyleBackColor = true;
			this->OKbutton->UseWaitCursor = true;
			this->OKbutton->Visible = false;
			this->OKbutton->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(564, 226);
			this->Controls->Add(this->OKbutton);
			this->Controls->Add(this->message);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"WaitAndGo";
			this->TopMost = true;
			this->UseWaitCursor = true;
			this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				 Close();
			 }

			// A timer to handle animations and screen refresh, and associated actions.
			static System::Windows::Forms::Timer^ waitTimer;
			void OnTimerElapsed( System::Object^ source, System::EventArgs^ e ) {
				message->Text = L"Press and hold the VR Remote Select\r\nbutton for 2 sec then press \'OK\'.";
				OKbutton->Enabled = true;
				OKbutton->Visible = true;
			}

	private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {
				waitTimer = gcnew( System::Windows::Forms::Timer );
				waitTimer->Interval = 10000;
				waitTimer->Tick += gcnew EventHandler( this, &Form1::OnTimerElapsed );
				waitTimer->Start();
			 }
};
}

