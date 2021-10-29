/*
 * @Author: your name
 * @Date: 2021-10-25 09:03:15
 * @LastEditTime: 2021-10-27 15:24:50
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \daoluguanli\627\SubMoudle\DataProcess\MsgProc.h
 */
#ifndef __MsgProc_H__
#define __MsgProc_H__
#include <stdint.h>
//#include "GP_protocol.h"


typedef enum MsgType
{
    MSG_TYPE_LORA_BROADCAST_TIME = 1,
    MSG_TYPE_LORA_STATE_QUERY_STATE,
    MSG_TYPE_LORA_STATE_QUERY_CONTROL,

    MSG_TYPE_MQTT_STATE_REPORT,
    MSG_TYPE_MQTT_STATE_COMMAND,
    MSG_TYPE_MQTT_STATE_ACK,

}MSGTYPE;

typedef enum sub_type
{
    GP_SET_P_ID = 1,
    GP_SET_PG_ID,
    GP_QUERY_G_ID,
    GP_SET_SYS_ON,
    GP_SET_CAR_ON,
    GP_SET_MODE_AUTO,
    GP_SET_MODE_CUSTOM,
    GP_SET_MODE_SNAP,
    GP_P_STAT,
    GP_BROADCAST_TIME,
    GP_RESET,

    GH_RESET = 0x41,
    GH_RESET_POINT,
    GH_SYNC,
    GH_QUERY_HOST,
    GH_SET_ID,
    GH_SET_LIST,
    GH_POINT_SWTICH,
    GH_MODE_SWITCH,


    GC_G_STAT = 0x8f,
    GC_P_STAT,
    GC_CONFIG,
    GC_RFSTATE,
    GC_MODE,
    GC_STRATEGY,
    GC_QUERY,
    GC_LOGIN,
    GC_ACK,
}SUB_TYPE;
/*typedef struct QueMsgInfo
{
    GP_TYPE subtype;
    uint32_t src;
    uint32_t dst;
    union cfg
    {
        struct sys_config //sys
        {
            uint8_t sys_on;
        } sys_config;
        struct auto_mode //auto
        {
            uint8_t mode_num;
        } auto_mode;
        struct custom_mode //custom
        {
            uint8_t mode_num;
        } auto_mode;
    };
}QueMsgInfo;*/

typedef struct QueMsg
{
    MSGTYPE type;
    SUB_TYPE subtype;
    uint16_t lenth;
    //当cls为REPORT/COMMAND的时候，sn号自己递增。 
    //当为cls为CMDREP/CMDACK时候，采用msg的sn号
    uint16_t cls;
    uint32_t src;
    uint32_t dst;
    uint32_t sn;//存的此条消息生命周期内的sn号，只为CMDREP CMDACK服务
    char data[0];
}QueMsg;

#endif



