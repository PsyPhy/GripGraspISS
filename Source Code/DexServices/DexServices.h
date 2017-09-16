// DexServices.h

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>
#include <Windows.h>

#include "../Trackers/Trackers.h"
#include "../Trackers/PoseTrackers.h"
#include "../VectorsMixin/VectorsMixin.h"

#include "TMdata.h"

// Connection to Dex hardware via the ET port.
#define DEFAULT_DEX_SERVER "10.80.12.103"
#define DEFAULT_DEX_PORT 2128

// IP address to be used when we pass through a proxy.
#define PROXY_DEX_SERVER		"127.0.0.1"
#define PROXY_DEX_PORT			8212
#define PROXY_DEX_PORT_STRING	"8212"

#define CLIENT_CONNECTION_TIMEOUT	2.0

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

typedef enum { TASK, SUBSTEP, TRACKER_STATUS, TRACKER_DATA, CLIENT_DATA, PICTURE } DexServicesPacketType;

typedef struct {
	DexServicesPacketType command;
	int user;
	int protocol;
	int task;
	int step;
	int substep;
	int script;
	int tracker;
	char tag[32];
	
	PsyPhy::TrackerPose	hmd;
	PsyPhy::TrackerPose	coda_hmd;
	PsyPhy::TrackerPose	hand;
	PsyPhy::TrackerPose	chest;
	PsyPhy::TrackerPose	mouse;
	MarkerFrame markers[2];

	unsigned char client[GRASP_RT_CLIENT_BYTES];

} DexServicesPacket;
namespace Grasp {




class DexServices : public PsyPhy::VectorsMixin
	{

	public:

		char			*server_name;
		unsigned short	server_port;

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

	private:

		char	log_filename[FILENAME_MAX];
		FILE	*log;
		int		not_sent_countdown;

		SOCKET	dexSocket;
		SOCKET	proxySocket;
		SOCKET	clientSocket;

		void printDateTime( FILE *fp = stderr );

		Timer	slice_timer;
		int		slice_count;
		Timer	stream_timer;
		int		stream_count;
		Timer	info_timer;
		Timer	client_connection_timer;

		// An object that serializes the data destined for DEX housekeeping telemetry packets.
		RT_packet rt;

	public:

		DexServices( char *server = DEFAULT_DEX_SERVER, unsigned short port = DEFAULT_DEX_PORT ) :

		  slice_count(0),
			  stream_count(0),
			  not_sent_countdown(10),
			  static_user(0),
			  static_protocol(0),
			  static_task(0),
			  static_step(0),
			  static_substep(STEP_EXECUTING),
			  static_tracker_status(TRACKERSTATUS_UNKNOWN),
			  dexSocket( INVALID_SOCKET ),
			  proxySocket( INVALID_SOCKET ),
			  clientSocket( INVALID_SOCKET )
		  {
			  server_name = server;
			  server_port = port;

			  strncpy( log_filename, "DexServices.dxl", sizeof( log_filename ) );
			  TimerSet( slice_timer, GRASP_RT_SLICE_INTERVAL );
			  TimerSet( info_timer, GRASP_HK_PACKET_INTERVAL );
			  TimerStart( stream_timer );
		  }

		  virtual void Initialize( char *filename = nullptr );
		  virtual int Connect( void );
		  virtual int Send( const unsigned char *packet, int size ) ;
		  virtual void Disconnect( void );
		  virtual void Release( void );


		  virtual int SendTaskInfo( int user, int protocol, int task, int step, 
			  unsigned short substep = STEP_EXECUTING, unsigned short tracker_status = TRACKERSTATUS_UNKNOWN, bool force = false );
		  virtual int ResetTaskInfo( void ) { return( SendTaskInfo( 0, 0, 0, 0, 0, 0, true ) ); }
		  virtual int SendSubstep( int substep ) { return( SendTaskInfo( static_user, static_protocol, static_task, static_step, STEP_EXECUTING + ( substep % 10000 ), static_tracker_status ) ); }
		  virtual int SendTrackerStatus( unsigned int status );

		  virtual int SnapPicture( const char *tag );

		  virtual void AddTrackerSlice( PsyPhy::TrackerPose &hmd, PsyPhy::TrackerPose &codaHmd, PsyPhy::TrackerPose &hand, PsyPhy::TrackerPose &chest, PsyPhy::TrackerPose &mouse, MarkerFrame frame[2] );
		  virtual void AddClientSlice( unsigned char *data, int bytes );
		  virtual void SendIfReady( void );

		  virtual void ParseCommandLine( char *command_line );

		  SOCKET		ProxySocket;
		  virtual void	InitializeProxySocket( void );
		  virtual bool	HandleProxyConnection( void );
		  void			ReleaseProxySocket( void );
	
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
		private: int SendScienceRealtimeData( void );

	};

	class DexServicesByProxy : public DexServices {

	public:

		DexServicesPacket	packet;
		int SendTaskInfo( int user, int protocol, int task, int step, 
			  unsigned short substep = STEP_EXECUTING, 
			  unsigned short tracker_status = TRACKERSTATUS_UNKNOWN, 
			  bool force = false ) {
				  packet.user = user;
				  packet.protocol = protocol;
				  packet.task = task;
				  packet.step = step;
				  packet.substep = substep;
				  packet.tracker = tracker_status;
				  packet.command = TASK;
				  return ( Send( (unsigned char *) &packet, sizeof( packet ) ) );
		}
		int SendSubstep( int substep ) {
			packet.substep = substep;
			packet.command = SUBSTEP;
			return ( Send( (unsigned char *) &packet, sizeof( packet ) ) );
		}
		int SendTrackerStatus( unsigned int status ) {
			packet.tracker = status;
			packet.command = TRACKER_STATUS;
			return ( Send( (unsigned char *) &packet, sizeof( packet ) ) );
		}
		int SnapPicture( const char *tag ) {
			strncpy( packet.tag, tag, sizeof( packet.tag ) );
			packet.command = PICTURE;
			return ( Send( (unsigned char *) &packet, sizeof( packet ) ) );
		}

		void AddTrackerSlice(  PsyPhy::TrackerPose &hmd, PsyPhy::TrackerPose &codaHmd, PsyPhy::TrackerPose &hand, PsyPhy::TrackerPose &chest, PsyPhy::TrackerPose &mouse, MarkerFrame frame[2] ) {
			CopyTrackerPose( packet.hmd, hmd );
			CopyTrackerPose( packet.coda_hmd, codaHmd );
			CopyTrackerPose( packet.hand, hand );
			CopyTrackerPose( packet.chest, chest );
			CopyTrackerPose( packet.mouse, mouse );
			PsyPhy::CopyMarkerFrame( packet.markers[0], frame[0] );
			PsyPhy::CopyMarkerFrame( packet.markers[1], frame[1] );
			packet.command = TRACKER_DATA;
			Send( (unsigned char *) &packet, sizeof( packet ) );
		}

		void AddClientSlice( unsigned char *data, int bytes ) {
			assert( bytes > sizeof( packet.client ) );
			memcpy( packet.client, data, bytes );
			packet.command = CLIENT_DATA;
			Send( (unsigned char *) &packet, sizeof( packet ) );
		}

		DexServicesByProxy( char *server = PROXY_DEX_SERVER, unsigned short port = PROXY_DEX_PORT ) {
			server_name = server;
			server_port = port;
		}


	};


}

