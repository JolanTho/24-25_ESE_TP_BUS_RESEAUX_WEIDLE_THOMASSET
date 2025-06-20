/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bmp280.h"
#include <stdio.h>
#include <stdlib.h>
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
uint32_t nc_temp=0;
uint32_t nc_pres=0;
HAL_StatusTypeDef result;
int cons_temp=20;

double Kp = 10.0;
double Ki = 0.1;
double integral = 0.0;
double output = 0.0;
double max_integral = 50.0;

// Facteur de lissage (plus il est proche de 1, plus la sortie est lissée)
double smoothing_factor = 0.9;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int chr){
	HAL_UART_Transmit(&huart2, (uint8_t*)&chr, 1, HAL_MAX_DELAY);
//	HAL_UART_Transmit(&huart1, (uint8_t*)&chr, 1, HAL_MAX_DELAY);
	return chr;
}

// Fonction pour mettre à jour la sortie PI avec lissage
void updatePI(double varia) { // varia représente l'erreur
    // Limite anti-windup pour l'intégrale
    integral += varia;
    if (integral > max_integral) integral = max_integral;
    if (integral < -max_integral) integral = -max_integral;

    double raw_output = Kp * varia + Ki * integral;

    output = smoothing_factor * output + (1 - smoothing_factor) * raw_output;

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	uint8_t aData[2]={0x54,0x00};
	CAN_TxHeaderTypeDef pHeader;
	uint32_t Mailbox;



	pHeader.StdId=0x61;
	pHeader.IDE=CAN_ID_STD;
	pHeader.RTR = CAN_RTR_DATA;
	pHeader.DLC=2;
	pHeader.TransmitGlobalTime=DISABLE;
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  HAL_CAN_Start(&hcan1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("=======TP RESEAU======== \r\n");

  // Début de connection avec le composant + Gestion d'erreurs

	if(checkID()==0){
		printf("Connection reussie\r\n");
	} else {
		printf("Erreur lors de la connection du composant\r\n");
	}

	// Début de configuration du composant avec gestion d'erreur

	if(BMP280_config()==0){
		printf("Configuration reussie\r\n");

	} else {
		printf("Erreur lors de la configuration du composant\r\n");
	}

	BMP_etalonage();

  while (1)
  {

	  double temp, pres;
	  int varia=0;

	  temp = bmp280_compensate_T_double((uint32_t)BMP_get_temp());
	  pres = bmp280_compensate_P_double((uint32_t)BMP_get_pres());

	  int temp_int = (int)(temp);
	  temp_int -= 5;
	  int pres_int = (int)(pres);

	  printf("Temperature: %d C, Pressure: %d hPa\r\n", temp_int, pres_int);


	  varia=temp_int-cons_temp;
	  if (varia>=0){
		  aData[1]=0x00;
	  }else if (varia<0){
		  aData[1]=0x01;
	  }

	  updatePI((double)varia);

	 // varia=varia*10;

	  printf("varia = %d\r\n",(int)output);

//	  if((int)output < 0){
//		  output += 180;
//	  }

	  aData[0]=(int)output;

  	  HAL_CAN_AddTxMessage(&hcan1,&pHeader,aData,&Mailbox);


	  result = HAL_UART_Transmit(&huart1, (uint8_t*)&temp_int, sizeof(temp_int), HAL_MAX_DELAY);


	  if (result == HAL_OK) {
	      // Transfert réussi
	      printf("Transmission reussie : %d\r\n", temp_int);
	  } else {
	      // Gestion de l'erreur
	      printf("Erreur de transmission\r\n");
	  }


	  HAL_Delay(10000);
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
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLRCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
