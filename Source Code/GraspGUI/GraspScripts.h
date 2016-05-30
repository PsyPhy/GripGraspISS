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

		Subject( int number, char *id, char *name, char *session_file ) {
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

		Protocol( int number, char *name, char *protocol_file ) {
			this->name = gcnew String( name );
			this->file = gcnew String( protocol_file );
			this->number = number;
		}
		~Protocol(){}
	};

	public ref class Step {
	public:
		int	    number;
		String^ type;

		// A step may simply be an INSTRUCTION to be displayed.
		// In that case, we have simply the name of the HTML file with the instruction.
		String^ instruction;

		// If instead the step involves invoking an external command, then the 
		// step is made up of the command string, to be exectued by a call to system(),
		// a instruction screen (html) to be displayed when the step is ready to be run,
		// another while it is running and then an array of html files to be displayed
		// depending on the exit code from the executed command.
		String^ command;
		String^	ready;
		String^ running;
		array<String ^> ^exit;

		// A flag to indicate if the step is a command step or an instruction step.
		bool	isCommand;

		Step( void ) {}
		~Step(){}
	};

	public ref class Task {
	public:
		int	    number;
		String^ name;
		String^ type;

		String^ task_file;
		Step^	isolated_step;

		Task( int number, char *type, char *name ) {
			this->number = number;
			this->type = gcnew String( type );
			this->name = gcnew String( name );
		}
		~Task(){}
	};

};

