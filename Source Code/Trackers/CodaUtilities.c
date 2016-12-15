/********************************************************************************/
/*                                                                              */
/*                                   CodaUtilities                              */
/*                                                                              */
/********************************************************************************/

/* Disable warnings about unsafe char routines. */
#define _CRT_SECURE_NO_WARNINGS

/*
 * Harmonize some common functionality of the CODA SDK.
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "codasys.h"
#include "CodaUtilities.h"

/*
* Some storage where the CODA SDK can put a single frame of marker data.
* This should probably be moved to dynamic storage so that the routines
*  that use it can be re-entrant.
*/
static CODA_FRAME_DATA_STRUCT _frame;
static codaBYTE _bInView[ CODA_MAX_MARKERS ];
static codaFLOAT _m[ CODA_MAX_MARKERS * 3 ];

/*
* This structure is used to configure the CODA.
*/
static CODA_MODE_STRUCT _mode;   

/*
* Current number of units attached to system.
* It would be better if this was somehow determined automatically.
*/
int codaUnits = 2;

static int first_marker = 0;
static int n_markers = 16;

int verbose = 1;

char default_transform_pathname[] = ".\\CodaAlignment.xfm";

/********************************************************************************/

int CodaConnectOnly( char *host ) {

	int status = CODA_ERROR;
	char errmsg[1024];

	/*
	 * If a host name or IP address is not specified by the caller, try to get
	 * it from a environment variable.
	 */
	if ( host == (char *) NULL || *host == 0 ) host = getenv( CODA_HOST_ENV );
	if ( host ) {
		if ( ( status = CodaConnect( host )) != CODA_OK ) {
			sprintf( errmsg, "CodaHost = %s\nCodaConnect returned value: %d\n", host, status );
			MessageBox( NULL, errmsg, "Coda Configuration Error", MB_OK );	
		}
	}
	return( status );
}

/********************************************************************************/

void CodaPrepare( void ) { 

	int status;
	char msg[512];

	/* Prepare for frame grabs */


#if 1
	status = CodaAcqPrepare();
	if ( status != CODA_OK ) {
		CodaGetErrorMessage( msg );
		MessageBox( NULL, msg, "CodaAcqPrepare() failed!", MB_OK );
	}
#endif
	status = CodaFramePrepare();
	if ( status != CODA_OK ) {
		CodaGetErrorMessage( msg );
		MessageBox( NULL, msg, "CodaFramePrepare() failed!", MB_OK );
	}

	/*
	* Turn off autograb so that we can grab a frame and then
	* process information from each CODA separately.
	*/

	status = CodaAutoGrab( FALSE );
	if( status != CODA_OK) {						
		CodaGetErrorMessage( msg );
		MessageBox( NULL, msg, "CodaAutoGrab() failed!", MB_OK );
	}

	/* 
	* The SDK starts up with the external strobe off by default.
	* Since we use the wired strobe units, we turn it on systematically.
	* As far as I know, it doesn't hurt to have it on even when using the 
	*  wireless strobe.
	*/
	status = CodaExternalStrobe( TRUE );
	if( status != CODA_OK) {						
		CodaGetErrorMessage( msg );
		MessageBox( NULL, msg, "CodaExternalStrobe() failed!", MB_OK );
	}

}

/********************************************************************************/

