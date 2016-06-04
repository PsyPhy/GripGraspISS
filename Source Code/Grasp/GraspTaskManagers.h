// GraspTaskManagers.h
//

namespace Grasp {

	typedef enum { NullState, StraightenHead, PresentTarget, TiltHead, ObtainResponse, ExitStateMachine } GraspTrialState;

	class GraspTaskManager : public GraspVR {


	protected:

		GraspTrialState previousState, currentState, nextState;
		bool UpdateStateMachine( void );

		// A general purpose timer used by multiple states.
		::Timer stateTimer;	

		// 
		// Now define the handlers for each state.
		//

		// The subject is guided to align the head with the body axis.
		void EnterStraightenHead( void );
		GraspTrialState UpdateStraightenHead( void );
		void ExitStraightenHead( void );

		// The target is diplayed to the subejct.
		void EnterPresentTarget( void );
		GraspTrialState UpdatePresentTarget( void );
		void ExitPresentTarget( void );

		// The target is diplayed to the subejct.
		void EnterTiltHead( void );
		GraspTrialState UpdateTiltHead( void );
		void ExitTiltHead( void );

		// The target is diplayed to the subejct.
		void EnterObtainResponse( void );
		GraspTrialState UpdateObtainResponse( void );
		void ExitObtainResponse( void );

	public:
		GraspTaskManager( HINSTANCE instance, OculusDisplayOGL *display, OculusMapper *mapper, GraspTrackers *trkrs  ) {
			this->GraspVR::GraspVR( instance, display, mapper, trkrs );
		}

		int RunTrialBlock( void );

	};

};

