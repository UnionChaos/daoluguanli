#include "includes.h"
//TODO
//第二个lora，并且支持多实例
//新开一个lora线程，接受数据并处理，现在的发送等回同步通过信号量来处理。

//TODO
//485能见度传感器

//TODO
//日志模块,并兼容printf和uart输出，用宏控制

//TODO
//定时器来分解locadata任务

//TODO
//代码整理

//TODONOW
//4G  物联网卡
//GPS NOTICE

#define QUE_LENTH       64


/*主线程，负责创建其他线程*/
void vTaskCodeMain( void * pvParameters );
/*
*1.定时授时 to do 在某个时间段
*2.定时轮询节点状态 to do 在某个非工作时间点，白天。
*3.定时维护local状态表 to do
*3.1 电压 1小时频率
*3.2 GPSINFO 1小时频率
*3.3 配置表（无需维护，实时刷新至E2）
*/
/*WTD*/
void vTaskCodeWTD( void * pvParameters );
void vTaskCodeLocal( void * pvParameters );
/*4G 模块无线任务*/
extern void vTaskCodeGPRS( void * pvParameters );  
/*以太网任务*/
extern void vTaskCodeETH( void * pvParameters );     
/*lora任务1，和终端节点通讯*/
void vTaskLoraApp1( void * pvParameters );
/*lora任务2，和手持仪器通讯*/
void vTaskLoraApp2( void * pvParameters );
/*异步处理mqtt接收到的消息线程*/
void vTaskMqttMsg( void * pvParameters );

void vTaskCodeGps( void * pvParameters );
void vTaskVision(void * pvParameters);
/*UART2(4G串口)异步通知信号量*/
SemaphoreHandle_t  xSemaphore_4G;  
/*LORA接受结束标志*/
SemaphoreHandle_t  xSemaphore_Lora1_rx;
SemaphoreHandle_t  xSemaphore_Lora2_rx;
/*LORA发送结束标志*/
SemaphoreHandle_t  xSemaphore_Lora1_tx;
SemaphoreHandle_t  xSemaphore_Lora2_tx;
SemaphoreHandle_t  xSemaphore_local;
SemaphoreHandle_t  xSemaphore_vision;
SemaphoreHandle_t  xSemaphore_barrier_local;

/*Lora消息队列(发送给节点)
*1.授时命令
*2.轮询节点状态：查
*3.控制命令 
*3.1 及时平台控制命令
*3.2 条件判断逻辑命令
*/
QueueHandle_t Queue_Lora;



/*Mqtt publish消息队列（发送给cloud）
*1.网关状态
*2.轮询节点状态：报
*3.自定义控制配置：报
*4.自动模式配置：报
*5.策略时间：报
*/
QueueHandle_t Queue_Mqtt;


/*Mqtt 异步处理队列（分情况）
*1.保存配置
*2.发送给lora队列
*3.发送给mqttpublish队列
*/
QueueHandle_t Queue_Mqtt_rc;
void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
TaskHandle_t xHandleMain;
TaskHandle_t xHandleWTD;
TaskHandle_t xHandleGprs;
TaskHandle_t xEnternetHandle;
TaskHandle_t xHandleMqttMsg;
TaskHandle_t xHandleLora1;
TaskHandle_t xHandleLora2;
TaskHandle_t xHandleVision;
TaskHandle_t xHandleLocal;
TaskHandle_t xHandleGps;
#ifdef CJY_DEBUG


unsigned portBASE_TYPE uxHighMain;
unsigned portBASE_TYPE uxHighWTD;
unsigned portBASE_TYPE uxHighGprs;
unsigned portBASE_TYPE uxHighMqtt;
unsigned portBASE_TYPE uxHighLora;
unsigned portBASE_TYPE uxHighLocal;
unsigned portBASE_TYPE uxHighGPS;
unsigned portBASE_TYPE uxHighTimer;
unsigned portBASE_TYPE uxHighIDLE;
#endif
int main(void)
{

    HAL_Init();
    BSP_SystemClkCfg();
    BSP_Init();
    //Debug_Init();
    EEPROM_Init();

    //Pumb_Init();
    //to do  RTCINIT

    //to do ADC
    //to do pin review

    Queue_Lora = xQueueCreate(QUE_LENTH, sizeof(QueMsg *));
    if( Queue_Lora == 0 )
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }

    Queue_Mqtt = xQueueCreate(QUE_LENTH, sizeof(QueMsg *));
    if( Queue_Mqtt == 0 )
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }

    Queue_Mqtt_rc = xQueueCreate(QUE_LENTH, sizeof(QueMsg *));
    if( Queue_Mqtt_rc == 0 )
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
#ifdef CJY_DEBUG
    vQueueAddToRegistry(Queue_Lora,"Loraque");
    vQueueAddToRegistry(Queue_Mqtt,"Mqttque");
    vQueueAddToRegistry(Queue_Mqtt_rc,"Rcque");
