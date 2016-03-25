/*
 * Maximum number of comonent objects in a given assembly.
 * Assemblies should be implemented as linked lists so as not
 *  to impose a limit, but I'm too lazy.
 */
#define MAX_COMPONENTS  100

/*
 * Maximum number of points in a polygon for structures like an extrusion.
 */
#define MAX_VERTICES 255


#define glTranslatedv( v )  glTranslated( v[X], v[Y], v[Z] )
#define glRotatedv( angle, v )  glRotated( angle, v[X], v[Y], v[Z] )


#ifdef  __cplusplus
  extern "C"
    {
#endif

void glDefaultLighting( void ); // Directional
void glCodaDefaultLighting( void ); 
void glAutoLighting( float intensity );    // Non-Directional
void glDefaultMaterial( void );

#ifdef  __cplusplus
  }
#endif
