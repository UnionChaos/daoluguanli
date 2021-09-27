#include "GP_protocol.h"
#include "localdata.h"
#include "MsgProc.h"
#include "string.h"
#define HANDMACHINE                 0x12345678

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

extern void hw_system_rest();
static __gh_reset(uint8_t* buff,uint8_t type,uint8_t lenth, void *out)
{
    //先判断终端需不需要复位
    if(buff[13] = 0x00)
    {
        hw_system_rest();
    }
    else
    {
        SysControl_once(1);//先关
        SysControl_once(2);//再开
    }
}

static __gh_query_id(uint8_t* buff,uint8_t type,uint8_t lenth, void *out)
{
    uint32_t src_id = Get_Self_Id();
    GP_TxPacket(out,type,src_id,0x12345678,&src_id);
}

static uint8_t __gh_reply_id(uint8_t* buff,uint8_t type,void* param)
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
    buff[13] = 0x00;//成功
    memcpy(&buff[14],param,4);//拷贝本机ID
    return attr.lenth;
}

static __gh_set_id(uint8_t* buff,uint8_t type,uint8_t lenth, void *out)
{
    uint32_t id = 0;
    id = buff[13]|buff[14]<<8|buff[15]<<16|buff[16]<<32;
    Idpush(id);
    //Idflush();
    //初始化设置ID时候，顺便将默认参数配置进去
    LocalDataflush();
}

static __gh_sync(uint8_t* buff,uint8_t type,uint8_t lenth, void *out)
{
    //做一次立即授时
    Time_service(0,0,0);
}
static __gh_reply_list(uint8_t* buff,uint8_t type,uint8_t lenth, void *out)
{
    uint32_t list[4];
    point_se_t data[2];
    pointSEpull(data);
    list[0] = data[0].start;
    list[1] = data[0].end;
    list[2] = data[1].start;
    list[3] = data[1].end;
    GP_TxPacket(out,type,Get_Self_Id(),0x12345678,list);
}

static uint8_t __gh_query_list(uint8_t* buff,uint8_t type,void* param)
{
    GP_Attr attr;
    attr.lenth = 17;
    attr.encrypt = 0;
    attr.trans = 3;
    attr.sep = 0;
    attr.res = 0;
    memcpy(&buff[9],&attr,2);
    buff[11] = type;
    buff[12] = 0x00;
    buff[13] = 0x00;//成功
    memcpy(&buff[14],param,16);//拷贝list
    return attr.lenth;
}

static __gh_set_list(uint8_t* buff,uint8_t type,uint8_t lenth, void *out)
{
    uint32_t id = 0;
    point_se_t data[2];
    data[0].start = buff[13]|buff[14]<<8|buff[15]<<16|buff[16]<<32;
    data[0].end = buff[17]|buff[18]<<8|buff[19]<<16|buff[20]<<32;
    data[1].start = buff[21]|buff[22]<<8|buff[23]<<16|buff[24]<<32;
    data[1].end = buff[25]|buff[26]<<8|buff[27]<<16|buff[28]<<32;
    pointSEpush(data);
    pointSEflush();
}


GP_map gmap[17]=
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
    {GH_RESET,NULL,__gh_reset},
    {GH_QUERY_ID,__gh_reply_id,__gh_query_id},
    {GH_SET_ID,NULL,__gh_set_id},
    {GH_SYNC,NULL,__gh_sync},
    {GH_QUERY_LIST,__gh_reply_list,__gh_query_list},
    {GH_SET_LIST,NULL,__gh_set_list},
};

uint8_t GP_TxPacket(uint8_t* buff,uint8_t type,uint32_t src, uint32_t dst,void *param)
{
    uint8_t lenth = 0;
    int i = 0;
    buff[0] = 0x68;
    buff[1] = src;
    buff[2] = src>>8;
    buff[3] = src>>16;
    buff[4] = src>>24;
    buff[5] = dst;
    buff[6] = dst>>8;
    buff[7] = dst>>16;
    buff[8] = dst>>24;  
    for(i = 0; i<17; i++)
    {
        if(type == gmap[i].type)
        {
            lenth = gmap[type].packet(buff,type,param);
            break;
        }
    }
    buff[13+lenth] = Fun_Bcc_Create(buff+1,12+lenth);
    buff[14+lenth] = 0x16;
    return 15+lenth;
}

uint8_t GP_RxPacket(uint8_t* buff,uint8_t type,uint8_t lenth, void *out)
{
    uint8_t crc = Fun_Bcc_Create(buff+1,lenth-3);
    uint8_t ret = 0;
    int i = 0;
    if(buff[0] == 0x68 &&
        buff[lenth-1] == 0x16 &&
        buff[lenth-2] == crc)
    {
        for(i = 0; i<17; i++)
        {
            if(type == gmap[i].type)
            {
                ret = gmap[type].unpacket(buff,type,lenth,out);
                return ret;
            }
        }
    }
    

    return 0;
}