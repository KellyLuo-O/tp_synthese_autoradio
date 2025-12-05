/*
 * sgtl5000.c
 *
 *  Created on: Nov 28, 2025
 *      Author: Kelly
 */

#include "sgtl5000.h"
#include "i2c.h"
#include <stdio.h>

#define SGTL_I2C_DEV_ADDRESS	0x14

#define REG_CHIP_ID 		0x0000

#define REG_CHIP_ANA_POWER     0x0030
#define REG_CHIP_LINREG_CTRL   0x0026
#define REG_CHIP_REF_CTRL      0x0028
#define REG_CHIP_LINE_OUT_CTRL 0x002C
#define REG_CHIP_SHORT_CTRL    0x003C
#define REG_CHIP_ANA_CTRL      0x0024
#define REG_CHIP_DIG_POWER     0x0002

#define REG_CHIP_LINE_OUT_VOL  0x002E
#define REG_CHIP_CLK_CTRL      0x0004
#define REG_CHIP_I2S_CTRL      0X0006
#define REG_CHIP_ADCDAC_CTRL   0x000E
#define REG_CHIP_DAC_VOL       0x0010

#define CHIP_ANA_POWER     	0x61FF
#define CHIP_LINREG_CTRL   	(0x1 << 5) | (0x1 << 6)
#define CHIP_REF_CTRL      	0x01FF
#define CHIP_LINE_OUT_CTRL 	0x031E
#define CHIP_SHORT_CTRL    	0x1106
#define CHIP_ANA_CTRL 		0x4 //(1 << 5)

#define CHIP_DIG_POWER		0x0073
#define CHIP_LINE_OUT_VOL 	0x0505
#define CHIP_CLK_CTRL 		(0x2 << 2) | 0x0
#define CHIP_I2S_CTRL 		(0x1 << 7) | (0x0 << 6) | (0x3 << 4) | (0x0 << 2)
#define CHIP_ADCDAC_CTRL	0x0
#define CHIP_DAC_VOL		0x3C3C



void sgtl_get_id()
{
	uint8_t rdata[1];

	HAL_I2C_Mem_Read(&hi2c2, SGTL_I2C_DEV_ADDRESS, REG_CHIP_ID, 2, rdata, 1, HAL_MAX_DELAY);

	printf("chip id du codec : %x\r\n", rdata[0]);
}



static void sgtl_write_config(uint16_t reg, uint16_t config_value)
{
	uint8_t tdata[2];
	uint8_t rdata[2];
	tdata[0] = (config_value >> 8) & 0xFF;  // MSB
	tdata[1] = config_value & 0xFF;         // LSB

	HAL_I2C_Mem_Write(&hi2c2, SGTL_I2C_DEV_ADDRESS, reg, 2, tdata, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c2, SGTL_I2C_DEV_ADDRESS, reg, 2, rdata, 2, HAL_MAX_DELAY);
	uint16_t config_ret = (rdata[0] << 8) | rdata[1];
	if (config_ret != config_value)
		printf("error in config sglt\r\n");
}


static void sgtl_set_bit(uint16_t reg, uint16_t mask)
{
	uint8_t tdata[2];
	uint8_t rdata[2];


	HAL_I2C_Mem_Read(&hi2c2, SGTL_I2C_DEV_ADDRESS, reg, 2, rdata, 2, HAL_MAX_DELAY);
	uint16_t config_value = (rdata[0] << 8) | rdata[1];
	config_value |= mask;
	tdata[0] = (config_value >> 8) & 0xFF;  // MSB
	tdata[1] = config_value & 0xFF;         // LSB

	HAL_I2C_Mem_Write(&hi2c2, SGTL_I2C_DEV_ADDRESS, reg, 2, tdata, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c2, SGTL_I2C_DEV_ADDRESS, reg, 2, rdata, 2, HAL_MAX_DELAY);
	uint16_t config_ret = (rdata[0] << 8) | rdata[1];
	if (config_ret != config_value)
		printf("error in config sglt\r\n");
}


static void sgtl_clear_bit(uint16_t reg, uint16_t mask)
{
	uint8_t tdata[2];
	uint8_t rdata[2];


	HAL_I2C_Mem_Read(&hi2c2, SGTL_I2C_DEV_ADDRESS, reg, 2, rdata, 2, HAL_MAX_DELAY);
	uint16_t config_value = (rdata[0] << 8) | rdata[1];
	config_value &= ~mask;
	tdata[0] = (config_value >> 8) & 0xFF;  // MSB
	tdata[1] = config_value & 0xFF;         // LSB

	HAL_I2C_Mem_Write(&hi2c2, SGTL_I2C_DEV_ADDRESS, reg, 2, tdata, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c2, SGTL_I2C_DEV_ADDRESS, reg, 2, rdata, 2, HAL_MAX_DELAY);
	uint16_t config_ret = (rdata[0] << 8) | rdata[1];
	if (config_ret != config_value)
		printf("error in config sglt\r\n");
}


void sgtl_configure()
{

	sgtl_clear_bit(REG_CHIP_ANA_POWER, (1 << 12) | (1 << 13));
	sgtl_set_bit(REG_CHIP_LINREG_CTRL, (1 << 5) | (1 << 6));
	sgtl_write_config(REG_CHIP_REF_CTRL, CHIP_REF_CTRL);
	sgtl_write_config(REG_CHIP_LINE_OUT_CTRL, CHIP_LINE_OUT_CTRL);
	sgtl_write_config(REG_CHIP_SHORT_CTRL, CHIP_SHORT_CTRL);
	sgtl_write_config(REG_CHIP_ANA_CTRL, CHIP_ANA_CTRL);

	sgtl_write_config(REG_CHIP_ANA_POWER, 0x6AFF);
	sgtl_write_config(REG_CHIP_DIG_POWER, CHIP_DIG_POWER);
	sgtl_write_config(REG_CHIP_LINE_OUT_VOL, 0X1111);
	sgtl_write_config(REG_CHIP_CLK_CTRL, 0x0004);
	sgtl_write_config(REG_CHIP_I2S_CTRL, 0x0130);
	sgtl_write_config(REG_CHIP_ADCDAC_CTRL, CHIP_ADCDAC_CTRL);
	sgtl_write_config(REG_CHIP_DAC_VOL, CHIP_DAC_VOL);

}
