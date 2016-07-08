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

typedef unsigned short u16;
typedef signed short s16;
typedef unsigned u32;
typedef signed s32;
typedef unsigned char u8;
typedef float f32;
typedef char sz;


class GraspPacket
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


class RT_packet : public GraspPacket
{
    public:
        virtual u32 serialize(u8 *buffer) const;
        virtual void FillPattern();
        virtual u16 getPktType() const;

        u32 rtdata_acq_id;
        u32 rtdata_pkt_counter;

        // DATA_MANIP_POSE subpackets
        u32 manip_pose_tick[10];
        s16 manip_posX[10];
        s16 manip_posY[10];
        s16 manip_posZ[10];
        f32 manip_oriX[10];
        f32 manip_oriY[10];
        f32 manip_oriZ[10];
        f32 manip_oriM[10];
        u32 markers_visib_1[10];
        u32 markers_visib_2[10];
        bool manip_visib[10];

        // DATA_IOC_FTG subpackets
        u32 science_data_tick[10];
        s16 manip_FX_L[10];
        s16 manip_FY_L[10];
        s16 manip_FZ_L[10];
        s16 manip_TX_L[10];
        s16 manip_TY_L[10];
        s16 manip_TZ_L[10];
        s16 manip_FX_R[10];
        s16 manip_FY_R[10];
        s16 manip_FZ_R[10];
        s16 manip_TX_R[10];
        s16 manip_TY_R[10];
        s16 manip_TZ_R[10];
        s32 manip_Low_Acc_X[10];
        s32 manip_Low_Acc_Y[10];
        s32 manip_Low_Acc_Z[10];
};

class CameraTrigger_packet : public GraspPacket
{
    public:
        virtual u32 serialize(u8 *buffer) const;
        virtual u16 getPktType() const;

        sz tag[10];
};

