#pragma once

namespace GraspGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

#define MAX_SUBJECTS	32
#define MAX_SESSIONS	32
#define MAX_PROTOCOLS	32
#define MAX_TASKS		64
#define MAX_STEPS		128
#define MAX_PAGES		128

#define MAX_TOKENS		16

	public ref class Subject {
	public:
		String^ ID;			// An ID code to be used when creating filenames and such.
		String^ name;		// The full name that appears in the GUI menu.
		int	    number;		// This is the subject number passed by DEX to the ground.
		String^ file;		// The name of the file containing the sessions that this subject might do.

		Subject( int number, char *id, char *session_file, char *name ) {
			this->number = number;
			this->ID = gcnew String( id );
			this->name = gcnew String( name );
			this->file = gcnew String( session_file );
		}
		~Subject(){}
	};

	public ref class Protocol {
	public:
		int	    number;		// This is the protocol number passed by DEX to the ground.
		String^ name;		// The full name that appears in the GUI menu.
		String^ file;		// The name of the file containing the tasks in this protocol.

		Protocol( int number, char *task_file, char *name ) {
			this->name = gcnew String( name );
			this->file = gcnew String( task_file );
			this->number = number;
		}
		~Protocol(){}
	};

	public ref class Task {
	public:
		int	    number;
		String^ name;
		String^ seatedCmd;
		String^ floatingCmd;

		Task( int number, char *seated_task_cmd, char *floating_task_cmd, char *name ) {
			this->number = number;
			this->seatedCmd = gcnew String( seated_task_cmd );
			this->floatingCmd = gcnew String( floating_task_cmd );
			this->name = gcnew String( name );
		}
		~Task(){}
	};
	public ref class Page {
	public:
		int	    number;
		String^ file;

		Page( int number, char *file ) {
			this->number = number;
			this->file = gcnew String( file );
		}
		~Page(){}
	};
};

