
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "st7735.h"
#include "fonts.h"
#include "stdio.h"
#include "string.h"
#include <stdint.h>
#include <stdlib.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef hlpuart1;
DMA_HandleTypeDef hdma_lpuart_rx;
DMA_HandleTypeDef hdma_lpuart_tx;

SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef hdma_spi3_rx;
DMA_HandleTypeDef hdma_spi3_tx;
//SPI_HandleTypeDef hspi3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
extern SPI_HandleTypeDef ST7735_SPI_PORT;
uint8_t DMA_Buff[255] = {0};
uint8_t main_Buff[255] = {0};
int rx_done = 0;
int tx_done = 0;
int game_done = 0;
uint8_t interpeter[255] = {0};
uint8_t storage[255] = {0};
int tx_complete = 0;
int up = 0;
int down = 1;
int complete = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_SPI3_Init(void);
volatile bool spi_dma_done = false;
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		memset(main_Buff, 0, strlen((char *)main_Buff));
		memcpy(main_Buff, (char *)DMA_Buff, strlen((char *)DMA_Buff));
		rx_done = 1;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);

    tx_done = true;
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
  MX_DMA_Init();
  MX_LPUART1_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  ST7735_Init();
//  ST7735_WriteString(20, 40, "DMA Test", Font_16x26, ST7735_RED, ST7735_WHITE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  ST7735_Backlight_On();
	  real1:
	  ST7735_FillScreenFast(ST7735_BLACK);

	  /* Display "Test" */


	  ST7735_WriteStringDMA(10, 10, "Shapes(E)", Font_11x18, ST7735_WHITE, ST7735_YELLOW);
	  ST7735_WriteStringDMA(10, 28, "List(E)", Font_11x18, ST7735_WHITE, ST7735_BLACK);

//	  HAL_Delay(5000);
//	  HAL_UARTEx_ReceiveToIdle_DMA(&hlpuart1, storage, sizeof(storage));
	  HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);
