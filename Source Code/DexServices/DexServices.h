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
#include "../Useful/fOutputDebugString.h"

#include "../Trackers/Trackers.h"
#include "../Trackers/PoseTrackers.h"
#include "../VectorsMixin/VectorsMixin.h"

#include "TMdata.h"

// Connection to DEX hardware via the ETD port.
#define DEX_ETD_SERVER "10.80.12.103"
// Connection to DEX hardware via the EPM port.
#define DEX_EPM_SERVER "192.168.200.133"
// Pick one of the above to be the default.
#define DEFAULT_DEX_SERVER DEX_ETD_SERVER
#define DEFAULT_DEX_PORT 2128

// IP address to be used when we pass through a proxy.
// Here we have the proxy running on the same machine as the client.
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

typedef enum { HOUSEKEEPING, SUBSTEP, TRACKER_STATUS, TRACKER_DATA, CLIENT_DATA, PICTURE, DISCONNECT } DexServicesPacketType;

typedef struct {

	DexServicesPacketType	command;
	int						sequence_number;
	int user;
	int protocol;
	int task;
	int step;
	int substep;
	int script;
	int tracker;
	char tag[32];

	PsyPhy::TrackerPose	hmd;
	PsyPhy::TrackerPose	hand;
	PsyPhy::TrackerPose	chest;
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

	public:

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
		Timer	housekeeping_timer;
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
			  clientSocket( INVALID_SOCKET ),
			  log( nullptr )
		  {
			  server_name = server;
			  server_port = port;
		  }

		  virtual void Initialize( char *filename = nullptr );
		  virtual int Connect( void );
		  virtual int Send( const unsigned char *packet, int size ) ;
		  virtual void Disconnect( void );
		  virtual void Release( void );

		  // Housekeeping
		  virtual int SendTaskInfo( int user, int protocol, int task, int step, 
			  unsigned short substep = STEP_EXECUTING, unsigned short tracker_status = TRACKERSTATUS_UNKNOWN, bool force = false );
		  virtual int ResetTaskInfo( void ) { return( SendTaskInfo( 0, 0, 0, 0, 0, 0, true ) ); }
		  virtual int ResendTaskInfo( void ); 
		  virtual int SendSubstep( int substep );
		  virtual int SendTrackerStatus( unsigned int status );
		  virtual void ParseCommandLine( char *command_line );

		  // Snapshots
		  virtual int SnapPicture( const char *tag );

		  // Realtime Science Data
		  virtual void AddTrackerSlice( PsyPhy::TrackerPose &hmd, PsyPhy::TrackerPose &hand, PsyPhy::TrackerPose &chest, MarkerFrame frame[2] );
		  virtual void AddClientSlice( unsigned char *data, int bytes, bool advance = false );
		  virtual void NullifyClientInfo( void );

		  // Proxy Server - Used to create a relay between local processes and DEX.
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
			  SendTaskInfo( static_user, static_protocol, static_task, static_step );
		  }
#endif
	private: 
		int SendScienceRealtimeData( void );
		virtual void Advance( void );
		virtual void AdvanceIfReady( void );
	};

	class DexServicesByProxy : public DexServices {

	private:
		Timer	tracker_slice_timer;
		Timer	client_slice_timer;

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
				packet.command = HOUSEKEEPING;
				if ( log ) {
					printDateTime( log );
					fprintf( log, " SendTaskInfo: %d %d %d %d %d .\n",
						packet.user, packet.protocol, packet.task, packet.step, packet.substep );
				}
				return ( SendToProxy( packet ) );
		}
		int SendSubstep( int substep ) {
			packet.substep = substep;
			packet.command = SUBSTEP;
			return ( SendToProxy( packet ) );
		}
		int SendTrackerStatus( unsigned int status ) {
			packet.tracker = status;
			packet.command = TRACKER_STATUS;
			return ( SendToProxy( packet ) );
		}
		int SnapPicture( const char *tag ) {
			strncpy( packet.tag, tag, sizeof( packet.tag ) );
			packet.command = PICTURE;
			if ( log ) {
				printDateTime( log );
				fprintf( log, " SnapPicture: %s.\n", packet.tag );
				fflush( log );
			}
			return ( SendToProxy( packet ) );
		}

		void AddTrackerSlice(  PsyPhy::TrackerPose &hmd, PsyPhy::TrackerPose &hand, PsyPhy::TrackerPose &chest, MarkerFrame frame[2] ) {
			CopyTrackerPose( packet.hmd, hmd );
			CopyTrackerPose( packet.hand, hand );
			CopyTrackerPose( packet.chest, chest );
			PsyPhy::CopyMarkerFrame( packet.markers[0], frame[0] );
			PsyPhy::CopyMarkerFrame( packet.markers[1], frame[1] );
			if ( TimerTimeout( tracker_slice_timer ) ) {
				packet.command = TRACKER_DATA;
				SendToProxy( packet );
				TimerSet( tracker_slice_timer, GRASP_RT_SLICE_INTERVAL );
			}
		}

		void AddClientSlice( unsigned char *data, int bytes ) {
			assert( bytes <= sizeof( packet.client ) );
			memcpy( packet.client, data, bytes );
			if ( TimerTimeout( client_slice_timer ) ) {
				packet.command = CLIENT_DATA;
				SendToProxy( packet );
				TimerSet( client_slice_timer, GRASP_RT_SLICE_INTERVAL );
			}
		}

		int SendToProxy( DexServicesPacket &packet ) {
			packet.sequence_number++;
			fOutputDebugString( "DexServicesByProxy::SendToProxy() - Send Command: %d  Count: %d\n", 
				packet.command, packet.sequence_number );
			// Attempt to send a packet. If the connection has not been established, this will fail, but we carry on.
			// The caller may wish to signal the error or not, depending on the return code.
			int retval = send( dexSocket, (const char *) &packet, sizeof( packet ), 0);
			// If we were fancier, we could check if the connection has been achieved before doing the send and depending
			// on the error we could try to reconnnect. But instead I just assume that if the connection was not established
			// by the first try, we just carry on without a connection.
			if ( retval == SOCKET_ERROR && not_sent_countdown > 0  ) {
				fOutputDebugString( "DexServices: send() failed: error %d (may just be waiting for connection.)\n", WSAGetLastError());
				not_sent_countdown--;
			}
			return retval;
		}	

		void Disconnect( void ) {
			packet.command = DISCONNECT;
			SendToProxy( packet );
			Sleep( 50 );
			DexServices::Disconnect();
		}

		DexServicesByProxy( char *server = PROXY_DEX_SERVER, unsigned short port = PROXY_DEX_PORT ) {
			server_name = server;
			server_port = port;
			packet.sequence_number = 0;
			TimerSet( client_slice_timer, GRASP_RT_SLICE_INTERVAL );
			TimerSet( tracker_slice_timer, GRASP_RT_SLICE_INTERVAL );
		}

	};

	// Tag in the telemetry client packet to say whether the
//  alignment transforms are pre-alignment or post-alignment
#define PRE 0
#define POST 1
#define CURRENT 2

	typedef struct {

		char ID[8];

		int	prePost;
		PsyPhy::fVector3		offsets[MAX_UNITS];
		PsyPhy::fMatrix3x3		rotations[MAX_UNITS];

	} AlignClientBuffer;


}

