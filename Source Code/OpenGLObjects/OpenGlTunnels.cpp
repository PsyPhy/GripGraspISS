//---------------------------------------------------------------------------

/********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <windows.h>
#include <mmsystem.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include "../Useful/Useful.h"

#include "OpenGLObjects.h"
#include "OpenGLColors.h"
#include "OpenGLUseful.h"

#include "OpenGlTunnels.h"

// Couloir
#define RSEG    10.0            // Laisser le ".0" pour forcer le type float
#define LSEG    66.0
#define NSLICES 16
#define NSTACKS 16
#define SREP    1
#define TREP    1               // Par segment

// Deplacement
#define ANTICIPATION    16      // Comparable à LSEG

/******************************************************************************************/

/////////////////////////
// Cylinder's definition
/////////////////////////

TunnelSegment::TunnelSegment( float length, float radius ) {

  this->length = length;
  this->radius = radius;

  OpenGLObject();   // Do what every OpenGlObject does at creation.

}


void TunnelSegment::Draw( void )
{
    GLfloat x, y, z, new_z;
    GLfloat nX, nY;
    GLfloat S, T, Tnew;

    int stack, slice;
    float teta;

    if ( ! enabled ) return;
    PrepDraw();

    if ( list < 0 ) {

      glNewList( list = 1, GL_COMPILE );

        z = 0;
        T = 0;
        for( stack=0 ; stack < NSTACKS ; stack++ )
        {
          Tnew =  T + (double) SREP / (double) NSTACKS;
          glBegin( GL_QUAD_STRIP );

            S = 0;
            new_z = z + length / NSTACKS;

            for( teta = 0, slice = 0 ; slice <= NSLICES ; slice++ )
            { 
              nX = cos( teta );
              nY = sin( teta );

              x = radius * cos( teta );
              y = radius * sin( teta );

              glNormal3f( nX, nY, 0.0 );
              glTexCoord2f( S, T );
              glVertex3f( x, y, z );

              glNormal3f( nX, nY, 0.0 );
              glTexCoord2f( S, Tnew );
              glVertex3f( x, y, new_z );        // 1er segment est -z

              teta += 2.0 * pi / (double) NSLICES;
              S += (double) SREP / (double) NSLICES;

            }
          glEnd();

          z = new_z;
          T =Tnew;
        }
      glEndList();
    }
    else glCallList( list );


    FinishDraw();

}

/******************************************************************************************/

/////////////////////////
// Junction's definition
/////////////////////////

void TunnelJunction::Draw( void )
{

    GLfloat *T;
    GLfloat dT;
    GLfloat Xold[NSLICES+1], Yold[NSLICES+1], Zold[NSLICES+1];
    GLfloat x, y, z;
    GLfloat nXold[NSLICES+1], nYold[NSLICES+1], nZold[NSLICES+1],
            nX, nY, nZ;
    GLfloat S, Tnew;

    int stack, slice;
    float teta, phi;

    if ( ! enabled ) return;
    PrepDraw();

    // Initialisation des Xolds (premier cercle de pts, phi=0)
    for(teta=0, slice=0 ; slice<=NSLICES ; slice++)
    {
      nXold[slice] = sin( teta );
      nYold[slice] = - cos( teta );
      nZold[slice] = 0;

      Xold[slice] = RSEG * sin( teta );
      Yold[slice] = - RSEG * cos( teta );
      Zold[slice] = 0;

      teta += 2.0 * pi / NSLICES;
    }

    phi = pi / 2.0 / NSTACKS;

    for(stack=0 ; stack<NSTACKS ; stack++)
    {
      glBegin(GL_QUAD_STRIP);
        Tnew = *T + dT * pi * RSEG / NSTACKS;

        S = 0;
        for( teta=0, slice=0; slice <= NSLICES ; slice++ )
        {
          nX = sin(teta);
          nY = - cos( teta ) * cos( phi );
          nZ = - cos( teta )* sin( phi );

          x = RSEG * sin( teta );
          y = - RSEG * ( cos( phi ) * ( 1.0 + cos(teta) ) - 1.0);
          z = - RSEG * sin( phi ) * ( 1.0 + cos( teta ) );

          glNormal3f( nXold[slice], nYold[slice], nZold[slice] );
          glTexCoord2f(S, *T);
          glVertex3f( Xold[slice], Yold[slice], Zold[slice] );

          glNormal3f( nX, nY, nZ );
          glTexCoord2f( S, Tnew );
          glVertex3f( x, y, z );

          nXold[slice] = nX;
          nYold[slice] = nY;
          nZold[slice] = nZ;

          Xold[slice] = x;
          Yold[slice] = y;
          Zold[slice] = z;

          S += (GLfloat) SREP / NSLICES;
          teta += 2.0 * pi / NSLICES;
        }
      glEnd();
      phi += 90.0/NSTACKS;
      *T = Tnew;
    }
    FinishDraw();
}


