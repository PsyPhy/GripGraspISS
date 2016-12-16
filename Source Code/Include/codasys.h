/** @file CodaSys.h Interface to the CODA SDK */

/*---------------------------------------------------------------------------

  Charnwood Dynamics Ltd.

	CODASYS.DLL: CODASYS.h

	Edited:
	980916 JRM: 
	VERSION 1.0 -------------------------------------------------------------
	980929 JRM: CODA_ACQ_PREPARE / CODA_FRAME_PREPARE functionality
	VERSION 1.1 -------------------------------------------------------------
	981221 JRM: Macros for Analog/Digital/Force/EMG support:
					CODA_GET_..._RATE
					CODA_FRAME_GET_...
					CODA_ACQ_GET_...
					CODA_ACQ_GET_NUM_FRAMES_...
					CODA_FRAME_GET_...
					CODA_IS_ACTIVE_...
				  NOTE: just because a macro is defined here, is
				      doesn't mean its implemented!
	            New numbering system for command ID's
	990103 JRM: CODA_ASYNC_GRAB_DIGITAL for fast digital input
				  CODA_ASYNC_OUTPUT_DIGITAL for digital output
	990104 JRM: Add CODA_GET_NUM_..._CHANNELS and CODA_GET_NUM_UNITS_...
	            to determine hardware config
				  Change CODA_GET_..._RATE to CODA_GET_RATE_...
	VERSION 1.20 ------------------------------------------------------------
	990107 JRM: Remove unwanted semicolon from CodaGetSystemStatus()
	990108 JRM: CODA_ACQ_SET_SAVE_MODE, CODA_ACQ_SAVE....
	            CODA_ACQ_BUFFER_UPDATE
	VERSION 1.26 ------------------------------------------------------------
	990923 JRM: Remove explicit support for Force and EMG (was never 
	            implemented in code).
	            Add CODA_GET_UNIT_INFO_ANALOG, CODA_GET_UNIT_INFO_DIGITAL
				      New numbers for command ID's CODA_GET_NUM_UNITS_...
                - any programs which use these require a re-compile
				      Add types for codaLPWORD, codaLPDWORD just for consistency
	991102 JRM:	New structure CODA_UNIT_INFO_STRUCT
				  New constant CODA_TITLE_LENGTH
				  Add command ID's for USER_OPTIONS_[SELECT|ADD|REMOVE]
				  CODA_ACQ_GET_INFO_ANALOG/CODA_ACQ_GET_INFO_DIGITAL require a 
				  ptr to CODA_ACQ_INFO_STRUCT, instead of just a float value.
				  Add typedef char codaCHAR
				  Rename CodaAcqLostData -> CodaAcqHasBackupData
	Version 1.27-dev/JRM -----------------------------------------------------
	000107  AAW/JRM: Add CODA_ACQ_SAVE_NONE
	Version 1.28 -------------------------------------------------------------
	Version 1.40 -------------------------------------------------------------
	Version 1.60 -------------------------------------------------------------
	011212	GPP	Started removing dead functions see Removed Funcs

	030704	GPP	Mapped network functions to calls in non network mode so that 
	            they will compile OK, but allways return CODA_ERROR
	VERSION 1.92 -------------------------------------------------------------
	031201 JRM: Removed codaserver.cpp and codaserver.h.  
				Client-side network support is now provided via the 
				codanetclient static library.
				Server-side code is in codanetserver, but the plan is no 
				longer to have server code in the codasys library 
				- instead use separate program which statically links to
				  codanetserver
  040203 JRM: CODA_DATA_MULTI_STRUCT and support for
              CodaAcqGetMultiMarker
              CODA_ALIGN_STRUCT correct Seperation -> Separation
	VERSION 1.9.17 -----------------------------------------------------------
	040324 GPP: Added functions to deal with the functionality of the coda sync chip i.e.
							CodaExternalSyncIn
							CodaExternalSyncActiveHigh
							CodaExternalStobe
							CodaSetTimestamping
							CodaGetTimestamps
							CodaGetTimeCalibration			
	VERSION 1.9.20 -----------------------------------------------------------
	040606 JRM: Dynamic alignment prototypes
	040716 JRM: Updated docs
              Corrected typo: CodaExternalStobe -> CodaExternalStrobe
              Removed typedef unsigned __int64 codaRAWTIME (non-standard
              C didn't work under linux)

---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef _CODASYS_H_
#define _CODASYS_H_

/*-------------------------------------------------------------------------*/
/* Symbolic constants */

#define CODA_OK	            0				
#define CODA_USER_CANCEL    1				
#define CODA_NEW_CONNECTION 2
#define CODA_TASK_PROCESSED 3
#define CODA_CLOSED_CONNECTION 4
#define CODA_ERROR         -1
#define CODA_NOT_WORKING   -2
#define CODA_NO_ALIGNMENT  -3

#define CODA_TITLE_LENGTH          64
#define CODA_ERROR_MESSAGE_LENGTH  512

#define CODA_ACQ_SAVE_RAM          0
#define CODA_ACQ_SAVE_DISK         1
#define CODA_ACQ_SAVE_BUFFER_ONLY  2
#define CODA_ACQ_SAVE_NONE         3

#define CODA_MAX_UNITS		10

/*-------------------------------------------------------------------------*/
/* Data structures */

/** Eight-bit signed value */
typedef char codaCHAR;					

/** Eight-bit unsigned value */
typedef unsigned char  codaBYTE;

/** Sixteen-bit unsigned value */
typedef unsigned short codaWORD;

/** Thirty-two-bit unsigned value */
typedef unsigned long  codaDWORD;

/** Floating point value */
typedef float codaFLOAT;

/** Pointer */
typedef void* codaLPVOID;

/** Pointer to eight-bit unsigned value */
typedef unsigned char*  codaLPBYTE;

/** Pointer to sixteen-bit unsigned value */
typedef unsigned short* codaLPWORD;

/** Pointer to thirty-two-bit unsigned value */
typedef unsigned long*  codaLPDWORD;

/** 64bit floating point used for calibrated time*/
typedef double codaTIME;

/** Data structure for acquired data required by
    CodaAcqGetMarker.                            */
typedef struct
{
	/** hardware unit number */
	codaDWORD dwUnit;

	/** zero-based marker number */
	codaDWORD dwChannel;

	/** zero-based index of the first frame to get */
	codaDWORD dwFrameStart;		
	
	/** the size of the buffer pointed to by pData */
	codaDWORD dwNumFrames;		

	/** data buffer - must point to an array of size dwNumFrames*3*sizeof(codaFLOAT) */
	codaLPVOID pData;

	/** data validity buffer -  must point to a buffer of size dwNumFrames*sizeof(codaBYTE) */
	codaLPBYTE pValid;			

} CODA_ACQ_DATA_STRUCT, *LPCODA_ACQ_DATA_STRUCT;

/** Data structure for multiple marker acquired data 
    required by CodaAcqGetMultiMarker */
