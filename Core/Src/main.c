/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body with scrolling menu
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
/////* USER CODE BEGIN Header */
#include "main.h"
#include "st7735.h"
#include "fonts.h"
#include <stdio.h>
#include <stddef.h>
#include "menu.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/* Peripheral Handles &
*  Private variables ---------------------------------------------------------*/
UART_HandleTypeDef hlpuart1;
DMA_HandleTypeDef hdma_lpuart_rx;
DMA_HandleTypeDef hdma_lpuart_tx;
SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef hdma_spi3_rx;
DMA_HandleTypeDef hdma_spi3_tx;
PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* Buffers for reception*/
///* USER CODE BEGIN PV */
uint8_t DMA_Buff[255] = {0};
uint8_t main_Buff[255] = {0};

/* Menu State */
typedef enum {
    MENU_MAIN,
    MENU_SHAPES,
    MENU_LIST,
    MENU_SHAPE_RECT,
    MENU_SHAPE_TRIANGLE
} MenuState;

static MenuState currentMenu = MENU_MAIN;
static int selection = 0;  // Scroll index position

/* Menu Item Arrays */
const char *mainMenuItems[] = {"Shapes", "List"}; //main menu items
const int   mainMenuSize    = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]);

const char *shapesMenuItems[] = {"Rectangle", "Triangle", "Back"}; //shapes selected menu items
const int   shapesMenuSize    = sizeof(shapesMenuItems) / sizeof(shapesMenuItems[0]);

const char *listMenuItems[] = {"Apple", "Orange", "Cherry", "Mango", "Back"}; //list selected items
const int   listMenuSize    = sizeof(listMenuItems) / sizeof(listMenuItems[0]);

/* Helper to draw menu */
/**
  * @brief drawMenu Function : Draws the passed menu array, Also toggle selected menu items bgcolor
  * @param items (pointer to pointer linking to various menus)
  * @param itemCount (Size of the array being passed)
  * @param selected (selection index/number being passed)
  * @param bgColor (Highlighting the Background of text)
  * @param textColor (Highlight the text in decided color)
  * @retval None
  */
	/* USER CODE BEGIN 0 */
void drawMenu(const char **items, int itemCount, int selected) {
    ST7735_FillScreenFast(ST7735_BLACK);
    for (int i = 0; i < itemCount; i++) {
        uint16_t bgColor = (i == selected) ? ST7735_YELLOW : ST7735_BLACK;
        uint16_t textColor = (i == selected) ? ST7735_BLACK : ST7735_WHITE;
        ST7735_WriteStringDMA(10, 10 + i * 18, items[i], Font_11x18, textColor, bgColor);
    }
}

