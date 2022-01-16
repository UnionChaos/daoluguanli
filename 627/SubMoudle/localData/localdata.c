#include "sim7600.h"
#include "localdata.h"
#include "bsp_adc.h"
#include "DataApp.h"
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "rx8010.h"
#include "GP_protocol.h"
#include "GC_protocol.h"
#include "MsgProc.h"
#include "task.h"
#include "EEPROM.h"
#include "crc.h"

//授时 GPS间隔
#define GPSINTER                 3600*3
//轮询时间点(小时)
#define POLLITIME                11
#define CHO_RESP                 1
#define CHO_REPORT               2
#define E2_CONFIG_ADDR           0
#define E2_RF_ADDR               64
#define E2_MODE_ADDR             512
#define E2_STRATEGY_ADDR         576
#define E2_LOCAL_ADDR            640
#define E2_MODE_NOW_ADDR         672
#define E2_ID_ADDR               768
//#define E2_ID_ADDR               704

#define SYS_ON                   2
#define SYS_OFF                  1
#define SYS_UNKNONW              0
#define CONTROL_RETRY_NUM        3
typedef struct
{
  uint32_t id[3];
}ChipID;
 
typedef struct{

    uint16_t p_num;
    point_state_tiny p_state[300];
}point_t;


typedef struct{

    uint64_t last_update;
    uint64_t now;
    uint8_t  doneflag;
    uint8_t  syson;
    uint8_t  syson_cloud;
    uint8_t  isday;
    point_se_t se[2];

}local_info_t;


static local_info_t localInfo;
static point_t pointsInfo;
static gps_info_t gps_info;

static Config_t config = {
   .cardetect = {"off"},
   .taillenth = 4,
   .alarmtime = 4,
   .led = {
       .change = 0,
       .redfreq = 1,
       .redduty = 1,
       .redlight = 1,
       .yellowfreq = 1,
       .yellowduty = 1,
       .yellowlight = 1,
   }

};//自定义参数配置
static RfState_t rfstate[7] = {
    [0] = {
        .vision = 1,
        .time = {"day"},
        .led = {.change = 0,.redfreq = 1,.redduty = 1,.redlight = 1,.yellowfreq = 1,.yellowduty = 1,.yellowlight = 1,}
    },
    [1] = {
        .vision = 1,
        .time = {"night"},
        .led = {.change = 0,.redfreq = 1,.redduty = 1,.redlight = 1,.yellowfreq = 1,.yellowduty = 1,.yellowlight = 1,}
    },    
    [2] = {
        .vision = 2,
        .time = {"day"},
        .led = {.change = 0,.redfreq = 1,.redduty = 1,.redlight = 1,.yellowfreq = 1,.yellowduty = 1,.yellowlight = 1,}
    },
    [3] = {
        .vision = 2,
        .time = {"night"},
        .led = {.change = 0,.redfreq = 1,.redduty = 1,.redlight = 1,.yellowfreq = 1,.yellowduty = 1,.yellowlight = 1,}
    },
    [4] = {
        .vision = 3,
        .time = {"day"},
        .led = {.change = 0,.redfreq = 1,.redduty = 1,.redlight = 1,.yellowfreq = 1,.yellowduty = 1,.yellowlight = 1,}
    },
    [5] = {
        .vision = 3,
        .time = {"night"},
        .led = {.change = 0,.redfreq = 1,.redduty = 1,.redlight = 1,.yellowfreq = 1,.yellowduty = 1,.yellowlight = 1,}
    },
    [6] = {
        .vision = 4,
        .time = {"day"},
        .led = {.change = 0,.redfreq = 1,.redduty = 1,.redlight = 1,.yellowfreq = 1,.yellowduty = 1,.yellowlight = 1,}
    },
};//能见度因素配置
static Mode_t mode = {
    .mode = 0,
    .rf = {"off"},
};//模式配置
static Strategy_t strategy = {
    .eph = {"off"},
    .online = {"2005"},
    .offline = {"0505"},
};
static uint8_t point_mode = 0;
static uint64_t strategy_online_ts;
static uint64_t strategy_offline_ts;

static uint32_t self_id = 0x80000002;
static uint32_t self_sn = 0;
static unsigned char self_uid[16];
static uint64_t self_ts;
static uint16_t self_vol;

uint32_t rain_now = 0;
uint32_t foggy_now = 0;


ChipID chipid = {0};
Login_t LogInfo = {"","1.0.0","1.0.0"};
uint8_t RfUpdate = 0;
uint8_t NodeUpdate = 0;


extern SemaphoreHandle_t xSemaphore_4G;
//extern volatile SemaphoreHandle_t xGprsMutex;
extern QueueHandle_t Queue_Lora;
extern QueueHandle_t Queue_Mqtt;
extern SemaphoreHandle_t  xSemaphore_barrier_local;

