// Couleurs : les defaults sont BLACK pour glClear et WHITE pour glColor

#pragma once

#define USE_PARENT_COLOR  -999.0
#define PARENT_COLOR USE_PARENT_COLOR, USE_PARENT_COLOR, USE_PARENT_COLOR, USE_PARENT_COLOR

namespace PsyPhy {

enum OPENGL_COLORS {BLACK = 0, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, ORANGE, GRAY, WHITE, COLORS };
#define Translucid( color ) ( color + COLORS )
#define Blinking( color ) ( color + 2*COLORS )
#define SelectColor( index ) ( ( index % 7 ) + 1 )

extern GLfloat OpenGLColors[][4];

};
