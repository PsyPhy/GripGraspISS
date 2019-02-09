// GraspTaskManagers.h
//
#include "Grasp.h"

namespace Grasp {

	class GraspTaskManager : public GraspVR {

	public:

		static double	targetPresentationDuration;
		static double	indicatorDisplayDuration;

		static double	alignHeadTimeout;
		static double	tiltHeadTimeout;
		static double	responseTimeout;
		static double	alignHandTimeout;
		static double	lowerHandPromptDelay;
		static double	raiseHandPromptDelay;
		static double	handErrorDelay;

		static double	targetHeadTiltTolerance;
		static double	responseHeadTiltTolerance;
		static double	hapticTargetOrientationTolerance;

		static int maxRetries;

		static StraightenHeadMethod straightenHeadMethod;

		// List of paramters for each trial.
		struct {
			double	targetHeadTilt;
			double	targetOrientation;
			double	responseHeadTilt;
			double	conflictGain;
			bool	provideFeedback;
		} trialParameters[MAX_GRASP_TRIALS];
		
		int nTrials;				// Total number of trials to be performed. This gets incremented if a trial is set to be repeated.
		int currentTrial;			// Where we are in the list of trials.
		int retriesRemaining;		// Erroneous trials get repeated at the end of the block, but only up to a certain number of repetitions.

		// Derived classes must provide a method identifying which paradigm it implements.
		virtual Paradigm GetParadigm( void ) = 0;
		char *tag;

		int LoadTrialParameters( char *filename );
		int WriteRemainingTrialParameters( char *filename );
		void RepeatTrial( int trial );
		virtual void Prepare( void ) {}
		int RunTrialBlock( char *sequence_filename, char *output_filename_root );
		void ShowProgress( int phase, int characteristic = 0 );

		FILE *response_fp;
		char responseFilename[FILENAME_MAX];
		FILE *pose_fp;
		char poseFilename[FILENAME_MAX];
		FILE *frame_fp;
		char frameFilename[FILENAME_MAX];

		// State Machine
		GraspTrialState previousState, currentState, nextState;
		bool UpdateStateMachine( void );
		DexServices *dexServices;

		// Timers used by various different states.
		// It would have been nice if each timer could be restricted in scope to the 3 methods associated 
		//  with the associated state, but that would mean making a class for each state. I may do that 
		//  someday, but in the mean time I give them names here to help keep track of which state is 
		//  using which time and for what.
		::Timer presentTargetTimer;
		::Timer straightenHeadTimer;
		::Timer alignHeadTimer;
		::Timer alignHandTimer;
		::Timer tiltHeadTimer;
		::Timer responseTimer;
		::Timer trialCompletedTimer;
		::Timer blockTimer;

		// 
		// Now define the handlers for each possible GraspTrialState.
		// All of these methods are declared virtual, with the expectation
		// that they will be overridden by derived classes.
		//

		// StartBlock
		// The system waits until the subject presses a button to start a block of trials.
		virtual void EnterStartBlock( void );
		virtual GraspTrialState UpdateStartBlock( void );
		virtual void ExitStartBlock( void );

		// StartTrial
		// Trial parameters are set.
		virtual void EnterStartTrial( void );
		virtual GraspTrialState UpdateStartTrial( void );
		virtual void ExitStartTrial( void );

		// StraightenHead
		// Conflict gain is applied with a black scene.
		virtual void EnterStraightenHead( void );
		virtual GraspTrialState UpdateStraightenHead( void );
		virtual void ExitStraightenHead( void );

		// AlignHead
		// The subject is guided to align the head with the body axis.
		virtual void EnterAlignHead( void );
		virtual GraspTrialState UpdateAlignHead( void );
		virtual void ExitAlignHead( void );

		// PresentTarget
		// The target is diplayed to the subejct.
		virtual void EnterPresentTarget( void );
		virtual GraspTrialState UpdatePresentTarget( void );
		virtual void ExitPresentTarget( void );

