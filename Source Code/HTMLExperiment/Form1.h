#pragma once

namespace HTMLExperiment {

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
	private: System::Windows::Forms::WebBrowser^  webBrowser1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::Button^  button1;
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
			this->webBrowser1 = (gcnew System::Windows::Forms::WebBrowser());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// webBrowser1
			// 
			this->webBrowser1->AllowWebBrowserDrop = false;
			this->webBrowser1->IsWebBrowserContextMenuEnabled = false;
			this->webBrowser1->Location = System::Drawing::Point(1, 52);
			this->webBrowser1->MinimumSize = System::Drawing::Size(20, 20);
			this->webBrowser1->Name = L"webBrowser1";
			this->webBrowser1->ScrollBarsEnabled = false;
			this->webBrowser1->Size = System::Drawing::Size(559, 651);
			this->webBrowser1->TabIndex = 0;
			this->webBrowser1->Url = (gcnew System::Uri(L"", System::UriKind::Relative));
			this->webBrowser1->WebBrowserShortcutsEnabled = false;
			this->webBrowser1->NewWindow += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::webBrowser1_NewWindow);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(417, 12);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(133, 31);
			this->button1->TabIndex = 1;
			this->button1->Text = L"OpenFile";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::Go);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(562, 705);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->webBrowser1);
			this->Name = L"Form1";
			this->Text = L"Grasp Instructions Previewer";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void webBrowser1_NewWindow(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
			 webBrowser1->DocumentText = "<html><body>" + "<p><em><em>Welcome to Grasp</em></em></p>" + "</html></body>";
			 }
	private: System::Void Go(System::Object^  sender, System::EventArgs^  e) {
				 openFileDialog1->ShowDialog();
				 webBrowser1->Navigate( openFileDialog1->FileName );
			 }
	};
}