typedef struct
{
  /** set this to nonzero to do automatic CodaAcqBufferUpdate() */
  codaDWORD bAutoBufferUpdate;

	/** hardware unit number - not used */
	codaDWORD dwUnit;

	/** zero-based index of first marker number */
	codaDWORD dwChannelStart;

	/** number of channels to get */
	codaDWORD dwNumChannels;

	/** set this to the number of frames which can be stored in pData and pValid */
	codaDWORD dwBufferFrames;

  /** set this to the zero-based index of the first frame to get */
	codaDWORD dwFrameStart;		
	
	/** in CODA_ACQ_SAVE_BUFFER_ONLY mode this will be set to the number
      of frames since the last buffer update, in CODA_ACQ_SAVE_RAM it
      will be set to the number of frames since AcqStart was called */
	codaDWORD dwNumFrames;		

	/** data buffer - must point to a buffer of size dwBufferFrames*dwNumChannels*3*sizeof(codaFLOAT)  */
	codaLPVOID pData;

	/** data validity buffer - must point to a buffer of size dwBufferFrames*dwNumChannels*sizeof(codaBYTE)  */
	codaLPBYTE pValid;			

} CODA_ACQ_DATA_MULTI_STRUCT, *LPCODA_ACQ_DATA_MULTI_STRUCT;


/** Data structure for single frame of data required by
    CodaFrameGetMarker.                                 */
typedef struct
{
	/** hardware unit number */
	codaDWORD dwUnit;			
	
	/** zero-based marker number or channel */
	codaDWORD dwChannelStart;	
	
	/** number of markers / channels to get */
	codaDWORD dwNumChannels;	
	
	/** will be set to the frame number since AcqStart */
	codaDWORD dwFrame;			
	
	/** data buffer - must point to a buffer of size dwNumChannels*3*sizeof(codaFLOAT) */
	codaLPVOID pData;			

	/** data validity buffer - must point to a buffer of size dwNumChannels*sizeof(codaBYTE) */
	codaLPBYTE pValid;			
} CODA_FRAME_DATA_STRUCT, *LPCODA_FRAME_DATA_STRUCT;


typedef struct
{
	codaDWORD dwUnit;
	codaDWORD dwDataSize;
	codaLPVOID pData;
} CODA_ASYNC_DATA_STRUCT, *LPCODA_ASYNC_DATA_STRUCT;

typedef struct
{
	codaDWORD dwUnit;
	codaDWORD dwNumChannels;
	codaCHAR  szTitle[CODA_TITLE_LENGTH];
} CODA_UNIT_INFO_STRUCT, *LPCODA_UNIT_INFO_STRUCT;

typedef struct
{
	codaDWORD dwUnit;
	codaDWORD dwChannel;
	codaFLOAT fRate;
	codaDWORD dwAcqNumFrames;
} CODA_ACQ_INFO_STRUCT, *LPCODA_ACQ_INFO_STRUCT;


/** CODA marker acquisition mode */
enum
{
	modeMarker100 = 0,  /**< 100 Hz */
	modeMarker120,		/**< 120 Hz */
	modeMarker200,		/**< 200 Hz */
	modeMarker400,		/**< 400 Hz */
	modeMarker800,		/**< 800 Hz */

	nNumMarkerModes
};


enum
{
	modeAlignCustom2 = -2,
	modeAlignCustom = -1,
	modeAlignSingle = 0,
	modeAlignBilateral = 1,
	modeAlignParallel = 2,
};
/**
	This defines a CODA mode, used with CodaModeSet.*/
typedef struct
{
	codaBYTE	bActiveCoda[CODA_MAX_UNITS];
	codaDWORD	dwMarkerMode;
	codaDWORD	dwDecimation;
	codaDWORD	dwMaxMarker;
} CODA_MODE_STRUCT, *LPCODA_MODE_STRUCT;

/** Specifies the identity of markers placed in the capture
    volume to perform an <I>alignment procedure </I>using
    CodaAlign, as required to define a room co-ordinate system. */
typedef struct
{
	/** Must be set to modeAlignCustom */
	codaDWORD	dwAlignmentMode;	

	/* Zero-based index of the marker used to define co-ordinate zero */
	codaDWORD	dwOriginMarker;

	/** Zero-based index of marker which defines X-axis direction */
	codaDWORD	dwXAxisMarker;

	/** Zero-based index of marker which defines the XY plane */
	codaDWORD	dwPlaneMarker;		

	/** Not used */
	codaDWORD	dwOriginMarker2;	

	/** Not used */
	codaDWORD	dwXAxisMarker2;		

	/** Not used */
	codaDWORD	dwPlaneMarker2;		

	/** Not used */
	codaFLOAT	fSeparation;		

} CODA_ALIGN_STRUCT, *LPCODA_ALIGN_STRUCT;

/** This specifies the times of the last sync pulses and the current time,
		all in seconds since the ActiveHub was turned on.*/
typedef struct
{
	/** The time of the last inernaly generated sync pulse.*/
	codaTIME tInternalSync;
	/** The time of the last externaly generated sync pulse.*/
	codaTIME tExternalSync;
	/** The time now. Note that it will have taken some time
			to deliver this meaage, so by the time you read it will
			be wrong. If your client is local, then the error will
			be much smaller than if it is accross the network. It is
			not easy to predict how large the erros will be for a
			network connection.*/
	codaTIME tNow;
} CODA_TIMESTAMP_STRUCT, *LPCODA_TIMESTAMP_STRUCT;

typedef struct
{
	codaDWORD	dwUnit;
	codaFLOAT		fOffset[3];
	codaFLOAT		fTransform[9];
} CODA_TRANSFORM_STRUCT, *LPCODA_TRANSFORM_STRUCT;

/** Specifies the configuration for a <I>dynamic alignment</I> procedure */
typedef struct
{
	/** Number of markers to use */
	codaDWORD dwNumMarkers;

	/** Number of frames to get (at current sample rate) */
	codaDWORD dwNumFrames;

	/** Zero-based marker numbers */
	codaDWORD* dwMarker;

	/** 0 to use the reference frame of the first CODA,
			1 to use first 3 markers to define the reference frame */
	codaDWORD dwReference;

} CODA_ALIGN_DYNAMIC_STRUCT, *LPCODA_ALIGN_DYNAMIC_STRUCT;

/** Expresses the current status of a <I>dynamic alignment</I> procedure */
typedef struct
{
	/** Frames yet acquired */
	codaDWORD dwNumFrames;

	/** Non-zero if enough in-view frames to compute alignment for all CODA's */
	codaDWORD bValid;

	/** Overall residual (mm RMS) */
	codaFLOAT fResidual;

} CODA_ALIGN_DYNAMIC_STATUS_STRUCT, *LPCODA_ALIGN_DYNAMIC_STATUS_STRUCT;

/** CODA SDK API Entry Point. All commands to the CODA system are
   implemented by through calls to this function, but <B><I>you
   should never call it directly</I></B>. Instead call by using
   one of the macros provided. (See CodaSys.h for a
   complete list.)                                               
   
   @param nCmd Numerical command ID
   @param dwParam Data or pointer to data
   @return A codaDWORD whose value depends upon the operation performed

   */

long CodaCommand(long nCmd, codaDWORD dwParam);

/*-------------------------------------------------------------------------*/
/* Command ID's for nCmd parameter of CodaCommand. 
   N.B. If commands are removed, they must be left here so that the ids are not reused, as it is better to give incompatable apps errors than odd behaviour*/
enum
{
	CODA_START_UP = 1,
	CODA_SHUT_DOWN,
	CODA_GET_ERROR_MESSAGE,
	CODA_GET_SYSTEM_STATUS,
	
	CODA_ACQ_MODE_DIALOG = 64,
	CODA_ALIGNMENT_DIALOG,
	CODA_ORIGIN_DIALOG,

	CODA_ALIGNMENT_CLEAR = 128,
	CODA_IS_ALIGNED,
	CODA_CAN_SET_ORIGIN,
	
