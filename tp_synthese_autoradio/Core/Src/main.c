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
#include "cmsis_os.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "led.h"
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
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int chr)
{
	HAL_UART_Transmit(&huart2, (uint8_t*) &chr, 1, HAL_MAX_DELAY);
	return chr;
}


uint8_t drv_uart_receive(h_shell_t *shell, char * pData, uint16_t size)
{
	HAL_UART_Receive_IT(&huart2, (uint8_t*)(pData), size);
	xSemaphoreTake(shell -> sem_shell_rx, portMAX_DELAY);
	return 0;
}

uint8_t drv_uart_transmit(char * pData, uint16_t size)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)pData, size, HAL_MAX_DELAY);
	return size;
}

h_shell_t shell =
{
		.huart = &huart2,
		.drv_shell =
		{
				.drv_shell_receive = drv_uart_receive,
				.drv_shell_transmit = drv_uart_transmit
		}
};

h_mcp_led_t mcp_led =
{
		.hspi = &hspi3
};

int fonction(h_shell_t *shell, int argc, char ** argv)
{
	printf("Je suis une fonction bidon\r\n");

	printf("argc = %d\r\n", argc);	//nb de caractere

	for (int i = 0; i < argc; i++)
	{
		printf("argv[%d] = %s\r\n", i, argv[i]);	//pointeur de table de caractere
	}
	return 0;
}

int shell_led(h_shell_t *shell, int argc, char ** argv)
{
	if (argc != 3)
	{
		printf("usage : l GPIOx 0xAA (bit 1 means led OFF and bit 0 means led ON)\r\n");
		return 0;
	}

	MCP_GPIO gpio;

	if (strcmp(argv[1], "GPIOA") == 0) gpio = MCP_GPIOA;
	else if (strcmp(argv[1], "GPIOB") == 0) gpio = MCP_GPIOB;
	else
	{
		printf("GPIO invalid : GPIOA or GPIOB\r\n");
		return 0;
	}

	uint8_t cmd = strtol(argv[2], NULL, 16);

	printf("test : %02x\r\n", cmd);
	transmit_LED(&mcp_led, gpio, cmd);

	return 0;
}

int shell_toggle_led(h_shell_t *shell, int argc, char ** argv)
{
	if (argc != 3)
	{
		printf("usage : L GPIOx LED1\r\n");
		return 0;
	}

	MCP_GPIO gpio;

	if (strcmp(argv[1], "GPIOA") == 0) gpio = MCP_GPIOA;
	else if (strcmp(argv[1], "GPIOB") == 0) gpio = MCP_GPIOB;
	else
	{
		printf("GPIO invalid : GPIOA or GPIOB\r\n");
		return 0;
	}

	MCP_LED led = 0xff;

	if (strcmp(argv[2], "LED1") == 0) led = LED1;
	else if (strcmp(argv[2], "LED2") == 0) led = LED2;
	else if (strcmp(argv[2], "LED3") == 0) led = LED3;
	else if (strcmp(argv[2], "LED4") == 0) led = LED4;
	else if (strcmp(argv[2], "LED5") == 0) led = LED5;
	else if (strcmp(argv[2], "LED6") == 0) led = LED6;
	else if (strcmp(argv[2], "LED7") == 0) led = LED7;
	else if (strcmp(argv[2], "LED8") == 0) led = LED8;
	else
	{
		printf("LED invalid : LEDx 1 to 8\r\n");
		return 0;
	}

	toggle_LED(&mcp_led, gpio, led);

	return 0;
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		shell_uart_rx_callback(&shell);
	}

}

void task_shell (void * unused)
{
	shell_init(&shell);
	shell_add(&shell, 'f', fonction, "Une fonction inutile");
	shell_add(&shell, 'l', shell_led, "Commande les LED");
	shell_add(&shell, 'L', shell_toggle_led, "Commande les LED");
	shell_run(&shell);
}


void task_spi_led (void * unused)
{
	uint8_t led_value[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	int led_index = 0;
	printf("SPI LED task started\r\n");

	MCP_Init(&mcp_led);

	// Allumer toutes les LED GPIOA
	transmit_LED(&mcp_led, MCP_GPIOA, 0x00);
	// Allume toutes les LED GPIOB
	transmit_LED(&mcp_led, MCP_GPIOB, 0x00);

	while (1)
	{
		// CHENILLLE SUr les LED GPIOA
		led_index++;
		if (led_index == 8)
			led_index = 0;

		transmit_LED(&mcp_led, MCP_GPIOA, ~led_value[led_index]);
		vTaskDelay(500);
	}
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
	MX_USART2_UART_Init();
	MX_SPI3_Init();
	/* USER CODE BEGIN 2 */

	printf("\r\n =========> TP SYNTHESE AUTO RADIO <======== \r\n");


	if (xTaskCreate(task_shell, "shell", 512, NULL, 2, NULL) != pdPASS)
	{
		printf("ERROR\r\n");
		Error_Handler();
	}

	if (xTaskCreate(task_spi_led, "spi_led", 512, NULL, 1, NULL) != pdPASS)
	{
		printf("ERROR\r\n");
		Error_Handler();
	}

	vTaskStartScheduler();

	/* USER CODE END 2 */

	/* Call init function for freertos objects (in cmsis_os2.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(500);
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
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6)
	{
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
