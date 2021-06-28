

#include  "bsp_clock.h"
#include  "bsp_led.h"
#include  "bsp_can.h"
#include  "bsp_usart.h"
#include  "bsp_spi.h"
#include  "bsp_pwr_clk.h"
#include  "bsp_relay.h"
#include  "bsp_adc.h"
#include  "stm32f4xx_hal.h"




/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    BSP_PWR_CLK_Init();
    BSP_SIM7600_PWR_SET(1);
    RF1_PWR_SET(1);
    //RF2_PWR_SET(1);
    //RS485_PWR_SET(1);
    NET_PWR_SET(1);
    GPS_PWR_SET(1);
    BSP_RELAY_Init();
    BSP_LED_Init();    
    BSP_USART_Init();
    BSP_SPI_Init();
    BSP_ADC_Init();
}