void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
void ADC_VolGetSoft(uint16_t *value);
uint8_t gprs_is_online();
void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen)
{
    char ddl,ddh;

    for (int i = 0; i < nLen; i++)
    {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;
        if (ddh > 57) ddh = ddh + 7;
        if (ddl > 57) ddl = ddl + 7;
        pbDest[i * 2] = ddh;
        pbDest[i * 2 + 1] = ddl;
    }
    pbDest[nLen * 2] = '\0';
}
void Get_ChipID(void)
{ 

    uint32_t u1 = CRC32_IEEE_Calc((unsigned char*)UID_BASE, 12);
    uint16_t u2 = CRC16_IBM_Calc((unsigned char*)UID_BASE, 12);
        unsigned char id[6] = {0};
    id[1] = (u1 >> 16) & 0x000000FF ;
    id[2] = (u1 >> 8)  & 0x000000FF ;
    id[3] = (u1 >> 0)  & 0x000000FF ;
    
    id[4] = (u2 >> 8)  & 0x000000FF ;
    id[5] = (u2 >> 0)  & 0x000000FF ;
    HexToStr(self_uid, id, 6);
    self_uid[13] = '\0';
}
unsigned char* Get_Self_Uid(void)
{
    return self_uid;
}
uint32_t Get_Self_Id(void)
{
    return self_id;
}
uint32_t Get_Self_Sn(void)
{
    return self_sn++;
}
uint64_t Get_Self_Ts(void)
{
    return self_ts;
}
uint16_t Get_Gw_Vol(void)
{
    return self_vol;
}

void ModeChange(uint8_t mode)
{
    point_mode = mode;
}
uint8_t GetNowMode()
{
    return point_mode;
}

void Idpush(uint32_t id)
{
    self_id = id;
}
void Idflush()
{
    EEPROM_Write(E2_ID_ADDR,&self_id,sizeof(uint32_t));
}
void Configpull(Config_t *cfg)
{
    memcpy(cfg,&config,sizeof(Config_t));
}
void Configpush(Config_t *cfg)
{
    memcpy(&config,cfg,sizeof(Config_t));
}

