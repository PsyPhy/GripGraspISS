
#pragma once

namespace PsyPhy {

// Create an OpenGLWindows that appears in specific panels of a Windows::Forms GUI.
OpenGLWindow *CreateOpenGLWindowInForm( System::Windows::Forms::Panel^ panel,  HGLRC shared_hRC );
OpenGLWindow *CreateOpenGLWindowInForm( System::Windows::Forms::Panel^ panel );
System::Void OpenGLWindowPanelSizeChanged(System::Object^  sender, System::EventArgs^  e);
};

