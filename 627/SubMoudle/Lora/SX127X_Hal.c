/**
  ******************************************************************************
  * 文件名 ：   SX127X_HAL.c
  * 作者   ：   LSD RF Team
  * 版本   ：   V1.0.0
  * 时间   ：   15-Aug-2018
  * 文件描述：
  *     该文件为SX127X模块的硬件层，包含MCU与SX127X模块的SPI配置，GPIO口初始化，以
  *及用于SX127X寄存器、FIFO读写操作；
  *    客户在使用模块时候需要移植该文件，保证各个函数名、函数形参不变的情况下
  *根据自己的MCU平台修改函数内容，使各个功能块正常运行。硬件层占用资源如下：
	*
  *SPI：本例程使用STM32L4的SPI5进行与SX127X模块通信。
  *GPIO口：本例程使用的GPIO口详情如下：
  *        PF3  ---> DIO0
  *        PF4 ---> DIO1
  *        无  ---> DIO2
  *        PB0  ---> TXE
  *        PC5  ---> RXE
	*        PF5  ---> RST
	*        PF6 ---> NSS
  *        PF9  ---> M0SI
  *        PF8  ---> MISO
  *        PF7 ---> SCK
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "SX127X_Hal.h"
#include "stm32f4xx_hal.h"
SPI_HandleTypeDef SPI2_InitStruct;

//-----------------------------GPIO-----------------------------//
//该部分函数为系统用到的GPIO的初始化函数，用户根据自己的平台相应修改
//--------------------------------------------------------------//

/**
  * @简介：该函数为DIO0输入初始化及中断、优先级配置；
  * @参数：无
  * @返回值：无
  */
void SX127X_DIO0_INPUT()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(EXTI3_IRQn, 6, 0);
}
/**
  * @简介：该函数为DIO0输入中断开启使能；
  * @参数：无
  * @返回值：无
  */
void SX127X_DIO0_INTENABLE()
{
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}
/**
  * @简介：该函数为DIO0输入中断关闭使能；
  * @参数：无
  * @返回值：无
  */
void SX127X_DIO0_INTDISABLE()
{
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);
}
/**
  * @简介：该函数为DIO0输入状态获取；
  * @参数：无
  * @返回值：DIO0状态"1"or"0"
  */
GPIO_PinState SX127X_DIO0_GetState()
{
    GPIO_PinState State;
    State = HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_3);
    return State;
}
/**
  * @简介：该函数为DIO1输入初始化及中断、优先级配置；
  * @参数：无
  * @返回值：无
  */
void SX127X_DIO1_INPUT()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    //HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
}
/**
  * @简介：该函数为DIO2输入初始化及中断、优先级配置；
  * @参数：无
  * @返回值：无
  */

void SX127X_DIO2_INPUT()
{
    /*
    GPIO_InitTypeDef GPIO_InitStruct;
    __GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    //HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);*/
}
/**
  * @简介：该函数为高频开关TXE输出控制；
  * @参数：PinState为"1"表示输出高电平，"0"输出低电平；
  * @返回值：无
  */
void SX127X_TXE_OUTPUT(GPIO_PinState PinState)
{
    /*GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, PinState);*/
}
/**
  * @简介：该函数为高频开关RXE输出控制；
  * @参数：PinState为"1"表示输出高电平，"0"输出低电平；
  * @返回值：无
  */
void SX127X_RXE_OUTPUT(GPIO_PinState PinState)
{
    /*GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, PinState);*/
}

/**
  * @简介：该函数为SPI的片选引脚NSS输出控制；
  * @参数：PinState为"1"表示输出高电平，"0"输出低电平；
  * @返回值：无
  */
void SX127X_NSS_OUTPUT(GPIO_PinState PinState)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, PinState);
}
/**
  * @简介：该函数为SX127X复位引脚NRST输出控制；
  * @参数：PinState为"1"表示输出高电平，"0"输出低电平；
  * @返回值：无
  */
void SX127X_RESET_OUTPUT(GPIO_PinState PinState)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, PinState);
}