		GraspTrialState UpdateVisualTarget( void );
		GraspTrialState UpdateKinestheticTarget( void );

		// TiltHead
		// The subject is guided to achieve a tilted head position.
		virtual void EnterTiltHead( void );
		virtual GraspTrialState UpdateTiltHead( void );
		virtual void ExitTiltHead( void );

		// ObtainResponse
		// The subject reproduces the remembered target orientation.
		virtual void EnterObtainResponse( void );
		virtual GraspTrialState UpdateObtainResponse( void );
		virtual void ExitObtainResponse( void );

		GraspTrialState UpdateVisualResponse( void );
		GraspTrialState UpdateKinestheticResponse( void );

		// ProvideFeedback
		// Show the subject the results of their response.
		virtual void EnterProvideFeedback( void );
		virtual GraspTrialState UpdateProvideFeedback( void );
		virtual void ExitProvideFeedback( void );

		// TrialCompleted
		// Show the subject the results of their response.
		virtual void EnterTrialCompleted( void );
		virtual GraspTrialState UpdateTrialCompleted( void );
		virtual void ExitTrialCompleted( void );

		// BlockCompleted
		// Show the subject the results of their response.
		virtual void EnterBlockCompleted( void );
		virtual GraspTrialState UpdateBlockCompleted( void );
		virtual void ExitBlockCompleted( void );

		// VRCompleted
		// Show the subject the results of their response.
		virtual void EnterVRCompleted( void );
		virtual GraspTrialState UpdateVRCompleted( void );
		virtual void ExitVRCompleted( void );

		// Demo
		// Show the subject the results of their response.
		virtual void EnterDemo( void );
		virtual GraspTrialState UpdateDemo( void );
		virtual void ExitDemo( void );

		// TrialInterrupted
		// The variable interruptCondition should have been set to indicate the cause.
		Anomaly interruptCondition;
		// A holder to remember which indicator has been turned on.
		Assembly *interrupt_indicator;
		virtual void EnterTrialInterrupted( void );
		virtual GraspTrialState UpdateTrialInterrupted( void );
		virtual void ExitTrialInterrupted( void );

	public:
		// Constructor, with initialization of some elements.
		GraspTaskManager( void ) : nTrials(0), retriesRemaining(0), response_fp(NULL), pose_fp(NULL), frame_fp(NULL), tag("??to??" ) {}
		~GraspTaskManager(){}
		void Initialize( GraspDisplay *dsply, GraspTrackers *trkrs, DexServices *dex ) {
			dexServices = dex;
			GraspVR::Initialize( dsply, trkrs );
		}
		void Release( void ) {
			GraspVR::Release();
		}

		bool Validate( void ) {
			return display->Validate();
		}
		void FillClientData( GraspClientData &data ) {

			strncpy( data.ID, "GRASP", sizeof( data.ID ));
			data.enableBits = 0;
			unsigned long bit = 0x01;

			// If you change the next lines, you need to change the #defines in Grasp.h.

			if ( renderer->orientationTarget->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->positionOnlyTarget->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->straightAheadTarget->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->response->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->projectiles->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->multiProjectile->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->monoProjectile->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->glasses->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->headTiltPrompt->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->gazeLaser->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->handLaser->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->successIndicator->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->vTool->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->kTool->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->vkTool->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->kkTool->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->handRollPrompt->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->lowerHandPrompt->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->wristZone->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->room->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->tunnel->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->starrySky->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->darkSky->enabled ) data.enableBits |= bit; bit = bit << 1;
			if ( renderer->aimingErrorSphere->enabled ) data.enableBits |= bit; bit = bit << 1;

			data.spinnerBits = 0;
			bit = 0x01;
			if ( renderer->readyToStartIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->blockCompletedIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->lowerHandIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->raiseHandIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->raiseArmTimeoutIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->headMisalignIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->manualRejectIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->invalidateTrialIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->headAlignTimeoutIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->timeoutIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->responseTimeoutIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->handRotateTimeoutIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->handTooSoonIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->handShouldNotBeRaisedIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->straightenHeadIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->vrCompletedIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;
			if ( renderer->demoIndicator->enabled ) data.spinnerBits |= bit; bit = bit << 1;

			data.currentState = currentState;
			data.currentTrial = currentTrial;
			data.paradigm = GetParadigm();

			CopyPose( data.headPose, headPose.pose );
			CopyPose( data.handPose, handPose.pose );
			CopyPose( data.chestPose, chestPose.pose );
			CopyQuaternion( data.rollQuaternion, rollPose.pose.orientation );

			data.targetHeadTiltD = trialParameters[currentTrial].targetHeadTilt * 100.0;
			data.targetOrientationD = trialParameters[currentTrial].targetOrientation * 100.0;
			data.responseHeadTiltD = trialParameters[currentTrial].responseHeadTilt * 100.0,
			data.conflictGain = (float) trialParameters[currentTrial].conflictGain;
			data.provideFeedback = trialParameters[currentTrial].provideFeedback;
		}


	};