/* Menu Drawing Functions */
void drawRectangle(void) {
    ST7735_FillScreenFast(ST7735_BLACK); //resets screen to black so previous text doesn't show
    ST7735_FillRectangleFast(10, 40, 108, 80, ST7735_GREEN); //Rectangle transmission function called on (x , y) coords with selected width and height;
    ST7735_WriteStringDMA(10, 130, "Back (B)", Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

void drawTriangle(void) {
    ST7735_FillScreenFast(ST7735_BLACK); //resets screen to black so previous text doesn't show
    ST7735_DrawTriangle(0, 100, 64, 40, 128, 100, ST7735_GREEN); //Triangle transmission function called on three different (x , y) coords
    ST7735_WriteStringDMA(10, 130, "Back (B)", Font_11x18, ST7735_WHITE, ST7735_BLACK);
}

/* Menu Loop */
/**
  * @brief MenuLoop Function : Used for Cycling through menus and selecting different functions
  * @param mainMenuItems (Array of main menu items)
  * @param mainMenuSize (Size of the main menu array)
  * @param selection (selection index/number being passed), default = 0
  * @param input (Array for receiving characters) used for if conditions
  * @param shapesMenuItems (Array of shapes menu items)
  * @param shapesMenuSize (Size of the shapes menu array)
  * @param listMenuItems (Array of list menu items)
  * @param listMenuSize (Size of list menu array)
  * @retval None
  */
void menuLoop(void) {
    uint8_t input[2] = {0};

    while (1) { //Switch Case State Machine
        switch (currentMenu) {
            case MENU_MAIN:
                drawMenu(mainMenuItems, mainMenuSize, selection); //Draws Main Menu items
                HAL_UART_Receive(&hlpuart1, input, 1, HAL_MAX_DELAY); //receives character from TeraTerm and puts it into input
                if (input[0] == 'w' || input[0] == 'W') //Scrolls up and updates scroll index selection
                    selection = (selection - 1 + mainMenuSize) % mainMenuSize;
                else if (input[0] == 's' || input[0] == 'S') //Scrolls down and updates scroll index selection
                    selection = (selection + 1) % mainMenuSize;
                else if (input[0] == 'e' || input[0] == 'E') { //Selects the menu you want to go in depending on scroll index (selection)
                    if (selection == 0) currentMenu = MENU_SHAPES;
                    else if (selection == 1) currentMenu = MENU_LIST;
                    selection = 0;
                }
                break;

            case MENU_SHAPES:
                drawMenu(shapesMenuItems, shapesMenuSize, selection); //Draws Shapes Menu items
                HAL_UART_Receive(&hlpuart1, input, 1, HAL_MAX_DELAY); //receives character from TeraTerm and puts it into input
                if (input[0] == 'w' || input[0] == 'W') //Scrolls up and updates scroll index selection
                    selection = (selection - 1 + shapesMenuSize) % shapesMenuSize;
                else if (input[0] == 's' || input[0] == 'S') //Scrolls down and updates scroll index selection
                    selection = (selection + 1) % shapesMenuSize;
                else if (input[0] == 'e' || input[0] == 'E') { //Selects the menu you want to go in depending on scroll index (selection)
                    if (selection == 0) currentMenu = MENU_SHAPE_RECT;
                    else if (selection == 1) currentMenu = MENU_SHAPE_TRIANGLE;
                    else if (selection == 2) { currentMenu = MENU_MAIN; selection = 0; }
                }
                break;

            case MENU_LIST:
                drawMenu(listMenuItems, listMenuSize, selection); //Draws List Menu items
                HAL_UART_Receive(&hlpuart1, input, 1, HAL_MAX_DELAY); //receives character from TeraTerm and puts it into input
                if (input[0] == 'w' || input[0] == 'W') //Scrolls up and updates scroll index selection
                    selection = (selection - 1 + listMenuSize) % listMenuSize;
                else if (input[0] == 's' || input[0] == 'S') //Scrolls down and updates scroll index selection
                    selection = (selection + 1) % listMenuSize;
                else if (input[0] == 'e' || input[0] == 'E') { //Selects the menu you want to go in depending on scroll index (selection)
                    if (selection == listMenuSize - 1) { currentMenu = MENU_MAIN; selection = 0; }
                }
                break;

            case MENU_SHAPE_RECT:
                drawRectangle(); //Draws a rectanlge
                HAL_UART_Receive(&hlpuart1, input, 1, HAL_MAX_DELAY); //receives character from TeraTerm and puts it into input
                if (input[0] == 'b' || input[0] == 'B') { currentMenu = MENU_SHAPES; selection = 0; } //if statement using b character to cycle back to the previous menu
                break;

            case MENU_SHAPE_TRIANGLE:
                drawTriangle(); //Draws a Triangle
                HAL_UART_Receive(&hlpuart1, input, 1, HAL_MAX_DELAY); //receives character from TeraTerm and puts it into input
                if (input[0] == 'b' || input[0] == 'B') { currentMenu = MENU_SHAPES; selection = 0; } //if statement using b character to cycle back to the previous menu
                break;
        }
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    memcpy(main_Buff, DMA_Buff, Size);
}
	/* USER CODE END 0 */


/* Boilerplate Init Functions */
  /* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_SPI3_Init(void);

/* Main Entry */
int main(void) {
  /* MCU Configuration--------------------------------------------------------*/
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init(); //Intializes GPIO pins
    MX_DMA_Init(); // Intializes DMA
    MX_LPUART1_UART_Init();
    MX_USB_OTG_FS_PCD_Init();
    MX_SPI3_Init();

    ST7735_Init(); //Intializes the Display
    while(1)
    {
  /* USER CODE BEGIN 3 */
    	menuLoop();
  /* USER CODE END 3 */
    }
  /* USER CODE END WHILE */
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
