/*
 * led.c
 *
 *  Created on: Nov 21, 2025
 *      Author: Kelly
 */

#define MCP_WRITE 0x40
#define MCP_READ 0x41

#define IODIRA 0x00
#define IODIRB 0x01

#define MCP_GPIOA_STATUS 0x12
#define MCP_GPIOB_STATUS 0x11

#define MCP_RESET_PORT GPIOA
#define MCP_RESET_PIN GPIO_PIN_0

#define MCP_CS_PORT GPIOB
#define MCP_CS_PIN GPIO_PIN_7

#include "main.h"
#include "gpio.h"
#include "led.h"


void MCP_Init(h_mcp_led_t *mcp_led)
{
	HAL_GPIO_WritePin(MCP_RESET_PORT, MCP_RESET_PIN, GPIO_PIN_SET);		// RESET HIGH
	HAL_GPIO_WritePin(MCP_CS_PORT, MCP_CS_PIN, GPIO_PIN_SET); 			// CS HIGH

	mcp_led->led_value[MCP_GPIOA] = 0xff;
	mcp_led->led_value[MCP_GPIOB] = 0xff;

	transmit_LED(mcp_led, MCP_GPIOA, mcp_led->led_value[MCP_GPIOA]);
	transmit_LED(mcp_led, MCP_GPIOB, mcp_led->led_value[MCP_GPIOB]);
}

static void MCP_Transmit(h_mcp_led_t *mcp_led, uint8_t *pData, uint16_t size)
{
	HAL_GPIO_WritePin(MCP_CS_PORT, MCP_CS_PIN, GPIO_PIN_RESET); // CS LOW

	HAL_SPI_Transmit(mcp_led->hspi, pData, size, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(MCP_CS_PORT, MCP_CS_PIN, GPIO_PIN_SET); // CS HIGH
}

void transmit_LED(h_mcp_led_t *mcp_led, MCP_GPIO gpio, uint8_t value)
{
	uint8_t pTxData[3] = {MCP_WRITE, gpio, value};
	MCP_Transmit(mcp_led, pTxData, 3);
	mcp_led->led_value[gpio] = value;
}

void toggle_LED(h_mcp_led_t *mcp_led, MCP_GPIO gpio, MCP_LED led)
{
	uint8_t led_value = mcp_led->led_value[gpio];

	if ((led_value & led))
		led_value &= ~led;
	else
		led_value |= led;

	uint8_t pTxData[3] = {MCP_WRITE, gpio, led_value};
	MCP_Transmit(mcp_led, pTxData, 3);
	mcp_led->led_value[gpio] = led_value;
}
