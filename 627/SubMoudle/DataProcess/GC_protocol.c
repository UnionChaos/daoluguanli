#include "GP_protocol.h"
#include "GC_protocol.h"
#include "DataApp.h"
#include "MsgProc.h"
#include "localdata.h"
#include "jsonb_c_base.h"
#include "stdlib.h"

uint32_t pstate_sn_tmp;
uint32_t rf_sn_tmp;
extern uint8_t NodeUpdate;
extern uint8_t RfUpdate;
void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );



static void __gate_state(uint8_t* obuff,uint8_t *ibuff, uint16_t* osize,QueMsg *msg)
{
    //组织一个cjson结构体
    GC_msg_t gc_msg;
    if(msg->cls == GC_CLS_CMDRESP)
    {
        memcpy(gc_msg.cls,"CmdResp",8);
        gc_msg.sn = msg->sn;
    }
    else
    {
        memcpy(gc_msg.cls,"Report",7);
        gc_msg.sn = Get_Self_Sn();
    }

    gc_msg.id = Get_Self_Id();
    gc_msg.ts = Get_Self_Ts();
    memcpy(gc_msg.rpt,"GwState",8);
    gc_msg.GwState.longg  = Get_Gps_long();
    gc_msg.GwState.at = Get_Gps_at();
    gc_msg.GwState.vol = Get_Gw_Vol();
    gc_msg.GwState.devid = gc_msg.id;

    cJSON *json = cJSON_CreateObject();   
    jsonb_opt_GC_msg_t(JSONB_OPT_S2J,json,&gc_msg,sizeof(GC_msg_t));
    cJSON_PrintPreallocated(json, (char *)obuff, 1024, 0);
    *osize  = strlen((char *)obuff) + 1;
    cJSON_Delete(json);

} 

static void __point_state(uint8_t* obuff,uint8_t *ibuff,uint16_t* osize,QueMsg *msg)
{
    //组织一个cjson结构体、
    //此处的ibuff由外部分配而来，因为节点状态是动态分配的内存,表示节点数据
    point_state *states = (point_state *)ibuff;
    GC_msg_t gc_msg;
    if(msg->cls == GC_CLS_CMDRESP)
    {
        memcpy(gc_msg.cls,"CmdResp",8);
        gc_msg.sn = msg->sn;
    }
    else
    {
        memcpy(gc_msg.cls,"Report",7);
        gc_msg.sn = Get_Self_Sn();
    }
    gc_msg.id = Get_Self_Id();
    gc_msg.ts = Get_Self_Ts();
    memcpy(gc_msg.rpt,"NodeState",10);
    gc_msg.NodeState.devid = states->id;
    gc_msg.NodeState.svol = states->vol_soloar;
    gc_msg.NodeState.vol = states->vol;
    memcpy(gc_msg.NodeState.uid,states->uid,13);
    //to do lastupdate
    memcpy(gc_msg.NodeState.lastupdate,states->lastupdate,13);
    cJSON *json = cJSON_CreateObject();
    jsonb_opt_GC_msg_t(JSONB_OPT_S2J,json,&gc_msg,sizeof(GC_msg_t));
    cJSON_PrintPreallocated(json, (char *)obuff, 1024, 0);
    *osize  = strlen((char *)obuff) + 1;
    cJSON_Delete(json);

} 

static void __config_pk(uint8_t* obuff,uint8_t *ibuff,uint16_t* osize,QueMsg *msg)
{
    //组织一个cjson结构体
    GC_msg_t gc_msg;
    if(msg->cls == GC_CLS_CMDRESP)
    {
        memcpy(gc_msg.cls,"CmdResp",8);
        gc_msg.sn = msg->sn;
    }
    else
    {
        memcpy(gc_msg.cls,"Report",7);
        gc_msg.sn = Get_Self_Sn();
    }
    gc_msg.id = Get_Self_Id();
    gc_msg.ts = Get_Self_Ts();
    memcpy(gc_msg.rpt,"Config",7);
    Configpull(&gc_msg.Config);

    cJSON *json = cJSON_CreateObject();   
    jsonb_opt_GC_msg_t(JSONB_OPT_S2J,json,&gc_msg,sizeof(GC_msg_t));
    cJSON_PrintPreallocated(json, (char *)obuff, 1024, 0);
    *osize  = strlen((char *)obuff) + 1;
    cJSON_Delete(json);

} 