#endif
    xSemaphore_4G = xSemaphoreCreateCounting(20,0);
    assert(xSemaphore_4G != NULL);
    
    xSemaphore_Lora1_rx = xSemaphoreCreateCounting(20,0);
    assert(xSemaphore_Lora1_rx != NULL);
    xSemaphore_Lora2_rx = xSemaphoreCreateCounting(20,0);
    assert(xSemaphore_Lora2_rx != NULL);
    xSemaphore_Lora1_tx = xSemaphoreCreateCounting(20,0);
    assert(xSemaphore_Lora1_tx != NULL);
    xSemaphore_Lora2_tx = xSemaphoreCreateCounting(20,0);
    assert(xSemaphore_Lora1_tx != NULL);
    xSemaphore_local = xSemaphoreCreateCounting(20,0);
    assert(xSemaphore_local != NULL);
    xSemaphore_vision = xSemaphoreCreateCounting(20,0);
    assert(xSemaphore_vision != NULL);
    xSemaphore_barrier_local = xSemaphoreCreateCounting(4,0);
    assert(xSemaphore_barrier_local != NULL);

    BaseType_t err = xTaskCreate( vTaskCodeMain,"Main",256,NULL,2,&xHandleMain );
    assert(err == pdPASS);


    err = xTaskCreate( vTaskCodeWTD,"WTG",256,NULL,2,&xHandleWTD );
    assert(err == pdPASS);

    if(1/*!eth_choose()*/)
    {

        err = xTaskCreate( vTaskCodeGPRS,"gprs",1024,NULL,2,&xHandleGprs );
        assert(err == pdPASS);
    }
    else
    {

        err = xTaskCreate( vTaskCodeETH,"EnternetMQTT",1024,NULL,2,&xEnternetHandle);
        assert(err == pdPASS);
    }



    err = xTaskCreate( vTaskMqttMsg,"MqttMsg",512,NULL,2,&xHandleMqttMsg );
    assert(err == pdPASS);


    err = xTaskCreate( vTaskLoraApp1,"lora1",512,NULL,2,&xHandleLora1 );
    assert(err == pdPASS);

    err = xTaskCreate( vTaskLoraApp2,"lora2",512,NULL,2,&xHandleLora2 );
    assert(err == pdPASS);

    err = xTaskCreate( vTaskCodeLocal,"local",512,NULL,2,&xHandleLocal );
    assert(err == pdPASS);

    err = xTaskCreate( vTaskVision,"vision",256,NULL,2,&xHandleVision );
    assert(err == pdPASS);

    err = xTaskCreate( vTaskCodeGps,"localGps",512,NULL,2,&xHandleGps );
    assert(err == pdPASS);
    vTaskStartScheduler();
	return 0;
}


void vTaskMqttMsg( void * pvParameters )
{
    (void)pvParameters;
    BaseType_t xResult;
    QueMsg * pMsg = NULL;
    QueMsg * pMsgSend = NULL;
    QueMsg * pMsgRep = NULL;
    
    
    while(1)
    {
        xResult = xQueueReceive(Queue_Mqtt_rc,                   /* 消息队列句柄 */
                                (void *)&pMsg,  /* 存储接收到的数据到变量ucQueueMsgValue中 */
                                (TickType_t)portMAX_DELAY );/* 设置阻塞时间 */   
        if(NULL == pMsg)
            continue;

        if(xResult == (BaseType_t)1)
        {
            mqtt_MsgUnPacket(pMsg,(void **)&pMsgSend); 
            if(NULL != pMsgSend)//空说明是保存配置的情况，无需发送消息
            {
                switch(pMsgSend->type)
                {
                    case MSG_TYPE_LORA_STATE_QUERY_CONTROL:
                        {
                            //回复云端消息
                            pMsgRep = pvPortMalloc(sizeof(QueMsg)+1);
                            pMsgRep->type = MSG_TYPE_MQTT_STATE_REPORT;
                            pMsgRep->subtype = GC_ACK;
                            pMsgRep->sn = pMsgSend->sn;//这里取巧获取sn号，从unpacket中截取
                            if( xQueueSend(Queue_Mqtt,(void *) &pMsgRep,(TickType_t)100) != pdPASS)
                            {
                                /* 发送失败，即使等待了100个时钟节拍 */
                                vPortFree(pMsgRep);
                            }
                            pMsgRep = NULL;
                        }
                        if( xQueueSend(Queue_Lora,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
                        {
                        /* 发送失败，即使等待了100个时钟节拍 */
                            vPortFree(pMsgSend);
                        }

                        break;
                    case MSG_TYPE_MQTT_STATE_REPORT:
                        if( xQueueSend(Queue_Mqtt,(void *) &pMsgSend,(TickType_t)100) != pdPASS)
                        {
                        /* 发送失败，即使等待了100个时钟节拍 */
                            vPortFree(pMsgSend);
                        }
                        break;
                    default:
                        break;
                }
            }
            vPortFree(pMsg);
            pMsgSend = NULL;
        }
        else
        {
            /* 超时 */

        }
        vTaskDelay(pdMS_TO_TICKS(50));
        pMsgSend = NULL;
    }
}
extern void PointStateResp();
extern void RfResp();
void vTaskCodeMain( void * pvParameters )
{
    (void)pvParameters;
    rx8010_Init();
    while(1)
    {      

        BSP_LED_On(1);
        vTaskDelay(pdMS_TO_TICKS(1500));
        BSP_LED_Off(1);
        vTaskDelay(pdMS_TO_TICKS(1500));
        PointStateResp();
        RfResp();

#ifdef CJY_DEBUG
    /*uxHighMain = uxTaskGetStackHighWaterMark( xHandleMain );
    uxHighGprs = uxTaskGetStackHighWaterMark( xHandleGprs );
    uxHighWTD = uxTaskGetStackHighWaterMark( xHandleWTD );
    uxHighMqtt = uxTaskGetStackHighWaterMark( xHandleMqttMsg );
    uxHighLora = uxTaskGetStackHighWaterMark( xHandleLora1 );
    uxHighLocal = uxTaskGetStackHighWaterMark( xHandleLocal );
    uxHighGPS = uxTaskGetStackHighWaterMark( xHandleGps );*/
#endif


    }
}

void vTaskCodeWTD( void * pvParameters )
{
    (void)pvParameters;

    while(1)
    {      
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
        vTaskDelay(pdMS_TO_TICKS(900));

    }
}

static volatile unsigned int _continue;
void HardFault_Handler(void)
{
  _continue = 0;
  while(_continue == 0u)
  {
  }
}



