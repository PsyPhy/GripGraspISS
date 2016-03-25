// Couleurs : les defaults sont BLACK pour glClear et WHITE pour glColor

#ifndef OpenGLColorsH
#define OpenGLColorsH

#ifdef __cplusplus 
extern "C" {
#endif

enum OPENGL_COLORS {BLACK = 0, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, ORANGE, GRAY, WHITE, COLORS };
#define Translucid( color ) ( color + COLORS )
#define SelectColor( index ) ( ( index % 7 ) + 1 )

extern GLfloat OpenGLColors[][4];

#ifdef __cplusplus 
}
#endif

#endif

 