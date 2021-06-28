
#include  "stm32f4xx_hal.h"
#include  "bsp_adc.h"
#include  "assert.h"



static ADC_HandleTypeDef adc1;//Declare a USART_HandleTypeDef handle structure.
uint16_t ADC_Value;
void  BSP_ADC_Init  (void)
{
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
  	ADC_ChannelConfTypeDef sConfig = {0};
        __HAL_RCC_ADC1_CLK_ENABLE();
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
	adc1.Instance = ADC1;
	adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	adc1.Init.Resolution = ADC_RESOLUTION_12B;
	adc1.Init.ScanConvMode = DISABLE;
	adc1.Init.ContinuousConvMode = DISABLE;
	adc1.Init.DiscontinuousConvMode = DISABLE;
	adc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc1.Init.NbrOfConversion = 1;
	adc1.Init.DMAContinuousRequests = DISABLE;
	adc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&adc1) != HAL_OK)
	{
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&adc1, &sConfig) != HAL_OK)
	{
	}
	//ADC校准 to do
	//HAL_ADCEx_Calibration_Start(&adc1);	
}


void ADC_VolGetSoft(uint16_t *value)
{
        int n = 0;
        uint16_t Value[22];
        uint32_t AD_Value = 0;
        uint16_t max;
        uint16_t min;
        double val;
        for(n=0; n<22; n++)
        {
            HAL_ADC_Start(&adc1);     //启动ADC转换
            HAL_ADC_PollForConversion(&adc1, 1000);   //等待转换完成，50为最大等待时间，单位为ms
            if(HAL_IS_BIT_SET(HAL_ADC_GetState(&adc1), HAL_ADC_STATE_REG_EOC))
            {
                Value[n] = HAL_ADC_GetValue(&adc1);
                AD_Value += Value[n];
            }
        }
  	max = Value[0];
        min = Value[0];
        for(n=0;n<22;n++)
        {
                    max=(Value[n]<max)?max:Value[n];    
                    min=(min<Value[n])?min:Value[n];
        }
        val = (double)((AD_Value -max-min)/20)*(3.3/4095)*11.82;

        *value=(uint16_t)(val);

}

