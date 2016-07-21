// GraspTaskManagers.h
//

#define MAX_GRASP_TRIALS 64

namespace Grasp {

	typedef enum { NullState, StartBlock, StartTrial, StraightenHead, AlignHead, PresentTarget, TiltHead, ObtainResponse, 
					ProvideFeedback, TrialCompleted, BlockCompleted, TrialInterrupted, Timeout,
					ExitStateMachine } GraspTrialState;

	class GraspTaskManager : public GraspVR {


	public:

		static const double indicatorDisplayDuration;

		// List of paramters for each trial.
		struct {
			double	targetHeadTilt;
			double	targetHeadTiltTolerance;
			double	targetHeadTiltDuration;
			double	targetOrientation;
			double  hapticTargetOrientationTolerance;
			double	targetPresentationDuration;
			double	responseHeadTilt;
			double	responseHeadTiltTolerance;
			double	responseHeadTiltDuration;
			double  responseTimeout;
			double	conflictGain;
			bool	provideFeedback;
		} trialParameters[MAX_GRASP_TRIALS];
		int nTrials;
		int currentTrial;
		int retriesRemaining;
		int SetMaxRetries( int max_retries ) {
			retriesRemaining = max_retries;
		}
		int LoadTrialParameters( char *filename );
		void RepeatTrial( int trial );
		virtual void Prepare( void ) {}
		int RunTrialBlock( char *sequence_filename, char *output_filename_root );

		FILE *response_fp;
		char responseFilename[FILENAME_MAX];
		FILE *pose_fp;
		char poseFilename[FILENAME_MAX];

		// State Machine
		GraspTrialState previousState, currentState, nextState;
		bool UpdateStateMachine( void );
		DexServices *dexServer;

		// General purpose timers used by multiple states.
		::Timer stateTimer;	
		::Timer auxStateTimer;
		::Timer blockTimer;

		// Detect action of the subject to record a response.
		bool Validate( void ) {
			ovrInputState state;
			ovr_GetInputState(	oculusMapper->session,  ovrControllerType_Remote, &state );
			return( (oculusDisplay->Button[MOUSE_LEFT]) || oculusDisplay->Button[MOUSE_MIDDLE] || oculusDisplay->Button[MOUSE_RIGHT] || (state.Buttons & ovrButton_Enter) );
		}

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

		// TrialInterrupted
		// The trial was interrupted because the head orientation was not maintained.
		virtual void EnterTrialInterrupted( void );
		virtual GraspTrialState UpdateTrialInterrupted( void );
		virtual void ExitTrialInterrupted( void );

		// Timeout
		// The trial was interrupted because the subject failed to respond.
		virtual void EnterTimeout( void );
		virtual GraspTrialState UpdateTimeout( void );
		virtual void ExitTimeout( void );

	public:
		GraspTaskManager( void ) : nTrials(0), retriesRemaining(2), response_fp(NULL), pose_fp(NULL) {}
		~GraspTaskManager(){}
		void Initialize( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, GraspTrackers *trkrs, DexServices *dex ) {
			dexServer = dex;
			GraspVR::Initialize( instance, display, mapper, trkrs );
		}
		void Release( void ) {
			GraspVR::Release();
		}

	};

	// V-V protocol. 
	class VtoV : public GraspTaskManager {
		void Prepare( void ) { renderer->selectedTool = renderer->vTool; }
		void EnterPresentTarget( void );
		void ExitPresentTarget( void );
		void EnterObtainResponse( void );
		void ExitObtainResponse( void );
	};
	// V-VK protocol. 
	class VtoVK : public GraspTaskManager {
		void Prepare( void ) { renderer->selectedTool = renderer->hand; }
		void EnterPresentTarget( void );
		void ExitPresentTarget( void );
		void EnterObtainResponse( void );
		void ExitObtainResponse( void );
	};
	// V-K protocol. 
	class VtoK : public GraspTaskManager {
		void Prepare( void ) { renderer->selectedTool = renderer->hand; }
		void EnterPresentTarget( void );
		void ExitPresentTarget( void );
		void EnterObtainResponse( void );
		void ExitObtainResponse( void );
	};
	// K-K protocol. 
	class KtoK : public GraspTaskManager {
		void Prepare( void ) { renderer->selectedTool = renderer->hand; }
		void EnterPresentTarget( void );
		GraspTrialState UpdatePresentTarget( void );
		void ExitPresentTarget( void );
		void EnterObtainResponse( void );
		GraspTrialState UpdateObtainResponse( void );
		void ExitObtainResponse( void );
	};


};

