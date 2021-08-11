#ifndef __SX127X_2_HAL_H__
#define __SX127X_2_HAL_H__
#include "stm32f4xx_hal.h"	
#include "stdbool.h"

typedef enum
{
    RADIO_RESET_OFF,
    RADIO_RESET_ON,
}tRadioResetState;

//===================================SPI函数声明===================================================
void SX127X_2_DIO0_INPUT(void);
void SX127X_2_DIO0_INTENABLE(void);
void SX127X_2_DIO0_INTDISABLE(void);
GPIO_PinState SX127X_2_DIO0_GetState(void);
void SX127X_2_DIO1_INPUT(void);
void SX127X_2_DIO2_INPUT(void);
void SX127X_2_TXE_OUTPUT(GPIO_PinState PinState);
void SX127X_2_RXE_OUTPUT(GPIO_PinState PinState);
void SX127X_2_NSS_OUTPUT(GPIO_PinState PinState);
void SX127X_2_RESET_OUTPUT(GPIO_PinState PinState);
void SX127X_2_SPIGPIO_Init(void);
void SX127X_2_SPI_Init(void);
unsigned char SX127X_2_ReadWriteByte(unsigned char data);
void SX127X_2_WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );
void SX127X_2_ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );
#endif //__SX127X_2_HAL_H__
