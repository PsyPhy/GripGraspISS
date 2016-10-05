/*!
*******************************************************************************
* \file TMdata.cpp
* \brief Data operations for GRASP dummy TM application
*
* <CENTER><B>
* QinetiQ Space N.V.</B><BR>
* Hogenakkerhoekstraat 9<BR>
* B-9150 Kruibeke<BR>
* Belgium* +32 3 250 14 14<BR>
* </CENTER>
*
* \par Copyright(c), QinetiQ Space N.V. 2016 :
* <BR>
* The software contained within this file is proprietary and may
* not be copied, modified, compiled or used for whatever purpose.
* People getting hold of this file, must immediately delete it
* from any data carrier or physically destroy all tangible copies
* in their possession.
*
* Note: File under source code control; refer to project under
*  http://svn.orion.local:18080/viewvc/dex/ (web browser) or
*  http://svn.orion.local:18080/svn/dex (SVN client)
* for change history
******************************************************************************/

#include "../Trackers/Trackers.h"
#include "../Trackers/PoseTrackers.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "TMdata.h"

u16 HK_packet::getPktType() const {return 0x3002u;}
u16 RT_packet::getPktType() const {return 0x1001u;}
u16 CameraTrigger_packet::getPktType() const {return 0x8003u;}

PsyPhy::VectorsMixin vm;

static u32 addTM(u16 tm, u8*buffer, u32 pos)
{
    buffer[pos++]=tm>>8;
    buffer[pos++]=tm&0xFF;
    return pos;
}

static u32 addTM(s16 tm, u8*buffer, u32 pos)
{
    return addTM((u16)tm, buffer, pos);
}


static u32 addTM(u32 tm, u8*buffer, u32 pos)
{
    buffer[pos++]=tm>>24;
    buffer[pos++]=(tm>>16)&0xFF;
    buffer[pos++]=(tm>>8)&0xFF;
    buffer[pos++]=tm&0xFF;
    return pos;
}

static u32 addTM(s32 tm, u8*buffer, u32 pos)
{
    return addTM((u32)tm, buffer, pos);
}

static u32 addTM(f32 tm, u8*buffer, u32 pos)
{
    return addTM(*(u32 *)&tm, buffer, pos);
}

static u32 addTM(u8 tm, u8*buffer, u32 pos)
{
    buffer[pos++]=(u8)tm;
    return pos;
}

static u32 addTM(bool tm, u8*buffer, u32 pos)
{
    buffer[pos++]=(u8)tm;
    return pos;
}

static u32 addTM( PsyPhy::TrackerPose pose, u8 *buffer, u32 pos ) {
	// Position information in 1Oths of mm.
	for ( int i = 0; i < 3; i++ ) pos = addTM( (s16) (pose.pose.position[i] * 10.0), buffer, pos );
	// Orientation quaternion as floats.
	for ( int i = 0; i < 4; i++ ) pos = addTM( (f32) pose.pose.orientation[i], buffer, pos );
	// Visibility
	pos = addTM( (u8) pose.visible, buffer, pos );
	// Time stamp
	pos = addTM( (f32) pose.time, buffer, pos );
	return( pos );
}

static u32 addTM( MarkerState m, u8 *buffer, u32 pos ) {
	// Position information in 1Oths of mm.
	for ( int i = 0; i < 3; i++ ) pos = addTM( (s16) ( m.position[i] * 10.0), buffer, pos );
	// Visibility
	pos = addTM( (u8) m.visibility, buffer, pos );
	return( pos );
}

static u32 addTM( MarkerFrame frame, u8 *buffer, u32 pos ) {
	// Position information in 1Oths of mm.
	u32 visibility_bits = 0;
	for ( int mrk = MAX_MARKERS - 1; mrk >= 0; mrk++ ) {
		for ( int i = 0; i < 3; i++ ) pos = addTM( (s16) ( frame.marker[mrk].position[i] * 10.0), buffer, pos );
		if ( frame.marker[mrk].visibility ) visibility_bits = (visibility_bits << 1) | 0x01;
		else  visibility_bits = (visibility_bits << 1);
	}
	// Visibility encoded as bits.
	pos = addTM( visibility_bits, buffer, pos );
	// Timestamp
	pos = addTM( (f32) frame.time, buffer, pos );
	return( pos );
}

u32 GraspPacket::addHeader(u8 *buffer) const
{
    u32 p=0;
    const u32 sync= 0x55AADE21;
    u16 pkttype = getPktType();
    p=addTM(sync,buffer,p);
    p=addTM(pkttype,buffer,p);
    return p;
}


u32 HK_packet::serialize(u8 *buffer) const
{
    u32 p= addHeader(buffer);
    p=addTM(current_user, buffer, p);
    p=addTM(current_protocol, buffer, p);
    p=addTM(current_task, buffer, p);
    p=addTM(current_step, buffer, p);
    p=addTM(scriptengine_status, buffer, p);
    p=addTM(motiontracker_status, buffer, p);
    assert(p==(6+12));
    return p;
}

u32 RT_packet::serialize(u8 *buffer ) const {

    u32 p= addHeader(buffer);
    p=addTM(rtdata_acq_id, buffer, p);
    p=addTM(rtdata_pkt_counter, buffer, p);

	for ( int i = 0; i < RT_SLICES_PER_PACKET; i++ ) {
		p = addTM( Slice[i].fillTime, buffer, p );
		p = addTM( (u32) Slice[i].globalCount, buffer, p );
		p = addTM( (u32) Slice[i].objectStateBits, buffer, p );
		p = addTM( Slice[i].hmd, buffer, p );
		p = addTM( Slice[i].codaHmd, buffer, p );
		p = addTM( Slice[i].hand, buffer, p );
		p = addTM( Slice[i].chest, buffer, p );
		p = addTM( Slice[i].mouse, buffer, p );
		for ( int j = 0; j < 2; j++ ) p = addTM( Slice[i].markerFrame, buffer, p );
	}
	// Fill the rest of the buffer with a constant value.
	while ( p < (6+758) ) p = addTM( (u8) 0xfa, buffer, p );

    assert(p==(6+758));
    return p;
}

u32 CameraTrigger_packet::serialize(u8 *buffer) const
{
    u32 p= addHeader(buffer);
    for(int n=0;n<10;n++)
    {
        buffer[p++] = (u8)tag[n];
    }
    assert(p==(6+10));
    return p;
}

