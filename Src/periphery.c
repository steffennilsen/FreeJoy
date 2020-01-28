/**
  ******************************************************************************
  * @file           : periphery.c
  * @brief          : Periphery driver implementation
  ******************************************************************************
  */

#include "periphery.h"

volatile uint64_t Ticks;
volatile uint32_t TimingDelay;

pin_config_t pin_config[USED_PINS_NUM] =
{
	{GPIOA, GPIO_Pin_0, 0},					// 0
	{GPIOA, GPIO_Pin_1, 1},					// 1
	{GPIOA, GPIO_Pin_2, 2},					// 2
	{GPIOA, GPIO_Pin_3, 3},					// 3
	{GPIOA, GPIO_Pin_4, 4},					// 4
	{GPIOA, GPIO_Pin_5, 5},					// 5
	{GPIOA, GPIO_Pin_6, 6},					// 6
	{GPIOA, GPIO_Pin_7, 7},					// 7
	{GPIOA, GPIO_Pin_8, 8},					// 8
	{GPIOA, GPIO_Pin_9, 9},					// 9
	{GPIOA, GPIO_Pin_10, 10},				// 10	
	{GPIOA, GPIO_Pin_15, 15},				// 11
	{GPIOB, GPIO_Pin_0, 0},					// 12
	{GPIOB, GPIO_Pin_1, 1},					// 13
	{GPIOB, GPIO_Pin_3, 3},					// 14
	{GPIOB, GPIO_Pin_4, 4},					// 15
	{GPIOB, GPIO_Pin_5, 5},					// 16
	{GPIOB, GPIO_Pin_6, 6},					// 17
	{GPIOB, GPIO_Pin_7, 7},					// 18
	{GPIOB, GPIO_Pin_8, 8},					// 19
	{GPIOB, GPIO_Pin_9, 9},					// 20
	{GPIOB, GPIO_Pin_10, 10},				// 21
	{GPIOB, GPIO_Pin_11, 11},				// 22
	{GPIOB, GPIO_Pin_12, 12},				// 23
	{GPIOB, GPIO_Pin_13, 13},				// 24
	{GPIOB, GPIO_Pin_14, 14},				// 25
	{GPIOB, GPIO_Pin_15, 15},				// 26
	{GPIOC, GPIO_Pin_13, 13},				// 27
	{GPIOC, GPIO_Pin_14, 14},				// 28
	{GPIOC, GPIO_Pin_15, 15},				// 29
};

/**
  * @brief SysTick Configuration
  * @retval None
  */
void SysTick_Init(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.SYSCLK_Frequency/1000);	
}
/**
  * @brief Get system ticks
  * @retval ticks
  */
uint64_t GetTick(void)
{
	return Ticks;
}


/**
  * @brief Delay implementation
  * @retval None
  */
void Delay_ms(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

/**
  * @brief Delay implementation
  * @retval None
  */
void Delay_us(__IO uint32_t nTime)
{
  for (int i=0; i<5;i++) __NOP();
}

/**
  * @brief Software SPI Initialization Function
  * @param None
  * @retval None
  */
void SoftSPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef	GPIO_InitStructureure;	
	GPIO_InitStructureure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;
	GPIO_InitStructureure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructureure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init (GPIOB,&GPIO_InitStructureure);
}
/**
  * @brief Software SPI Send Half-Duplex Function
  * @param None
  * @retval None
  */
void SoftSPI_HalfDuplex_Transmit(uint8_t * data, uint16_t length)
{
	GPIO_InitTypeDef 					GPIO_InitStructureure;
	
	GPIO_InitStructureure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructureure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructureure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructureure);
	// Set SCK low
	GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
	
	for (uint16_t i=0; i<length; i++)
	{
		int8_t j = 7;
		do
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, (data[i] & (1<<j)) ? Bit_SET : Bit_RESET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
			__NOP();
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
			__NOP();
			
			j--;
		} while(j>=0);
	}
}
/**
  * @brief Software SPI Receive Half-Duplex Function
  * @param None
  * @retval None
  */
