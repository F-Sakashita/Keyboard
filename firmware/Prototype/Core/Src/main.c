/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "usbd_hid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for(DataIdx=0; DataIdx<len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  uint32_t uiDebugTime = 0u;
  uint32_t uiLedTime = 0u;
  uint8_t ucSwState[6];
  bool bSwToggle = false;
  const uint32_t uiPeriod = 1u;

  typedef struct{
	  uint8_t ucId;
	  uint8_t ucModifiers;
	  uint8_t reserved;
	  uint8_t ucKey[6];
  }KeyboardHID_t;
  KeyboardHID_t stkeyboardData;

  stkeyboardData.ucId = 1;
  stkeyboardData.ucModifiers = 0;
  stkeyboardData.reserved = 0;
  for(uint8_t i = 0; i < 6; i++)  stkeyboardData.ucKey[i] = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(!bSwToggle){
		  LL_GPIO_SetOutputPin(SW_OUT_ROW0_GPIO_Port, SW_OUT_ROW0_Pin);
		  LL_GPIO_ResetOutputPin(SW_OUT_ROW1_GPIO_Port, SW_OUT_ROW1_Pin);
		  ucSwState[0] = LL_GPIO_IsInputPinSet(SW_IN_COL0_GPIO_Port, SW_IN_COL0_Pin);
		  ucSwState[1] = LL_GPIO_IsInputPinSet(SW_IN_COL1_GPIO_Port, SW_IN_COL1_Pin);
		  ucSwState[2] = LL_GPIO_IsInputPinSet(SW_IN_COL2_GPIO_Port, SW_IN_COL2_Pin);
	  }else{
		  LL_GPIO_SetOutputPin(SW_OUT_ROW1_GPIO_Port, SW_OUT_ROW1_Pin);
		  LL_GPIO_ResetOutputPin(SW_OUT_ROW0_GPIO_Port, SW_OUT_ROW0_Pin);
		  ucSwState[3] = LL_GPIO_IsInputPinSet(SW_IN_COL0_GPIO_Port, SW_IN_COL0_Pin);
		  ucSwState[4] = LL_GPIO_IsInputPinSet(SW_IN_COL1_GPIO_Port, SW_IN_COL1_Pin);
		  ucSwState[5] = LL_GPIO_IsInputPinSet(SW_IN_COL2_GPIO_Port, SW_IN_COL2_Pin);
	  }
	  bSwToggle = !bSwToggle;

	  //USB??????
	  //ref : https://wiki.onakasuita.org/pukiwiki/?HID%2F???????????????
	  stkeyboardData.ucKey[0] = (ucSwState[0]) ? 0x04 : 0;	//input a
	  USBD_HID_SendReport(MX_USB_DEVICE_GetHandlePtr(), (uint8_t *)&stkeyboardData, sizeof(stkeyboardData));


	  if(uiDebugTime >= 100u){
		  uiDebugTime = 0u;
		  printf("%d, %d, %d, %d, %d, %d\n", ucSwState[0]
										   , stkeyboardData.ucKey[0]
										   , ucSwState[2]
										   , ucSwState[3]
										   , ucSwState[4]
										   , ucSwState[5]);
	  }else{
		  uiDebugTime += uiPeriod;
	  }

	  if(uiLedTime >= 500u){
		  LL_GPIO_TogglePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin);
		  uiLedTime = 0u;
	  }else{
		 uiLedTime += uiPeriod;
	  }

	  LL_mDelay(uiPeriod);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(72000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