static void __rf_pk(uint8_t* obuff,uint8_t *ibuff,uint16_t* osize,QueMsg *msg)
{
    //组织一个cjson结构体
    GC_msg_t gc_msg;
    if(msg->cls == GC_CLS_CMDRESP)
    {
        memcpy(gc_msg.cls,"CmdResp",8);
        gc_msg.sn = msg->sn;
    }
    else
    {
        memcpy(gc_msg.cls,"Report",7);
        gc_msg.sn = Get_Self_Sn();
    }
    gc_msg.id = Get_Self_Id();
    gc_msg.ts = Get_Self_Ts();
    memcpy(gc_msg.rpt,"RfState",8);
    //此处ibuff为私有数据，表示rf数据的index
    RfStatepull(&gc_msg.RfState,ibuff[0]);

    cJSON *json = cJSON_CreateObject();   
    jsonb_opt_GC_msg_t(JSONB_OPT_S2J,json,&gc_msg,sizeof(GC_msg_t));
    cJSON_PrintPreallocated(json, (char *)obuff, 1024, 0);
    *osize  = strlen((char *)obuff) + 1;
    cJSON_Delete(json);

} 

static void __mode_pk(uint8_t* obuff,uint8_t *ibuff,uint16_t* osize,QueMsg *msg)
{
    //组织一个cjson结构体
    GC_msg_t gc_msg;
    if(msg->cls == GC_CLS_CMDRESP)
    {
        memcpy(gc_msg.cls,"CmdResp",7);
        gc_msg.sn = msg->sn;
    }
    else
    {
        memcpy(gc_msg.cls,"Report",7);
        gc_msg.sn = Get_Self_Sn();
    }
    gc_msg.id = Get_Self_Id();
    gc_msg.ts = Get_Self_Ts();
    memcpy(gc_msg.rpt,"Mode",5);
    Modepull(&gc_msg.Mode);
    gc_msg.Mode.mode = GetNowMode();

    cJSON *json = cJSON_CreateObject();   
    jsonb_opt_GC_msg_t(JSONB_OPT_S2J,json,&gc_msg,sizeof(GC_msg_t));
    cJSON_PrintPreallocated(json, (char *)obuff, 1024, 0);
    *osize  = strlen((char *)obuff) + 1;
    cJSON_Delete(json);
} 

static void __strategy_pk(uint8_t* obuff,uint8_t *ibuff,uint16_t* osize,QueMsg *msg)
{
    //组织一个cjson结构体
    GC_msg_t gc_msg;
    if(msg->cls == GC_CLS_CMDRESP)
    {
        memcpy(gc_msg.cls,"CmdResp",8);
        gc_msg.sn = msg->sn;
    }
    else
    {
        memcpy(gc_msg.cls,"Report",7);
        gc_msg.sn = Get_Self_Sn();
    }
    gc_msg.id = Get_Self_Id();
    gc_msg.ts = Get_Self_Ts();
    memcpy(gc_msg.rpt,"Strategy",9);
    Strategypull(&gc_msg.Strategy);

    cJSON *json = cJSON_CreateObject();   
    jsonb_opt_GC_msg_t(JSONB_OPT_S2J,json,&gc_msg,sizeof(GC_msg_t));
    cJSON_PrintPreallocated(json, (char *)obuff, 1024, 0);
    *osize  = strlen((char *)obuff) + 1;
    cJSON_Delete(json);
} 


