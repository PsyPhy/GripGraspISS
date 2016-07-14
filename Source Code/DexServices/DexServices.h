// DexServices.h

#pragma once

// Connection to Dex hardware via the ET port.
#define DEFAULT_SERVER "10.80.12.103"
#define DEFAULT_PORT 2128

#define STEP_SHOWN				0xAAAA
#define STEP_READY_TO_EXECUTE	0xBBBB
#define STEP_EXECUTING			0x2000
#define STEP_FINISHED_NORMAL	0xE000
#define STEP_FINISHED_ABNORMAL	0xF000

namespace Grasp {

	class DexServices 
	{

	private:

		SOCKET  dexSocket;
		FILE	*log;

		void printDateTime( FILE *fp = stderr );

		// These are used by programs that are executed as a single step vis-à-vis GRASP.
		// The values are set by the calling process (a priori, GraspGUI).
		// The program then indicates progress by setting the subtask, which is then 
		// reflected in the script engine field of the DEX housekeeping packets.
		int static_user;
		int static_protocol;
		int static_task;
		int static_step;

	public:
		
		DexServices( void ){}

		void Initialize( const char *filename = "DexServices.log" );
		int Connect( void );
		int Send( const unsigned char *packet, int size ) ;
		void Disconnect( void );
		void Release( void );

		void SendTaskInfo( int user, int protocol, int task, int step, unsigned short substep = STEP_EXECUTING );
		void ResetTaskInfo( void ) {
			SendTaskInfo( 0, 0, 0, 0, 0 );
		}
		int SnapPicture( const char *tag );

		bool ParseForInt( System::String ^argument, const char *flag, int &value );
		void ParseCommandLineArguments( array<System::String ^> ^args );
		void SendSubstep( int substep ) {
			SendTaskInfo( static_user, static_protocol, static_task, static_step, STEP_EXECUTING + substep );
		}

	};
}