void SoftSPI_HalfDuplex_Receive(uint8_t * data, uint16_t length)
{
	GPIO_InitTypeDef 					GPIO_InitStructureure;
	
	GPIO_InitStructureure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructureure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructureure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructureure);
	// Set SCK low
	GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
	
	for (uint16_t i=0; i<length; i++)
	{
		data[i] = 0;
		int8_t j = 7;
		do
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
			__NOP();			
			data[i] |= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) << j;
			GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
			__NOP();
			
			j--;
		} while(j>=0);
	}
}

/**
  * @brief Generator Initialization Function
  * @param None
  * @retval None
  */
void Generator_Init(void)
{
  TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  				TIM_OCInitStructure;
	GPIO_InitTypeDef 					GPIO_InitStructureure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 18-1;																								
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 9;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM4, ENABLE);
	
	/*GPIOB Configuration: TIM4 channel1*/
	GPIO_InitStructureure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructureure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructureure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructureure);

  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);
}

/* IO init function */
void IO_Init (app_config_t * p_config)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIO Ports Clock Enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	while (p_config->firmware_version != FIRMWARE_VERSION)
	{
		// blink LED if firmware version doesnt match
		GPIOB->ODR ^= GPIO_Pin_12;
		GPIOC->ODR ^=	GPIO_Pin_13;
		Delay_ms(300);
	}
	

	// setting up GPIO according confgiguration
	for (int i=0; i<USED_PINS_NUM; i++)
	{
		// buttons
		if (p_config->pins[i] == BUTTON_GND)
		{
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_InitStructure.GPIO_Pin = pin_config[i].pin;
			GPIO_Init(pin_config[i].port, &GPIO_InitStructure);
		}
		else if (p_config->pins[i] == BUTTON_VCC)
		{
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_InitStructure.GPIO_Pin = pin_config[i].pin;
			GPIO_Init(pin_config[i].port, &GPIO_InitStructure);
		}
		else if (p_config->pins[i] == BUTTON_ROW)
		{
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_InitStructure.GPIO_Pin = pin_config[i].pin;
			GPIO_Init(pin_config[i].port, &GPIO_InitStructure);
		}
		else if (p_config->pins[i] == BUTTON_COLUMN)
		{
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_InitStructure.GPIO_Pin = pin_config[i].pin;
			GPIO_Init(pin_config[i].port, &GPIO_InitStructure);
		}		
		else if (p_config->pins[i] == AXIS_ANALOG)
		{
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_InitStructure.GPIO_Pin = pin_config[i].pin;
			GPIO_Init(pin_config[i].port, &GPIO_InitStructure);
		}
		else if (p_config->pins[i] == SPI_SCK)//  && i == 14)
		{
			//SoftSPI_Init();		// Half duplex SPI
			/**SPI GPIO Configuration    
			PB3     ------> SPI_SCK
			*/
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = pin_config[i].pin;
			GPIO_Init(pin_config[i].port, &GPIO_InitStructure);
		}
		else if (p_config->pins[i] == SPI_DATA)// && i == 16)
		{
			//PB5     ------> SPI_MOSI 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = pin_config[i].pin;
			GPIO_Init(pin_config[i].port, &GPIO_InitStructure);
		}
		else if (p_config->pins[i] == TLE5011_CS || p_config->pins[i] == SHIFT_REG_CS)
		{
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = pin_config[i].pin;
			GPIO_Init(pin_config[i].port, &GPIO_InitStructure);
			GPIO_WriteBit(pin_config[i].port, pin_config[i].pin, Bit_SET);
		}
		else if (p_config->pins[i] == TLE5011_GEN  && i == 17)
		{
			Generator_Init();	// 4MHz output at PB6 pin
		}
		else if (p_config->pins[i] == SHIFT_REG_DATA)
		{
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = pin_config[i].pin;
			GPIO_Init(pin_config[i].port, &GPIO_InitStructure);
		}
		
	}

#ifdef DEBUG
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
}