void RfStatepull(RfState_t *rf,uint8_t index)
{
    memcpy(rf,&rfstate[index],sizeof(RfState_t));
}
void RfStatepush(RfState_t *rf)
{
    if(rf->vision > 4)
        return;
    if(rf->vision == 4)
    {
        memcpy(&rfstate[6],rf,sizeof(RfState_t));
        return;
    }
    
    if(!strcmp(rf->time,"day"))
    {
        memcpy(&rfstate[(rf->vision-1)*2],rf,sizeof(RfState_t));
    }
    else
        memcpy(&rfstate[(rf->vision-1)*2 + 1],rf,sizeof(RfState_t));
}
void RfResp()
{
    int i = 0;
    QueMsg * pMsgSend = NULL;
    if(RfUpdate == 0)
        return;
    for(i = 0;i<7;i++)
    {
        pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
        pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
        pMsgSend->subtype = GC_RFSTATE; 
        pMsgSend->cls = GC_CLS_CMDRESP;
        pMsgSend->sn = rf_sn_tmp;
        pMsgSend->data[0] = i;
        if( xQueueSend(Queue_Mqtt,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
        {
        /* 发送失败，即使等待了100个时钟节拍 */
            vPortFree(pMsgSend);
        }
        pMsgSend = NULL;
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    RfUpdate = 0;
}


void Modepull(Mode_t *m)
{
    memcpy(m,&mode,sizeof(Mode_t));
}
void Modepush(Mode_t *m)
{
    memcpy(&mode,m,sizeof(Mode_t));
}


void Strategypull(Strategy_t *stg)
{
    memcpy(stg,&strategy,sizeof(Strategy_t));
}


void Strategypush(Strategy_t *stg)
{
    memcpy(&strategy,stg,sizeof(Strategy_t));
    char h[3];
    char m[3];
    char s[3];
    memcpy(h,strategy.online,2);
    memcpy(m,strategy.online+2,2);
    memcpy(s,strategy.online+4,2);    
    h[2] = '\0';
    m[2] = '\0';
    s[2] = '\0';
    strategy_online_ts = atoi(h) * 3600 + atoi(m)*60 + atoi(s);

    memcpy(h,strategy.offline,2);
    memcpy(m,strategy.offline+2,2);
    memcpy(s,strategy.offline+4,2);    
    h[2] = '\0';
    m[2] = '\0';
    s[2] = '\0';
    strategy_offline_ts = atoi(h) * 3600 + atoi(m)*60 + atoi(s);
}

void Loginpull(Login_t *info)
{
  
    memcpy(info,&LogInfo,sizeof(Login_t));
}

void SysControlflush(uint8_t on)
{
    localInfo.syson_cloud = on;
}
void Configflush()
{
    //to do E2 xie
    EEPROM_Write(E2_CONFIG_ADDR,&config,sizeof(Config_t));
}

void RfStateflush()
{
    //to do E2 xie
    EEPROM_Write(E2_RF_ADDR,&rfstate[0],sizeof(RfState_t)*7);
}

void Modeflush()
{
    //to do E2 xie
    EEPROM_Write(E2_MODE_ADDR,&mode,sizeof(Mode_t));
}

void Strategyflush()
{
  //to do E2 xie
    EEPROM_Write(E2_STRATEGY_ADDR,&strategy,sizeof(Strategy_t));
}

void NowModeflush()
{
  //to do E2 xie
    EEPROM_Write(E2_MODE_NOW_ADDR,&point_mode,sizeof(uint8_t));
}

void pointSEflush()
{
    //to do E2 xie
    EEPROM_Write(E2_LOCAL_ADDR,&localInfo,sizeof(local_info_t));
}

void pointSEpull(point_se_t *se)
{
    memcpy(se,&localInfo.se,sizeof(point_se_t)*2);
}
void pointSEpush(point_se_t *se)
{
    memcpy(&localInfo.se,se,sizeof(point_se_t)*2);
}


void LocalDataPull()
{
    //to do E2 du chu 
    EEPROM_Read(E2_CONFIG_ADDR,&config,sizeof(Config_t));
    EEPROM_Read(E2_RF_ADDR,&rfstate[0],sizeof(RfState_t)*7);
    EEPROM_Read(E2_MODE_ADDR,&mode,sizeof(Mode_t));
    EEPROM_Read(E2_STRATEGY_ADDR,&strategy,sizeof(Strategy_t));
    EEPROM_Read(E2_LOCAL_ADDR,&localInfo,sizeof(local_info_t));
    EEPROM_Read(E2_MODE_NOW_ADDR,&point_mode,sizeof(uint8_t));
    EEPROM_Read(E2_ID_ADDR,&self_id,sizeof(uint32_t));
}

void LocalDataflush()
{
  //to do E2 xie
    EEPROM_Write(E2_CONFIG_ADDR,&config,sizeof(Config_t));
    EEPROM_Write(E2_RF_ADDR,&rfstate[0],sizeof(RfState_t)*7);
    EEPROM_Write(E2_MODE_ADDR,&mode,sizeof(Mode_t));
    EEPROM_Write(E2_STRATEGY_ADDR,&strategy,sizeof(Strategy_t));
    EEPROM_Write(E2_LOCAL_ADDR,&localInfo,sizeof(local_info_t));
    EEPROM_Write(E2_MODE_NOW_ADDR,&point_mode,sizeof(uint8_t));
    EEPROM_Write(E2_ID_ADDR,&self_id,sizeof(uint32_t));
}

///***********************************************************************************
//*GPS详细信息返回
//***********************************************************************************/
double Get_Gps_long(void)
{
    return gps_info.j_t;
}

double Get_Gps_at(void)
{
    return  gps_info.w_t;
}



///***********************************************************************************/
//能见度仪信息查询
//***********************************************************************************/
extern     SemaphoreHandle_t  xSemaphore_vision;
//static uint32_t notice_cnt = 0;
//static uint32_t rmc_cnt = 0;
void vTaskVision( void * pvParameters)
{
  uint8_t cnt = 0;
  char buf[128];
  
  while(1)
  {
        if(xSemaphoreTake( xSemaphore_vision,(TickType_t)0xffffffffUL) == pdPASS)
        {
            if(UART7_Rx((uint8_t *)buf,128) == 0)
               continue;
            if(NULL == strstr(buf,"VM"))
            {
                continue;
            }
            else
            {
                char *pToken = NULL;
                char *pDelimiter = " ";
                pToken = strtok((char *)buf,pDelimiter);
                while(pToken)
                {
                    pToken = strtok(NULL,pDelimiter);
                    switch(cnt)
                    {
                    case 2:
                      if(strlen(pToken)== 5)
                      {
                          foggy_now  = (uint16_t)atoi(pToken);
                      }
                      break;
                    default:
                      break;
                    }
                    
                    cnt++;
                }
                //to do rtc
            }
        }
       vTaskDelay(pdMS_TO_TICKS(50));
       cnt = 0;
    }
}
///***********************************************************************************
//*GPS信息查询
//***********************************************************************************/
extern     SemaphoreHandle_t  xSemaphore_local;
//static uint32_t notice_cnt = 0;
//static uint32_t rmc_cnt = 0;
void vTaskCodeGps( void * pvParameters )
{
  uint8_t cnt = 0;
  uint8_t hh_t;
  char buf[128];
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
  vTaskDelay(pdMS_TO_TICKS(1200));
  //取消GGA
  sprintf(buf,"$CFGMSG,%d,%d,%d\r",0,0,0);
  UART8_Tx((uint8_t*)buf,14);
  //取消GLL
  sprintf(buf,"$CFGMSG,%d,%d,%d\r",0,1,0);
  UART8_Tx((uint8_t*)buf,14);
  //取消GSA
  sprintf(buf,"$CFGMSG,%d,%d,%d\r",0,2,0);
  UART8_Tx((uint8_t*)buf,14);
  //取消GSV
  sprintf(buf,"$CFGMSG,%d,%d,%d\r",0,3,0);
  UART8_Tx((uint8_t*)buf,14);
  //取消VTG
  sprintf(buf,"$CFGMSG,%d,%d,%d\r",0,5,0);
  UART8_Tx((uint8_t*)buf,14);
  
  sprintf(buf,"$CFGMSG,%d,%d,%d\r",6,0,0);
  UART8_Tx((uint8_t*)buf,14);
  HAL_NVIC_EnableIRQ(UART8_IRQn);
  
  while(1)
  {
        if(xSemaphoreTake( xSemaphore_local,(TickType_t)0xffffffffUL) == pdPASS)
        {
            if(UART8_Rx((uint8_t *)buf,128) == 0)
               continue;
            if(NULL == strstr(buf,"RMC"))
            {
                continue;
            }
            else
            {
                char *pToken = NULL;
                char *pDelimiter = ":,";
                char h[3];
                char m[3];
                char s[3];
                pToken = strtok((char *)buf,pDelimiter);
                while(pToken)
                {
                    pToken = strtok(NULL,pDelimiter);
                    switch(cnt)
                    {
                    case 2:
                      //防止模块抽风导致死机
                      if(strlen(pToken) > 6)
                      {
                          memcpy(gps_info.j,pToken,strlen(pToken)+1);
                          gps_info.j_t = atof(pToken)/100;
                      }
                      break;
                    case 4:
                      if(strlen(pToken) > 6)
                      {
                        memcpy(gps_info.w,pToken,strlen(pToken)+1);
                        gps_info.w_t = atof(pToken)/100;
                      }
                      break;
                    case 7:
                      if(strlen(pToken) > 5)
                      {
                        memcpy((char *)&gps_info.time + 6,pToken,strlen(pToken)+1);
                        
                      }
                      break;
                    case 0:
                      if(strlen(pToken)== 9)
                      {
                          memcpy(h,pToken,2);
                          memcpy(m,pToken+2,2);
                          memcpy(s,pToken+4,2);    
                          h[2] = '\0';
                          m[2] = '\0';
                          s[2] = '\0';
                          hh_t = (uint8_t)atoi(h)+8;
                          if( hh_t >= 24)
                            gps_info.hour = hh_t%24;
                          else 
                            gps_info.hour = hh_t;
                          gps_info.min = (uint8_t)atoi(m);
                          gps_info.s = (uint8_t)atoi(s);   
                          self_ts = gps_info.hour*3600 + gps_info.min*60 + gps_info.s;
                          memcpy((char *)&gps_info.time,pToken,strlen(pToken));
                      }
                      break;
                    default:
                      break;
                    }
                    
                    cnt++;
                }
                //to do rtc
            }
        }
       vTaskDelay(pdMS_TO_TICKS(50));
       cnt = 0;
    }
}
uint8_t Points_auto_mode_change(uint8_t mode)
{
    QueMsg * pMsgSend = NULL;
    pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(auto_mode));
    auto_mode *atmode = (auto_mode*)pMsgSend->data;
    //gc2gs
    atmode->mode_num = mode;
    pMsgSend->subtype = GP_SET_MODE_AUTO;    

    pMsgSend->type = MSG_TYPE_LORA_STATE_QUERY_CONTROL;
    pMsgSend->src = self_id;
    if( xQueueSend(Queue_Lora,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
    {
    /* 发送失败，即使等待了100个时钟节拍 */
        vPortFree(pMsgSend);
    }
    return 1;
}
uint8_t Points_reset()
{
    QueMsg * pMsgSend = NULL;

    pMsgSend = pvPortMalloc(sizeof(QueMsg) + sizeof(gps_time));
    pMsgSend->type = MSG_TYPE_LORA_BROADCAST_TIME;
    pMsgSend->subtype = GP_RESET;
    pMsgSend->src = self_id;
    if( xQueueSend(Queue_Lora,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
    {
    /* 发送失败，即使等待了100个时钟节拍 */
        vPortFree(pMsgSend);
    }
    return 1;
}
//CHOOSE 0 直接授时
//CHOOSE 1 确认间隔来做授时
//后续考虑挪至EXTI中断内来做判断

uint8_t Time_service(uint8_t choose,uint64_t now, uint64_t last)
{
    uint64_t  interval = 0;
    int i = 0;
    QueMsg * pMsgSend = NULL;
    if(choose == 0)
    {
        pMsgSend = pvPortMalloc(sizeof(QueMsg) + sizeof(gps_time));
        pMsgSend->type = MSG_TYPE_LORA_BROADCAST_TIME;
        pMsgSend->subtype = GP_BROADCAST_TIME;
        pMsgSend->src = self_id;
        gps_time *broadcast = (gps_time *)pMsgSend->data;
        broadcast->hour = gps_info.hour;
        broadcast->min = gps_info.min;
        broadcast->sec = gps_info.s; 
        broadcast->ms = 0;  
        if( xQueueSend(Queue_Lora,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
        {
        /* 发送失败，即使等待了100个时钟节拍 */
            vPortFree(pMsgSend);
        }
        else
            localInfo.last_update = now;

        return 1;
    }

    if(now < last)
        interval = now + 86400 - last;
    else
        interval = now - last;
    if((interval)>= GPSINTER)
    {
        for(i = 0; i< CONTROL_RETRY_NUM ;i++)
        {
            pMsgSend = pvPortMalloc(sizeof(QueMsg) + sizeof(gps_time));
            pMsgSend->type = MSG_TYPE_LORA_BROADCAST_TIME;
            pMsgSend->subtype = GP_BROADCAST_TIME;
            pMsgSend->src = self_id;
            gps_time *broadcast = (gps_time *)pMsgSend->data;
            broadcast->hour = gps_info.hour;
            broadcast->min = gps_info.min;
            broadcast->sec = gps_info.s; 
            broadcast->ms = 0;  
            

            if( xQueueSend(Queue_Lora,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
            {
            /* 发送失败，即使等待了100个时钟节拍 */
                vPortFree(pMsgSend);
            }
            else
            {
                localInfo.last_update = now;
            }
            pMsgSend = NULL;
            vTaskDelay(pdMS_TO_TICKS(6000));
        }
        return 1;
        
    }
    return 1;
}

void GWstateReport()
{
    QueMsg * pMsgSend = NULL;

    pMsgSend = pvPortMalloc(sizeof(QueMsg));
    pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
    pMsgSend->subtype = GC_G_STAT; 
    pMsgSend->cls = GC_CLS_REPORT;
    //pMsgSend->sn = Get_Self_Sn();
    if( xQueueSend(Queue_Mqtt,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
    {
    /* 发送失败，即使等待了100个时钟节拍 */
        vPortFree(pMsgSend);
    }   
    //vTaskDelay(pdMS_TO_TICKS(3000));
}
///***********************************************************************************
//*
//***********************************************************************************/
void PointStateOne(uint16_t id)
{
    int i = 0;
    QueMsg * pMsgSend = NULL;
    for(i = 0;i<pointsInfo.p_num;i++)
    {
        if(id == pointsInfo.p_state[i].id)
        {
            pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(point_state));
            pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
            pMsgSend->subtype = GC_P_STAT; 
            pMsgSend->cls = GC_CLS_REPORT;
            point_state *state = (point_state *)pMsgSend->data;
            state->id = pointsInfo.p_state[i].id;
            state->vol = pointsInfo.p_state[i].vol;
            state->vol_soloar = pointsInfo.p_state[i].vol_soloar;
            memcpy(state->uid,pointsInfo.p_state[i].uid,13);
            memcpy(state->lastupdate,pointsInfo.p_state[i].lastupdate,13);
            if( xQueueSend(Queue_Mqtt,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
            {
            /* 发送失败，即使等待了100个时钟节拍 */
                vPortFree(pMsgSend);
            }
            pMsgSend = NULL;
            break;
        }
    }
}
void PointStateReport()
{
    int i = 0;
    QueMsg * pMsgSend = NULL;
    for(i = 0;i<pointsInfo.p_num;i++)
    {
        pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(point_state));
        pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
        pMsgSend->subtype = GC_P_STAT; 
        pMsgSend->cls = GC_CLS_REPORT;
        point_state *state = (point_state *)pMsgSend->data;
        state->id = pointsInfo.p_state[i].id;
        state->vol = pointsInfo.p_state[i].vol;
        state->vol_soloar = pointsInfo.p_state[i].vol_soloar;
        memcpy(state->uid,pointsInfo.p_state[i].uid,13);
        memcpy(state->lastupdate,pointsInfo.p_state[i].lastupdate,13);
        if( xQueueSend(Queue_Mqtt,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
        {
        /* 发送失败，即使等待了100个时钟节拍 */
            vPortFree(pMsgSend);
        }
        pMsgSend = NULL;
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

void PointStateResp()
{
    int i = 0;
    QueMsg * pMsgSend = NULL;
    if(NodeUpdate == 0)
        return;
    for(i = 0;i<pointsInfo.p_num;i++)
    {
        pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(point_state));
        pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
        pMsgSend->subtype = GC_P_STAT; 
        pMsgSend->cls = GC_CLS_CMDRESP;
        pMsgSend->sn = pstate_sn_tmp;
        point_state *state = (point_state *)pMsgSend->data;
        state->id = pointsInfo.p_state[i].id;
        state->vol = pointsInfo.p_state[i].vol;
        state->vol_soloar = pointsInfo.p_state[i].vol_soloar;
        memcpy(state->uid,pointsInfo.p_state[i].uid,13);
        memcpy(state->lastupdate,pointsInfo.p_state[i].lastupdate,13);
        if( xQueueSend(Queue_Mqtt,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
        {
        /* 发送失败，即使等待了100个时钟节拍 */
            vPortFree(pMsgSend);
        }
        pMsgSend = NULL;
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
    NodeUpdate = 0;
}
void PointStatePush(void *p)
{
    QueMsg * pMsgSend = NULL;
    point_state *pstate = (point_state *)p;
    int i = 0;
    for(i = 0;i<pointsInfo.p_num;i++)
    {
        if(pointsInfo.p_state[i].id == pstate->id)
        {
            pointsInfo.p_state[i].vol = pstate->vol;
            pointsInfo.p_state[i].vol_soloar = pstate->vol_soloar;
            memcpy(pointsInfo.p_state[i].uid,pstate->uid,13);
            memcpy(pointsInfo.p_state[i].lastupdate,gps_info.time,13);
            pointsInfo.p_state[i].vol_soloar = pstate->vol_soloar;
            /*推送云端*/
            pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(point_state));
            pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
            pMsgSend->subtype = GC_P_STAT; 
            pMsgSend->cls = GC_CLS_REPORT;
            point_state *state = (point_state *)pMsgSend->data;
            state->id = pointsInfo.p_state[i].id;
            state->vol = pointsInfo.p_state[i].vol;
            state->vol_soloar = pointsInfo.p_state[i].vol_soloar;
            memcpy(state->uid,pointsInfo.p_state[i].uid,13);
            memcpy(state->lastupdate,pointsInfo.p_state[i].lastupdate,13);
            if( xQueueSend(Queue_Mqtt,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
            {
            /* 发送失败，即使等待了100个时钟节拍 */
                vPortFree(pMsgSend);
            }
            pMsgSend = NULL;
            break;
        }
    }
}

void PointStatePoll()
{
    int i = 0;
    QueMsg * pMsgSend = NULL;
    for(i = 0;i<pointsInfo.p_num;i++)
    {
        pMsgSend = pvPortMalloc(sizeof(QueMsg));
        pMsgSend->type = MSG_TYPE_LORA_STATE_QUERY_STATE;
        pMsgSend->subtype = GP_P_STAT; 
        pMsgSend->src = self_id;
        pMsgSend->dst = pointsInfo.p_state[i].id;
        if( xQueueSend(Queue_Lora,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
        {
        /* 发送失败，即使等待了100个时钟节拍 */
            vPortFree(pMsgSend);
        }
        pMsgSend = NULL;
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void RfModeCmd(uint8_t cho, uint8_t index)
{

    QueMsg * pMsgSend = NULL;
    if(cho == 0)
        return;
    if(cho == MODE_RF)
    {
        pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(snap_mode));
        pMsgSend->subtype = GP_SET_MODE_SNAP; 
        pMsgSend->src = self_id;
        snap_mode *snap = (snap_mode *)pMsgSend->data;
        snap->change = rfstate[index].led.change;
        snap->alarm_induce.duty = (uint8_t)rfstate[index].led.yellowduty;
        snap->alarm_induce.freq = rfstate[index].led.yellowfreq;
        snap->alarm_induce.light = rfstate[index].led.yellowlight;
        snap->alarm_alarm.duty = (uint8_t)rfstate[index].led.redduty;
        snap->alarm_alarm.freq = rfstate[index].led.redfreq;
        snap->alarm_alarm.light = rfstate[index].led.redlight;
    }
    else if(cho == MODE_CFG)
    {
        pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(custom_mode));
        custom_mode *csmode = (custom_mode*)pMsgSend->data;
        Config_t cfg;

        Configpull(&cfg);
        csmode->car_info.change = cfg.led.change;
        csmode->alarm_induce.duty = (uint8_t)cfg.led.yellowduty;
        csmode->alarm_induce.freq = cfg.led.yellowfreq;
        csmode->alarm_induce.light = cfg.led.yellowlight;

        csmode->alarm_alarm.duty = (uint8_t)cfg.led.redduty;
        csmode->alarm_alarm.freq = cfg.led.redfreq;
        csmode->alarm_alarm.light = cfg.led.redlight;

        csmode->car_info.detect_on = strcmp(cfg.cardetect,"on")?0:1;
        csmode->car_info.tail_lenth = cfg.taillenth;
        csmode->car_info.tail_time = cfg.alarmtime;
        pMsgSend->subtype = GP_SET_MODE_CUSTOM;
        pMsgSend->src = self_id;
    }
    else
    {
        pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(auto_mode));
        pMsgSend->subtype = GP_SET_MODE_AUTO; 
        auto_mode *atmode = (auto_mode*)pMsgSend->data;
        atmode->mode_num = cho;
    }
    pMsgSend->type = MSG_TYPE_LORA_STATE_QUERY_CONTROL;
    if( xQueueSend(Queue_Lora,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
    {
    /* 发送失败，即使等待了100个时钟节拍 */
        vPortFree(pMsgSend);
    }   
}

void ModReport()
{
    QueMsg * pMsgSend = NULL;

    pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
    pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
    pMsgSend->subtype = GC_MODE;
    pMsgSend->cls = GC_CLS_REPORT; 
    if( xQueueSend(Queue_Mqtt,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
    {
    /* 发送失败，即使等待了100个时钟节拍 */
        vPortFree(pMsgSend);
    }
    //vTaskDelay(pdMS_TO_TICKS(4000));
}

void SysControl_once(uint8_t on)
{
    QueMsg * pMsgSend = NULL;

    pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
    pMsgSend->type = MSG_TYPE_LORA_STATE_QUERY_CONTROL;
    pMsgSend->subtype = GP_SET_SYS_ON; 
    pMsgSend->src = self_id;
    pMsgSend->data[0] = on-1;
    if( xQueueSend(Queue_Lora,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
    {
    /* 发送失败，即使等待了100个时钟节拍 */
        vPortFree(pMsgSend);
    }
    else
    {
    localInfo.syson = on;  
    }
    pMsgSend = NULL;
      
}

void SysControl(uint8_t on)
{
    int i = 0;
    QueMsg * pMsgSend = NULL;
    for(i = 0; i< CONTROL_RETRY_NUM; i++)
    {
      pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
      pMsgSend->type = MSG_TYPE_LORA_STATE_QUERY_CONTROL;
      pMsgSend->subtype = GP_SET_SYS_ON; 
      pMsgSend->src = self_id;
      pMsgSend->data[0] = on-1;
      if( xQueueSend(Queue_Lora,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
      {
      /* 发送失败，即使等待了100个时钟节拍 */
          vPortFree(pMsgSend);
      }
      else
      {
        localInfo.syson = on;  
      }
      pMsgSend = NULL;
      vTaskDelay(pdMS_TO_TICKS(6000));
    }     
}

void LogIn()
{
    QueMsg * pMsgSend = NULL;

    pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
    pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
    pMsgSend->subtype = GC_LOGIN; 
    if( xQueueSend(Queue_Mqtt,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
    {
    /* 发送失败，即使等待了100个时钟节拍 */
        vPortFree(pMsgSend);
    }

}

void localInfoInit()
{
    int i = 0;
    localInfo.now = gps_info.hour*3600 + gps_info.min*60 + gps_info.s;

    pointsInfo.p_num = (localInfo.se[0].end - localInfo.se[0].start + 1) +
                        (localInfo.se[1].end - localInfo.se[1].start + 1);
    char h[3];
    char m[3];
    char s[3];
    memcpy(h,strategy.online,2);
    memcpy(m,strategy.online+2,2);
    memcpy(s,strategy.online+4,2);    
    h[2] = '\0';
    m[2] = '\0';
    s[2] = '\0';
    strategy_online_ts = atoi(h) * 3600 + atoi(m)*60 + atoi(s);

    memcpy(h,strategy.offline,2);
    memcpy(m,strategy.offline+2,2);
    memcpy(s,strategy.offline+4,2);    
    h[2] = '\0';
    m[2] = '\0';
    s[2] = '\0';
    strategy_offline_ts = atoi(h) * 3600 + atoi(m)*60 + atoi(s);
    
      //临时写死网关ID 为A服务 1-140 421-560
    for(i= 0; i< pointsInfo.p_num;i++)
    {
        if(i<localInfo.se[0].end)
            pointsInfo.p_state[i].id = i+ localInfo.se[0].start;
        else
            pointsInfo.p_state[i].id = i+ localInfo.se[1].start - localInfo.se[0].end;
    }
    //临时写死网关ID 为B服务 141-280 281-420
    /*pointsInfo.p_num = 6; 
    for(i=0; i< pointsInfo.p_num ;i++)
    {
        pointsInfo.p_state[i].id = i+1;
    }*/
    //memset(&localInfo,0,sizeof(local_info_t));
    localInfo.isday = 0;
    localInfo.doneflag = 0;
    localInfo.syson = 0;
    localInfo.syson_cloud = 0;
    localInfo.last_update = 0;
}

void vTaskCodeLocal( void * pvParameters )
{
    Get_ChipID();
    sprintf(LogInfo.uid,"%s",self_uid);
    /*localInfo.se[0].start = 1;
    localInfo.se[0].end = 140;
    localInfo.se[1].start = 421;
    localInfo.se[1].end = 560;*/
    
  /* localInfo.se[0].start = 141;
    localInfo.se[0].end = 280;
    localInfo.se[1].start = 281;
    localInfo.se[1].end = 420; */
    //self_id = 0x80000002; 
    //LocalDataflush();
    /*Idpush(0x80000001);

    Idflush();*/
    LocalDataPull();
    //最好等待MQTT初始化完成
    /*while(1)
    {
        if(xSemaphoreTake( xSemaphore_barrier_local,(TickType_t)0xffffffffUL) == pdPASS)
        {
            printf("gprs start!\n");
            break;
        }
    }
*/
    vTaskDelay(pdMS_TO_TICKS(12000));
    LogIn();
    localInfoInit();
    Time_service(0,localInfo.now,localInfo.last_update);//to G

    //用GPS做一次授时
    while(1)
    {
        
        localInfo.now = gps_info.hour*3600 + gps_info.min*60 + gps_info.s;   
        //开关机判断 
        if(!strcmp(strategy.eph,"off"))
        {   
            if(strategy_online_ts < strategy_offline_ts) 
            {
                if((localInfo.now >= strategy_online_ts)&&
                (localInfo.now < strategy_offline_ts)&&
                (localInfo.syson == SYS_OFF||localInfo.syson == SYS_UNKNONW))
                {
                    localInfo.isday = 0;
                    SysControl(SYS_ON);

                }
                if(((localInfo.now < strategy_online_ts)
                ||(localInfo.now >= strategy_offline_ts))&&
                (localInfo.syson == SYS_ON||localInfo.syson == SYS_UNKNONW))
                {
#ifdef CJY_DEBUG
                    printf("sys off!\n");
#endif
                    localInfo.isday = 1;
                    SysControl(SYS_OFF);
                }
            }
            else
            {
                if(((localInfo.now >= strategy_online_ts)||
                (localInfo.now < strategy_offline_ts))&&
                (localInfo.syson == SYS_OFF||localInfo.syson == SYS_UNKNONW))
                {
                    localInfo.isday = 0;
                    SysControl(SYS_ON);
                }
                if((localInfo.now >= strategy_offline_ts)&&
                (localInfo.now < strategy_online_ts)&&
                (localInfo.syson == SYS_ON||localInfo.syson == SYS_UNKNONW))
                {
#ifdef CJY_DEBUG
                     printf("sys off 2!\n");
#endif
                    localInfo.isday = 1;
                    SysControl(SYS_OFF);
                }
            }

        }    
        else
        {
            //TODO
            // 星历
        }
        
        //6小时一次授时
        //GPSINFO 每4小时一次
        Time_service(1,localInfo.now,localInfo.last_update);
        
        //ADCGET
        ADC_VolGetSoft(&self_vol);
        
        //定时轮询节点状态 每天白天中午12点01分
        if((gps_info.hour == POLLITIME)&&(localInfo.doneflag == 0))
        {    
            //if(gprs_is_online())
            if(1)
            {
                PointStatePoll();
                //PointStateReport();
                localInfo.doneflag = 1;
            }

        }
        //临时测试 每两分钟上报一次节点/网关状态
        //Time_service(0,localInfo.now,localInfo.last_update);
        //PointStatePoll();

        //PointStateReport();
        if(gprs_is_online())
        {
            GWstateReport();
            //当前工作模式上报
            ModReport();
        }

        //配置表逻辑判断（能见度，策略时间）
        //开启了能见度因素
        if(!strcmp(mode.rf,"on"))
        {
            if((point_mode != MODE_RF)&&(foggy_now < 1000))
            {
                point_mode = MODE_RF;
                if(foggy_now > 500)
                {
                    if(localInfo.isday)
                        RfModeCmd(point_mode,3);
                    else
                        RfModeCmd(point_mode,2);
                }
                else if(foggy_now > 250)
                {
                    if(localInfo.isday)
                        RfModeCmd(point_mode,5);
                    else
                        RfModeCmd(point_mode,4);

                }
                else if(foggy_now > 0)
                {
                    RfModeCmd(point_mode,6);
                }

            }
            if((point_mode != MODE_RF)&&(foggy_now >= 1000))
            {
                //需要切回之前的工作模式
                point_mode = MODE_RF;
                if(localInfo.isday)
                    RfModeCmd(point_mode,1);
                else
                    RfModeCmd(point_mode,0);
            }
        }
        else
        {
            if((point_mode == MODE_RF))
            {
                //需要切回之前的工作模式
                point_mode = mode.mode;
                RfModeCmd(point_mode,0);
            }
        }


        
        if(gps_info.hour == 23)
        {
            localInfo.doneflag = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(120000));
    }
            


}