#include "stm32f4xx_hal.h"
#include "softi2c_port_c.h"



void _n_i2c_io_init(void)
{
	
	GPIO_InitTypeDef  gpio_init;
	
    gpio_init.Pin   = GPIO_PIN_8;
    gpio_init.Mode  = GPIO_MODE_OUTPUT_OD;
    gpio_init.Pull  = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio_init);
	
    gpio_init.Pin   = GPIO_PIN_9;
    gpio_init.Mode  = GPIO_MODE_OUTPUT_OD;
    gpio_init.Pull  = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &gpio_init);
}




