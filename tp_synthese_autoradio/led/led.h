/*
 * led.h
 *
 *  Created on: Nov 21, 2025
 *      Author: Kelly
 */

#ifndef LED_H_
#define LED_H_

#include "spi.h"

typedef enum
{
	MCP_GPIOA = 0x00,
	MCP_GPIOB = 0x01
} MCP_GPIO;

typedef enum
{
	LED1 = 0x01,
	LED2 = 0x02,
	LED3 = 0x04,
	LED4 = 0x08,
	LED5 = 0x10,
	LED6 = 0x20,
	LED7 = 0x40,
	LED8 = 0x80,
} MCP_LED;

typedef struct
{
	SPI_HandleTypeDef *hspi;
	uint8_t led_value[2];
} h_mcp_led_t;


void MCP_Init(h_mcp_led_t *mcp_led);
void transmit_LED(h_mcp_led_t *mcp_led, MCP_GPIO gpio, uint8_t value);

void toggle_LED(h_mcp_led_t *mcp_led, MCP_GPIO gpio, MCP_LED led);

#endif /* LED_H_ */