	// V-V protocol. 
	class VtoV : public GraspTaskManager {
		Paradigm GetParadigm( void ) { return( VTOV ); }
		void Prepare( void ) { renderer->selectedTool = renderer->vTool; tag = "VtoV"; }
		GraspTrialState UpdatePresentTarget( void ) { return UpdateVisualTarget(); }
		GraspTrialState UpdateObtainResponse( void ) { return UpdateVisualResponse(); }
		void EnterObtainResponse( void );
		void EnterPresentTarget( void );
	};
	// V-VK protocol. 
	class VtoVK : public GraspTaskManager {
		Paradigm GetParadigm( void ) { return( VTOVK ); }
		void Prepare( void ) { renderer->selectedTool = renderer->hand; tag = "VtoVK"; }
		GraspTrialState UpdatePresentTarget( void ) { 	return UpdateVisualTarget(); }
		GraspTrialState UpdateObtainResponse( void ) { return UpdateKinestheticResponse(); }
		void EnterPresentTarget( void );
		void EnterObtainResponse( void );
	};
	// V-K protocol. 
	class VtoK : public GraspTaskManager {
		Paradigm GetParadigm( void ) { return( VTOK ); }
		void Prepare( void ) { renderer->selectedTool = renderer->hand; tag = "VtoK"; }
		GraspTrialState UpdatePresentTarget( void ) { 	return UpdateVisualTarget(); }
		GraspTrialState UpdateObtainResponse( void ) { return UpdateKinestheticResponse(); }
		void EnterPresentTarget( void );
		void EnterObtainResponse( void );
	};
	// K-K protocol. 
	class KtoK : public GraspTaskManager {
		Paradigm GetParadigm( void ) { return( KTOK ); }
		void Prepare( void ) { renderer->selectedTool = renderer->hand; tag = "KtoK"; }
		GraspTrialState UpdatePresentTarget( void ) { return UpdateKinestheticTarget(); }
		GraspTrialState UpdateObtainResponse( void ) { return UpdateKinestheticResponse(); }
		void EnterPresentTarget( void );
		void EnterObtainResponse( void );
	};
	// DEMO protocol. 
	class DemoP : public GraspTaskManager {
		Paradigm GetParadigm( void ) { return( DEMO ); }
		// Initialize the tool to be used and also override the intial state of the state machine.
		void Prepare( void ) { renderer->selectedTool = renderer->hand; tag = "DEMO"; currentState = Demo; }
	};

	// QuitVR protocol. 
	class QuitVR : public GraspTaskManager {
		Paradigm GetParadigm( void ) { return( QUITVR ); }
		// Initialize the tool to be used and also override the intial state of the state machine.
		void Prepare( void ) { renderer->selectedTool = renderer->hand; tag = "QUIT"; currentState = VRCompleted; }
	};

};