static void __login_pk(uint8_t* obuff,uint8_t *ibuff,uint16_t* osize,QueMsg *msg)
{
    //组织一个cjson结构体
    GC_msg_t gc_msg;
    memcpy(gc_msg.cls,"Report",7);
    gc_msg.sn = Get_Self_Sn();
    gc_msg.id = Get_Self_Id();
    gc_msg.ts = Get_Self_Ts();
    memcpy(gc_msg.rpt,"Login",6);
    Loginpull(&gc_msg.Login);

    cJSON *json = cJSON_CreateObject();   
    jsonb_opt_GC_msg_t(JSONB_OPT_S2J,json,&gc_msg,sizeof(GC_msg_t));
    cJSON_PrintPreallocated(json, (char *)obuff, 1024, 0);
    *osize  = strlen((char *)obuff) + 1;
    cJSON_Delete(json);
} 

static void __rep_pk(uint8_t* obuff,uint8_t *ibuff,uint16_t* osize,QueMsg *msg)
{
    //组织一个cjson结构体
    GC_msg_t gc_msg;
    memcpy(gc_msg.cls,"CmdResp",8);
    gc_msg.sn = msg->sn;
    gc_msg.id = Get_Self_Id();
    gc_msg.ts = Get_Self_Ts();
    memcpy(gc_msg.rpt,"Ack",4);
    memcpy(gc_msg.Ack.ack,"ok",3);

    cJSON *json = cJSON_CreateObject();   
    jsonb_opt_GC_msg_t(JSONB_OPT_S2J,json,&gc_msg,sizeof(GC_msg_t));
    cJSON_PrintPreallocated(json, (char *)obuff, 1024, 0);
    *osize  = strlen((char *)obuff) + 1;
    cJSON_Delete(json);
} 
//uint8_t gprs_kkkuf[512] = {0};

