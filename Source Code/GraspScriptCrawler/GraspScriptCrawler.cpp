// GraspScriptCrawler.cpp : Defines the entry point for the console application.
//

#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include "../Useful/fMessageBox.h"
#include "../Useful/fOutputDebugString.h"

FILE *log;
int error_count = 0;

char *delimiters = " ;,";
void ParseFile( const char *filename ) {

	FILE *fp = fopen( filename, "r" );
	fAbortMessageOnCondition( !fp, "GraspScriptCrawler", "Could not open file %s for read.", filename );

	char line[1024];
	char *ptr;
	while (  fgets( line, sizeof( line ), fp ) ) {
		ptr = strtok( line, delimiters );
		while ( ptr ) {
			// Skip lines that start with # as comments.
			if ( *ptr == '#' ) break;
			else { 
				fOutputDebugString( "Token: %s\n", ptr );
				if ( strlen( ptr ) > 5 ) {
					if ( !strncmp( ptr + strlen( ptr ) - 5, ".html", 5 ) ) fOutputDebugString( "HTML file: %s\n", ptr );
				}
				else if ( strlen( ptr ) > 4 ) {
					if ( !strncmp( ptr + strlen( ptr ) - 4, ".jpg", 4 ) ) fOutputDebugString( "JPEG file: %s\n", ptr );
					if ( !strncmp( ptr + strlen( ptr ) - 4, ".bmp", 4 ) ) fOutputDebugString( "BMP  file: %s\n", ptr );
					if ( !strncmp( ptr + strlen( ptr ) - 4, ".sss", 4 ) ) {
						fOutputDebugString( "Session  file: %s\n", ptr );
						ParseFile( ptr );
					}
					if ( !strncmp( ptr + strlen( ptr ) - 4, ".pcl", 4 ) ) {
						fOutputDebugString( "Protocol  file: %s\n", ptr );
						ParseFile( ptr );
					}
					if ( !strncmp( ptr + strlen( ptr ) - 4, ".tsk", 4 ) ) {
						fOutputDebugString( "Task  file: %s\n", ptr );
						ParseFile( ptr );
					}
				}
			}
			ptr = strtok( NULL, delimiters );
		}
	}
	fclose( fp );
}

int main(int argc, char *argv[])
{

	SYSTEMTIME  st;
	
	fAbortMessageOnCondition( argc < 2, "GraspScriptCrawler", "Missing required subject filename." );
	char *rootfile = argv[1];

	char *logfilename = "GraspScriptCrawler.log";
	log = fopen( logfilename, "w" );
	fAbortMessageOnCondition( !log, "GraspScriptCrawler", "Could not open file %s for write.", logfilename );
    GetSystemTime( &st );
	fprintf( log, "GraspScriptCrawler %04d-%02d-%02d %02d:%02d:%02d\n", 
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

	ParseFile( rootfile );

	fclose( log );

	fAbortMessageOnCondition ( error_count != 0, "GraspScriptCrawler", "Script crawler detected errors.\nSee %s for details.", logfilename );
	fprintf( stderr, "GraspScriptCrawler: Grasp script tree %s processed normally.\n", rootfile );
	return( 0 );
}