	CODA_USER_OPTIONS_LOAD = 192,
	CODA_USER_OPTIONS_SAVE,
	CODA_GET_NUM_USER_OPTIONS,
	CODA_USER_OPTIONS_SELECT,
	CODA_USER_OPTIONS_ADD,
	CODA_USER_OPTIONS_REMOVE,
	
	CODA_GET_NUM_UNITS_DIGITAL = 224,
	CODA_GET_NUM_UNITS_ANALOG,

	CODA_GET_NUM_MARKERS = 256,
	CODA_GET_UNIT_INFO_DIGITAL,
	CODA_GET_UNIT_INFO_ANALOG,

	CODA_GET_RATE_MARKER = 320,

	CODA_FRAME_PREPARE = 384,
	CODA_FRAME_GRAB= 385,
	CODA_ACQ_IN_PROGRESS= 386,
	CODA_ACQ_PREPARE= 387,
	CODA_ACQ_START= 388,
	CODA_ACQ_STOP= 389,
	CODA_ACQ_HAS_BACKUP_DATA= 390,
	CODA_ACQ_SET_SAVE_MODE= 391,
	CODA_ACQ_BUFFER_UPDATE= 392,

	CODA_FRAME_GET_MARKER = 448,
	CODA_FRAME_GET_DIGITAL,
	CODA_FRAME_GET_ANALOG,
	
	CODA_ACQ_GET_NUM_FRAMES_MARKER = 512,
	CODA_ACQ_GET_INFO_DIGITAL,
	CODA_ACQ_GET_INFO_ANALOG,
	
	CODA_ACQ_GET_MARKER = 576,
	CODA_ACQ_GET_DIGITAL,
	CODA_ACQ_GET_ANALOG,

	CODA_IS_ACTIVE_DIGITAL = 640,
	CODA_IS_ACTIVE_ANALOG,
	
	CODA_ASYNC_GRAB_DIGITAL = 704,
	
	CODA_ASYNC_OUTPUT_DIGITAL = 768,

	CODA_NET_CONNECT = 800,
	CODA_NET_DISCONNECT,
	CODA_NET_START_SERVER,
	CODA_NET_STOP_SERVER,
	CODA_NET_GET_SOCKET_ERROR,
	CODA_NET_DO_TASK,
	CODA_NET_WAIT_FRAME_GET_MARKER,
	CODA_NET_WAIT_ACQ_START,
	CODA_NET_WAIT_ACQ_STOP,

	CODA_MODE_SET = 900,
	CODA_ALIGN,
	CODA_GET_TRANSFORM,
	CODA_SET_TRANSFORM_ACTIVE,
	CODA_AUTO_GRAB,
	CODA_SET_INTEREST,
	CODA_SET_TRANSFORM,
	CODA_ALIGN_DYNAMIC_START,
	CODA_ALIGN_DYNAMIC_GET_STATUS,
	CODA_ALIGN_DYNAMIC_FINISH,

  CODA_ACQ_GET_MULTI_MARKER = 1024,

	CODA_SET_EXT_SYNC_IN = 2048,
	CODA_SET_EXT_SYNC_ACTIVE_HIGH,
	CODA_SET_EXT_STOBE,
	CODA_SET_TIMESTAMPING,
	CODA_GET_TIMESTAMPS,
	CODA_GET_TIMECAL,
	CODA_TIMESTAMP_PREPARE,
};

