/*
 * shell.h
 *
 *  Created on: 7 juin 2019
 *      Author: laurent
 */

#ifndef INC_LIB_SHELL_SHELL_H_
#define INC_LIB_SHELL_SHELL_H_

#include <stdint.h>

#include "usart.h"
#include "cmsis_os.h"

#define UART_DEVICE huart2

#define ARGC_MAX 8
#define BUFFER_SIZE 100
#define SHELL_FUNC_LIST_MAX_SIZE 64

struct h_shell_struct;


typedef uint8_t (* drv_shell_transmit_t)(char * pData, uint16_t size);	//pointeur de fonction (on definit une type de fonction: drv_shell_transmit_t)(on peut utiliser drv_shell_transmit_t a la place de tout char * pData ...)
typedef uint8_t (* drv_shell_receive_t)(struct h_shell_struct* shell, char * pData, uint16_t size);

typedef struct drv_shell_struct
{
	drv_shell_transmit_t drv_shell_transmit;
	drv_shell_receive_t drv_shell_receive;
} drv_shell_t;


typedef struct{
	char c;
	int (* func)(struct h_shell_struct *shell, int argc, char ** argv);
	char * description;
} shell_func_t;

typedef struct h_shell_struct{
	drv_shell_t drv_shell;

	UART_HandleTypeDef * huart;

	SemaphoreHandle_t  sem_shell_rx;

	int shell_func_list_size;
	shell_func_t shell_func_list[SHELL_FUNC_LIST_MAX_SIZE];

	char print_buffer[BUFFER_SIZE];
	char cmd_buffer[BUFFER_SIZE];
}h_shell_t;

void shell_init(h_shell_t *shell);
int shell_add(h_shell_t *shell, char c, int (* pfunc)(h_shell_t *shell, int argc, char ** argv), char * description);
int shell_run(h_shell_t *shell);
void shell_uart_rx_callback(h_shell_t *shell);




#endif /* INC_LIB_SHELL_SHELL_H_ */