//	  while(rx_done != 1)
//	  {
//		  HAL_Delay(2000);
//	  }
//	  rx_done = 0;
	  	  while(1)
	  	  {
	  		 if(strcmp((char*)storage,"e") == 0)
			 {
	  			  goto stop;
			 }
	  		 else if(strcmp((char*)storage,"s") == 0)
	  		 {
	  			 goto scroll2;
	  		 }
	  		 else
	  		 {
	  			 goto real1;
	  		 }
	  	  }

	  	 if(strcmp((char *)storage, "e") == 0)
	  	 {
	  		 stop:
		  	  goto real;
		  	  memset(storage, 0, 255);
	  	 }

	  	if (strcmp((char *)storage, "s") == 0)
	    {
			scroll2:
	        ST7735_WriteStringDMA(10, 10, "Shapes(E)", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	        ST7735_WriteStringDMA(10, 28, "List(E)", Font_11x18, ST7735_WHITE, ST7735_YELLOW);
	        down = 0;
	        up = 1;
	        memset(storage, 0, 255);
	        HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);
	        if(strcmp((char *)storage, "e") == 0)
	        {
	        	ST7735_FillScreenFast(ST7735_BLACK);
		        ST7735_WriteStringDMA(10, 10, "-> Apple", Font_11x18, ST7735_WHITE, ST7735_BLACK);
		        ST7735_WriteStringDMA(10, 28, "-> Orange", Font_11x18, ST7735_WHITE, ST7735_BLACK);
		        ST7735_WriteStringDMA(10, 46, "-> Cherry", Font_11x18, ST7735_WHITE, ST7735_BLACK);
		        ST7735_WriteStringDMA(10, 64, "-> Mango", Font_11x18, ST7735_WHITE, ST7735_BLACK);
				do{
					memset(storage, 0, 255);
					HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);
					if(strcmp((char *)storage, "b") == 0)
					{
						goto real1;
					}
				}while((char *)storage != "b");
	        }
	        else
	        {
	        	if(strcmp((char *)storage, "w") == 0)
	        	{
	        		memset(storage, 0, 255);
	        		goto scroll;
	        	}
	        	else if(strcmp((char *)storage, "s") == 0)
	        	{
	        		memset(storage, 0, 255);
	        		goto scroll1;
	        	}
	        	else
	        	{
	        		memset(storage, 0, 255);
	        		goto real1;
	        	}
	        }
	        while(complete != 1);
	    }

	    if (strcmp((char *)storage, "w") == 0)
	    {
	    	scroll:
	    	scroll1:
	        ST7735_WriteStringDMA(10, 10, "Shapes(E)", Font_11x18, ST7735_WHITE, ST7735_YELLOW);
	        ST7735_WriteStringDMA(10, 28, "List(E)", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	        memset(storage, 0, 255);
	        HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);
	        if(strcmp((char *)storage, "e") == 0)
	        {
	        	goto real;
	        }
	        else
	        {
	        	if(strcmp((char *)storage, "s") == 0)
	        	{
	        		memset(storage, 0, 255);
	        		goto scroll2;
	        	}
	        	else if(strcmp((char *)storage, "w") == 0)
	        	{
	        		memset(storage, 0, 255);
	        		goto scroll2;
	        	}
	        	else
	        	{
	        		memset(storage, 0, 255);
	        		goto real1;
	        	}
	        }
	        memset(storage, 0, 255);
	        down = 1;
	        up = 0;
	        while(complete != 1);
	    }
	  while(complete != 1);
     }

  if(strcmp((char *)storage, "e") == 0)
  	 {
	  real:
	  do{
			memset(storage, 0, 255);
			ST7735_FillScreenFast(ST7735_BLACK);
			ST7735_WriteStringDMA(10, 10, "Rectang(E)", Font_11x18, ST7735_WHITE, ST7735_YELLOW);
			ST7735_WriteStringDMA(10, 28, "Triang(E)", Font_11x18, ST7735_WHITE, ST7735_BLACK);
			HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);

			if(strcmp((char *)storage, "e") == 0)
			{
				ST7735_FillScreenFast(ST7735_BLACK);
				ST7735_FillRectangleFast(10, 40, 108, 80, ST7735_GREEN);
				ST7735_DrawLine(0, 30, 127, 30, ST7735_GREEN);
				ST7735_DrawLine(0, 130, 127, 130, ST7735_GREEN);
				ST7735_DrawLine(0, 30, 0, 130, ST7735_GREEN);
				ST7735_DrawLine(127, 30, 127, 130, ST7735_GREEN);
				do{
					memset(storage, 0, 255);
					HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);
					if(strcmp((char *)storage, "b") == 0)
					{
						goto real;
					}
				}while((char *)storage != "b");
			}
 	  	    if (strcmp((char *)storage, "s") == 0)
 	    	{

				scroll3:
				ST7735_WriteStringDMA(10, 10, "Rectang(E)", Font_11x18, ST7735_WHITE, ST7735_BLACK);
				ST7735_WriteStringDMA(10, 28, "Triang(E)", Font_11x18, ST7735_WHITE, ST7735_YELLOW);
				down = 0;
				up = 1;
				memset(storage, 0, 255);
				HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);


				if(strcmp((char *)storage, "e") == 0)
				{
					ST7735_FillScreenFast(ST7735_BLACK);
					ST7735_DrawTriangle(0, 100, 64, 40, 128, 100, ST7735_GREEN);
					do{
						memset(storage, 0, 255);
						HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);
						if(strcmp((char *)storage, "b") == 0)
						{
							goto real;
						}
					}while((char *)storage != "b");
				}
				else if(strcmp((char *)storage, "b") == 0)
				{
					goto real1;
				}
			else
 	        {
 	        	if(strcmp((char *)storage, "w") == 0)
 	        	{
 	        		memset(storage, 0, 255);
 	        		goto scroll4;
 	        	}
 	        	else if(strcmp((char *)storage, "s") == 0)
 	        	{
 	        		memset(storage, 0, 255);
 	        		goto scroll5;
 	        	}

 	        }
 	        while(complete != 1);
 	    }

 	    else if (strcmp((char *)storage, "w") == 0 )
 	    {
 	    	scroll4:
 	    	scroll5:
			ST7735_WriteStringDMA(10, 10, "Rectang(E)", Font_11x18, ST7735_WHITE, ST7735_YELLOW);
			ST7735_WriteStringDMA(10, 28, "Triang(E)", Font_11x18, ST7735_WHITE, ST7735_BLACK);
 	        memset(storage, 0, 255);
 	        HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);
			if(strcmp((char *)storage, "e") == 0)
			{
				ST7735_FillScreenFast(ST7735_BLACK);
				ST7735_FillRectangleFast(10, 40, 108, 80, ST7735_GREEN);
				ST7735_DrawLine(0, 30, 127, 30, ST7735_GREEN);
				ST7735_DrawLine(0, 130, 127, 130, ST7735_GREEN);
				ST7735_DrawLine(0, 30, 0, 130, ST7735_GREEN);
				ST7735_DrawLine(127, 30, 127, 130, ST7735_GREEN);
				do{
					memset(storage, 0, 255);
					HAL_UART_Receive(&hlpuart1, storage, 1, HAL_MAX_DELAY);
					if(strcmp((char *)storage, "b") == 0)
					{
						goto real;
					}
				}while((char *)storage != "b");
			}
			else if(strcmp((char *)storage, "b") == 0)
			{
				goto real1;
			}
 	        else
 	        {
 	        	if(strcmp((char *)storage, "s") == 0)
 	        	{
 	        		memset(storage, 0, 255);
 	        		goto scroll3;
 	        	}
 	        	else if(strcmp((char *)storage, "w") == 0)
 	        	{
 	        		memset(storage, 0, 255);
 	        		goto scroll3;
 	        	}
 	        }
 	        memset(storage, 0, 255);
 	        down = 1;
 	        up = 0;
 	        while(complete != 1);
 	    }
 	   else if (strcmp((char *)storage, "b") == 0 )
 	   {
 		   goto real1;
 	   }
	  }while((char *)storage != "b");



 	  while(complete != 1);
      }
  	 }


  /* USER CODE END 3 */
