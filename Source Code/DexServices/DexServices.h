// DexServices.h

#pragma once

#include "../Trackers/Trackers.h"
#include "../VectorsMixin/VectorsMixin.h"
#include "TMData.h"

// Connection to Dex hardware via the ET port.
#define DEFAULT_SERVER "10.80.12.103"
#define DEFAULT_PORT 2128

#define STEP_SHOWN				10000
#define STEP_READY_TO_EXECUTE	20000
#define STEP_EXECUTING			30000
#define STEP_FINISHED_NORMAL	40000
#define STEP_FINISHED_ABNORMAL	50000

#define TRACKER_CONNECTED		10000
#define TRACKER_PREPARED		20000
#define TRACKER_ACQUIRING		30000
#define TRACKER_ANOMALY			44444
#define TRACKERSTATUS_UNKNOWN	55555

namespace Grasp {

	class DexServices : public PsyPhy::VectorsMixin
	{

	private:

		SOCKET  dexSocket;
		char	log_filename[FILENAME_MAX];
		FILE	*log;
		int		not_sent_countdown;

		void printDateTime( FILE *fp = stderr );

		// These are used by programs that are executed as a single step vis-à-vis GRASP.
		// The values are set by the calling process (a priori, GraspGUI).
		// The program then indicates progress by setting the subtask, which is then 
		// reflected in the script engine field of the DEX housekeeping packets.
		int static_user;
		int static_protocol;
		int static_task;
		int static_step;
		int static_substep;
		int static_tracker_status;

		Timer	slice_timer;
		int		slice_count;
		Timer	stream_timer;
		int		stream_count;
		Timer	info_timer;

		// An object that serializes the data destined for DEX housekeeping telemetry packets.
		RT_packet rt;

	public:
		
		DexServices( void ) :
		
		  slice_count(0),
		  stream_count(0),
		  not_sent_countdown(10),
		  static_user(0),
		  static_protocol(0),
		  static_task(0),
		  static_step(0),
		  static_substep(STEP_EXECUTING),
		  static_tracker_status(TRACKERSTATUS_UNKNOWN)
		{
		  strncpy( log_filename, "DexServices.dxl", sizeof( log_filename ) );
		  TimerSet( slice_timer, GRASP_RT_SLICE_INTERVAL );
		  TimerSet( info_timer, GRASP_HK_PACKET_INTERVAL );
		  TimerStart( stream_timer );
		 }

		void Initialize( char *filename = nullptr );
		int Connect( void );
		int Send( const unsigned char *packet, int size ) ;
		void Disconnect( void );
		void Release( void );

		void AddDataSlice(  unsigned int objectStateBits, PsyPhy::TrackerPose &hmd, PsyPhy::TrackerPose &codaHmd, PsyPhy::TrackerPose &hand, PsyPhy::TrackerPose &chest, PsyPhy::TrackerPose &mouse, MarkerFrame frame[2] );

		int SendTaskInfo( int user, int protocol, int task, int step, 
			unsigned short substep = STEP_EXECUTING, unsigned short tracker_status = TRACKERSTATUS_UNKNOWN );
		int ResetTaskInfo( void ) { return( SendTaskInfo( 0, 0, 0, 0, 0, 0 ) ); }
		int SendSubstep( int substep ) { return( SendTaskInfo( static_user, static_protocol, static_task, static_step, STEP_EXECUTING + ( substep % 10000 ), static_tracker_status ) ); }
		int SendTrackerStatus( unsigned int status );

		int SnapPicture( const char *tag );

		int SendScienceRealtimeData( void );

		void ParseCommandLine( char *command_line );

// If the program is being compiled with /clr, then expose these methods that use String objects.
#ifdef _MANAGED
	bool ParseForInt( System::String ^argument, const char *flag, int &value ) {
		if ( argument->StartsWith( gcnew System::String( flag ) ) ) {
			int index = argument->IndexOf( '=' ) + 1;
			value = System::Convert::ToInt32( argument->Substring( index ) );
			return true;
		}
		return false;
	}
	void ParseCommandLineArguments( array<System::String ^> ^args ){
		static_user = 0;
		static_protocol = 0;
		static_task = 0;
		static_step = 0;
		for ( int arg = 0; arg < args->Length; arg++ ) {
			ParseForInt( args[arg], "--user", static_user );
			ParseForInt( args[arg], "--protocol", static_protocol );
			ParseForInt( args[arg], "--task", static_task );
			ParseForInt( args[arg], "--step", static_step );
		}
	}
#endif

	};
}

