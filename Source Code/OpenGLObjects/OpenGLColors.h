// Couleurs : les defaults sont BLACK pour glClear et WHITE pour glColor

#pragma once

namespace PsyPhy {

enum OPENGL_COLORS {BLACK = 0, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, ORANGE, GRAY, WHITE, COLORS };
#define Translucid( color ) ( color + COLORS )
#define SelectColor( index ) ( ( index % 7 ) + 1 )

extern GLfloat OpenGLColors[][4];

};
