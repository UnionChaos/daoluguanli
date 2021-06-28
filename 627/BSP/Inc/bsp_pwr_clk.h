/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
*                                         STM32746G-EVAL2
*                                         Evaluation Board
*
* Filename      : bsp_led.h
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_PWR_CLK_H_
#define  BSP_PWR_CLK_H_


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <stdint.h>

/*
*********************************************************************************************************
*                                     EXTERNAL C LANGUAGE LINKAGE
*
* Note(s) : (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
*               declarations for correct C language linkage.
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {                                  /* See Note #1.                                         */
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
#define  SIM7600_PWR_PORT                     GPIOC
#define  RF1_PWR_PORT                         GPIOF
#define  RF2_PWR_PORT                         GPIOF
#define  RS485_PWR_PORT                       GPIOG
#define  NET_PWR_PORT                         GPIOD
#define  GPS_PWR_PORT                         GPIOB
  
#define  SIM7600_PWR_PIN                   GPIO_PIN_3
#define  RF1_PWR_PIN                       GPIO_PIN_2
#define  RF2_PWR_PIN                       GPIO_PIN_1
#define  RS485_PWR_PIN                     GPIO_PIN_0
#define  NET_PWR_PIN                       GPIO_PIN_11
#define  GPS_PWR_PIN                       GPIO_PIN_4
#define BSP_SIM7600_PWR_SET(on) \
    {                                                                                       \
        if (on == 0) {                                                                      \
            HAL_GPIO_WritePin(SIM7600_PWR_PORT, SIM7600_PWR_PIN, GPIO_PIN_RESET);           \
        } else {                                                                            \
            HAL_GPIO_WritePin(SIM7600_PWR_PORT, SIM7600_PWR_PIN, GPIO_PIN_SET);             \
        }                                                                                   \
    }

#define RF1_PWR_SET(on) \
    {                                                                                       \
        if (on == 0) {                                                                      \
        HAL_GPIO_WritePin(RF1_PWR_PORT, RF1_PWR_PIN, GPIO_PIN_RESET);                       \
        } else {                                                                            \
            HAL_GPIO_WritePin(RF1_PWR_PORT, RF1_PWR_PIN, GPIO_PIN_SET);                     \
        }                                                                                   \
    } 


#define RF2_PWR_SET(on) \
    {                                                                                       \
        if (on == 0) {                                                                      \
        HAL_GPIO_WritePin(RF2_PWR_PORT, RF2_PWR_PIN, GPIO_PIN_RESET);                       \
        } else {                                                                            \
            HAL_GPIO_WritePin(RF2_PWR_PORT, RF2_PWR_PIN, GPIO_PIN_SET);                     \
        }                                                                                   \
    } 


#define RS485_PWR_SET(on) \
    {                                                                                       \
        if (on == 0) {                                                                      \
        HAL_GPIO_WritePin(RS485_PWR_PORT, RS485_PWR_PIN, GPIO_PIN_RESET);                   \
        } else {                                                                            \
            HAL_GPIO_WritePin(RS485_PWR_PORT, RS485_PWR_PIN, GPIO_PIN_SET);                 \
        }                                                                                   \
    } 

#define NET_PWR_SET(on) \
    {                                                                                       \
        if (on == 0) {                                                                      \
        HAL_GPIO_WritePin(NET_PWR_PORT, NET_PWR_PIN, GPIO_PIN_RESET);                       \
        } else {                                                                            \
            HAL_GPIO_WritePin(NET_PWR_PORT, NET_PWR_PIN, GPIO_PIN_SET);                     \
        }                                                                                   \
    } 

    
#define GPS_PWR_SET(on) \
{                                                                                       \
    if (on == 0) {                                                                      \
    HAL_GPIO_WritePin(GPS_PWR_PORT, GPS_PWR_PIN, GPIO_PIN_RESET);                       \
    } else {                                                                            \
        HAL_GPIO_WritePin(GPS_PWR_PORT, GPS_PWR_PIN, GPIO_PIN_SET);                     \
    }                                                                                   \
} 
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/


void  BSP_PWR_CLK_Init(void);



/*
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*/

#ifdef __cplusplus
}                                              /* End of 'extern'al C lang linkage.                    */
#endif


/*
*********************************************************************************************************
*                                          CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
