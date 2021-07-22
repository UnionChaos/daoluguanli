#ifndef __LOCALDATA_H__
#define __LOCALDATA_H__

#include <stdint.h>
#include "DataApp.h"
typedef enum MODE_TYPE
{
    MODE_AUTO_0 = 1,
    MODE_AUTO_1,
    MODE_AUTO_2,
    MODE_CFG,
    MODE_RF,
}MODE_TYPE;
typedef struct{
    char j[32];
    double j_t;
    char w[32];
    double w_t;
    char time[32];
    uint8_t hour;
    uint8_t min;
    uint8_t s;
    uint8_t ms;
}gps_info_t;

void ModeChange(uint8_t mode);
uint8_t GetNowMode();
void NowModeflush();

void PointStateOne(uint16_t id);
void PointStateReport();
void PointStateResp();
void PointStatePush(void *p);
void PointStatePoll();

void Configpull(Config_t *cfg);
void Configpush(Config_t *cfg);
void Configflush();

void RfStatepull(RfState_t *rf,uint8_t index);
void RfStatepush(RfState_t *rf);
void RfStateflush();

void Modepull(Mode_t *m);
void Modepush(Mode_t *m);
void Modeflush();

void Strategypull(Strategy_t *stg);
void Strategypush(Strategy_t *stg);
void Strategyflush();

void Loginpull();
void SysControlflush(uint8_t on);

extern uint16_t Get_Gw_Vol(void);
extern unsigned char* Get_Self_Uid(void);
extern uint32_t Get_Self_Id(void);
extern uint32_t Get_Self_Sn(void);
extern uint64_t Get_Self_Ts(void);

extern int Gsm_Gps_Open(void);
extern int Gsm_Gps_IsOpen(void);
extern int Gsm_Gps_Close(void);
extern int Gsm_Gps_Info();
extern double Get_Gps_long(void);
extern double Get_Gps_at(void);



uint8_t Time_service(uint8_t choose,uint64_t now, uint64_t last);


void ModeCmd(uint8_t cho);

void SysControl(uint8_t on);
  
  
  
////AT


			  
#endif