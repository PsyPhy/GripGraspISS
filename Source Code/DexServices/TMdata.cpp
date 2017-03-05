
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

    u32 p = addHeader(buffer);
    p = addTM( rtdata_acq_id, buffer, p );
    p = addTM( rtdata_pkt_counter, buffer, p );

	for ( int i = 0; i < GRASP_RT_SLICES_PER_PACKET; i++ ) {
		p = addTM( Slice[i].fillTime, buffer, p );
		p = addTM( (u32) Slice[i].globalCount, buffer, p );
		p = addTM( (u32) Slice[i].objectStateBits, buffer, p );
		p = addTM( Slice[i].hmd, buffer, p );
		p = addTM( Slice[i].codaHmd, buffer, p );
		p = addTM( Slice[i].hand, buffer, p );
		p = addTM( Slice[i].chest, buffer, p );
		p = addTM( Slice[i].mouse, buffer, p );
		// There is a bug in the following line. Each slice has two
		// marker frames, so one should specify Slice[i].markerFrame[j].
		// But the [j] is missing. This caused addTM to interpret the first
		// parameter as a bool and placed only one byte into the stream, instead
		// of copying the entire markerFrame. I need to fix this, but I leave it
		// like this for now to stay consistent with the flight model.
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

u32 extractTM(u16 &tm, u8 *buffer, u32 pos)
{
	tm = buffer[pos] << 8 | buffer[pos+1];
    return ( pos + 2 );
}

u32 extractTM(s16 &tm, u8 *buffer, u32 pos)
{
 	tm = buffer[pos] << 8 | buffer[pos+1];
    return ( pos + 2 );
}

u32 extractTM(u32 &tm, u8 *buffer, u32 pos)
{
	tm = buffer[pos] << 24 | buffer[pos+1] << 16 | buffer[pos+2] << 8 | buffer[pos+3];
    return ( pos + 4 );
}

u32 extractTM(s32 &tm, u8 *buffer, u32 pos)
{
	tm = buffer[pos] << 24 | buffer[pos+1] << 16 | buffer[pos+2] << 8 | buffer[pos+3];
    return ( pos + 4 );
}

u32 extractTM(f32 &tm, u8 *buffer, u32 pos)
{
	union {
		u8 bytes[4];
		f32 value;
	} mix;
	mix.bytes[3] = buffer[pos++];
	mix.bytes[2] = buffer[pos++];
	mix.bytes[1] = buffer[pos++];
	mix.bytes[0] = buffer[pos++];
	tm = mix.value;
    return pos;
}

u32 extractTM( u8 &tm, u8 *buffer, u32 pos)
{
    tm = buffer[pos++];
    return pos;
}

u32 extractTM(bool &tm, u8*buffer, u32 pos)
{
    tm = (bool) buffer[pos++];
    return pos;
}

u32 extractTM( PsyPhy::TrackerPose &pose, u8 *buffer, u32 pos ) {
	short int tenths;
	f32 value;
	// Position information in 1Oths of mm.
	for ( int i = 0; i < 3; i++ ) {
		pos = extractTM( tenths, buffer, pos );
		pose.pose.position[i] = (double) tenths / 10.0;
	}
	// Orientation quaternion as floats.
	for ( int i = 0; i < 4; i++ ) {
		pos = extractTM( value, buffer, pos );
		pose.pose.orientation[i] = value;
	}
	// Visibility
	pos = extractTM( pose.visible, buffer, pos );
	// Time stamp
	pos = extractTM( value, buffer, pos );
	pose.time = value;
	return( pos );
}

u32 extractTM( MarkerState &m, u8 *buffer, u32 pos ) {
	short int tenths;
	// Position information in 1Oths of mm.
	for ( int i = 0; i < 3; i++ ) {
		pos = extractTM( tenths, buffer, pos );
		m.position[i] = (double) tenths / 10.0;
	}
	// Visibility
	pos = extractTM( m.visibility, buffer, pos );
	return( pos );
}

u32 extractTM( MarkerFrame &frame, u8 *buffer, u32 pos ) {
	// Position information in 1Oths of mm.
	short int tenths;
	u32 visibility_bits;
	f32 value;
	for ( int mrk = MAX_MARKERS - 1; mrk >= 0; mrk++ ) {
		for ( int i = 0; i < 3; i++ ) {
			pos = extractTM( tenths, buffer, pos );
			frame.marker[mrk].position[i] = (double) tenths / 10.0;
		}
	}
	pos = extractTM( visibility_bits, buffer, pos );
	for ( int mrk = 0; mrk < MAX_MARKERS; mrk++ ) {
		frame.marker[mrk].visibility = visibility_bits & 0x01;
		visibility_bits = visibility_bits >> 1;
	}
	// Timestamp
	pos = extractTM( value, buffer, pos );
	frame.time = value;
	return( pos );
}
