#pragma once

#define MAX_GRASP_TRIALS 64
#define MANUAL_BLOCK_INTERRUPTION -2
#define MAX_RETRIES_EXCEEDED -3

namespace Grasp {

	typedef enum { unknown = 0, VTOV, VTOK, KTOK, VTOVK, VKTOVK, QUITVR, DEMO } Paradigm;
	static char *GraspParadigmString[] = { "????", "VtoV", "VtoK", "KtoK", "VtoVK", "VKtoVK", "QUITVR", "DEMO" };

	typedef enum { NullState = 0, StartBlock = 10, BlockCompleted = 19, 
					StartTrial = 20, 
					StraightenHead = 31, AlignHead = 32, 
					PresentTarget = 33, TiltHead = 34, ObtainResponse = 35, 
					ProvideFeedback = 36, TrialCompleted = 39, 
					TrialInterrupted = 40, VRCompleted = 50, Demo = 98, ExitStateMachine = 99 } GraspTrialState;

	typedef enum { RAISE_HAND_TIMEOUT = 1, LOWER_HAND_TIMEOUT, RAISED_HAND_VIOLATION, HAND_TOO_SOON, ALIGN_HAND_TIMEOUT, HEAD_ALIGNMENT_TIMEOUT, HEAD_TILT_TIMEOUT, HEAD_MISALIGNMENT, RESPONSE_TIMEOUT, MANUAL_REJECT_TRIAL, INVALIDATE_TRIAL } Anomaly;
	typedef enum { MANUAL_STRAIGHTEN, CODA_STRAIGHTEN, CHEST_STRAIGHTEN } StraightenHeadMethod;

	typedef struct {

		char ID[8];

		unsigned long enableBits;
		unsigned long spinnerBits;

		CompactPose	headPose;
		CompactPose	handPose;
		CompactPose	chestPose;
		float rollQuaternion[4];

		short	targetHeadTiltD;
		short	targetOrientationD;
		short	responseHeadTiltD;
		short	currentTrial;
		float	conflictGain;

		Paradigm		paradigm;
		GraspTrialState	currentState;
		bool	provideFeedback;

	} GraspClientData;

}
