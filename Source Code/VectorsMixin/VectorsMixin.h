
/*********************************************************************************/
/*                                                                               */
/*                                   VectorsMixin.h                              */
/*                                                                               */
/*********************************************************************************/

// Suport routines for vector and matrix operations.
// Vectors here are 3D. Quaternions are 4D.

#pragma once

// #include "../Useful/Useful.h" 

namespace PsyPhy {

// I am also putting here support for calculations on 3D rigid bodies.
// It should probably be a separate class, but I will deal with that later.

#define MAX_RIGID_BODY_MARKERS	256

typedef double Vector3[3];
typedef float  Vector3f[3];
typedef double Quaternion[4];
typedef double Matrix3x3[3][3];
typedef struct {
	Vector3	position;
	Quaternion	orientation;
} Pose;
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

	static const Vector3 iVector;
	static const Vector3 jVector;
	static const Vector3 kVector;

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
	void CopyPose( Pose &destination, const Pose &source );

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
	void   ComputeCrossProduct( Vector3 result, const Vector3 v1, const Vector3 v2 );

	void CopyMatrix( Matrix3x3 destination, const Matrix3x3 source );
	void CopyMatrix( float destination[3][3], const Matrix3x3 source );
	void TransposeMatrix( Matrix3x3 destination, const Matrix3x3 source );
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
	double AngleBetween( const Quaternion q1, const Quaternion q2 );
	
	void SetRotationMatrix( Matrix3x3 result, double radians, const Vector3 axis );
	// Compute a rotation matrix that will align v1 with v2, ignoring the roll around the vectors.
	void SetRotationMatrix( Matrix3x3 result, const Vector3 v2, const Vector3 v1 );

	void RotateVector( Vector3 result, const Quaternion q, const Vector3 v );
	void MatrixToQuaternion( Quaternion result, Matrix3x3 m );
	void QuaternionToMatrix( Matrix3x3 result, const Quaternion q );

	void QuaternionToCannonicalRotations( Vector3 rotations, Quaternion q );

	bool ComputeRigidBodyPose( Vector3 position, Quaternion orientation,
								Vector3 model[], Vector3 actual[], 
								int N, const Quaternion default_orientation );

	void TransformPose( Pose &result, Transform &xform, Pose &source );

	char *vstr( const Vector3 v );
	char *qstr( const Quaternion q );
	char *mstr( const Matrix3x3 m );

};

};