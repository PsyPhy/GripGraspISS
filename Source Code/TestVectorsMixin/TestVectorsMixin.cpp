// TestVectorsMixin.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <conio.h>
#include <math.h>

#include "../VectorsMixin/VectorsMixin.h"

#define X 0
#define Y 1
#define Z 2

using namespace PsyPhy;

VectorsMixin vm;

int main(int argc, char *argv[])
{

	double		roll, pitch, yaw;
	Matrix3x3	ROT, ROTt, R, Rt, PY, PYt, noPY, noPYt, CK;
	Vector3		eulers;

	vm.SetRotationMatrix( ROT, 0.0, 20.0, 30.0 );
	vm.SetRotationMatrix( R, ROT[Z], vm.kVector );
	vm.TransposeMatrix( Rt, R );
	vm.TransposeMatrix( ROTt, ROT );
	printf( "%s\n", vm.mstr( ROTt ) );
	printf( "%s\n", vm.mstr( Rt ) );


	//double step = 45.0;

	//for ( double roll = -90.0; roll <= 90.0; roll += step ) {
	//	for ( double pitch = -180.0; pitch <= 180.0; pitch += step ) {
	//		for ( double yaw = -180.0; yaw <= 180.0; yaw += step ) {

	//	vm.SetRotationMatrix( ROT, vm.ToRadians( roll ), vm.ToRadians( pitch ), vm.ToRadians( yaw ) );
	//	vm.SetRotationMatrix( R, vm.ToRadians( roll ), 0.0, 0.0 );
	//	vm.SetRotationMatrix( PY, 0.0,  vm.ToRadians( pitch ), vm.ToRadians( yaw ) );
	//	vm.TransposeMatrix( PYt, PY );
	//	vm.MultiplyMatrices( noPY, PYt, ROT );
	//	vm.TransposeMatrix( noPYt, noPY );
	//	vm.MultiplyMatrices( CK, R, noPYt );

	//	double azimuth = atan2( ROT[Z][X], ROT[Z][Z] );
	//	double elevation= atan2( ROT[Z][Y], sqrt(  ROT[Z][X] * ROT[Z][X] + ROT[Z][Z] * ROT[Z][Z] ) );

	//	//printf( "Yaw:  %7.2f  %7.2f    Pitch: %7.2f  %7.2f\n", yaw, vm.ToDegrees( azimuth ), pitch, vm.ToDegrees( elevation ) );
	//	//printf( "Rot:        %s\n", vm.mstr( ROT ) );
	//	//printf( "Roll:       %s\n", vm.mstr( R ) );
	//	//printf( "PitchYaw:   %s\n", vm.mstr( PY ) );
	//	//printf( "noPitchYaw: %s\n", vm.mstr( noPY ) );
	//	//printf( "CK:         %s\n", vm.mstr( CK ) );

	// double retrieved_roll = vm.RollAngleFromMatrix( ROT );

	//	printf( "%7.2f   %7.2f ?=? %7.2f %7.2f %7.2f  %s\n", vm.ToDegrees( retrieved_roll ) - roll, vm.ToDegrees( retrieved_roll ), roll, pitch, yaw,  vm.mstr( ROT ) );

	//		}
	//	}
	//}

	getch();
}

