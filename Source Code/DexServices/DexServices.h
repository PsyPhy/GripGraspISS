// DexServices.h

#pragma once

// Connection to Dex hardware via the ET port.
#define DEFAULT_SERVER "10.80.12.103"
#define DEFAULT_PORT 2128

namespace Grasp {

	class DexServices 
	{
		SOCKET  dex_socket;

	private:

	public:
		
		DexServices( void ){}
		int Connect( void );
		int Send( const char *packet, int size ) ;

		int SendTaskInfo( int subject, int protocol, int task, int step );

		void Disconnect( void );

	};
}