/******************************************************************************************/

/////////////////////////
// Cylinder's definition
/////////////////////////

Tunnel::Tunnel( void ) {

  texture = new Texture( "Lime.bmp" );
  OpenGLObject();   // Do what every OpenGlObject does at creation.

}

#if 0

//////////////////////////////////
// Camera's position calculation
//////////////////////////////////
void __fastcall Tunnel::CameraPosition( GLfloat t )
{
    GLfloat N[3];
    GLfloat tmp[3], tmp2[3];
    double r, teta;

    //////////////////
    // Position keys
    //////////////////

    if(t >= PosKeyTime[NextPosKey])
    {
      if(NextPosKey == nPosKey)       // fin d'animation : quite
        return(false);
      else
      {
        LastPosKey++;
        NextPosKey++;
      }
    }

    if(Compare(PosKeyDir[LastPosKey], PosKeyDir[NextPosKey]))
    {
      // Segment de droite
      r = (t - PosKeyTime[LastPosKey])/(PosKeyTime[NextPosKey] - PosKeyTime[LastPosKey]);
      Scal(tmp, (GLfloat) LSEG*r, PosKeyDir[LastPosKey]);
      Add(View, tmp, PosKey[LastPosKey]);
    }
    else
    {
      // Arc de cercle
      teta = M_PI_2 * (t - PosKeyTime[LastPosKey])/(PosKeyTime[NextPosKey] - PosKeyTime[LastPosKey]);

      Scal(tmp, (GLfloat) RSEG*sin(teta), PosKeyDir[LastPosKey]);
      Scal(tmp2, (GLfloat) -RSEG*cos(teta), PosKeyDir[NextPosKey]);
      Add(View, tmp, tmp2);
      Scal(tmp, (GLfloat) RSEG, PosKeyDir[NextPosKey]);
      Add(View, View, tmp);
      Add(View, View, PosKey[LastPosKey]);
    }

    //////////////////
    // Rotation keys
    //////////////////

    if(t >= RotKeyTime[NextRotKey])
    {
      if(NextRotKey == nRotKey)       // fin d'animation : quite
        return(false);
      else
      {
        LastRotKey++;
        NextRotKey++;
      }

      // Initialisations de keyframe au cas ou pas de rotation envisagée
      Affect(ViewDir, RotKeyDir[LastRotKey]);
      Affect(ViewUp, RotKeyUp[LastRotKey]);
    }

    r = (t - RotKeyTime[LastRotKey])/(RotKeyTime[NextRotKey] - RotKeyTime[LastRotKey]);

    if(! Compare(RotKeyUp[LastRotKey], RotKeyUp[NextRotKey]))
      Interpolate(ViewUp, r, RotKeyUp[LastRotKey], RotKeyUp[NextRotKey]);

    if(! Compare(RotKeyDir[LastRotKey], RotKeyDir[NextRotKey]))
      Interpolate(ViewDir, r, RotKeyDir[LastRotKey], RotKeyDir[NextRotKey]);

#ifdef FPS_DEBUG
    frames++;
    if(frames >= 10)
    {
      char str[40];
      sprintf(str, "Navigation 3D (%.2ffps)", 10000/(t-oldt));
      fprintf(fdebug, "\n%.2ffps", 10000/(t-oldt));
//      SetWindowText(GLwin.hWnd, str);
      oldt = t;
      frames = 0;
    }
#endif

    // Calcule Ref à partir de viewdir
    Add(Ref, View, ViewDir);

    return(true);
}


//////////////////////////////////////////
// Creates a direction array
// for MakeKeyFrames() and MakeCouloir()
//////////////////////////////////////////

