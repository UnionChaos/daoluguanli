#include "GP_protocol.h"
#include "MsgProc.h"
#include "string.h"
static unsigned char Fun_Bcc_Create (unsigned char *p1, unsigned short int len)
{
    unsigned char bcc = 0;
    for(;len > 0; len--)
    {
        bcc ^= *p1++;
    }

    return bcc;
}


static uint8_t __sys_on(uint8_t* buff,uint8_t type,void* param)
{
    GP_Attr attr;
    attr.lenth = 1;
    attr.encrypt = 0;
    attr.trans = 3;
    attr.sep = 0;
    attr.res = 0;
    memcpy(&buff[9],&attr,2);
    buff[11] = type;
    buff[12] = 0x00;
    buff[13] = *(uint8_t*)param;
    return attr.lenth;
} 

static uint8_t __car_on(uint8_t* buff,uint8_t type,void* param)
{
    GP_Attr attr;
    attr.lenth = 0;
    attr.encrypt = 0;
    attr.trans = 3;
    attr.sep = 0;
    attr.res = 0;
    memcpy(&buff[9],&attr,2);
    buff[11] = type;
    buff[12] = 0x00;
    return attr.lenth;
}

static uint8_t __auto_mode(uint8_t* buff,uint8_t type,void* param)
{
    GP_Attr attr;
    attr.lenth = 1;
    attr.encrypt = 0;
    attr.trans = 3;
    attr.sep = 0;
    attr.res = 0;
    memcpy(&buff[9],&attr,2);
    buff[11] = 0x06;
    buff[12] = 0x00;
    buff[13] = *(uint8_t*)param;
    return attr.lenth;
}


static uint8_t __custom_mode(uint8_t* buff,uint8_t type,void* param)
{
    GP_Attr attr;
    attr.lenth = 3;
    attr.encrypt = 0;
    attr.trans = 3;
    attr.sep = 0;
    attr.res = 0;
    memcpy(&buff[9],&attr,2);
    buff[11] = 0x07;
    buff[12] = 0x00;
    memcpy(&buff[13],param,sizeof(custom_mode));
    return attr.lenth;
}

static uint8_t __snap_mode(uint8_t* buff,uint8_t type,void* param)
{
    GP_Attr attr;
    attr.lenth = 3;
    attr.encrypt = 0;
    attr.trans = 3;
    attr.sep = 0;
    attr.res = 0;
    memcpy(&buff[9],&attr,2);
    buff[11] = 0x08;
    buff[12] = 0x00;
    memcpy(&buff[13],param,sizeof(snap_mode));
    return attr.lenth;
}

static uint8_t __state_query(uint8_t* buff,uint8_t type,void* param)
{
    GP_Attr attr;
    attr.lenth = 0;
    attr.encrypt = 0;
    attr.trans = 0;
    attr.sep = 0;
    attr.res = 0;
    memcpy(&buff[9],&attr,2);
    buff[11] = type;
    buff[12] = 0x00;
    return attr.lenth;
}

static uint8_t  __state_prase(uint8_t* buff,uint8_t type,uint8_t lenth, void *out)
{
    point_state *states = NULL;
    states = (point_state*)out;
    states->id = buff[1]|buff[2]<<8|buff[3]<<16|buff[4]<<24;
    memcpy((char *)states+4,buff+13,sizeof(point_state)-4);
    return 1;
}

static uint8_t __broadcast_time(uint8_t* buff,uint8_t type,void* param)
{
    GP_Attr attr;
    attr.lenth = 5;
    attr.encrypt = 0;
    attr.trans = 3;
    attr.sep = 0;
    attr.res = 0;
    memcpy(&buff[9],&attr,2);
    buff[11] = type;
    buff[12] = 0x00;
    memcpy(&buff[13],param,sizeof(gps_time));
    return attr.lenth;
}


GP_map gmap[11]=
{
    {0,NULL,NULL},
    {GP_SET_P_ID,NULL,NULL},
    {GP_SET_PG_ID,NULL,NULL},
    {GP_QUERY_G_ID,NULL,NULL},
    {GP_SET_SYS_ON,__sys_on,NULL},
    {GP_SET_CAR_ON,__car_on,NULL},
    {GP_SET_MODE_AUTO,__auto_mode,NULL},
    {GP_SET_MODE_CUSTOM,__custom_mode,NULL},
    {GP_SET_MODE_SNAP,__snap_mode,NULL},
    {GP_P_STAT,__state_query,__state_prase},
    {GP_BROADCAST_TIME,__broadcast_time,NULL},
};

uint8_t GP_TxPacket(uint8_t* buff,uint8_t type,uint32_t src, uint32_t dst,void *param)
{
    uint8_t lenth = 0;
    buff[0] = 0x68;
    buff[1] = src;
    buff[2] = src>>8;
    buff[3] = src>>16;
    buff[4] = src>>24;
    buff[5] = dst;
    buff[6] = dst>>8;
    buff[7] = dst>>16;
    buff[8] = dst>>24;  
    lenth = gmap[type].packet(buff,type,param);
    buff[13+lenth] = Fun_Bcc_Create(buff+1,12+lenth);
    buff[14+lenth] = 0x16;
    return 15+lenth;
}

uint8_t GP_RxPacket(uint8_t* buff,uint8_t type,uint32_t *src, uint32_t *dst,uint8_t lenth, void *out)
{
    uint8_t crc = Fun_Bcc_Create(buff+1,lenth-3);
    if(buff[0] == 0x68 &&
        buff[lenth-1] == 0x16 &&
        buff[lenth-2] == crc)
    {
        gmap[type].unpacket(buff,type,lenth,out);
        *src = buff[1]|buff[2]<<8|buff[3]<<16|buff[4]<<24;
        *dst = buff[5]|buff[6]<<8|buff[7]<<16|buff[8]<<24;   
        return 1;
    }
    

    return 0;
}