//-----------------------------SPI-----------------------------//
//该部分函数为MCU对SX127X模块SPI通信部分，包含SPI口及配置初始化
//--------------------------------------------------------------//

/**
  * @简介：该函数用于MCU对SPI对应IO口初始化；
  * @参数：无
  * @返回值：无
  */
void SX127X_SPIGPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* Configure the SX126X_NSS pin */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_7;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    /* SPI MoSi GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_9;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}
/**
  * @简介：该函数用于MCU对SPI配置初始化；
  * @参数：无
  * @返回值：无
  */
void SX127X_SPI_Init()
{
    __HAL_RCC_SPI5_CLK_ENABLE();//SPI2时钟使能
    SX127X_SPIGPIO_Init();

    SPI2_InitStruct.Instance = SPI5; //使用SPI5
    SPI2_InitStruct.Init.Mode = SPI_MODE_MASTER;//SPI模式：主机模式
    SPI2_InitStruct.Init.Direction = SPI_DIRECTION_2LINES;//两线全双工
    SPI2_InitStruct.Init.DataSize = SPI_DATASIZE_8BIT;//数据宽度：8位
    SPI2_InitStruct.Init.CLKPolarity = SPI_POLARITY_LOW; //串行同步字时钟控制为低速时钟
    SPI2_InitStruct.Init.CLKPhase = SPI_PHASE_1EDGE;      //CPOL=0;CPHA=0模式
    SPI2_InitStruct.Init.NSS = SPI_NSS_SOFT;//NSSD由软件管理
    SPI2_InitStruct.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;//波特率分频，8分频
    SPI2_InitStruct.Init.FirstBit = SPI_FIRSTBIT_MSB;//数据从MSB开始
    SPI2_InitStruct.Init.TIMode = SPI_TIMODE_DISABLE;//SPI Motorola mode
    SPI2_InitStruct.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;//CRC校验不使能
    SPI2_InitStruct.Init.CRCPolynomial = 7;//CRC值计算的多项式
    //SPI2_InitStruct.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    //SPI2_InitStruct.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

    if (HAL_SPI_Init(&SPI2_InitStruct) != HAL_OK)

    {
        while(1);
    }
    __HAL_SPI_ENABLE(&SPI2_InitStruct);
}

//-----------------------SX127X Read and Write-------------------//
//该部分函数为MCU对SX127X模块寄存器进行读写
//--------------------------------------------------------------//

/**
  * @简介：SX127X  向寄存器地址连续发送数据
  * @参数：uint8_t addr,寄存器地址 uint8_t *buffer,发送数组指针 uint8_t size指针长度
  * @返回值：无
  */
unsigned char SX127X_ReadWriteByte(unsigned char data)
{
    unsigned char RxDat;
    HAL_SPI_TransmitReceive(&SPI2_InitStruct, &data, &RxDat, 1, 1000);
    return RxDat;
}
/**
  * @简介：SX127X  向寄存器地址连续发送数据
  * @参数：uint8_t addr,寄存器地址 uint8_t *buffer,发送数组指针 uint8_t size指针长度
  * @返回值：无
  */
void SX127X_WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;
    SX127X_NSS_OUTPUT(GPIO_PIN_RESET);
    SX127X_ReadWriteByte(addr | 0x80);
    for( i = 0; i < size; i++ )
    {
        SX127X_ReadWriteByte(buffer[i]);
    }
    SX127X_NSS_OUTPUT(GPIO_PIN_SET);
}
/**
  * @简介：SX127X  向寄存器地址连续读数据
  * @参数：uint8_t addr,寄存器地址 uint8_t *buffer,发送数组指针 uint8_t size指针长度
  * @返回值：数据返回到*buffer中
  */
void SX127X_ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;
    SX127X_NSS_OUTPUT(GPIO_PIN_RESET);
    SX127X_ReadWriteByte(addr & 0x7F);
    for( i = 0; i < size; i++ )
    {
        buffer[i] = SX127X_ReadWriteByte(0x00);
    }

    SX127X_NSS_OUTPUT(GPIO_PIN_SET);
}



