#ifndef __GC_PROTOCOL_H__
#define __GC_PROTOCOL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include "MsgProc.h"
#include <stdint.h>

typedef enum gc_cls_type
{
    GC_CLS_COMMAND = 1,
    GC_CLS_REPORT,
    GC_CLS_CMDRESP,
    GC_CLS_CMDACK,

}GC_CLS_TYPE;


typedef struct gc_map
{
    uint8_t type;
    void (*packet)(uint8_t* obuff,uint8_t *ibuff, uint16_t* osize,QueMsg *msg);
    void (*unpacket)(uint8_t* obuff,uint8_t *ibuff, void* osize);
}GC_map;



void  mqtt_MsgPacket(QueMsg *msg,uint8_t *obuff,uint8_t *ibuff,uint16_t* osize);
void  mqtt_MsgUnPacket(QueMsg *msg,void **obuff);
extern uint32_t pstate_sn_tmp;
extern uint32_t rf_sn_tmp;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif