
/*********************************************************************************/
/*                                                                               */
/*                                   VectorsMixin.h                              */
/*                                                                               */
/*********************************************************************************/

// Suport routines for vector and matrix operations.
// Vectors here are 3D. Quaternions are 4D.
#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "../Useful/Useful.h"

// I am also putting here support for calculations on 3D rigid bodies.
// It should probably be a separate class, but I will deal with that later.

#define MAX_RIGID_BODY_MARKERS	256


namespace PsyPhy {


typedef struct {
	Vector3	position;
	Quaternion	orientation;
} Pose;

// A pose that takes less memory, at the cost of precision.
typedef struct {
	short		position[4];
	fQuaternion	orientation;
} CompactPose;

typedef struct {
	Vector3	displacement;
	Quaternion	rotation;
} Transform;


class VectorsMixin {

protected:	
	
public:

	static const Vector3 zeroVector;
	static const Quaternion nullQuaternion;
	static const Matrix3x3 identityMatrix;
	static const Matrix3x3 zeroMatrix;
	static const Pose nullPose;

	static const Vector3 missingVector;
	static const Quaternion missingQuaternion;
	static const Pose missingPose;

	static const Vector3 iVector;
	static const Vector3 jVector;
	static const Vector3 kVector;
	static const double  canonicalVector[6][3];

	static const Vector3 iVectorMinus;
	static const Vector3 jVectorMinus;
	static const Vector3 kVectorMinus;

	static const double pi;
	static const double epsilon;
	double ToDegrees( double radians );
	double ToRadians( double degrees );

	void CopyVector( Vector3  destination, const Vector3  source );
	void CopyVector( Vector3f  destination, const Vector3  source );
	void CopyVector( Vector3  destination, const Vector3f source );
	void CopyVector( Vector3f  destination, const Vector3f  source );

	void CopyQuaternion( Quaternion destination, const Quaternion source );
	void CopyQuaternion( fQuaternion destination, const Quaternion source );
	void CopyQuaternion( Quaternion destination, const fQuaternion source );
	void CopyQuaternion( fQuaternion destination, const fQuaternion source );

	void CopyPose( Pose &destination, const Pose &source );
	void CopyPose( CompactPose &destination, const Pose &source );
	void CopyPose( Pose &destination, const CompactPose &source );

	void AddVectors( Vector3f result, const Vector3 a, const Vector3 b );
	void AddVectors( Vector3f result, const Vector3f a, const Vector3f b );
	void AddVectors( Vector3 result, const Vector3f a, const Vector3f b );
	void AddVectors( Vector3 result, const Vector3 a, const Vector3 b );
	void AddVectors( Vector3 result, const Vector3f a, const Vector3 b );
	void AddVectors( Vector3 result, const Vector3 a, const Vector3f b );

	void SubtractVectors( Vector3 result, const Vector3 a, const Vector3 b );
	void SubtractVectors( Vector3f result, const Vector3 a, const Vector3 b );
	void SubtractVectors( Vector3 result, const Vector3f a, const Vector3f b );
	void SubtractVectors( Vector3 result, const Vector3f a, const Vector3 b );
	void SubtractVectors( Vector3f result, const Vector3f a, const Vector3f b );

	void ScaleVector( Vector3 result, const Vector3 a, const double scaling );
	void ScaleVector( Vector3f result, const Vector3 a, const double scaling );
	void ScaleVector( Vector3f result, const Vector3f a, const double scaling );

	double VectorNorm( const Vector3 vector );
	void   NormalizeVector( Vector3 v );
	double DotProduct( const Vector3 v1, const Vector3 v2 );
	double AngleBetweenVectors( const Vector3 v1, const Vector3 v2 );
	void   ComputeCrossProduct( Vector3 result, const Vector3 v1, const Vector3 v2 );

	void CopyMatrix( Matrix3x3 destination, const Matrix3x3 source );
	void CopyMatrix( fMatrix3x3 destination, const Matrix3x3 source );
	void CopyMatrix( Matrix3x3 destination, const fMatrix3x3 source );
	void CopyMatrix( fMatrix3x3 destination, const fMatrix3x3 source );

	void TransposeMatrix( Matrix3x3 destination, const Matrix3x3 source );
	void TransposeMatrix( Matrix3x3 destination, const fMatrix3x3 source );
	void ScaleMatrix( Matrix3x3 destination, const Matrix3x3 source, const double scaling );
	void MultiplyMatrices( Matrix3x3 result, const Matrix3x3 left, const Matrix3x3 right );

	double Determinant( const	Matrix3x3 m );
	double InvertMatrix( Matrix3x3 result, const Matrix3x3 m );
	void OrthonormalizeMatrix( Matrix3x3 result, Matrix3x3 m );

