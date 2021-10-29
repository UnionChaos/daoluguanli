#ifndef  __BSP_USART_H_
#define  __BSP_USART_H_

#include <assert.h>
#include "stm32f4xx_hal.h"
#include "string.h"



void BSP_USART_Init(void);


void UART4_Tx(uint8_t* buf,uint16_t size);
void UART7_Tx(uint8_t* buf,uint16_t size);
void UART8_Tx(uint8_t* buf,uint16_t size);
uint16_t UART4_Rx(uint8_t* buf,uint16_t len);
uint16_t UART7_Rx(uint8_t* buf,uint16_t size);
uint16_t UART8_Rx(uint8_t* buf,uint16_t len);
void UART4_ClearBuf(void);
void UART8_ClearBuf(void);
void UART7_ClearBuf(void);




#endif  