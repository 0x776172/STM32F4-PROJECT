#include "main.h"
#include <stdbool.h>

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
uint8_t detect500(void);
uint8_t detect1000(void);
void ONproses(void);
void OFFproses(void);
void ONkembalian500(void);
void ONkembalian1000(void);
uint8_t stateBatal(void);
uint8_t stateProses(void);
void dropBrg(void);

typedef enum {s0, s500, s1000, dropCC, kembalian1000, kembalian500, batal} myState;
myState currState, prevstate;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  currState = s0;
  while (1)
  {
	  switch (currState){

	  case s0:
		  if(detect500()){
			  currState = s500;
		  }
		  else if(detect1000()){
			  currState = s1000;
		  }
		  break;

	  case s500:
		  if(detect500()){
			  currState = dropCC;
		  }
		  else if(detect1000()){
			  currState = kembalian500;
		  }
		  else if(stateBatal()){
			  currState = batal;
			  prevstate = s500;

		  }
		  break;

	  case s1000:
		  if(detect500()){
			  currState = kembalian500;
		  }
		  else if(detect1000()){
			  currState = kembalian1000;
		  }
		  else if(stateBatal()){
			  currState = batal;
			  prevstate = s1000;
		  }
		  else currState = dropCC;
		  break;

	  case dropCC:
		  ONproses();
		  if(stateBatal()){
			  currState = batal;
			  prevstate = dropCC;
			  OFFproses();
		  }
		  else if(stateProses()){
			  dropBrg();
			  currState = s0;
			  OFFproses();
		  }
		  else if(detect500()){
			  currState = kembalian500;
		  }
		  else if(detect1000()){
			  currState = kembalian1000;
		  }
		  break;

	  case kembalian500: //change of money
		  ONkembalian500();
		  currState = dropCC;
		  break;

	  case kembalian1000: //chang of money
		  ONkembalian1000();
		  currState = dropCC;
		  break;

	  case batal: //state for cancelling process
		  if(prevstate == s500){
			  ONkembalian500();
			  currState = s0;
		  }
		  else if(prevstate == s1000){
			  ONkembalian1000();
			  currState = s0;
		  }
		  else if(prevstate == dropCC){
			  ONkembalian1000();
			  currState = s0;
		  }
		  break;
	  }
  }
}

uint8_t detect500(void){ //detect coin of 500
	uint8_t flag500 = 0;
	if(HAL_GPIO_ReadPin(GPIOB, btn500_Pin) == GPIO_PIN_SET){
		HAL_Delay(50);
	}
	if(HAL_GPIO_ReadPin(GPIOB, btn500_Pin) == GPIO_PIN_SET){
		while(HAL_GPIO_ReadPin(GPIOB, btn500_Pin) == GPIO_PIN_SET);
		flag500 = 1;
		}
	return flag500;
}

uint8_t detect1000(void){ // detect coin 1000
	uint8_t flag1000 = 0;
	if(HAL_GPIO_ReadPin(GPIOB, btn1000_Pin) == GPIO_PIN_SET){
		HAL_Delay(50);
	}
	if(HAL_GPIO_ReadPin(GPIOB, btn1000_Pin) == GPIO_PIN_SET){
		while(HAL_GPIO_ReadPin(GPIOB, btn1000_Pin) == GPIO_PIN_SET);
		flag1000 = 1;
	}
	return flag1000;
}

uint8_t stateBatal(void){ //cancel the process
	uint8_t flagBatal = 0;
	if(HAL_GPIO_ReadPin(GPIOB, btnbatal_Pin) == GPIO_PIN_SET){
		HAL_Delay(50);
	}
	if(HAL_GPIO_ReadPin(GPIOB, btnbatal_Pin) == GPIO_PIN_SET){
		while(HAL_GPIO_ReadPin(GPIOB, btnbatal_Pin) == GPIO_PIN_SET);
		flagBatal = 1;
	}
	return flagBatal;
}

uint8_t stateProses(void){ //finish the process
	uint8_t flagproses = 0;
	if(HAL_GPIO_ReadPin(GPIOB, btnpros_Pin) == GPIO_PIN_SET){
		HAL_Delay(50);
	}
	if(HAL_GPIO_ReadPin(GPIOB, btnpros_Pin) == GPIO_PIN_SET){
		while(HAL_GPIO_ReadPin(GPIOB, btnpros_Pin) == GPIO_PIN_SET);
		flagproses = 1;
	}
	return flagproses;
}

void dropBrg(void){ //drop the coke
	HAL_GPIO_WritePin(GPIOA, dropCola_Pin, SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, dropCola_Pin, RESET);
}

void ONproses(void){ // indicator that vending is ready to process
	HAL_GPIO_WritePin(GPIOA, ledpros_Pin, SET);
	HAL_Delay(50);
}

void OFFproses(void){ //indicator for finished process or process is cancelled
	HAL_GPIO_WritePin(GPIOA, ledpros_Pin, RESET);
	HAL_Delay(50);
}

void ONkembalian500(void){ // indicator for change with coin 500
	HAL_GPIO_WritePin(GPIOA, chg500_Pin, SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, chg500_Pin, RESET);
}

void ONkembalian1000(void){ // indicator for change with coin 1000
	HAL_GPIO_WritePin(GPIOA, chg1000_Pin, SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, chg1000_Pin, RESET);
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, dropCola_Pin|ledpros_Pin|chg500_Pin|chg1000_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : btn500_Pin btnpros_Pin btnbatal_Pin */
  GPIO_InitStruct.Pin = btn500_Pin|btnpros_Pin|btnbatal_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : btn1000_Pin */
  GPIO_InitStruct.Pin = btn1000_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(btn1000_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : dropCola_Pin ledpros_Pin chg500_Pin chg1000_Pin */
  GPIO_InitStruct.Pin = dropCola_Pin|ledpros_Pin|chg500_Pin|chg1000_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