int CodaConnectStartAndPrepare( char *host ) {

	char errmsg[512] = "";
	int	status;
	int	unit;

	/* string for error message */

	char szError[CODA_ERROR_MESSAGE_LENGTH];

	/*
	* Set the default coda mode, acquiring off of all available CODAs.
	* This will get sent only if the coda is being started up in this process.
	* If another process has already started the codas then we'll stay in that mode.
	* But we set the structure here so that CodaSetMarkerMode(), CodaUseAll(), etc.
	*  will have a properly filled structure.
	* What really needs to be done, however, is to be able to read the mode so that
	*  the individual routines can save the current state and only change what they
	*  want to change.
	*/
	memset( &_mode, 0, sizeof( CODA_MODE_STRUCT ) );
	for ( unit = 0; unit < codaUnits; unit++ ) _mode.bActiveCoda[unit] = TRUE;
	for ( unit = unit; unit < CODA_MAX_UNITS; unit++ ) _mode.bActiveCoda[unit] = FALSE;
	_mode.dwDecimation = CODA_DEFAULT_DECIMATION;
	_mode.dwMarkerMode = CODA_DEFAULT_MARKER_MODE;
	_mode.dwMaxMarker = CODA_DEFAULT_MAX_MARKER;

	/*
	* Do this only if we are not already connected and running.
	*/
	if ( CodaGetSystemStatus() != CODA_OK ) {

		if ( verbose ) fprintf( stderr, "CodaGetSystemStatus() NOK.\nTrying CodaConnect() ... " );

		/* 
		* If we are here, it means that either we are not connected to the server or
		*  we are connected to the server but the CODA is not running.
		*/

		if ( (status = CodaConnectOnly( host ) ) == CODA_OK ) {

			if ( verbose ) fprintf( stderr, "OK.\n" );
			/*
			* Now we are connected.
			* Check if the CODAs are already running.
			* If not, start them up.
			*/
			if ( CodaGetSystemStatus() != CODA_OK ) {

				if ( verbose ) fprintf( stderr, "CodaGetSystemStatus() NOK.\nTrying CodaShutDown() ... " );
				/*
				* Try forcing a shutdown, just in case the CODA is still running but in a bad state.
				* Then try starting up again.
				*/
				CodaShutDown();

				if ( verbose ) fprintf( stderr, "OK.\nCodaStartUp() ..." );			  
				if ( (status = CodaStartUp( NULL )) != CODA_OK ) {

					/*
					* If we got here, nothing we tried seems to have worked.
					* So we signal the failure to the user and bail out.
					*/
					if (verbose ) fprintf( stderr, "NOK.\n" );
					CodaGetErrorMessage( szError );
					strcpy( errmsg, "CodaStartUp( NULL ) failed.\n" );
					strcat( errmsg, szError );
					MessageBox( NULL, errmsg, "Coda Configuration.", MB_OK );
					CodaShutDown();
					CodaDisconnect();
					return( status );
				}
				if (verbose ) fprintf( stderr, "OK.\n" );

				/* 
				* Now set the CODA operating mode.
				*/
				if( CodaModeSet( &_mode )!= CODA_OK ) {
					CodaGetErrorMessage( szError );
					MessageBox( NULL, errmsg, "Coda Configuration.", MB_OK );
					CodaShutDown();
					return( status );
				}
				/*
				* Do some other stuff to prepare the CODA for acquisitions.
				*/
				CodaPrepare();
			}
			return( CODA_OK );
		}
		else {
			/*
			* If we are here, it means we failed to connect to the server.
			*/
			if ( verbose ) fprintf( stderr, " NOK.\n" );
			return( status );
		}
	}
	/*
	* If we are here it means that we were already connected and the CODA
	*  was already up and running.
	*/
	return( CODA_OK );
}

/********************************************************************************/

void CodaQuit( void ) {

	int status;

	char msg[512];

	/* 
	* Stop any acquisitions that may be in progress. 
	*/
	status = CodaAcqStop();
	if ( status != CODA_OK ) {
		CodaGetErrorMessage( msg );
		MessageBox( NULL, msg, "CodaAcqStop() failed!", MB_OK );
	}

#ifdef ALWAYS_SHUTDOWN
	status = CodaShutDown();
	if ( status != CODA_OK ) {
		CodaGetErrorMessage( msg );
		MessageBox( NULL, msg, "CodaShutDown() failed!", MB_OK );
	}
#endif

	/* Disconnect from server. */
	status = CodaDisconnect();

}

