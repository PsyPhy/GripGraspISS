#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OculusInterface/OculusViewpoint.h"

extern PsyPhy::OculusViewpoint *viewpoint;
extern PsyPhy::Assembly	*tool;
extern PsyPhy::Assembly *room;
void CreateObjects ( void );
void DrawObjects ( int eye );