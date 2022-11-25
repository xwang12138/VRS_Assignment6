/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "usart.h"
#include "lis3mdltr.h"
#include "lsm6ds0.h"
//#include "stdio.h"
//#include "string.h"
#include "dma.h"

#include "lps22hb.h"
#include "hts221.h"
#include <stdio.h>
#include <string.h>
#define CHAR_BUFF_SIZE	30

//uint8_t temp = 0;
float mag[3], acc[3];
char formated_text[40], value_x[10], value_y[10], value_z[10];
float temp,press, alti;
int hum;

void SystemClock_Config(void);
void USART2_PutBuffer1();

int main(void)
{
  //char str[4][40];
 // char str1[]="Buffer capacity: ";
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  lsm6ds0_init();

  lps22hb_init();
  hts221_init();
//  for(int i = 0; i<4; i++){
//	  changeMode(str, i);
//  }
  while (1)
  {
	  //os			   x      y        z
//	  for(uint8_t mode= 0; mode<4; mode++){
//		  changeMode(str, mode);
//		  USART2_PutBuffer(str[mode], strlen(str[mode]));
//		  LL_mDelay(100);
//	  }
	  //temp = lps22hb_get_temp();
	  temp = hts221_get_temp();
	  hum = (int)(hts221_get_humidity());
	  press = lps22hb_get_press();
	  alti = lps22hb_get_altitude();


	  memset(formated_text, '\0', sizeof(formated_text));
	  sprintf(formated_text, "teplota[C]:%03.1f,humidity[%%]:%02d,press[hPa]:%05.1f,altitude[m]:%05.1f\r\n", temp,hum,press,alti);
	  USART2_PutBuffer(formated_text, strlen(formated_text));
	  LL_mDelay(1500);

//	  lsm6ds0_get_acc(acc, (acc+1), (acc+2));
//	  memset(formated_text, '\0', sizeof(formated_text));
//	  sprintf(formated_text, "%0.4f,%0.4f,%0.4f\r\n", acc[0], acc[1], acc[2]);
//	  USART2_PutBuffer(formated_text, strlen(formated_text));
	  //LL_USART_TransmitData8(USART2,'5');
  }
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
  LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_HSI);
}

void changeMode(char str[4][40], uint8_t mode)
{
	float tempt;
	switch(mode)
	{
		case 0: // temperature
			tempt = lps22hb_get_temp();
			if(tempt >= 100){
				tempt = 99.9;
			}
			if(tempt <= -100){
				tempt = -99.9;
			}
			if(tempt >= 0){
				sprintf(str[0], "teplota[C]:%04.1f\r\n",tempt);
			}else{
				sprintf(str[0], "teplota[C]:%05.1f\r\n",tempt);
			}
			break;
		case 1: // humidity
			sprintf(str[1], "humidity[%%]:%02d\r\n",(int)(hts221_get_humidity()));
			break;
		case 2: //pressure
			sprintf(str[2], "press[hPa]:%06.1f\r\n",lps22hb_get_press());
			break;
		case 3: // altitude
			sprintf(str[3], "altitude[m]:%06.1f\r\n",lps22hb_get_altitude());
			break;
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