/********************************************************************************/
/*                                                                              */
/*                                 Useful Utilities                             */
/*                                                                              */
/********************************************************************************/

/*

One can do frame grabs in two modes. The slow mode sends out a query to the CODA
and retrieves a single frame of data. There is a fair amount of overhead involved.

In fast mode, you tell the CODA to acquire at a fixed rate into a buffer. Then, 
when you perform a frame grab, it just gives you the latest frame.

The only problem is that you must occasionally empty the buffer, or it will overflow
and generate an error. So in this mode you must occasionally call AcqBufferUpdate, even 
if you throw the buffered data away.

*/

void CodaFastFrameGrabs( void ) {

	int  status;
	char msg[512];

	if ( ! CodaAcqInProgress() ) {

		/* Prepare for fast frame grabs */
		status = CodaAcqSetSaveMode( CODA_ACQ_SAVE_NONE );
		if( status !=CODA_OK ) {
			CodaGetErrorMessage( msg );
			MessageBox( NULL, msg, "CodaAcqSetSaveMode( CODA_ACQ_SAVE_NONE ) failed!", MB_OK );
		}

		/* Start running until told to stop. */
		status = CodaAcqStart( -1 );
		if( status != CODA_OK ) {
			CodaGetErrorMessage( msg );
			MessageBox( NULL, msg, "CodaAcqStart( -1 ) failed!", MB_OK );
		}

	}
}

/********************************************************************************/

/*
 * Grab a frame of data from each unit, then generate a text string that 
 *  indicates which markers are visible to which CODAs.
 */

char *CodaVisibilityString( void ) {

	static char results[256] = "";
	int    status, mrk, unit;

	/* Set up the frame data structure for real-time data */
	_frame.dwChannelStart = first_marker;
	_frame.dwNumChannels = n_markers;
	_frame.pData = _m;
	_frame.pValid = _bInView;

	strcpy( results, "" );

	for ( unit = 0; unit < 2; unit++ ) {

		CodaUseOnly( unit );

		status = CodaFrameGrab();
		if ( status != CODA_OK ) strcpy( results, "CodaFrameGrab() Error!" );
		else {
			status = CodaFrameGetMarker( &_frame );
			if ( status != CODA_OK ) strcpy( results, "CodaFrameGetMarker( &frame ) Error!" );
			else {
				for ( mrk = first_marker; mrk < first_marker + n_markers; mrk++ ) {
					if ( _bInView[mrk] ) strcat( results, "O" );
					else strcat( results, "." );
				}	
			}
		}
		strcat( results, "\n" );
	}
	CodaUseAll();
	return( results );

}


/********************************************************************************/

/*
 * An easy way to report the latest CODA error to the user.
 */

void errmsg()
{
	char msg[512];
	CodaGetErrorMessage( msg );
	MessageBox(NULL, msg, "CODA Error!", MB_OK );
}

/********************************************************************************/

/* 
 * Select or change the marker mode. 
 * Automatically fill in the max number of markers for the selected mode.
 */

static int _max_marker_map[5] = { 56, 56, 28, 12, 6 };

void CodaSetMarkerMode( codaDWORD marker_mode ) {

	_mode.dwMarkerMode = marker_mode;
	_mode.dwMaxMarker = _max_marker_map[marker_mode];
	if( CodaModeSet( &_mode )!= CODA_OK ) errmsg();
	CodaPrepare();

}


/********************************************************************************/

/* 
 * Determines which CODAs are activated. 
 */
void CodaUseOnly( int coda ) {

	int unit;

	for ( unit = 0; unit < CODA_MAX_UNITS; unit++ ) _mode.bActiveCoda[unit] = FALSE;
	_mode.bActiveCoda[coda] = TRUE;
	if( CodaModeSet( &_mode )!= CODA_OK ) errmsg();


}

/*
 * Activate all of the CODAs.
 */ 

