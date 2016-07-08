// DexServices.h

#pragma once

// Connection to Dex hardware via the ET port.
#define DEFAULT_SERVER "10.80.12.103"
#define DEFAULT_PORT 2128

namespace Grasp {

	class DexServices 
	{

	private:

		SOCKET  dexSocket;
		FILE	*log;

		void printDateTime( FILE *fp = stderr );

	public:
		
		DexServices( void ){}

		void Initialize( const char *filename = "DexServices.log" );
		int Connect( void );
		int Send( const unsigned char *packet, int size ) ;
		void Disconnect( void );
		void Release( void );

		int SendTaskInfo( int user, int protocol, int task, int step );
		int SnapPicture( const char *tag );


	};
}
