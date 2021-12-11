#include "MQTTClient.h"
#include "sim7600.h"
#include "mqtt_gprs_interface.h"
#include "DataPub.h"
#include <assert.h>
#include "jsonb_c_base.h"
#include "DataApp.h"
#include "MsgProc.h"
#include "localdata.h"


#define FAILED_NUM_MAX 128

extern QueueHandle_t Queue_Mqtt_rc;
volatile SemaphoreHandle_t xGprsMutex;
extern SemaphoreHandle_t  xSemaphore_barrier_local;
uint8_t gprs_obuf[512] = {0};
static uint8_t gprs_is_connected = 0;
static uint8_t gprs_connect_failed = 0;
static uint8_t mqtt_connect_failed = 0;
static uint8_t mqtt_subscribe_failed = 0;
uint16_t gprs_olen = 0;
MQTTClient gprsclient;
void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
//void LogIn();
/* MQTT服务器推送的消息 */
uint8_t gprs_is_online()
{
    return gprs_is_connected;
}
static void messageArrived(MessageData* data)
{

    QueMsg * pMsgSend = NULL;
    uint8_t* pdate = (uint8_t*)data->message->payload;
    pMsgSend = pvPortMalloc(sizeof(QueMsg)+data->message->payloadlen);
    pMsgSend->type = MSG_TYPE_MQTT_STATE_COMMAND;
    memcpy(pMsgSend->data,pdate,data->message->payloadlen);
        //int a  = 0;
    /* 消息发给MQTT队列 */
    if(xQueueSend(Queue_Mqtt_rc,(void *) &pMsgSend,(TickType_t)100)!= pdPASS)
    {
        vPortFree(pMsgSend);
    }
    
    
}
void system_restart()
{
    __disable_irq();
    HAL_NVIC_SystemReset();
}

void vTaskCodeGPRS( void * pvParameters )
{    
    //xGprsMutex = xSemaphoreCreateMutex();
    //assert(xGprsMutex != NULL);
    
    (void)pvParameters;
    
    Gsm_TurnON();      //模块开机


	Network gprs_network;
    
	uint8_t sendbuf[512], readbuf[512];
	int rc = 0;

	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
    connectData.willFlag = 1;
    connectData.will.topicName.cstring = "home/die/";
    connectData.will.message.cstring = "fuck,i m die!";
    gprs_NewNetwork(&gprs_network,1);
    MQTTClientInit(&gprsclient, &gprs_network, 10000,sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

    char address[] = "47.98.103.98";
        
	//char address[] = "116.62.38.211";//36013


    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        if ((rc = gprs_ConnectNetwork(&gprs_network, (char*)address, 1883)) != 0)
        {
            gprs_connect_failed++;
#ifdef CJY_DEBUG
            printf("gprs connect fail , try again! ip = %s , port = 1883\n",address);
#endif
            if(gprs_connect_failed >= FAILED_NUM_MAX)
                system_restart();
            continue;
        }   
        else
            break;          
    }
#ifdef CJY_DEBUG
    printf("gprs connect success! ip = %s , port = 36013\n",address);
#endif

#if defined(MQTT_TASK)
	if ((rc = MQTTStartTask(&gprsclient)) != pdPASS)
#ifdef CJY_DEBUG
		printf("Return code from start tasks is %d\n", rc);
#endif
#endif
    
    connectData.MQTTVersion = 3;
    char name[32] = "gw_";
    sprintf(name,"%s%s",name,Get_Self_Uid());
    connectData.clientID.cstring = name;
    
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        if ((rc = MQTTConnect(&gprsclient, &connectData)) != 0)
        {
            mqtt_connect_failed++;
#ifdef CJY_DEBUG
            printf("mqtt connect fail , try again!%d\n",rc);
#endif
            if(mqtt_connect_failed >= FAILED_NUM_MAX)
                system_restart();
            continue; 
        }
        else
            break;   
    }
#ifdef CJY_DEBUG
    printf("mqtt connect success!\n");
#endif
    char  str[64];
    char* strr = "hwtopic/down/hwiot/gw/";
    sprintf(str,"%s%u",strr,(Get_Self_Id()));
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if ((rc = MQTTSubscribe(&gprsclient, (const char*)str, QOS0, messageArrived)) != 0)
        {
            mqtt_subscribe_failed++;
#ifdef CJY_DEBUG
            printf("mqtt subcribe fail , try again! topic = %s,rc=%d\n",str,rc);
#endif
            if(mqtt_subscribe_failed >= FAILED_NUM_MAX)
                system_restart();
            continue; 
        }
        else
            break;   
    }    
#ifdef CJY_DEBUG   
    printf("mqtt subcribe success! topic = %s\n",str);
#endif
    //发送信号量 让其他线程能够正常运转启动
    //xSemaphoreGive(xSemaphore_barrier_local);
    gprs_is_connected = 1;

	while (1)
	{
#if !defined(MQTT_TASK)
		if ((rc = MQTTYield(&gprsclient, 1000)) != 0)
#ifdef CJY_DEBUG
			printf("Return code from yield is %d\n", rc);
#endif
#endif
        if(!gprsclient.isconnected)
        {
             //MQTT Disconnect,reconnect
//            __disable_irq();
//            HAL_NVIC_SystemReset();
            
        }
        Pub_Poll();
        vTaskDelay(pdMS_TO_TICKS(50));
	}
}