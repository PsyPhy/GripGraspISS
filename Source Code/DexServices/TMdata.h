/*!
*******************************************************************************
* \file TMdata.h
* \brief Data types for GRASP dummy TM application
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
#include "../VectorsMixin/VectorsMixin.h"


typedef unsigned short u16;
typedef signed short s16;
typedef unsigned u32;
typedef signed s32;
typedef unsigned char u8;
typedef float f32;
typedef char sz;

class GraspPacket : public PsyPhy::VectorsMixin
{
  public:
    virtual u32 serialize(u8 *buffer) const = 0;
    virtual u16 getPktType() const = 0;
  protected:
    u32 addHeader(u8 *buffer) const ;

};

class HK_packet : public GraspPacket
{
    public:
        virtual u32 serialize(u8 *buffer) const;
        virtual u16 getPktType() const;

        u16 current_user;
        u16 current_protocol;
        u16 current_task;
        u16 current_step;
        u16 scriptengine_status;
        u16 motiontracker_status;
};

#define GRASP_RT_SLICES_PER_PACKET 5
#define GRASP_RT_SLICE_INTERVAL 0.100
#define GRASP_HK_PACKET_INTERVAL 1.0

class RT_packet : public GraspPacket
{
    public:
        virtual u32 serialize(u8 *buffer) const;
        virtual u16 getPktType() const;

        u32 rtdata_acq_id;
        u32 rtdata_pkt_counter;

		struct {
			float	fillTime;
			u32		globalCount;
			u32		objectStateBits;
			PsyPhy::TrackerPose	hmd;
			PsyPhy::TrackerPose	codaHmd;
			PsyPhy::TrackerPose	hand;
			PsyPhy::TrackerPose	chest;
			PsyPhy::TrackerPose mouse;
			MarkerFrame markerFrame[2];
		} Slice[GRASP_RT_SLICES_PER_PACKET];

};

class CameraTrigger_packet : public GraspPacket
{
    public:
        virtual u32 serialize(u8 *buffer) const;
        virtual u16 getPktType() const;

        sz tag[10];
};

