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

typedef struct
{
	SPI_HandleTypeDef *hspi;
	uint8_t led_value[2];
} h_mcp_led_t;


void MCP_Init(h_mcp_led_t *mcp_led);
void transmit_LED(h_mcp_led_t *mcp_led, MCP_GPIO gpio, uint8_t value);

#endif /* LED_H_ */
