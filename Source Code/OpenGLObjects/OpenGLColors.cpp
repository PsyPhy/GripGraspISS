#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <mmsystem.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include "../Useful/Useful.h"
//#include <3dMatrix.h">

#include "OpenGLObjects.h"
#include "OpenGLColors.h"
#include "OpenGLUseful.h"

GLfloat OpenGLColors[][4] =
        { 
          {0.0f, 0.0f, 0.0f, 1.0},    // BLACK
          {1.0f, 0.0f, 0.0f, 1.0},    // RED
          {0.0f, 1.0f, 0.0f, 1.0},    // GREEN
          {0.0f, 0.0f, 1.0f, 1.0},    // BLUE
          {1.0f, 1.0f, 0.0f, 1.0},    // YELLOW
          {0.0f, 1.0f, 1.0f, 1.0},    // CYAN
          {1.0f, 0.0f, 1.0f, 1.0},    // MAGENTA
          {1.0f, 0.29f, 0.0f, 1.0},    // ORANGE
          {0.75f, 0.75f, 0.75f, 1.0},    // GRAY
          {1.0f, 1.0f, 1.0f, 1.0},    // WHITE

          // Translucid versions

          {0.0f, 0.0f, 0.0f, 0.25},    // BLACK
          {1.0f, 0.0f, 0.0f, 0.25},    // RED
          {0.0f, 1.0f, 0.0f, 0.25},    // GREEN
          {0.0f, 0.0f, 1.0f, 0.25},    // BLUE
          {1.0f, 1.0f, 0.0f, 0.25},    // YELLOW
          {0.0f, 1.0f, 1.0f, 0.25},    // CYAN
          {1.0f, 0.0f, 1.0f, 0.25},    // MAGENTA
          {1.0f, 0.5f, 0.0f, 0.25},    // ORANGE
          {0.5f, 0.5f, 0.5f, 0.25},    // GRAY
          {1.0f, 1.0f, 1.0f, 0.25},    // WHITE

}; 


 