/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bluetooth.h"
#include "control.h"
#include "dwt.h"
#include "user_adc.h"
#include "pcs.h"
#include "motor.h"
#include "periph_spi.h"
#include "string.h"
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

/* Largura de pulso dos canais PWM em [us] */
int32_t Radio3, Radio4, Radio5;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  int32_t val_MR, val_ML; // usado no RC

//  uint16_t Current_MR, Current_ML;
  jogada3Kg_TypeDef state_machine = CONFIG;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  // DWT initialization
  DWT_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  user_adc_Init();
  HAL_TIM_Base_Start_IT(&htim3);
  bluetoothPrint((uint8_t*) "Bem vindo :D\n");
  motorL(0);
  motorR(0);

  // Help variableS
  uint8_t lock = 0;
  uint8_t alternador = 0;

  int32_t ang_degrees;
  int32_t dist_micrometers;
  int32_t pos_x = 3000000, pos_y = 3000000;
  uint8_t aux[20];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
/* LOOP ***********************************************************************/
  while (1)
  {
	  switch (state_machine)
	  {
	  case CONFIG:
		  memset(aux,0,sizeof(aux));
		  HAL_UART_Receive(&huart3, (uint8_t *)aux, 8, 100);
		  if(aux[0] != 0 && pos_x == 3000000){
			  pos_x = 0;
			  if(aux[0] == '-'){
				  for(int i = 1;i < strlen(aux); i++){
					  pos_x*=10;
					  pos_x +=(aux[i]-'0');
				  }
				  pos_x*=-1;
			  }
			  else {
				  for(int i = 0;i < strlen(aux); i++){
					  pos_x*=10;
					  pos_x +=(aux[i]-'0');
				  }
			  }
			  bluetoothPrint((uint8_t *)"\nX: ");
			  bluetoothPrintVal(pos_x);
			  bluetoothPrint((uint8_t *)" um\n");
		  }
		  else if (aux[0] != 0){
			  pos_y = 0;
			  if(aux[0] == '-'){
				  for(int i = 1;i < strlen(aux); i++){
					  pos_y*=10;
					  pos_y +=(aux[i]-'0');
				  }
				  pos_y*=-1;
			  }
			  else {
				  for(int i = 0;i < strlen(aux); i++){
					  pos_y*=10;
					  pos_y +=(aux[i]-'0');
				  }
			  }
			  bluetoothPrint((uint8_t *)"\nY: ");
			  bluetoothPrintVal(pos_y);
			  bluetoothPrint((uint8_t *)" um\n");

			  control_setXYSetPoint(pos_x, pos_y);

			  state_machine = JOGADA_POS;
		  }
		  break;

	  case JOGADA_POS:
		  if(control_process()){
			  state_machine = JOGADA_STOP;
		  }
		  HAL_Delay(10);
		  break;

	  case JOGADA_STOP:
		  motorL(0);
		  motorR(0);
		  HAL_Delay(2000);

		  break;

	  case CONFIG_GIRO:
		  aux[0] = 0;
		  HAL_UART_Receive(&huart3, (uint8_t *)aux, 3, 100);
		  if(!aux[0])
		  {
			  state_machine = CONFIG;
		  }
		  else{
			  if(aux[0] != '-')
				  ang_degrees = (aux[0]-'0')*10 + (aux[1]-'0');
			  else	ang_degrees = (-1)*((aux[1]-'0')*10 + (aux[2]-'0'));
			  bluetoothPrint((uint8_t *)"\nAng: ");
			  bluetoothPrintVal(ang_degrees);
			  bluetoothPrint((uint8_t *)"??\n");
			  control_setThetaSetPoint(ang_degrees*0.0174533);

			  state_machine = JOGADA_GIRO;
		  }
		  break;
/* END CONFIG GIRO STATE ******************************************************/

	  case CONFIG_RETO:
		  memset(aux,0,sizeof(aux));
		  HAL_UART_Receive(&huart3, (uint8_t *)aux, 8, 100);
		  if(!aux[0])
		  {
			  state_machine = CONFIG;
		  }
		  else{
			  dist_micrometers = 0;
			  for(int i = 0;i < strlen(aux); i++){
				  dist_micrometers*=10;
				  dist_micrometers +=(aux[i]-'0');
			  }
			  bluetoothPrint((uint8_t *)"\nDist: ");
			  bluetoothPrintVal(dist_micrometers);
			  bluetoothPrint((uint8_t *)" um\n");
			  control_setXYSetPoint(0, dist_micrometers);
			  state_machine = JOGADA_RETO;
		  }
		  break;
/* END CONFIG RETO STATE ******************************************************/

	  case JOGADA_RC:
		  // DEBUG RC READ
//		  bluetoothPrint((uint8_t*) "THR: ");
//		  bluetoothPrintVal(Radio4);
//		  bluetoothPrint((uint8_t*) "\nRUD: ");
//		  bluetoothPrintVal(Radio5);
		  Radio4 = readPWM(Radio4_GPIO_Port, Radio4_Pin);
		  Radio5 = readPWM(Radio5_GPIO_Port, Radio5_Pin);
		  Radio4 = ((Radio4 - 1500) * 63) / 500;
		  Radio5 = ((Radio5 - 1500) * 63) / 500;

		  val_MR = (Radio4 + Radio5);
		  val_MR = val_MR > 63 ? 63 : val_MR;
		  val_ML = (Radio4 - Radio5);
		  val_ML = val_ML < -63 ? -63 : val_ML;
		  motorR(val_MR);
		  motorL(val_ML);

		  // DEBUG MOTOR CURRENT
//		  readMotorCurrent(&Current_ML, &Current_MR);
//		  bluetoothPrint((uint8_t *)"ML: ");
//		  bluetoothPrintVal(Current_ML);
//		  bluetoothPrint((uint8_t *)"\nMR: ");
//		  bluetoothPrintVal(Current_MR);
//		  bluetoothPrint((uint8_t*) "\n\n");

		  // PRINT EVERY 1000 ms
//		  if(HAL_GetTick() % 1000 < 100 && !lock){
//			  lock = 1;
//			  bluetoothPrint((uint8_t*) "\n--------------------------\nEncoder DIR:");
//			  bluetoothPrintVal(control_getDesD());
//			  bluetoothPrint((uint8_t*) "\nVelD: ");
//			  bluetoothPrintVal(control_getVelD());
//			  bluetoothPrint((uint8_t*) "\n\nEncoder ESQ:");
//			  bluetoothPrintVal(control_getDesE());
//			  bluetoothPrint((uint8_t*) "\nVelE: ");
//			  bluetoothPrintVal(control_getVelE());
//			  bluetoothPrint((uint8_t*)"\n--------------------------\n");
//		  }
//		  else {
//			  lock = 0;
//		  }

		  break;

/* END RC STATE ***************************************************************/
	  case JOGADA_GIRO:
		  if(control_rotacao())
			  state_machine = JOGADA_STOP;

		  HAL_Delay(10);
		  break;
/* END GIRO *******************************************************************/

	  case JOGADA_RETO:
		  if(control_reta())
			  state_machine = JOGADA_STOP;
		  HAL_Delay(10);
		  break;
/* END RETO *******************************************************************/
	  case TESTE_SPI:

		  // PRINT EVERY 1000 ms
		  if(HAL_GetTick() % 1000 == 0 && !lock){
			  lock = 1;
			  switch (alternador) {
				case 0:
					periph_spi_sendBatteryVoltage();
					break;
				case 1:
					periph_spi_sendLeftMotorPower(-63);
					break;
				case 2:
					periph_spi_sendMotorCurrent();
					break;
				case 3:
					periph_spi_sendMotorSpeed();
					break;
				case 4:
					periph_spi_sendRightMotorPower(63);
					break;
				case 5:
					periph_spi_sendSelectedMove(TESTE_SPI);
					break;
			}
			alternador++;
			alternador %=6;
		  }
		  else
			  lock = 0;
		  break;
/* END TESTE SPI STATE ***************************************************************/
	  } // END STATE MACHINE
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
