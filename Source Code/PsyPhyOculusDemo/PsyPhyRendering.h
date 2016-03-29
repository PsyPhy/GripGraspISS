#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OculusInterface/OculusViewpoint.h"
#include "../Trackers/PoseTrackers.h"

extern PsyPhy::OculusViewpoint *viewpoint;
extern PsyPhy::Assembly	*tool;
extern PsyPhy::Assembly *room;
void CreatePsyPhyObjects ( void );
void DrawPsyPhyObjects ( int eye, PsyPhy::PoseTracker *headTracker );