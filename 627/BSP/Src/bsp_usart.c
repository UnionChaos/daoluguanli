#include "bsp_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern SemaphoreHandle_t  xSemaphore_4G;
extern SemaphoreHandle_t  xSemaphore_local;
extern SemaphoreHandle_t  xSemaphore_vision;
static UART_HandleTypeDef huart1;//FOR 4G
static UART_HandleTypeDef huart2;//FOR 4851
static UART_HandleTypeDef huart3;//FOR gps
static TIM_HandleTypeDef  htim2;
static TIM_HandleTypeDef  htim3;

#define UART_TIME_OUT  200   //串口超时时间

static volatile uint16_t rx_index1 = 0;
static uint8_t rx_buf1[1024] = {0};


static volatile uint16_t rx_index2 = 0;
static uint8_t rx_buf2[512] = {0};


static volatile uint16_t rx_index3 = 0;
static uint8_t rx_buf3[512] = {0};


static void USART_Init(void)
{
    __HAL_RCC_UART4_CLK_ENABLE();

    /**UART4 GPIO Configuration    
    PA0     ------> UART4_TX
    PA1    ------> UART4_RX 
    */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
    huart1.Instance = UART4;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);

    
    __HAL_RCC_UART8_CLK_ENABLE();

    /*USART8 GPIO Configuration    
    PE0     ------> UART8_TX
    PE1     ------> UART8_RX 
    */

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART8;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
        
    huart3.Instance = UART8;
    huart3.Init.BaudRate = 9600;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart3);
    
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
    HAL_NVIC_SetPriority(UART8_IRQn, 6, 0);

        __HAL_RCC_UART7_CLK_ENABLE();

    /**USART7 GPIO Configuration    
    PE8     ------> UART7_RX
    PE7     ------> UART7_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART7;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
        
    huart2.Instance = UART7;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart2);
    //能见度仪用被动召测的形式
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    HAL_NVIC_SetPriority(UART7_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(UART7_IRQn);
    
}

static void TIM_Init(void)
{
    /*定时器2，用于UART2的同步,接收4G模块信息*/
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 8601 - 1; //定时器2再APB2总线上，时钟频率84MHz,分频都时钟为10K
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = UART_TIME_OUT * 10 - 1;  //超时时间 200ms
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
        assert(0);

    __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);

    HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    
    //定时器3，用于UART8的同步,接受gps
    __HAL_RCC_TIM3_CLK_ENABLE();
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 8601 - 1; //定时器3再APB1总线上，时钟频率84MHz,分频都时钟为10K
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = UART_TIME_OUT * 10 - 1;  //超时时间 200ms
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
        assert(0);
     __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);

    HAL_NVIC_SetPriority(TIM3_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
    


}
void BSP_USART_Init(void)
{
    USART_Init();  //初始化串口模块
    TIM_Init();
}

void UART7_IRQHandler(void)
{
    uint32_t tmp_flag = 0, tmp_it_source = 0;
    tmp_flag = __HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE);
	tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE);
    if((tmp_flag != RESET) && (tmp_it_source != RESET))
    {
        if((uint8_t)(huart2.Instance->DR & (uint8_t)0x00FF) == 'V')
            rx_index3 = 0;
        rx_buf3[rx_index3++] = (uint8_t)(huart2.Instance->DR & (uint8_t)0x00FF);
        if(rx_index3>=2)
        {
            if((rx_buf3[rx_index3-2] == 0x0D) && (rx_buf3[rx_index3-1] == 0x0A))
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xSemaphoreGiveFromISR(xSemaphore_vision,&xHigherPriorityTaskWoken);
            }
        }
    }
}

void UART8_IRQHandler(void)
{
    uint32_t tmp_flag = 0, tmp_it_source = 0;
    tmp_flag = __HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE);
	tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE);
    if((tmp_flag != RESET) && (tmp_it_source != RESET))
    {
        rx_buf1[rx_index1++] = (uint8_t)(huart3.Instance->DR & (uint8_t)0x00FF);
        if(rx_buf1[rx_index1-1] == 0x0A )
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(xSemaphore_local,&xHigherPriorityTaskWoken);
        }
    }
}