/*-------------------------------------------------------------------------*/
/* Macros */

	/** Connect to CODA system server over network. For diagnostic
	    error messages, use CodaGetSocketError.
	    @param pszServer  Name or IP address of server
	    
	    @return CODA_OK if successful, CODA_ERROR otherwise
	    Summary
	    Connect to CODA system over network.
	                                                               */
	#define CodaConnect(pszServer) CodaCommand(CODA_NET_CONNECT, (codaDWORD)(codaCHAR*)pszServer)

	/** Disconnect from network CODA system */
	#define CodaDisconnect()	CodaCommand(CODA_NET_DISCONNECT,0L)

	/** Retrieve error message associated with network connection to
	    CODA host. If a call to CodaConnect fails, you may use this
	    function to retrieve a diagnostic error message which may be
	    reported to the user.
	    @param pszError  Pointer to string into which error message will be
	                     put. Must be at least of length
	                     CODA_ERROR_MESSAGE_LENGTH.
	    
	    Summary
	    Retrieve network error information.                                 */
	#define CodaGetSocketError(pszError)	CodaCommand(CODA_NET_GET_SOCKET_ERROR,(codaDWORD)pszError)

	/* Network server code - not part of user SDK */
	#define CodaStartServer()	CodaCommand(CODA_NET_START_SERVER,0L)
	#define CodaStopServer()	CodaCommand(CODA_NET_STOP_SERVER,0L)
	#define CodaDoTask()				CodaCommand(CODA_NET_DO_TASK,0L)
	/* End network server code */

	/** This can be used in a multiple client situation. The client
	    that calls this function will block until another client
	    calls CodaAcqStart, then it will return giving the same
	    \return value to both clients.
	    
	    
	    @return CODA_OK if the starting of acquisition was
	    successful, CODA_ERROR otherwise.
	    Summary
	    Advanced multiple-client function.
	                                                                */
	#define CodaWaitAcqStart()			CodaCommand(CODA_NET_WAIT_ACQ_START,0L)
	/** This can be used in a multiple client situation. The client
	    that calls this function will block until another client
	    calls CodaAcqStop, then it will return giving the same return
	    value to both clients.
	    
	    
	    @return CODA_OK if the stopping of acquisition was
	    successful, CODA_ERROR otherwise.
	    Summary
	    Advanced multiple-client function.
	                                                                  */
	#define CodaWaitAcqStop()			CodaCommand(CODA_NET_WAIT_ACQ_STOP,0L)
	/** This can be used in a multiple client situation. The client
	    that calls this function will block until another client
	    calls CodaFrameGetMarker, then it will return giving the same
	    \return value to both clients.
	    @return CODA_OK if the frame retrieval was successful,
	    CODA_ERROR otherwise
	    Summary
	    Advanced multiple-client function.                            */
	#define CodaWaitFrameGetMarker(pFrame)	CodaCommand(CODA_NET_WAIT_FRAME_GET_MARKER,(codaDWORD)pFrame)

	/** @see CodaConnect
	    @see CodaShutDown
	    @see CodaGetSystemStatus
	    Description
	    Call this function to read all appropriate configuration
	    files on the CODA host machine, and initialise the CODA
	    hardware. If you are accessing the CODA host machine over a
	    network, call CodaConnect before calling this function.
	    
	    CodaStartUp must be successfully called before any other
	    functions in the library (except for CodaConnect), and should
	    \only be called once before CodaShutDown.Typically,
	    CodaStartUp is called during program initialisation, and
	    CodaShutDown is called during program shutdown.
	    
	    This function may fail if necessary configuration files are
	    not present in an expected directory on the host, if one or
	    more configuration files contain invalid data, if the host
	    machine is very low on memory resources, or if no hardware
	    units are responding. A descriptive error message is
	    generated, which may be retrieved using CodaGetErrorMessage.
	    
	    This function may succeed even if some hardware units are not
	    responding (e.g. only one of two CODA units is responding).
	    Data can be acquired using the responding units, but not with
	    the unresponsive ones. To acquire data from a unit which was
	    been switched on after CodaStartUp, you must call
	    CodaShutDown and then CodaStartUp again in order to
	    re-initialise the system.
	    
	    If pszSysDir is a non-empty string, the CODA system
	    configuration files must be there for this function to
	    succeed.
	    
	    If pszSysDir is NULL or an empty string, other directories
	    will be searched in order to find the appropriate
	    configuration files for the system. The correct directory is
	    identified by the presence of the CODASYS.DAT file. First,
	    the current directory is searched, then a directory specified
	    in the Windows registry as
	    HKEY_LOCAL_MACHINE\\SOFTWARE\\CODA\\SystemDir is searched.
	    
	    
	    @param pszSysDir  Null\-terminated string giving the directory in which
	                      CODA system configuration files are kept, or NULL to
	                      look in the default directories.
	    
	    @return CODA_OK if successful, CODA_ERROR otherwise.
	    Note
	    The CODAmotion system start-up performed by this function may
	    take several seconds to complete, so it is advisable to call
	    this function just once when your program starts, and again
	    when it exits.
	    Summary
	    CODA system start-up
	                                                                            */
	#define CodaStartUp(pszSysDir) CodaCommand(CODA_START_UP, (codaDWORD)(const char*)pszSysDir)

	/** Frees all resources associated with the system. This is the
	    last or penultimate function which must be called from your
	    program. Typically it is called just before the calling
	    process terminates. If you are accessing the CODA host over a
	    network, you must also call CodaDisconnect after calling this
	    function.
	    
	    
	    @return CODA_ERROR if the system was not initialised in the
	    first place, CODA_OK otherwise.
	    Summary
	    CODA system shut-down                                         */
	#define CodaShutDown() CodaCommand(CODA_SHUT_DOWN, 0L)
	/** @see CodaGetSocketError
	    @see CodaStartUp
	    Description
	    You should call this function if CodaStartUp fails. A
	    null-terminated string containing a diagnostic error message
	    will be copied into pszError. You may wish to display the
	    message to the user.
	    
	    When connecting to the CODA host over a network, failure may
	    also be caused by network problems. An error message
	    associated with network problems can be retrieved using
	    CodaGetSocketError.
	    
	    
	    @param pszError  Pointer to string into which error message will be
	                     put. Must be at least of length
	                     CODA_ERROR_MESSAGE_LENGTH.<P>
	    
	    @return CODA_OK, unless the error message could not be
	    determined due to a networking problem in which case
	    CODA_ERROR is returned. In the case of a networking problem,
	    use CodaGetSocketError instead.
	    
	    
	    Summary
	    Retrieve error information.                                         */
	#define CodaGetErrorMessage(szMessage) ((void)CodaCommand(CODA_GET_ERROR_MESSAGE, (codaDWORD)(char*)szMessage))

	/** 
	    @return CODA_OK if a successful call to CodaStartUp has been
	    made, CODA_ERROR otherwise.
	                                                                 */
	#define CodaGetSystemStatus() CodaCommand(CODA_GET_SYSTEM_STATUS, 0L)

	/** @see CodaAlign
	    Description
	    Clears the current CODA alignment settings. After calling
	    this function you need to call CodaAlign before attempting to
	    acquire any marker position data.
	    Summary
	    Clear alignment settings.                                     */
	#define CodaAlignmentClear() CodaCommand(CODA_ALIGNMENT_CLEAR, 0L)

	/** <FLAG Deprecated>
	    
	    @see CodaAlign
	    Description
	    This function causes a modal dialog box to appear on the CODA
	    active hub, which allows the user to specify the identity of
	    three markers which are used to define a co-ordinate system.
	    
	    To determine whether an appropriate alignment has been done,
	    call CodaIsAligned. You can clear the current alignment
	    settings using CodaAlignmentClear. The current alignment
	    settings are automatically saved to CODASYS.INI, and are
	    restored during CodaStartUp.
	    @return CODA_OK if the dialog was successful,
	    CODA_USER_CANCEL if the user cancelled the dialog,
	    CODA_NOT_WORKING if no CODA units are responding, or
	    CODA_ERROR if there was an error showing the dialog box.
	    Note
	    This function is here for backwards compatibility only. For
	    many applications it is more desirable to perform alignment
	    within your own code using the CodaAlign function.
	    
	    
	    Summary
	    Dialog box to align CODA system.                              */
	#define CodaAlignmentDialog() CodaCommand(CODA_ALIGNMENT_DIALOG, 0L)

	/** <FLAG Deprecated>
	    
	    Causes a modal dialog box to appear on the CODA host machine
	    which allows the user to re-define the co-ordinate origin by
	    placing a marker in the field of view of the CODA units.
	    Usually this function cannot be used until an initial
	    co-ordinate system has been defined using
	    CodaAlignmentDialog. In some cases however (such as when just
	    \one CODA mpx30 unit is active), no prior alignment settings
	    are needed. Use CodaCanSetOrigin to determine whether the
	    \origin can be set.
	    
	    
	    Note
	    This function is here for backwards compatability only. For
	    many applications it may not be desirable for dialog boxes to
	    appear on the CODA host machine. You may implement your own
	    'origin' procedure by using CodaFrameGetMarker, storing the
	    results for a user-specified marker, and offsetting all
	    subsequent data by those values.
	    Summary
	    Dialog box to set CODA origin.
	                                                                  */
	#define CodaOriginDialog() CodaCommand(CODA_ORIGIN_DIALOG, 0L)

	/** <FLAG Deprecated>
	    
	    This function causes a modal dialog box to appear on the CODA
	    active hub. it can be used to set parameters such frame rate,
	    numbers of markers, and hardware devices from which to
	    acquire data.
	    Note
	    This function is here for backwards compatibility only. For
	    many applications it is more desirable to do this within your
	    \own code using CodaModeSet.
	    
	    This function allows the user to select and de-select CODA
	    machines for acquisition. It is possible that the user
	    selects a machine for which there is no current co-ordinate
	    alignment information. See CodaAlign for more information.
	    Summary
	    Dialog box to set CODA mode.                                  */
	#define CodaAcqModeDialog() CodaCommand(CODA_ACQ_MODE_DIALOG, 0L)

	/** 
	    @return Non-zero if alignment settings exist for all active
	    CODA units, zero otherwise.
	    
	    
	    Note
	    This function has no way of knowing whether the current set
	    \of alignment data is appropriate for the current positions
	    \of CODA machines. It simply indicates whether alignment
	    information exists for the machines which are currently
	    switched on and were selected during CodaModeSet or
	    CodaAcqModeDialog.
	    Summary
	    Determine whether system alignment information is present.  */
	#define CodaIsAligned() CodaCommand(CODA_IS_ALIGNED, 0L)

	/** <FLAG Deprecated>
	    
	    @see CodaAlignmentDialog
	    @see CodaOriginDialog
	    @see CodaAlign
	    Description
	    Determines whether there is sufficient information about the
	    alignment of a CODA system to allow the co-ordinate origin to
	    be set using a single marker (with CodaOriginDialog). If this
	    function returns zero, you should call CodaAlignmentDialog
	    instead of CodaOriginDialog.
	    @return Non-zero if the co-ordinate origin can be set, zero
	    \otherwise.
	    Summary
	    Determine whether system is in state where origin can be set. */
	#define CodaCanSetOrigin() CodaCommand(CODA_CAN_SET_ORIGIN, 0L)

	/** 
	    @return The maximum number of markers which may be used with
	    the current configuration of the CODA system, or CODA_ERROR
	    if there is a problem. To change the configuration, use
	    CodaModeSet.
	    Summary
	    Retrieve number of markers available.                        */
	#define CodaGetNumMarkers() CodaCommand(CODA_GET_NUM_MARKERS, 0L)

	/** pfRate will be set equal to the rate (Hz) at which marker
	    data may be acquired with CodaAcqStart using the current
	    acquisition mode. This value is determined during CodaModeSet
	    \or CodaAcqModeDialog.
	    @param pfRate  Pointer to a codaFLOAT value to get the current
	                   marker rate (Hz).
	    
	    @return CODA_OK if rate was retrieved, CODA_ERROR if there
	    was a problem.
	    
	    
	    Summary
	    Retrieve marker sample rate.
	                                                                   */
	#define CodaGetRateMarker(fRate) ((void)CodaCommand(CODA_GET_RATE_MARKER, (codaDWORD)(float*)fRate))

	/** @see CodaFrameGrab
	    @see CodaAcqPrepare
	    @see CodaAcqStart
	    Description
	    Prepares the system to acquire data in 'frame grab' mode.
	    This function must be called at least once before acquiring
	    frames using CodaFrameGrab or CodaAutoGrab. If you intend to
	    grab frames during a continuous acquisition, you should call
	    CodaAcqPrepare first, followed by CodaFramePrepare, before
	    starting the acquisition using CodaAcqStart.
	    
	    This function may take several seconds to complete.
	    @return CODA_OK if the system was successfully prepared for
	    frame grabs, CODA_ERROR otherwise.
	    
	    
	    Summary
	    Prepare to do single frame grab.                             */
	#define CodaFramePrepare() CodaCommand(CODA_FRAME_PREPARE, 0L)
	/** @see CodaFramePrepare
	    @see CodaFrameGetMarker
	    @see CodaAutoGrab
	    Description
	    Acquires a single frame of data, which can be retrieved
	    afterwards using CodaFrameGetMarker. Use this function for
	    real-time monitoring of CODA data. To acquire a set of frames
	    at a precisely fixed rate, use CodaAcqStart.
	    
	    You must call CodaFramePrepare at least once before any calls
	    to this function. This function can be used at any time after
	    that, even during continuous acquisition.
	    Note
	    Instead of using this function, you can put the system into
	    auto-grab mode by calling CodaAutoGrab(true). Use of
	    CodaAutoGrab is advisable when connecting to the CODA hub
	    \over a network, since it cuts down on the number of network
	    calls.
	    @return CODA_OK if a frame of data was grabbed. CODA_ERROR if
	    CodaFramePrepare has not yet been called.
	    Summary
	    Perform single frame grab.                                    */
	#define CodaFrameGrab() CodaCommand(CODA_FRAME_GRAB, 0L)
	/** 
	    @return Non-zero if a continuous acquisition is currently in
	    progress, zero otherwise.
	    
	    @see CodaAcqStart                                           
	*/
	#define CodaAcqInProgress() CodaCommand(CODA_ACQ_IN_PROGRESS, 0L)
	/** @see CodaAcqStart
	    Description
	    Prepares the system to acquire continuous data. You must call
	    CodaAcqPrepare immediately before each attempt to acquire
	    data using CodaAcqStart.
	    
	    This function may take several seconds to complete.
	    
	    
	    @return CODA_OK if the system was successfully prepared for
	    acquisition, CODA_ERROR otherwise.
	    
	    
	    Summary
	    Prepare system for acquisition.
	                                                                  */
	#define CodaAcqPrepare() CodaCommand(CODA_ACQ_PREPARE, 0L)
	
	/**CodaAcqStart
	   @see CodaStartUp
	   @see CodaAcqStop
	   @see CodaAcqGetMarker
	   @see CodaAcqGetNumFramesMarker
	   @see CodaGetRateMarker
	   @see CodaAcqSetSaveMode
	   Description
	   This initiates continuous data acquisition. Data from the
	   CODA system is acquired to at a very precisely fixed rate.
	   Acquisition continues asyncronously with the calling program,
	   until dwMaxFrames frames of data have been acquired at the
	   current rate, or until CodaAcqStop is called. To determine
	   whether acquisition has finished use CodaAcqInProgress.
	   
	   Before acquiring marker position data, you must ensure that a
	   CODA alignment procedure has been done, using CodaAlign. Use
	   CodaIsAligned to determine whether this is the case. Also,
	   you must allow the system to prepare for acquisition by
	   calling CodaAcqPrepare before every call to CodaAcqStart - it
	   will fail if you do not.
	   
	   After acquisition, data may be retrieved using
	   CodaAcqGetMarker, CodaAcqGetMultiMarker. For real-time
	   monitoring during acquisition, use CodaFrameGrab.
	   
	   WARNING: acquisition data does not persist forever. Data from
	   the most recent acquisition is deleted when the library
	   terminates, and when any of the following functions are
	   called: CodaAcqModeDialog, CodaAlignmentClear, CodaAcqStart,
	   CodaAlignmentDialog, CodaOriginDialog, CodaShutDown,
	   CodaUserOptionsLoad. It is the reponsibility of your program
	   to ensure data is appropriately saved to disk if required.
	   
	   The actual number of frames acquired may be slightly greater
	   than dwNumFrames. After acquisition, use
	   CodaAcqGetNumFramesMarker to determine the exact number of
	   frames acquired.
	   
	   You may wish to determine whether data is stored to RAM or
	   your own customised buffer by using CodaAcqSetSaveMode prior
	   to acquisition. This is an advanced function, provided
	   primarily for users who want fast real-time data, but do not
	   want to consume computer resources by retaining every frame.
	   
	   
	   @param dwNumFrames  The number of frames to acquire (at the current
	                       marker frame rate), or \-1 to continue until
	                       CodaAcqStop is called.
	   
	   @return CODA_OK if a continuous acquisition was successfully
	   initiated. CODA_ERROR if insufficient memory/disk space could
	   be allocated for acquisition, or if CodaAcqPrepare has not
	   been called.
	   Summary
	   Begin continuous acquisition.                                       */
	#define CodaAcqStart(dwMaxFrames) CodaCommand(CODA_ACQ_START, dwMaxFrames)
	/** Terminates a continuous acquisition which was started using
	    CodaAcqStart.
	    
	    
	    @return CODA_ERROR if no continuous acquisition was in
	    progress, CODA_OK otherwise.
	    Summary
	    Stop continuous acquisition.
	                                                                */
	#define CodaAcqStop() CodaCommand(CODA_ACQ_STOP, 0L)
	/**CodaAcqSetSaveMode
	   @see CodaAcqBufferUpdate
	   Description
	   This is an advanced function which allows you to determine
	   the way in which data is stored during continuous
	   acquisition. You can only call this function prior to
	   acquisition, not when acquisition is in progress.
	   
	   
	   
	   CODA_ACQ_SAVE_RAM means the whole acquisition will be saved
	   in RAM, and all data can be retrieved after acquisition is
	   complete - this is the default.
	   
	   
	   
	   CODA_ACQ_SAVE_BUFFER_ONLY allows you to take charge of the
	   storage of acquisition data yourself. In this mode, you must
	   call CodaAcqBufferUpdate at least every 512 frames during
	   acquisition. Here, the functions CodaAcqGetNumFramesMarker,
	   etc. only refer to the number of frames since the last call
	   to CodaAcqBufferUpdate. You must therefore retrieve all
	   required data, using CodaAcqGetMarker, etc., after every
	   successful call to CodaAcqBufferUpdate.
	   
	   
	   
	   CODA_ACQ_SAVE_NONE means that no acquisition data will be
	   saved. Data is available only in real-time using the
	   CodaFrameGrab functions, CodaFrameGetMarker, etc.
	   
	   
	   @param nMode  The new acquisition mode. Must be one of the following
	                 symbolic constants\:
	                 CODA_ACQ_SAVE_RAM,CODA_ACQ_SAVE_BUFFER_ONLY,
	                 CODA_ACQ_SAVE_NONE.
	   
	   @return CODA_OK the mode was successfully set, CODA_ERROR
	   \otherwise.
	   Summary
	   Advanced function for use with continuous acquisiton                 */
	#define CodaAcqSetSaveMode(nMode) CodaCommand(CODA_ACQ_SET_SAVE_MODE, nMode)
	
	/** You need to use this function during continuous acquisition
	    if you called CodaAcqSetSaveMode(CODA_ACQ_SAVE_BUFFER_ONLY)
	    before the acquisition began. It refreshes internal buffers
	    within the SDK to store all data which is coming in from the
	    CODA hardware. You must call this function at regular
	    intervals (e.g. 10 times per second) during such an
	    acquisition, or the acquisition will terminate prematurely.
	    
	    Here, the functions CodaAcqGetNumFramesMarker, etc. only
	    refer to the number of frames since the last call to this
	    function. You must therefore retrieve all required data,
	    using CodaAcqGetMarker, etc., after every successful call.
	    
	    
	    @return CODA_OK if data was updated; CODA_ERROR if
	    acquisition was not in progress or if there were no new
	    frames of data since the last call.
	    
	    
	    Summary
	    Advanced function for use in continuous acquisition.
	                                                                 */
	#define CodaAcqBufferUpdate() CodaCommand(CODA_ACQ_BUFFER_UPDATE, 0L)

	/** @see CodaFrameGrab
	    @see CodaAutoGrab
	    @see CodaFrameGetMarker
	    Description
	    Retrieves marker positions acquired during a single frame
	    grab. In the default mode, this function retrieves the marker
	    positions which were recorded during the most recent call to
	    CodaFrameGrab. Alternatively, you may put the system into
	    auto-grab mode by calling CodaAutoGrab(true). When in
	    auto-grab mode, the system automatically records a new frame
	    during the call to this function, so CodaFrameGrab does not
	    have to be called.
	    
	    If this function is successful, the <I>pData</I> array is
	    filled with sets of 3 floating point values, representing the
	    \x, y, z co-ordinates of each marker, measured in
	    millimetres:
	    
	    
	    
	    <I>pData</I>[0] = marker <I>dwChannelStart</I>, co-ord X
	    
	    
	    
	    <I>pData</I>[1] = marker <I>dwChannelStart</I>, co-ord Y
	    
	    
	    
	    <I>pData</I>[2] = marker <I>dwChannelStart</I>, co-ord Z
	    
	    
	    
	    <I>pData</I>[3] = marker <I>dwChannelStart</I>+1, co-ord X
	    
	    
	    
	    <I>pData</I>[4] = marker <I>dwChannelStart</I>+1, co-ord Y
	    
	    
	    
	    <I>pData</I>[5] = marker <I>dwChannelStart</I>+1, co-ord Z
	    
	    
	    
	    ...etc.
	    
	    
	    
	    Each byte element in the pValid array corresponds to a
	    marker. An element will be set to non-zero if the
	    corresponding marker position was correctly measured; or zero
	    if the marker was out of view and hence its position is not
	    valid.
	    
	    The calculation ((float)<I>dwFrame</I> / marker frame rate)
	    gives the elapsed time between the start of continuous
	    acquisition and the time at which this frame was grabbed.
	    
	    
	    @param pFrame  Pointer to a CODA_FRAME_DATA_STRUCT
	    
	    @return CODA_OK if data was successfully placed in the
	    structure; CODA_ERROR if the structure members were invalid
	    \or no successful calls to CodaFrameGrab or CodaAutoGrab have
	    been made.
	    
	    
	    Note
	    Due to internal operations performed on acquired data,
	    multiple calls to CodaAcqGetMarker may differ in their least
	    significant bits. Although the difference is small (at the
	    micron level and much less than measurement noise), this
	    behaviour means that you cannot use direct comparison to work
	    \out whether or not two data sets came from the same frame.
	    Instead use the dwFrame member of the CODA_FRAME_DATA_STRUCT
	    whilst in continuous acquisition mode.
	    Summary
	    Retrieve marker data from single frame grab                   */
	#define CodaFrameGetMarker(pFrame) CodaCommand(CODA_FRAME_GET_MARKER, (codaDWORD)(LPCODA_FRAME_DATA_STRUCT)pFrame)

	/**CodaAcqGetNumFramesMarker
	   @see CodaAcqStart
	   @return The total number of frames of marker data acquired in
	   the most recent continuous acquisition, or zero if no marker
	   data is present. If CodaAcqSetSaveMode(CODA_ACQ_SAVE_RAM) is
	   used (the default), this is the number of frames since
	   CodaAcqStart. If
	   CodaAcqSetSaveMode(CODA_ACQ_SAVE_BUFFER_ONLY) is used, this
	   is the number of frames since the most recent buffer update.
	   Summary
	   Number of frames of marker data available.                    */
	#define CodaAcqGetNumFramesMarker() ((codaDWORD)CodaCommand(CODA_ACQ_GET_NUM_FRAMES_MARKER, 0L))

	/** Summary
	    Use this function when a continuous acquisition (started
	    using CodaAcqStart) is complete. It retrieves the marker
	    position data which was collected during the acquisition.
	    Description
	    If this function is successful, the pData array is filled
	    with sets of 3 floating point values, representing the x, y,
	    z co-ordinates of the marker at each frame, measured in
	    millimeters:
	    
	    
	    
	    <I><C>pData</I>[0] = frame <I>dwFrameStart</I>, co-ord X
	    
	    
	    
	    <I>pData</I>[1] = frame <I>dwFrameStart</I>, co-ord Y
	    
	    
	    
	    <I>pData</I>[2] = frame <I>dwFrameStart</I>, co-ord Z
	    
	    
	    
	    <I>pData</I>[3] = frame <I>dwFrameStart</I> + 1, co-ord X
	    
	    
	    
	    <I>pData</I>[4] = frame <I>dwFrameStart</I> + 1, co-ord Y
	    
	    
	    
	    <I>pData</I>[5] = frame <I>dwFrameStart</I> + 1, co-ord Z</C>
	    
	    
	    
	    ...etc.
	    
	    
	    
	    Each byte element in the pValid array corresponds to a single
	    frame of data. An element will be set to non-zero if the
	    marker position was correctly measured during the
	    corresponding frame; or zero if the marker was out of view
	    and hence its position is not valid.
	    @param pAcqData  Pointer to a CODA_ACQ_DATA_STRUCT
	    
	    @return CODA_OK if data was successfully placed in the
	    structure; CODA_ERROR if no data is present or the structure
	    members were invalid.
	    Summary
	    Retrieve marker data after or during continuous acquisition.  */
	#define CodaAcqGetMarker(pData) CodaCommand(CODA_ACQ_GET_MARKER, (codaDWORD)(LPCODA_ACQ_DATA_STRUCT)pData)

	/**CodaAcqGetMultiMarker
	   Summary
	   This function can be used after or during acquisition to
	   perform the same task as multiple calls to CodaAcqGetMarker()
	   Description
	   You must specify the zero-based index of the first marker in
	   the dwChannelStart member of pAcqData, and set dwNumChannels
	   to the number of markers to get. If bAutoBufferUpdate is set,
	   a call to CodaAcqBufferUpdate is automatically made from
	   inside this function.
	   
	   dwBufferSize should be set to the maximum number of frames
	   required. To ensure that all available frames are retrieved,
	   this should be greater than or equal to the number returned
	   by CodaAcqGetNumFramesMarker.
	   
	   pData must point to a buffer for the new marker data, which
	   must be of size
	   dwBufferSize*dwNumChannels*3*sizeof(codaFLOAT)
	   
	   pValid must point to a buffer for marker validity data, of
	   size dwBufferSize*dwNumChannels*sizeof(codaBYTE)
	   
	   If this function is successful, pData will be filled with
	   sets of 3 floating point values, representing the x, y, z
	   co-ordinates of the marker at each frame, measured in
	   millimeters:
	   
	   <I><C>pData</I>[0] = marker 0, frame 0, co-ord X
	   
	   <I>pData</I>[1] = marker 0, frame 0, co-ord Y
	   
	   <I>pData</I>[2] = marker 0, frame 0, co-ord Z
	   
	   <I>pData</I>[3] = marker 1, frame 0, co-ord X
	   
	   <I>pData</I>[4] = marker 1, frame 0, co-ord Y
	   
	   <I>pData</I>[5] = marker 1, frame 0, co-ord Z
	   
	   ...
	   
	   <I>pData</I>[3*N ] = marker 0, frame 1, co-ord X
	   
	   <I>pData</I>[3*N+1] = marker 0, frame 1, co-ord Y
	   
	   <I>pData</I>[3*N+2] = marker 0, frame 1, co-ord Z
	   
	   ...</C>
	   
	   etc.
	   
	   
	   
	   Each byte element in pValid wcorresponds to a given frame for
	   a given marker. An element will be set to non-zero if the
	   marker position was correctly measured during the
	   corresponding frame; or zero if the marker was out of view
	   and hence its position is not valid.
	   
	   The dwNumFrames member will return the total number of frames
	   which were available for this call. In CODA_ACQ_SAVE_RAM
	   mode, this will correspond to the total number of frames
	   since CodaAcqStart. In CODA_ACQ_SAVE_BUFFER_ONLY mode, it
	   will correspond to the number of frames since the previous
	   buffer update.
	   
	   If dwNumFrames is greater than the value you gave for
	   dwBufferSize, some frames will have been missed. You can
	   prevent this from happening by ensuring that the buffer size
	   is always greater than or equal to CodaAcqGetNumFramesMarker.
	   @param pAcqData  Pointer to a CODA_ACQ_DATA_MULTI_STRUCT
	   
	   @return CODA_OK if data was successfully placed in the
	   structure; CODA_ERROR if no data is present or the initial
	   values for dwChannelStart or dwNumChannels were invalid.      */
	#define CodaAcqGetMultiMarker(pData) CodaCommand(CODA_ACQ_GET_MULTI_MARKER, (codaDWORD)(LPCODA_ACQ_DATA_MULTI_STRUCT)pData)