static void __unpk(void** obuff,uint8_t *ibuff,QueMsg *msg)
{
    //组织一个cjson结构体
    QueMsg * pMsgSend = NULL;
    GC_msg_t gc_msg;
                 // memcpy(gprs_kkkuf,ibuff,260);
    cJSON * json = cJSON_Parse((const char *)ibuff);
    jsonb_opt_GC_msg_t(JSONB_OPT_J2S,json,&gc_msg,sizeof(GC_msg_t));
    if(!strcmp(gc_msg.rpt,"Config"))
    {
        //自定义模式的配置参数，将数据配置保存好，并回复ack

        Configpush(&gc_msg.Config);
        Configflush();
        cJSON_Delete(json);

        //回复消息
        pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
        pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
        pMsgSend->subtype = GC_ACK;
        pMsgSend->sn = gc_msg.sn;
    }
    if(!strcmp(gc_msg.rpt,"RfState"))
    {
        //雨雾因素下发，等待常态线程进行判断雨量条件下发配置信息
        RfStatepush(&gc_msg.RfState);
        //最后刷入
        if(gc_msg.RfState.vision == 4)
        RfStateflush();
        cJSON_Delete(json);
        
        //回复消息
        pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
        pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
        pMsgSend->subtype = GC_ACK;
        pMsgSend->sn = gc_msg.sn;
    }
    if(!strcmp(gc_msg.rpt,"Mode"))
    {
        cJSON_Delete(json);
        //保存模式的配置
        Modepush(&gc_msg.Mode);
        Modeflush();
        ModeChange(gc_msg.Mode.mode);
        NowModeflush();
        if(gc_msg.Mode.mode <= 3)
        {
            pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(auto_mode));
            auto_mode *atmode = (auto_mode*)pMsgSend->data;
            //gc2gs
            atmode->mode_num = gc_msg.Mode.mode;
            pMsgSend->subtype = GP_SET_MODE_AUTO;

            
        }
        else
        {
            pMsgSend = pvPortMalloc(sizeof(QueMsg)+sizeof(custom_mode));
            custom_mode *csmode = (custom_mode*)pMsgSend->data;
            Config_t cfg;
            //gc2gs
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
        }
        
        pMsgSend->type = MSG_TYPE_LORA_STATE_QUERY_CONTROL;
        pMsgSend->src = Get_Self_Id();
        pMsgSend->sn = gc_msg.sn;
      //发给lora处理线程，将数据配置下发给节点
    }
    if(!strcmp(gc_msg.rpt,"Control"))
    {

        pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
        cJSON_Delete(json);
        pMsgSend->type = MSG_TYPE_LORA_STATE_QUERY_CONTROL;
        pMsgSend->subtype = GP_SET_SYS_ON;
        pMsgSend->src = Get_Self_Id();
        pMsgSend->sn = gc_msg.sn;
        pMsgSend->data[0] = (gc_msg.Control.type == "on")?2:1;
        SysControlflush(pMsgSend->data[0]);
        //保持开关等策略
    }
    if(!strcmp(gc_msg.rpt,"Strategy"))
    {

       //只保存配置
        Strategypush(&gc_msg.Strategy);
        Strategyflush();
        cJSON_Delete(json);
        //保持开关等策略
        //回复云端消息
        pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
        pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
        pMsgSend->subtype = GC_ACK;
        pMsgSend->sn = gc_msg.sn;
    }
    
    if(!strcmp(gc_msg.rpt,"Query"))
    {
        cJSON_Delete(json);
        if(!strcmp(gc_msg.Query.type,"GwState"))
        {
            pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);//+1防止野指针操作，其实没必要，真来了也防不住
            pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
            pMsgSend->subtype = GC_G_STAT;
        }
        if(!strcmp(gc_msg.Query.type,"NodeState"))
        {
            //to do 临时变量 用以上报循环节点数据
            pstate_sn_tmp = gc_msg.sn;
            NodeUpdate = 1;
        }
        if(!strcmp(gc_msg.Query.type,"Config"))
        {
            pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);//+1防止野指针操作，其实没必要，真来了也防不住
            pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
            pMsgSend->subtype = GC_CONFIG;

        }
        if(!strcmp(gc_msg.Query.type,"RfState"))
        {
            rf_sn_tmp = gc_msg.sn;
            RfUpdate = 1;
        }
        if(!strcmp(gc_msg.Query.type,"Mode"))
        {
            pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
            pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
            pMsgSend->subtype = GC_MODE;
        }
        if(!strcmp(gc_msg.Query.type,"Strategy"))
        {
            pMsgSend = pvPortMalloc(sizeof(QueMsg)+1);
            pMsgSend->type = MSG_TYPE_MQTT_STATE_REPORT;
            pMsgSend->subtype = GC_STRATEGY;
        }
        pMsgSend->cls = GC_CLS_CMDRESP;
        pMsgSend->sn = gc_msg.sn;
    }
    *obuff = pMsgSend;
} 

//to do return null que1 que2


GC_map gcmap[10]=
{
    {GC_G_STAT,__gate_state,NULL},
    {GC_P_STAT,__point_state,NULL},
    {GC_CONFIG,__config_pk,NULL},
    {GC_RFSTATE,__rf_pk,NULL},
    {GC_MODE,__mode_pk,NULL},
    {GC_STRATEGY,__strategy_pk,NULL},
    {GC_QUERY,NULL,NULL},
    {GC_LOGIN,__login_pk,NULL},
    {GC_ACK,__rep_pk,NULL},

};


void mqtt_MsgPacket(QueMsg *msg,uint8_t *obuff,uint8_t *ibuff,uint16_t* osize)
{
    int i = 0;
    for(i = 0; i<10; i++)
    {
        if(msg->subtype == gcmap[i].type)
        {
            gcmap[i].packet(obuff,ibuff,osize,msg);
            break;
        }

    }
    
}

void mqtt_MsgUnPacket(QueMsg *msg,void **obuff)
{
    __unpk(obuff,(uint8_t *)msg->data, msg);
}