void CodaUseAll( void ) {

	int coda;

	for ( coda = 0; coda < CODA_MAX_UNITS; coda++ ) _mode.bActiveCoda[coda] = TRUE;
	if( CodaModeSet( &_mode )!= CODA_OK ) errmsg();			

}

/********************************************************************************/

/* 
 * From which CODA do we grab data? 
 */

void CodaGetAll( void ) {

	codaDWORD list[ CODA_MAX_UNITS ];
	int i;

	for ( i = 0; i < codaUnits; i++ ) list[i] = TRUE;
	for ( i = i; i < CODA_MAX_UNITS; i++ ) list[i] = FALSE;
	CodaSetInterest( list );

}

void CodaGetOnly( int unit ) {

	codaDWORD list[ CODA_MAX_UNITS ];
	int i;

	for ( i = 0; i < CODA_MAX_UNITS; i++ ) list[i] = FALSE;
	list[ unit ] = TRUE;
	CodaSetInterest( list );

}

/********************************************************************************/

/* 
 * To use or not the CODA alignment transformation. 
 * Normally, one uses the alignment transform> But if you are trying to 
 *  recompute the alignment transform, it may be necessary to acquire data
 *  in the intrinsic coordinate frame of each CODA.
 */

void CodaIntrinsicCoordinates( void ) {
	if ( CodaSetTransformActive( FALSE ) != CODA_OK ) {
		MessageBox( NULL, "CodaTransformActive( FALSE ) failed!", 
			"Coda Configuration Error", MB_OK );
	}
}

void CodaAlignedCoordinates( void ) {
	if ( CodaSetTransformActive( TRUE ) != CODA_OK ) {
		MessageBox( NULL, "CodaTransformActive( TRUE ) failed!", 
			"Coda Configuration Error", MB_OK );
	}
}


/********************************************************************************/

/* 
* Here we provide a convenient utility to store and retrieve an alignment transform.
*/

int CodaSaveAlignment( char *transform_pathname ) {

	FILE *fp;
	CODA_TRANSFORM_STRUCT  codaTransform;
	char msg[256];
	char pathname[1024];
	int  unit;

	if ( transform_pathname ) strcpy( pathname, transform_pathname );
	else strcpy( pathname, default_transform_pathname );

	fp = fopen( pathname, "w" );
	if ( !fp ) {
		sprintf( msg, "Error opening transform file.\n%s", pathname );
		MessageBox( NULL, msg, "CodaSaveAlignment()", MB_OK );
		return( CODA_ERROR );
	}


	for ( unit = 0; unit < CODA_MAX_UNITS; unit++ ) {
		codaTransform.dwUnit = unit;
		if ( CodaGetTransform( &codaTransform ) != CODA_OK ) {
			MessageBox( NULL, "CodaGetTransform() failed!", "CodaSaveAlignment()", MB_OK );
			return( CODA_ERROR );
		}
		if ( 1 != fwrite( &codaTransform, sizeof( codaTransform ), 1, fp ) ) {
			sprintf( msg, "Error writing transform for CODA %d.\n%s", unit, pathname );
			MessageBox( NULL, msg, "CodaSaveAlignment()", MB_OK );
			return( CODA_ERROR );
		}
	}

	fclose( fp );
	return( CODA_OK );

}

int CodaRestoreAlignment( char *transform_pathname ) {

	FILE *fp;
	CODA_TRANSFORM_STRUCT  codaTransform;
	char pathname[1024];
	int  unit;

	if ( transform_pathname ) strcpy( pathname, transform_pathname );
	else strcpy( pathname, default_transform_pathname );

	fp = fopen( pathname, "r" );
	if ( !fp ) return( CODA_ERROR );

	for ( unit = 0; unit < CODA_MAX_UNITS; unit++ ) {
		if ( 1 != fread(&codaTransform, sizeof( codaTransform ), 1, fp ) ) return( CODA_ERROR );
		CodaSetTransform( &codaTransform );
	}

	fclose( fp );
	return( CODA_OK );

}

