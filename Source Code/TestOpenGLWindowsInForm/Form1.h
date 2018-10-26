#pragma once

#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLWindowsInForms.h"

namespace TestOpenGLWindowsInForm {

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

		PsyPhy::OpenGLWindow *gWindow;

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
	private: System::Windows::Forms::Panel^  panel1;
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
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(218, 221);
			this->panel1->TabIndex = 0;
			//this->panel1->SizeChanged += gcnew System::EventHandler(this, &Form1::panel1_SizeChanged);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(636, 500);
			this->Controls->Add(this->panel1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
			this->SizeChanged += gcnew System::EventHandler(this, &Form1::Form1_SizeChanged);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: void Redraw( void ) {
				 gWindow->Activate();
				 gWindow->Clear( 0.0, 0.0, 0.5 );
				 gWindow->Swap();
			 }

	private: System::Void Form1_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
				 panel1->Size = System::Drawing::Size( this->ClientSize.Width / 2, this->ClientSize.Height / 2 );
				 Redraw();
			 }
	private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {
				 gWindow = PsyPhy::CreateOpenGLWindowInForm( panel1 );
				 Redraw();
			 }
	};
}