//}

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 71;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV6;
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

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_2;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

	SPI_HandleTypeDef *hspi3 = &ST7735_SPI_PORT;
  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3->Instance = SPI3;
  hspi3->Init.Mode = SPI_MODE_MASTER;
  hspi3->Init.Direction = SPI_DIRECTION_2LINES;
  hspi3->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3->Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3->Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3->Init.NSS = SPI_NSS_SOFT;
  hspi3->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi3->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3->Init.CRCPolynomial = 7;
  hspi3->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.battery_charging_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel1_IRQn);
  /* DMA2_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);
  /* DMA2_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel6_IRQn);
  /* DMA2_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable clocks
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_PWREx_EnableVddIO2();


     /*Configure GPIO pin Output Level */
     HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

     /*Configure GPIO pin Output Level */
     HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

     /*Configure GPIO pin : B1_Pin */
     GPIO_InitStruct.Pin = B1_Pin;
     GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

     /*Configure GPIO pins : LD3_Pin LD2_Pin */
     GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

     /*Configure GPIO pin : USB_OverCurrent_Pin */
     GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
     GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

     /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
     GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
     HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

    // Display Pins
    // Backlight (PB15)
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);  // Backlight ON

    // Reset (PB2)
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // DC (PB1)
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // CS (PA4)
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Set initial states
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);   // Reset high
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // DC low (command)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // CS high

    // ... rest of your existing GPIO configuration ...
}
//static void MX_GPIO_Init(void)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  /* USER CODE BEGIN MX_GPIO_Init_1 */
//
//  /* USER CODE END MX_GPIO_Init_1 */
//
//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  __HAL_RCC_GPIOH_CLK_ENABLE();
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();
//  __HAL_RCC_GPIOG_CLK_ENABLE();
//  HAL_PWREx_EnableVddIO2();
//
//  /*Configure GPIO pin Output Level */
//  GPIO_InitStruct.Pin = GPIO_PIN_15;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//  // Display Control Pins
//  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//  GPIO_InitStruct.Pin = GPIO_PIN_4;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|LD3_Pin|GPIO_PIN_15
//                          |LD2_Pin, GPIO_PIN_RESET);
//
//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);
//
//  /*Configure GPIO pin : B1_Pin */
//  GPIO_InitStruct.Pin = B1_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pins : PB1 PB2 LD3_Pin PB15
//                           LD2_Pin */
//  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|LD3_Pin|GPIO_PIN_15
//                          |LD2_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : USB_OverCurrent_Pin */
//  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
//  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);
//
//  /* USER CODE BEGIN MX_GPIO_Init_2 */
//
//  /* USER CODE END MX_GPIO_Init_2 */
//}

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
