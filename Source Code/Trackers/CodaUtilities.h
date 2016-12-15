#ifndef _CodaUtilities_
#define _CodaUtilities_

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Eventually we may use the registry to get the IP address of the CODA hub.
 */
//#define USE_REGISTRY

/*
 * For the moment we get the CODA IP address from the environment.
 * If it doesn't find an address or hostname, it will assume that 
 * we are using the BrainBoxes cards directly.
 */
#define CODA_HOST_ENV "CodaHost"

#include "codasys.h"


/*
 * If ALWAYS_SHUTDOWN is defined, then CodaQuit() will always perform a
 * CodaShutdown(). Otherwise, we leave the Coda running to save time the
 * next time we want to start it up.
 */
#define ALWAYS_SHUTDOWN

/*
 * The CODA system needs a number of calibration files and sharc code to be
 * downloaded into the cx1 units. This tells us where they are. I can't remember,
 * though, if they need to be on a client machine or just on the server
 * machine that is directly attached to the CODA cx1 units.
 */
#define CODA_SYSTEM_DIRECTORY "c:\\CodaSDK\\system"

/*
	Define the maximum number of markers that one might find on a CODA system.
	This is used to allocate arrays at compile time. 
	To know the actual number of markers for a given system at run time,
	use CodaGetNumMarkers().
*/
#define CODA_MAX_MARKERS 56

/*
	Define the maximum number of frames that can be in a buffer full of
	data when in continuous buffered mode.
*/
#define CODA_BUFFER_FRAMES	512

/*
 *	Signal missing data when a marker is not visible.
 */
#define CODA_NA	-9999.0

#define CODA_DEFAULT_DECIMATION 1
#define CODA_DEFAULT_MARKER_MODE modeMarker200
#define CODA_DEFAULT_MAX_MARKER 56

int CodaConnectOnly( char *hostname );
void CodaPrepare( void );
int CodaConnectStartAndPrepare( char *hostname );
void CodaFastFrameGrabs( void );
void CodaQuit( void );

void CodaSetMarkerMode( codaDWORD marker_mode );

char *CodaVisibilityString( void );
void CodaUseOnly( int coda );
void CodaUseAll( void );
void CodaGetOnly( int unit );
void CodaGetAll( void );

void CodaIntrinsicCoordinates( void );
void CodaAlignedCoordinates( void );

int CodaSaveAlignment( char *transform_pathname );
int CodaRestoreAlignment( char *transform_pathname );

// Current number of units attached to system.
// Should be somehow determined automatically, but it isn't.
extern int codaUnits;		
							
#ifdef __cplusplus
}
#endif

#endif