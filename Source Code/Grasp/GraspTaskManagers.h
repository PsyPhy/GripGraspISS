// GraspTaskManagers.h
//

#define MAX_GRASP_TRIALS 64

namespace Grasp {

	typedef enum { NullState, StraightenHead, PresentTarget, TiltHead, ObtainResponse, 
					ProvideFeedback, TrialCompleted, TrialInterrupted, ExitStateMachine } GraspTrialState;

	class GraspTaskManager : public GraspVR {


	public:

		// List of paramters for each trial.
		struct {
			double	targetHeadTilt;
			double	targetHeadTiltTolerance;
			double	targetHeadTiltDuration;
			double	targetOrientation;
			double	targetPresentationDuration;
			double	responseHeadTilt;
			double	responseHeadTiltTolerance;
			double	responseHeadTiltDuration;
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
		int RunTrialBlock( char *sequence_filename, char *output_filename_root );

		FILE *fp;
		char responseFilename[FILENAME_MAX];

		// State Machine
		GraspTrialState previousState, currentState, nextState;
		bool UpdateStateMachine( void );

		// A general purpose timer used by multiple states.
		::Timer stateTimer;	

		// 
		// Now define the handlers for each possible GraspTrialState.
		// All of these methods are declared virtual, with the expectation
		// that they will be overridden by derived classes.
		//

		// StraightenHead
		// The subject is guided to align the head with the body axis.
		virtual void EnterStraightenHead( void );
		virtual GraspTrialState UpdateStraightenHead( void );
		virtual void ExitStraightenHead( void );

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

		// TrialInterrupted
		// The trial was interrupted, perhaps because of a timeout to respond
		// or perhaps because the head orientation was not maintained.
		virtual void EnterTrialInterrupted( void );
		virtual GraspTrialState UpdateTrialInterrupted( void );
		virtual void ExitTrialInterrupted( void );

	public:
		GraspTaskManager( void ) : nTrials(0), retriesRemaining(2) {}
		~GraspTaskManager(){}

		void Initialize( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, GraspTrackers *trkrs  ) {
			GraspVR::Initialize( instance, display, mapper, trkrs );
		}
		void Release( void ) {
			GraspVR::Release();
			if ( fp ) fclose( fp );
		}

	};

	// V-V protocol. 
	class VtoV : public GraspTaskManager {
		void EnterPresentTarget( void );
		void ExitPresentTarget( void );
		void EnterObtainResponse( void );
		void ExitObtainResponse( void );
	};
	// V-V protocol. 
	class VtoVK : public GraspTaskManager {
		void EnterPresentTarget( void );
		void ExitPresentTarget( void );
		void EnterObtainResponse( void );
		void ExitObtainResponse( void );
	};

};

