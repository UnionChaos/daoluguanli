#include "FreeRTOS.h"
#include "task.h"

#include "dhcp.h"
#include "stm32f4xx_hal.h"
static int _mallocfailed = 0;
void vApplicationTickHook( void )
{
    static uint32_t tick = 0;
      HAL_IncTick();
    tick++;
    if(tick >= 1000)
    {
        tick = 0;
        DHCP_time_handler();
    }

    
}
static volatile unsigned int _continue1;


void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
  _continue1 = 0;
  while(_continue1 == 1u);
  
    (void)xTask;
    (void)pcTaskName;
}


void vApplicationMallocFailedHook()
{
    _mallocfailed++;
}