	void MultiplyVector( Vector3 result, const Vector3 v, const Matrix3x3 m );
	void MultiplyVector( Vector3 result, const Vector3f v, const Matrix3x3 m );
	void MultiplyVector( Vector3f result, const Vector3f v, const Matrix3x3 m );
	void MultiplyVector( Vector3f result, const Vector3 v, const Matrix3x3 m );

	void CrossVectors( Matrix3x3 result, const Vector3 left[], const Vector3 right[], int rows );
	void BestFitTransformation( Matrix3x3 result, const Vector3 input[], const Vector3 output[], int rows );
		
	void SetQuaternion( Quaternion result, double radians, const Vector3 axis );
	void SetQuaterniond( Quaternion result, double degrees, const Vector3 axis );
	void NormalizeQuaternion( Quaternion q );
	void MultiplyQuaternions( Quaternion result, const Quaternion q1, const Quaternion q2 );
	double QuaternionDifference(  Quaternion result, const Quaternion q1, const Quaternion q2 );
	void ComputeQuaternionConjugate( Quaternion conjugate, Quaternion q );
	double AngleBetweenOrientations( const Quaternion q1, const Quaternion q2 );
	double RotationAngle( const Quaternion q1 );
	double RollAngle( const Quaternion q1 );

	void SetRotationMatrix( Matrix3x3 result, double radians, const Vector3 axis );
	// Compute a rotation matrix that will align v1 with v2, ignoring the roll around the vectors.
	void SetRotationMatrix( Matrix3x3 result, const Vector3 v2, const Vector3 v1 );
	// Compute a rotation matrix from Euler angles. Angles are in radians.
	void SetRotationMatrix( Matrix3x3 result, double roll, double pitch, double yaw );
	// Compute the final roll angle of a rotation, ignoring pitch and yaw.
	double RollAngleFromMatrix( const Matrix3x3 m );

	void RotateVector( Vector3 result, const Quaternion q, const Vector3 v );
	void MatrixToQuaternion( Quaternion result, Matrix3x3 m );
	void QuaternionToMatrix( Matrix3x3 result, const Quaternion q );

	void QuaternionToCannonicalRotations( Vector3 rotations, Quaternion q );

	bool ComputeRigidBodyPose( Vector3 position, Quaternion orientation,
								Vector3 model[], Vector3 actual[], 
								int N, const Quaternion default_orientation );

	void TransformPose( Pose &result, Transform &xform, Pose &source );

	char *vstr( const Vector3 v, const char *format = nullptr );
	char *qstr( const Quaternion q, const char *format = nullptr );
	char *mstr( const Matrix3x3 m, const char *format = nullptr );
	
	char *vstr( const fVector3 v, const char *format = nullptr );
	char *qstr( const fQuaternion q, const char *format = nullptr );
	char *mstr( const fMatrix3x3 m, const char *format = nullptr );

	bool ReadVector( Vector3 v, FILE *fid ) {
		return( 3 == fscanf( fid, " < %lf %lf %lf>", &v[X], &v[Y], &v[Z] ) );
	}
	bool ReadQuaternion( Quaternion q, FILE *fid ) {
		return( 4 == fscanf( fid, " {%lfi %lfj %lfk %lf}", &q[X], &q[Y], &q[Z], &q[M] ) ); 
	}
	bool ReadMatrix( Matrix3x3 m, FILE *fid ) {
		return( 9 == fscanf( fid, " [%lfi %lfj %lfk | %lfi %lfj %lfk | %lfi %lfj %lfk ]", 
			&m[X][X], &m[Y][X],&m[Z][X], &m[X][Y], &m[Y][Y],&m[Z][Y],&m[X][Z], &m[Y][Z],&m[Z][Z] ) ); 
	}

	char *defaultVectorFormatString;
	char *defaultQuaternionFormatString;
	char *defaultMatrixFormatString;

	VectorsMixin( void ) {
		defaultVectorFormatString = "<%+8.3f %+8.3f %+8.3f>";
		defaultQuaternionFormatString = "{%6.3fi %+6.3fj %+6.3fk %+6.3f}";
		defaultMatrixFormatString = "[%8.3f %8.3f %8.3f | %8.3f %8.3f %8.3f | %8.3f %8.3f %8.3f ]";
	}
};

// Create a global inherit VectorsMixin to be used outside of classes
// that inherit VectorsMixin. Incorporating VectorsMixin into your 
// class is preferred, because it allows you to override functions and
// to have your own state for the vector processing apparatus. But
// managed classes don't allow you to mix in an unmanaged class and in other
// cases you may wish to use the functionality that is part of a global
// function that is not associated with any class.
extern VectorsMixin _vm;

}