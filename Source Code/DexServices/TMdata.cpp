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

#include "TMdata.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

u16 HK_packet::getPktType() const {return 0x3002u;}
u16 RT_packet::getPktType() const {return 0x1001u;}
u16 CameraTrigger_packet::getPktType() const {return 0x8003u;}

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

static u32 addTM(bool tm, u8*buffer, u32 pos)
{
    buffer[pos++]=(u8)tm;
    return pos;
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

u32 RT_packet::serialize(u8 *buffer) const
{
    u32 p= addHeader(buffer);
    p=addTM(rtdata_acq_id, buffer, p);
    p=addTM(rtdata_pkt_counter, buffer, p);

    for(u32 i=0;i<10;i++)
    {
        p=addTM(manip_pose_tick[i], buffer, p);
        p=addTM(manip_posX[i], buffer, p);
        p=addTM(manip_posY[i], buffer, p);
        p=addTM(manip_posZ[i], buffer, p);
        p=addTM(manip_oriX[i], buffer, p);
        p=addTM(manip_oriY[i], buffer, p);
        p=addTM(manip_oriZ[i], buffer, p);
        p=addTM(manip_oriM[i], buffer, p);
        p=addTM(markers_visib_1[i], buffer, p);
        p=addTM(markers_visib_2[i], buffer, p);
        p=addTM(manip_visib[i], buffer, p);
        p=addTM(science_data_tick[i], buffer, p);
        p=addTM(manip_FX_L[i], buffer, p);
        p=addTM(manip_FY_L[i], buffer, p);
        p=addTM(manip_FZ_L[i], buffer, p);
        p=addTM(manip_TX_L[i], buffer, p);
        p=addTM(manip_TY_L[i], buffer, p);
        p=addTM(manip_TZ_L[i], buffer, p);
        p=addTM(manip_FX_R[i], buffer, p);
        p=addTM(manip_FY_R[i], buffer, p);
        p=addTM(manip_FZ_R[i], buffer, p);
        p=addTM(manip_TX_R[i], buffer, p);
        p=addTM(manip_TY_R[i], buffer, p);
        p=addTM(manip_TZ_R[i], buffer, p);
        p=addTM(manip_Low_Acc_X[i], buffer, p);
        p=addTM(manip_Low_Acc_Y[i], buffer, p);
        p=addTM(manip_Low_Acc_Z[i], buffer, p);
    }

    assert(p==(6+758));
    return p;
}

void RT_packet::FillPattern()
{
    static u32 cnt=0;

    rtdata_acq_id = ++cnt;
    rtdata_pkt_counter = ++cnt;

    for(u32 i=0;i<10;i++)
    {
        manip_pose_tick[i] = ++cnt;
        manip_posX[i] = ++cnt;
        manip_posY[i] = ++cnt;
        manip_posZ[i] = ++cnt;
        manip_oriX[i] = (++cnt)/10000.0f;
        manip_oriY[i] = (++cnt)/10000.0f;
        manip_oriZ[i] = (++cnt)/10000.0f;
        manip_oriM[i] = (++cnt)/10000.0f;
        markers_visib_1[i] = ++cnt;
        markers_visib_2[i] = ++cnt;
        manip_visib[i] = (i%2)==0;
        science_data_tick[i] = ++cnt;
        manip_FX_L[i] = ++cnt;
        manip_FY_L[i] = ++cnt;
        manip_FZ_L[i] = ++cnt;
        manip_TX_L[i] = ++cnt;
        manip_TY_L[i] = ++cnt;
        manip_TZ_L[i] = ++cnt;
        manip_FX_R[i] = ++cnt;
        manip_FY_R[i] = ++cnt;
        manip_FZ_R[i] = ++cnt;
        manip_TX_R[i] = ++cnt;
        manip_TY_R[i] = ++cnt;
        manip_TZ_R[i] = ++cnt;
        manip_Low_Acc_X[i] = ++cnt;
        manip_Low_Acc_Y[i] = ++cnt;
        manip_Low_Acc_Z[i] = ++cnt;
    }
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

