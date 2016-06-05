// GraspTaskManagers.h
//

namespace Grasp {

	typedef enum { NullState, StraightenHead, PresentTarget, TiltHead, ObtainResponse, 
					ProvideFeedback, TrialCompleted, TrialInterrupted, ExitStateMachine } GraspTrialState;

	class GraspTaskManager : public GraspVR {


	public:

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
		GraspTaskManager( void ) {}

		void Initialize( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, GraspTrackers *trkrs  ) {
			GraspVR::Initialize( instance, display, mapper, trkrs );
		}

		int RunTrialBlock( void );

	};

	// V-V protocol. 
	class VtoV : public GraspTaskManager {
		void EnterPresentTarget( void );
		void ExitPresentTarget( void );
		void EnterObtainResponse( void );
		void ExitObtainResponse( void );
	};

};