void __fastcall Tunnel::MakeDir( char *nom, short nseg, GLfloat dir[10][3] )
{
    short i, sig;

    // Pour le bon fonctionnement de la rotation
    dir[0][0] = 0;
    dir[0][1] = -1;
    dir[0][2] = 0;

    for(i=1 ; i<=nseg ; i++)
    {
      sig = (nom[2*(i-1)] == '+')? 1 : -1;
      switch(nom[2*(i-1)+1])
      {
        case 'x' :
          dir[i][0] = 1.0*sig; dir[i][1] = 0; dir[i][2] = 0;
          break;
        case 'y' :
          dir[i][1] = 1.0*sig; dir[i][0] = 0; dir[i][2] = 0;
          break;
        case 'z' :
          dir[i][2] = 1.0*sig; dir[i][0] = 0; dir[i][1] = 0;
          break;
      }
    }
}


///////////////////////////////////////
// KeyFrames for camera's position
///////////////////////////////////////

void __fastcall Tunnel::MakeKeyFrames( char *nom, short nseg, short condition )
{
    short i, k;
    GLfloat tmp[3];
    GLfloat dir[10][3];

    MakeDir(nom, nseg, dir);

    ///////////////////////
    // Position KeyFrames
    ///////////////////////

    // 1st
    PosKey[0][0] = PosKey[0][1] = PosKey[0][2] = 0;
    Affect(PosKeyDir[0], dir[1]);
    PosKeyTime[0] = 0;

    // 2st
    Scal(PosKey[1], LSEG, dir[1]);
    Affect(PosKeyDir[1], dir[1]);
    PosKeyTime[1] = (double) LSEG / SPEED;

    for(k=2, i=2 ; i<=nseg ; i++)
    {
      // k-ieme
      Add(tmp, dir[i-1], dir[i]);
      Scal(tmp, RSEG, tmp);
      Add(PosKey[k], PosKey[k-1], tmp);
      Affect(PosKeyDir[k], dir[i]);
      PosKeyTime[k] = PosKeyTime[k-1] + (double) M_PI_2 * RSEG / SPEED;
      k++;

      // k-ieme (+1)
      Scal(tmp, LSEG, dir[i]);
      Add(PosKey[k], PosKey[k-1], tmp);
      Affect(PosKeyDir[k], dir[i]);
      PosKeyTime[k] = PosKeyTime[k-1] + (double) LSEG / SPEED;
      k++;
    }
    nPosKey = k-1;
    LastPosKey = 0;
    NextPosKey = 1;


    ////////////////////////////////////////////////
    // Rotation KeyFrames, fonction des conditions
    ////////////////////////////////////////////////

    if(condition == 1)
    {
      // 1st
      Affect(RotKeyDir[0], dir[1]);
      RotKeyUp[0][0] = RotKeyUp[0][2] = 0; RotKeyUp[0][1] = 1;
      RotKeyTime[0] = PosKeyTime[0];

      for(k=1, i=1 ; i<nseg ; )
      {
        // k-ieme
        Affect(RotKeyDir[k], dir[i]);
        Affect(RotKeyUp[k], RotKeyUp[k-1]);    // Meme que précédement
        RotKeyTime[k] = PosKeyTime[k] - (double) ANTICIPATION/SPEED;
        k++; i++;

        // k-ieme +1
        Affect(RotKeyDir[k], dir[i]);
        Cross(tmp, RotKeyDir[k-1], RotKeyDir[k]);
        if(CompareAbs(tmp, RotKeyUp[k-1]))         // Si rotation autour de RotKeyUp
          Affect(RotKeyUp[k], RotKeyUp[k-1]);
        else                                    // Sinon applique la rotation à RotKeyUp
          Cross(RotKeyUp[k], tmp, RotKeyUp[k-1]);

        RotKeyTime[k] = PosKeyTime[k];
        k++;
      }
      // dernier
      Affect(RotKeyDir[k], dir[i]);
      Affect(RotKeyUp[k], RotKeyUp[k-1]);    // Meme que précédement
      RotKeyTime[k] = PosKeyTime[k];

      nRotKey = k;
      LastRotKey = 0;
      NextRotKey = 1;
    }

    else if(condition == 2)
    {
      // 1st
      Affect(RotKeyDir[0], dir[1]);
      RotKeyUp[0][0] = RotKeyUp[0][2] = 0; RotKeyUp[0][1] = 1;
      RotKeyTime[0] = PosKeyTime[0];

      for(k=1, i=1 ; i<nseg ; )
      {
        // k-ieme
        Affect(RotKeyDir[k], dir[i]);
        Affect(RotKeyUp[k], RotKeyUp[k-1]);    // Meme que précédement
        RotKeyTime[k] = PosKeyTime[k] - (double) ANTICIPATION/SPEED;
        k++; i++;

        // k-ieme +1
        Affect(RotKeyDir[k], dir[i]);
        Cross(tmp, RotKeyDir[k-1], RotKeyDir[k]);
        if(CompareAbs(tmp, RotKeyUp[k-1]))         // Si rotation autour de RotKeyUp
          Affect(RotKeyUp[k], RotKeyUp[0]);
        else                                       // Sinon applique la rotation à RotKeyUp
          Cross(RotKeyUp[k], tmp, RotKeyUp[k-1]);

        RotKeyTime[k] = PosKeyTime[k];
        k++;
      }
      // dernier
      Affect(RotKeyDir[k], dir[i]);
      Affect(RotKeyUp[k], RotKeyUp[k-1]);    // Meme que précédement
      RotKeyTime[k] = PosKeyTime[k];

      nRotKey = k;
      LastRotKey = 0;
      NextRotKey = 1;
    }

    else if(condition == 3)
    {
      // 1st
      Affect(RotKeyDir[0], dir[1]);
      RotKeyUp[0][0] = RotKeyUp[0][2] = 0; RotKeyUp[0][1] = 1;
      RotKeyTime[0] = PosKeyTime[0];

      for(k=1, i=1 ; i<nseg ; )
      {
        // k-ieme
        Affect(RotKeyDir[k], RotKeyDir[k-1]);
        Affect(RotKeyUp[k], RotKeyUp[0]);    // Meme que précédement
        RotKeyTime[k] = PosKeyTime[k] - (double) ANTICIPATION/SPEED;
        k++; i++;

        // k-ieme +1
        if(dir[i][1] == 1 || dir[i][1] == -1)   // si segment vertical
        {
          if(i+1 <= nseg)
            Affect(RotKeyDir[k], dir[i+1]);
          else
            Affect(RotKeyDir[k], RotKeyDir[k-1]);
        }
        else
          Affect(RotKeyDir[k], dir[i]);

        Affect(RotKeyUp[k], RotKeyUp[0]);
        RotKeyTime[k] = PosKeyTime[k];
        k++;
      }
      // dernier
      if(dir[i][1] == 1 || dir[i][1] == -1)   // si segment vertical
        Affect(RotKeyDir[k], RotKeyDir[k-1]);
      else
        Affect(RotKeyDir[k], dir[i]);
      Affect(RotKeyUp[k], RotKeyUp[0]);    // Meme que précédement
      RotKeyTime[k] = PosKeyTime[k];

      nRotKey = k;
      LastRotKey = 0;
      NextRotKey = 1;
    }
}