/** @see CodaStartUp
    @see Main codaMotion user manual
    Description
    <C>pMode-\>bActiveCoda</C> is an array of Boolean values
    indicating whether or not to use the CODA with the
    corresponding index to acquire data.
    
    <C>pMode-\>dwMarkerMode</C> must be set to one of the
    following constants:-
    
    � modeMarker100
    
    � modeMarker120
    
    � modeMarker200
    
    � modeMarker400
    
    � modeMarker800
    
    This determines the rate at which data is acquired during
    continuous acquisition and the maximum number of markers.
    
    <C>pMode-\>dwDecimation</C> is the amount by which to divide
    the rate specified by dwMarkerMode.
    
    <C>pMode-\>dwMaxMarker</C> is the maximum marker number you
    are interested in acquiring. Latency can be reduced slightly
    by setting this value, as it allows the system to avoid
    transferring data you will not use. If in doubt set this
    value to 56.
    
    See main User Manual for explanation of the relationship
    between sampling rate and maximum number of markers which can
    be acquired.
    @param pMode  Pointer to a CODA_MODE_STRUCT
    
    @return CODA_OK if mode was successfully updated, CODA_ERROR
    if there was a problem.
    
    
    Summary
    Change hardware mode settings, frame rate, etc.               */
	#define CodaModeSet(pMode) CodaCommand(CODA_MODE_SET, (codaDWORD)pMode)

	/** @see CodaStartUp
	    @see CodaIsAligned
	    @see CodaAlignmentClear
	    @see CodaGetErrorMessage
	    Description
	    Many applications make this function available to the user
	    via a mode in the user interface. It allows the co-ordinate
	    system for the CODA system to be defined based upon the
	    current location of three markers which the user must place
	    in the capture volume. The identities of the markers to be
	    used are specified using a CODA_ALIGN_STRUCT.
	    
	    This procedure may fail if the user placed the markers in an
	    inappropriate configuration, or if one or more of them were
	    not visible to all CODA units. The return value should be
	    tested and any errors reported to the user.
	    
	    To determine whether an appropriate alignment has been done,
	    call CodaIsAligned. You can clear the current alignment
	    settings and put the system into an un-aligned state using
	    CodaAlignmentClear. The current alignment settings are
	    automatically saved to CODASYS.INI, and are restored during
	    CodaStartUp.
	    @param pAlign  Pointer to a CODA_ALIGN_STRUCT
	    
	    @return This function returns CODA_OK if the system was
	    successfully aligned and CODA_ERROR if it was not. If it
	    \returns CODA_ERROR call CodaGetErrorMessage to find out why.
	    Summary
	    Align CODA system.                                            */
	#define CodaAlign(pData) CodaCommand(CODA_ALIGN, (codaDWORD)pData)

	/** Description
			Used to fill out a CODA_TRANSFORM_STRUCT with the transfor used to align data from a particular CODA.
			
			@param pData	A pointer to a CODA_TRANSFORM_STRUCT. The dwUnit member must be filled with the index of
										the CODA you want to see the transform for.
										
			@return	This function returns CODA_OK if sucsessfull and CODA_ERROR otherwise.*/
	#define CodaGetTransform(pData) CodaCommand(CODA_GET_TRANSFORM,(codaDWORD)pData)

	/** Description
			Used to turn on or off using the alignment transform. Note that if the alignment transform is off it
			is expected that you will look at the CODA units one at a time using CodaSetInterest as the positions
			cannot be meaningfully combine between CODAs without the alignment transform.
			Summary
			Turns on/off the alignment transform.
			@param bActive	True if alignment transform is to be used, false if not.
			@return CODA_OK if sucsessfull CODA_ERROR otherwise.*/
	#define CodaSetTransformActive(bActive) CodaCommand(CODA_SET_TRANSFORM_ACTIVE,(codaDWORD)bActive)

	/** Description
			Used for real-time monitoring of CODA data. Enabling
	    auto-grab mode by calling CodaAutoGrab(true) causes the
	    system to automatically record a new frame of data whenever
	    CodaFrameGetMarker is called. This is done by making internal
	    calls to CodaFrameGrab, so there is no need to call
	    CodaFrameGrab explicity from your code.

	    You must call CodaFramePrepare at least once before getting
	    any data in single-frame mode.
	    @param bActive  true to enable auto\-grab mode, false to
	                    disable
	    
	    @return CODA_OK if auto-grab mode was succesfully updated,
	    CODA_ERROR otherwise.
	    Summary
	    Allow simultaneous frame grab and data retrieval.
	                                                                  */
	#define CodaAutoGrab(bActive)	CodaCommand(CODA_AUTO_GRAB,(codaDWORD)bActive)
	/** Only codas marked as true will contributed to the
			results of CodaAcqGetMarker or CodaFrameGetMarker. Note
			that this will cause a frame grab if AutoGrab is on, so
			if you want to see the same data from different combinations
			of codas you must set that to false and do FrameGrabs yourself.
			Summary
			Set which CODAs you are intrested in.
			@param bInterest	Takes an array of length 10 of boolean values that
												indicate which codas you are intrested in seeing data
												from.
												
			@return CODA_OK if sucsessfull, CODA_ERROR otherwise. If you try to set interest
							to a coda which is not active.*/
	#define CodaSetInterest(bInterest) CodaCommand(CODA_SET_INTEREST,(codaDWORD)bInterest)

	/**	Description
			This is used to set the transformas used to align the coda system. This can be used
			to impliment your own custom alignment system, for unusal circumstances. For example
			you may wish to use codas with non-overlapping feilds of view.
			Summary
			Set the alignment transform manually.
			@param pData This is a pointer to a CODA_TRANSFORM_STRUCT.
			
			@return CODA_OK if sucsessfull, CODA_ERROR otherwise.*/
	#define CodaSetTransform(pData) CodaCommand(CODA_SET_TRANSFORM,(codaDWORD)pData)

	/** Description
			This is used to active/deavtive using the external sync. You cannot do single shot frame grabs
			while in this mode. Also you must garentee that at least one external sync is recived after calling 
			CodaAcqStop(). Also your external sync frequency must never equal or exceed the frequency of the mode you have
			set with CodaModeSet() or the dialogue box.
			Summary
			Enable/Disable external sync mode.
			@param bOn Set to true to use external syncing and false to use the internal sync generator.
			
			@return CODA_OK if sucsessfull, CODA_ERROR otherwise.*/
	#define CodaSetExternalSync(bOn) CodaCommand(CODA_SET_EXT_SYNC_IN,(codaDWORD)bOn)

	/** Description
			This is used to set the Sync Out from the ActiveHub as being active high or active low. Coda CX-1
			uses an active low sync, this option is only for dealing with non Charnwood Dynamics equipment.
			Summary
			Set sync to active high/low.
			@param bHigh Set to true for active high and set to false for active low.

			@return CODA_OK if sucsessfull, and CODA_ERROR otherwise.*/
	#define CodaExternSyncActiveHigh(bHigh) CodaCommand(CODA_SET_EXT_SYNC_ACTIVE_HIGH,(codaDWORD)bHigh)

	/** Description
			This is used to switch on and off any external strobe unit which is attached to the active hub.
			Summary
			Enable/Disable external strobes.
			@param bOn True to turn on the external strobe and false to turn it off.
			
			@return CODA_OK if sucsessfull and CODA_ERROR if other wise. This will allways fail on passive huib systems.*/
	#define	CodaExternalStrobe(bOn) CodaCommand(CODA_SET_EXT_STOBE,(codaDWORD)bOn)

	/**	Description
			This is used to enable timestamping of frames. Note that the internal sync will generate timestamps even when
			it is not being used to drive the CODAS (i.e. when the system is stopped or in external sync mode).
			Summary
			Enable/Disable timestamping.
			@param bOn True to enable timestamping and false to disable it.
			
			@return CODA_OK if sucsessfull and CODA_ERROR otherwise. This will allways fail on passive hub systems.*/
	#define	CodaSetTimestamping(bOn) CodaCommand(CODA_SET_TIMESTAMPING,(codaDWORD)bOn)

	/**CodaGetTimestamps
	   Description
	   This gets the most recent set of timestamps for the system,
	   and also the time now.
	   
	   \Note that if your client is operating over a network link
	   then the time it takes to transfere this message, therefore
	   the time now is only really acurate on local network systems,
	   \or direct call systems. The accuary of these timestamps
	   effected by a noise (due toother interrupts on the system
	   preventing ours from running) which is typicaly +/-1us, and a
	   slow drift (due to changes in temprature of the crystals) of
	   about +/-4us.
	   
	   \Note that windows is not a hard realtime system, and
	   therefore other activites on the system may periodicaly cause
	   a sync pulse to be missed entirely. This does not happen very
	   \often (on average 1 in 250000 samples), however it does mean
	   that you should write your code with the expectation that it
	   could.
	   
	   These timestamps are available all the time, even when the
	   system is not running, or not using one of the sync sources
	   (i.e. the internal sync time is available when running in
	   externaly synced mode and the external sync time is available
	   when running with internal sync.
	   Summary
	   Retrives the timestamps of the last sync pulses.
	   @param pTimestamp  Pointer to a CODA_TIMESTAMP_STRUCT into which timestamps
	                      will be placed. Note that if timestamping is not enabled
	                      these will be the timestamps from the last time it was
	                      enabled. Also note that these will be 0 if no timestamps
	                      have ever been set.
	   
	   @return CODA_OK if sucsessfull and CODA_ERROR otherwise. This
	   will allways fail on a passive hub system.                                  */
	#define	CodaGetTimestamps(pTimestamp) CodaCommand(CODA_GET_TIMESTAMPS,(codaDWORD)pTimestamp)

	/** Description
			If you want faster access to the time now, you may which to use the RDTSC instruction yourself. However
			you will need to get the scale to adjust from RDTSC units (which are processor cycles, and therefore vary
			a great deal from system to system) to seconds. An offset is but into the external timestamp member, and 
			a scale is put into the time now. You must do the offset first, i.e. CalibratedTime = (RawTime-Offset)*Scale.
			Summary
			Retrives the RDTSC unit to seconds calibration.
			@param pTimestamps	A pointer to a timestamp structure. The calibration factor will be placed in the tNow
													member.
			@return CODA_OK if sucsessfull, CODA_ERROR otherwise. This will allways fail on a passive hub system.*/
	#define CodaGetTimeCalibration(pTimestamp) CodaCommand(CODA_GET_TIMECAL,(codaDWORD)pTimestamp)

	/**CodaTimestampPrepare
	   Description
	   This must be called before enabling timestamping so that the
	   host clock may be calibrated against the coda system clock.
	   Summary
	   Readys the system for timestamping.
	   @return CODA_OK if sucsessfull, CODA_ERROR otherwise.        */
	#define CodaTimestampPrepare() CodaCommand(CODA_TIMESTMAP_PREPARE,0UL)

	/** Description
			Begins a dynamic alignment procedure which will compute relative CODA alignments based on the
			movements of a single marker 
			@return CODA_OK if alignment acquisition successfully started, CODA_ERROR otherwise */
	#define CodaAlignDynamicStart(pAlign) CodaCommand(CODA_ALIGN_DYNAMIC_START,(codaDWORD)(LPCODA_ALIGN_DYNAMIC_STRUCT)pAlign)

	/** Description
			Retrieves the status of a dynamic alignment procedure which was started using CodaAlignDynamicStart
			@return CODA_OK if status retrieved CODA_ERROR if alignment has not been started
			@see CodaAlignDynamicStart */
	#define CodaAlignDynamicGetStatus(pStatus) CodaCommand(CODA_ALIGN_DYNAMIC_GET_STATUS,(codaDWORD)(LPCODA_ALIGN_DYNAMIC_STATUS_STRUCT)pStatus)

	/** Description
			Compute and store a dynamic alignment using all frames acquired since CodaAlignDynamicStart
			@return CODA_OK if alignment performed and saved, CODA_ERROR if alignment has not been started or insufficient 
			in-view marker data is present
			@see CodaAlignDynamicStart */
	#define CodaAlignDynamicFinish() CodaCommand(CODA_ALIGN_DYNAMIC_FINISH,0UL)

/*-------------------------------------------------------------------------*/
 
#endif

#ifdef __cplusplus
}
#endif   


/*-------------------------------------------------------------------------*/

