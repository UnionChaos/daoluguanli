#ifndef __GP_PROTOCOL_H__
#define __GP_PROTOCOL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include <stdint.h>


typedef struct gp_map
{
    uint8_t type;
    uint8_t (*packet)(uint8_t* buff,uint8_t type,void* param);
    uint8_t (*unpacket)(uint8_t* buff,uint8_t type,uint8_t lenth, void *out);
}GP_map;

#pragma pack(1)
typedef struct led_attr
{
    uint8_t duty:2;
    uint8_t light:3;
    uint8_t freq:3;

}led_attr;
typedef struct car_attr
{
    uint8_t detect_on:1;
    uint8_t change:1;
    uint8_t tail_lenth:3;
    uint8_t tail_time:3;
}car_attr;
typedef struct sys_config //sys
{
    uint8_t sys_on;
} sys_config;
typedef struct  auto_mode //auto
{
    uint8_t mode_num;
} auto_mode;
typedef struct  custom_mode //custom
{
    led_attr  alarm_induce;
    led_attr  alarm_alarm;
    car_attr  car_info;
} custom_mode;
typedef struct  snap_mode //custom
{
    led_attr  alarm_induce;
    led_attr  alarm_alarm;
    uint8_t  change;
}snap_mode;
typedef struct  point_state //custom
{
    uint32_t id;
    char uid[13];
    uint8_t sys_on;
    led_attr  induce;
    led_attr  alarm_alarm;
    car_attr  car_info;
    uint16_t   vol;
    uint16_t   vol_soloar;
    char lastupdate[16];
} point_state;
typedef struct  point_state_tiny //custom
{
    uint16_t id;
    uint16_t  vol;
    uint16_t  vol_soloar;
    char uid[16];
    char lastupdate[16];
} point_state_tiny;
typedef struct  gps_time //custom
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t ms;
} gps_time;
typedef struct gp_attr
{
    uint16_t lenth:10;
    uint16_t trans:2;
    uint16_t encrypt:2;
    uint16_t sep:1;
    uint16_t res:1;

}GP_Attr;
typedef struct gp_msg
{
    uint8_t head;
    uint32_t src;
    uint32_t dst;
    GP_Attr Attr;
    uint8_t type;
    uint8_t res;
    uint8_t bcc;
    uint8_t tail;
    char data[0];

}GP_Msg;

#pragma pack()

uint8_t GP_TxPacket(uint8_t* buff,uint8_t type,uint32_t src, uint32_t dst,void *param);
uint8_t GP_RxPacket(uint8_t* buff,uint8_t type,uint32_t *src, uint32_t *dst,uint8_t lenth, void *out);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif