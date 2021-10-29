#include "DataPub.h"
#include <assert.h>
#include "DataApp.h"
#include "SX127X_Hal.h"
#include "SX127X_Driver.h"
#include "SX127X_2_Hal.h"
#include "MsgProc.h"

#include "GP_protocol.h"
#include  "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "stdlib.h"
#include "localdata.h"
#define LORA_TX_DONE           1
#define LORA_RX_DONE           2 

#define RETRY_NUM              2
//to do后续改成3
extern QueueHandle_t Queue_Lora;
extern QueueHandle_t Queue_Mqtt;
static uint16_t lenn;
extern SemaphoreHandle_t  xSemaphore_Lora1_rx;
extern SemaphoreHandle_t  xSemaphore_Lora2_rx;
extern SemaphoreHandle_t  xSemaphore_Lora1_tx;
extern SemaphoreHandle_t  xSemaphore_Lora2_tx;
void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
        //taskENTER_CRITICAL();
    if(DIO0_GetState() == GPIO_PIN_SET)
    {
        
        uint8_t flag;
        SX127X_Read(REG_LR_IRQFLAGS, &flag);
        SX127X_Write(REG_LR_IRQFLAGS, 0xff); //clear flags
        if(flag & RFLR_IRQFLAGS_TXDONE)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(xSemaphore_Lora1_tx,&xHigherPriorityTaskWoken);
        }
        else if(flag & RFLR_IRQFLAGS_RXDONE)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(xSemaphore_Lora1_rx,&xHigherPriorityTaskWoken);

        }
        else
        {
            SX127X_StartRx();
        }
    }

    if(DIO0_2_GetState() == GPIO_PIN_SET)
    {
        
        uint8_t flag2;
        SX127X_2_Read(REG_LR_IRQFLAGS, &flag2);
        SX127X_2_Write(REG_LR_IRQFLAGS, 0xff); //clear flags
        if(flag2 & RFLR_IRQFLAGS_TXDONE)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(xSemaphore_Lora2_tx,&xHigherPriorityTaskWoken);
        }
        else if(flag2 & RFLR_IRQFLAGS_RXDONE)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(xSemaphore_Lora2_rx,&xHigherPriorityTaskWoken);

        }
        else
        {
            SX127X_2_StartRx();
        }
    }
     //taskEXIT_CRITICAL();
}

static void Lora_WRInit(void)
{

    uint8_t wrInitRxBuffer[256];
    lenn = 0;
    while(1)
    {
        if(xSemaphoreTake( xSemaphore_Lora1_rx,0 ) != pdPASS)     //清除信号量
            break;
        lenn = SX127X_RxPacket_cjy(wrInitRxBuffer); 
    }
    while(1)
    {
        if(xSemaphoreTake( xSemaphore_Lora1_tx,0 ) != pdPASS)     //清除信号量
            break;
    }
}

static void Lora2_WRInit(void)
{

    uint8_t wrInitRxBuffer[256];
    lenn = 0;
    while(1)
    {
        if(xSemaphoreTake( xSemaphore_Lora2_rx,0 ) != pdPASS)     //清除信号量
            break;
        lenn = SX127X_2_RxPacket_cjy(wrInitRxBuffer); 
    }
    while(1)
    {
        if(xSemaphoreTake( xSemaphore_Lora2_tx,0 ) != pdPASS)     //清除信号量
            break;
    }
}
//=============================================================
//函 数 名: Lora_SendAndWait
//输入参数: cmd      :发送的命令字符串
//			strwait  :需要等待的字符串
//			num_sema :总共需要同步的信号量的个数，有的数据回应时，数据包会被分成几个部分
//			trynum   :重传的次数
//			timeout  :超时时间，信号量等待的时间
//返 回 值: 0:成功，1：失败
//=============================================================
static uint8_t Lora_SendAndWait(uint8_t *cmd,uint8_t size,uint8_t* data,uint8_t trynum,uint32_t timeout,uint8_t type_id)
{

    uint8_t lenth = 0;
	for(int i = 0 ; i < trynum ; i++)
	{
		//尝试发送
		Lora_WRInit();        //清除缓冲区
		SX127X_TxPacket_cjy(cmd,size);
        if(xSemaphoreTake( xSemaphore_Lora1_tx,timeout) != pdPASS)
            break;
        SX127X_StartRx();
        while(1)
        {
            if(xSemaphoreTake( xSemaphore_Lora1_rx,timeout) != pdPASS)
                 break;       
            lenth = SX127X_RxPacket_cjy(data);
            uint32_t dst = data[5]|data[6]<<8|data[7]<<16|data[8]<<24;
            if(dst == Get_Self_Id()&&(data[11]& 0x7f) == type_id)
            {
                //SX127XWriteRxTx(true);
                //Lora_WRInit();
                return lenth;
            }
                

        }
             
	}
    //SX127XWriteRxTx(true);
    //Lora_WRInit();
        
	return 0; 
}

static uint8_t Lora_Send(uint8_t *cmd,uint8_t size,uint8_t trynum,uint32_t timeout)
{

	for(int i = 0 ; i < trynum ; i++)
	{
        //尝试发送
		Lora_WRInit();        //清除缓冲区
		SX127X_TxPacket_cjy(cmd,size);
        if(xSemaphoreTake( xSemaphore_Lora1_tx,timeout) == pdPASS)
            break;
	}
    //Lora_WRInit();
        
	return 1; 
}

static uint8_t Lora2_Send(uint8_t *cmd,uint8_t size,uint8_t trynum,uint32_t timeout)
{

	for(int i = 0 ; i < trynum ; i++)
	{
        //尝试发送
		Lora2_WRInit();        //清除缓冲区
		SX127X_2_TxPacket_cjy(cmd,size);
        if(xSemaphoreTake( xSemaphore_Lora2_tx,timeout) == pdPASS)
            break;
	}
    //Lora_WRInit();
        
	return 1; 
}
//和节点通讯用    

void vTaskLoraApp1( void * pvParameters )
{
    (void)pvParameters;
    BaseType_t xResult;
    uint8_t lenth = 0;
    QueMsg * pMsg = NULL;
    QueMsg * pMsgSend = NULL;
    point_state pstate;
    uint8_t RxBuffer[256];
    uint8_t TxBuffer[64];

    uint8_t ret;
    SX127X_Lora_init();
    DIO0_EnableInterrupt();
    //vTaskDelay(pdMS_TO_TICKS(7000));

    while(1)
    {
        xResult = xQueueReceive(Queue_Lora,                   /* 消息队列句柄 */
                                (void *)&pMsg,  /* 存储接收到的数据到变量ucQueueMsgValue中 */
                                (TickType_t)portMAX_DELAY );/* 设置阻塞时间 */
        if(NULL == pMsg)
            continue;

        if(xResult == (BaseType_t)1)
        {
            if(pMsg->subtype > 10)
            {
#ifdef CJY_DEBUG
                printf("subtype failed %d\n",pMsg->subtype);
#endif
                continue;
            }
            switch(pMsg->type)
            {
                case MSG_TYPE_LORA_BROADCAST_TIME:
                {
                    lenth = GP_TxPacket(TxBuffer,pMsg->subtype,pMsg->src,0xffffffff,pMsg->data);
                    Lora_Send(TxBuffer,lenth,3,2000);
                    break;
                }      
                case MSG_TYPE_LORA_STATE_QUERY_STATE:
                {

                    lenth = GP_TxPacket(TxBuffer,pMsg->subtype,pMsg->src,pMsg->dst,pMsg->data);
                    lenth = Lora_SendAndWait(TxBuffer,lenth,RxBuffer,RETRY_NUM,2000,pMsg->subtype);
                    if(lenth>0)
                    {
                        ret = GP_RxPacket(RxBuffer,pMsg->subtype,lenth,&pstate);
                        //解出pstate数据，并刷新到本地内存
                        if(ret > 0)
                        {
                            PointStatePush((void *)&pstate);
                        }

    
                    }
                    //查完立即上报模式 没查到的就不报了
                    //PointStateOne(pMsg->dst);
                }
                break;
                case MSG_TYPE_LORA_STATE_QUERY_CONTROL:
                {

                    lenth = GP_TxPacket(TxBuffer,pMsg->subtype,pMsg->src,0xffffffff,pMsg->data);
                    Lora_Send(TxBuffer,lenth,3,2000);
          
                    break;
                }
                default :
                    break;
            }
            vPortFree(pMsg);
            pMsg = NULL;
            SX127X_StandbyMode();//to do 后续改成平时接收模式，起一个线程接受判断处理，需要发送同步等待消息的，新起一个信号量。
        }
        else
        {
            /* 超时 */

        }
        vTaskDelay(pdMS_TO_TICKS(50));

    }
}
void vTaskLoraApp2( void * pvParameters )
{
    (void)pvParameters;
    BaseType_t xResult;
    uint8_t lenth = 0;
    uint8_t RxBuffer[256];
    uint8_t TxBuffer[64];
    uin32_t Txsrc,Txdst;

    uint8_t ret;
    SX127X_2_Lora_init();
    DIO0_2_EnableInterrupt();
    //vTaskDelay(pdMS_TO_TICKS(7000));
    SX127X_2_StartRx();
    while(1)
    {
        memset(TxBuffer,0,64);
        if(xSemaphoreTake( xSemaphore_Lora2_rx,(TickType_t)0xffffffffUL) == pdPASS)
        {  
            lenth = SX127X_2_RxPacket_cjy(RxBuffer);
            uint32_t dst = RxBuffer[5]|RxBuffer[6]<<8|RxBuffer[7]<<16|RxBuffer[8]<<24;
            if(dst == Get_Self_Id()|| dst == 0xffffffff)
            {
                ret = GP_RxPacket(RxBuffer,RxBuffer[11],TxBuffer);
                if(ret == 0)
                    continue;
                if(TxBuffer[0] != 0)
                {
                    Lora_2_Send(TxBuffer,ret,3,2000);
                    SX127X_2_StartRx();
                }
            }

        }
    }
}