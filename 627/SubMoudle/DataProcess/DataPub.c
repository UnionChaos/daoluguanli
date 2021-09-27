#include "stm32f429xx.h"
#include "DataPub.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "MQTTClient.h"
#include "MsgProc.h"
#include "GC_protocol.h"
#include "wizchip_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern QueueHandle_t Queue_Mqtt;
extern MQTTClient gprsclient;
extern MQTTClient client;

extern uint8_t gprs_obuf[512];
extern uint16_t gprs_olen;
void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
uint8_t eth_choose()
{
	if(!wizchip_init(NULL,NULL))
		return 0;
    if(wizphy_getphylink() == PHY_LINK_ON)
		return 1;
	else 
		return 0;
}

void hw_system_rest()
{
	__disable_irq();
	HAL_NVIC_SystemReset();
}

static uint16_t publish_failed_cnt = 0;
void Pub_Poll(void)
{
	BaseType_t xResult;
	QueMsg * pMsg = NULL;
	if(publish_failed_cnt >= 64)
	{
          __disable_irq();
          HAL_NVIC_SystemReset();
	}
	xResult = xQueueReceive(Queue_Mqtt,                   /* 消息队列句柄 */
                                (void *)&pMsg,  /* 存储接收到的数据到变量ucQueueMsgValue中 */
                                (TickType_t)portMAX_DELAY );/* 设置阻塞时间 */
	if(NULL == pMsg)
            return;
	if(xResult == (BaseType_t)1)
	{
		mqtt_MsgPacket(pMsg,gprs_obuf,(uint8_t *)pMsg->data,&gprs_olen);
		MQTTMessage message;
		message.qos = QOS1;
		message.retained = 0;
		message.payload = gprs_obuf;
		message.payloadlen = gprs_olen-1;
		if (MQTTPublish(&gprsclient, "hwtopic/up/hwiot/gw/", &message) != 0)
		{
#ifdef CJY_DEBUG
        	printf("Publish failure %d\n");
#endif
			publish_failed_cnt++;
			
		}

		memset(gprs_obuf,0,sizeof(gprs_obuf));
    	gprs_olen = 0;	
		vPortFree(pMsg);
	}
	else
	{
		/*超时*/
	}
}