#endif


/////////////////////////
// Couloir's definition
/////////////////////////
#if 0
void __fastcall Tunnel::MakeCouloir( GLuint listNb, char *nom )
{
    short i;
    GLfloat prod[3];
    GLfloat T, dT;
    GLfloat dir[10][3];
    short nseg = strlen(nom)/2;

//    MakeDir(nom, nseg, dir);

    T = 0;
    dT = (GLfloat) (TREP / (LSEG + (1.0 - 1.0/nseg) * PI * RSEG ));

    ///////////////////////
    // Couloir's list
    ///////////////////////

    glNewList(listNb, GL_COMPILE);
      // Cylindre 1
      MakeCylinder(&T, dT);
      glTranslatef(0, 0, -LSEG);

      for(i=2 ; i<=nseg ; i++)
      {
        // Rotation initiale pour s'aligner avec la definition de la jonction
        Cross(prod, dir[i-2], dir[i]);
        if(Compare(prod, Vnul))
        {
          if(Compare(dir[i-2], dir[i])) glRotatef(180, 0, 0, 1);
        }
        else
        {
          if(Compare(prod, dir[i-1])) glRotatef(90, 0, 0, 1);
          else glRotatef(-90, 0, 0, 1);
        }

        // Jonction i
        MakeJunction(&T, dT);

        // Translation & rotation
        glTranslatef(0, RSEG, -RSEG);   // remise en position
        glRotatef(90, 1, 0, 0);         // rotation d'axe x

        // Cylindre i+1
        MakeCylinder(&T, dT);

        // Translation
        glTranslatef(0, 0, -LSEG);
      }
    glEndList();
}
#endif