void UART4_IRQHandler(void)
{
    uint32_t tmp_flag = 0, tmp_it_source = 0;
	tmp_flag = __HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE);
	tmp_it_source = __HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_RXNE);
    if((tmp_flag != RESET) && (tmp_it_source != RESET))
    {
        __HAL_TIM_SET_COUNTER(&htim2, 0);
        if(rx_index2 == 0) //第一次接受数据，则打开定时器
            HAL_TIM_Base_Start_IT(&htim2);
        rx_buf2[rx_index2++] = (uint8_t)(huart1.Instance->DR & (uint8_t)0x00FF);
        if(rx_index2 >= sizeof(rx_buf2))
        {
            rx_index2 = 0;
            HAL_TIM_Base_Stop_IT(&htim2);
        }
    }
}


void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2);
}

void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim3);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
        if(rx_index2 != 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(xSemaphore_4G,&xHigherPriorityTaskWoken);
        }
        HAL_TIM_Base_Stop_IT(&htim2);
	}
    else if(htim->Instance == TIM3)
    {
        if(rx_index1 != 0)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(xSemaphore_local,&xHigherPriorityTaskWoken);
        }
        HAL_TIM_Base_Stop_IT(&htim3);        
    }
        
}


/*uint16_t USART1_Rx(uint8_t* buf,uint16_t len)
{
    uint16_t ret = 0;
    if(rx_index1 <= len)
    {
        memcpy(buf,rx_buf1,rx_index1);
        ret = rx_index1;
        rx_index1 = 0;       
    }
    else
    {
        memcpy(buf,rx_buf1,len);
        ret = len;
        rx_index1 -= len;
        memcpy(rx_buf1,rx_buf1 + len,rx_index1);
    }
    return ret;
}*/


uint16_t UART4_Rx(uint8_t* buf,uint16_t len)
{
    uint16_t ret = 0;
    if(rx_index2 <= len)
    {
        memcpy(buf,rx_buf2,rx_index2);
        ret = rx_index2;
        rx_index2 = 0;       
    }
    else
    {
        memcpy(buf,rx_buf2,len);
        ret = len;
        rx_index2 -= len;
        memcpy(rx_buf2,rx_buf2 + len,rx_index2);
    }
    return ret;
}

uint16_t UART7_Rx(uint8_t* buf,uint16_t size)
{
    uint16_t ret = 0;
    if(rx_index3 <= size)
    {
        memcpy(buf,rx_buf3,rx_index3);
        ret = rx_index3;
        rx_index3 = 0;       
    }
    else
    {
        memcpy(buf,rx_buf3,size);
        ret = size;
        rx_index3 -= size;
        memcpy(rx_buf3,rx_buf3 + size,rx_index3);
    }
    return ret;
}

uint16_t UART8_Rx(uint8_t* buf,uint16_t len)
{
    uint16_t ret = 0;
    if(rx_index1 <= len)
    {
        memcpy(buf,rx_buf1,rx_index1);
        ret = rx_index1;
        rx_index1 = 0;       
    }
    else
    {
        memcpy(buf,rx_buf1,len);
        ret = len;
        rx_index1 -= len;
        memcpy(rx_buf1,rx_buf1 + len,rx_index1);
    }
    return ret;
}


void UART4_ClearBuf(void)
{
    memset(rx_buf2,0,sizeof(rx_buf2));
    rx_index2 = 0;
}

void UART8_ClearBuf(void)
{
    memset(rx_buf1,0,sizeof(rx_buf1));
    rx_index1 = 0;
}

void UART7_ClearBuf(void)
{
    memset(rx_buf3,0,sizeof(rx_buf3));
    rx_index3 = 0;
}

void UART4_Tx(uint8_t* buf,uint16_t size)
{
    HAL_UART_Transmit(&huart1, buf, size, 1000);
}

void UART7_Tx(uint8_t* buf,uint16_t size)
{
    HAL_UART_Transmit(&huart2, buf, size, 1000);
}


void UART8_Tx(uint8_t* buf,uint16_t size)
{
    HAL_UART_Transmit(&huart3, buf, size, 1000/*(size + 9)/10*/);
}
