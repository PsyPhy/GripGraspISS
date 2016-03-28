#pragma once
#define _USE_MATH_DEFINES
#include <math.h>


#ifdef __cplusplus
namespace PsyPhy {
#endif

// Useful constants for accessing components of 
//  vectors and quaternions.
#define X	0
#define Y	1
#define Z	2
#define M	3

#define ROLL	0
#define PITCH	1
#define YAW		2


#define MISSING_DOUBLE	999999.999999
#define MISSING_FLOAT	999999.999999f
#define MISSING_CHAR	127
#define NaN				-9999999999999.9999999999999

#define Pi	M_PI

#define UNDEFINED		-1

// Some useful constants.

#define NORMAL_EXIT 0
#define ESCAPE_EXIT 1
#define IGNORE_EXIT 2
#define ERROR_EXIT 3
#define ABORT_EXIT 4
#define RETRY_EXIT 5

#ifdef __cplusplus
};
#endif
