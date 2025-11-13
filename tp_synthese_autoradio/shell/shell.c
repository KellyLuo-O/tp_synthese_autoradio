/*
 * shell.c
 *
 *  Created on: 7 juin 2019
 *      Author: Laurent Fiack
 */

#include "shell.h"


#include <stdio.h>

#include "gpio.h"



static char uart_read(h_shell_t *shell)
{
	char c;
	shell -> drv_shell.drv_shell_receive(shell, &c, 1);
	return c;

}

static int uart_write(h_shell_t *shell, char * s, uint16_t size)
{

	shell -> drv_shell.drv_shell_transmit(s, size);
	return size;

}

static int sh_help(h_shell_t *shell, int argc, char ** argv)
{
	int i;

	for(i = 0 ; i < shell -> shell_func_list_size ; i++) {
		int size;
		size = snprintf (shell -> print_buffer, BUFFER_SIZE, "%c: %s\r\n", shell -> shell_func_list[i].c, shell -> shell_func_list[i].description);
		uart_write(shell, shell -> print_buffer, size);
	}

	return 0;
}


// fonction to call in the uart rx interrupt
void shell_uart_rx_callback(h_shell_t *shell)
{
	BaseType_t hptw;

	xSemaphoreGiveFromISR(shell -> sem_shell_rx, &hptw);

	portYIELD_FROM_ISR(hptw);
}


void shell_init(h_shell_t *shell)
{
	int size = 0;


	shell -> shell_func_list_size = 0;
	shell -> sem_shell_rx = xSemaphoreCreateBinary();

	size = snprintf (shell -> print_buffer, BUFFER_SIZE, "\r\n\r\n===== Monsieur Shell v0.2 =====\r\n");
	uart_write(shell, shell -> print_buffer, size);

	shell_add(shell, 'h', sh_help, "Help");

}

int shell_add(h_shell_t *shell, char c, int (* pfunc)(h_shell_t *shell, int argc, char ** argv), char * description)
{
	if (shell -> shell_func_list_size < SHELL_FUNC_LIST_MAX_SIZE) {
		shell -> shell_func_list[shell -> shell_func_list_size].c = c;
		shell -> shell_func_list[shell -> shell_func_list_size].func = pfunc;
		shell -> shell_func_list[shell -> shell_func_list_size].description = description;
		shell -> shell_func_list_size++;
		return 0;
	}

	return -1;
}

static int shell_exec(h_shell_t *shell, char * buf)
{
	int i;

	char c = buf[0];

	int argc;
	char * argv[ARGC_MAX];
	char *p;

	for(i = 0 ; i < shell -> shell_func_list_size ; i++) {
		if (shell -> shell_func_list[i].c == c) {
			argc = 1;
			argv[0] = buf;

			for(p = buf ; *p != '\0' && argc < ARGC_MAX ; p++){
				if(*p == ' ') {
					*p = '\0';
					argv[argc++] = p+1;
				}
			}

			return shell -> shell_func_list[i].func(shell, argc, argv);
		}
	}

	int size;
	size = snprintf (shell -> print_buffer, BUFFER_SIZE, "%c: no such command\r\n", c);
	uart_write(shell, shell -> print_buffer, size);
	return -1;
}


static char prompt[] = "> ";

int shell_run(h_shell_t *shell)
{
	int reading = 0;
	int pos = 0;


	while (1) {
		uart_write(shell, prompt, 2);
		reading = 1;

		while(reading) {
			char c = uart_read(shell);
			int size;

			switch (c) {
			//process RETURN key
			case '\r':
				//case '\n':
				size = snprintf (shell -> print_buffer, BUFFER_SIZE, "\r\n");
				uart_write(shell, shell -> print_buffer, size);
				shell -> cmd_buffer[pos++] = 0;     //add \0 char at end of string
				size = snprintf (shell -> print_buffer, BUFFER_SIZE, ":%s\r\n", shell -> cmd_buffer);
				uart_write(shell, shell -> print_buffer, size);
				reading = 0;        //exit read loop
				pos = 0;            //reset buffer
				break;
				//backspace
			case '\b':
				if (pos > 0) {      //is there a char to delete?
					pos--;          //remove it in buffer

					uart_write(shell, "\b \b", 3);	// delete the char on the terminal
				}
				break;
				//other characters
			default:
				//only store characters if buffer has space
				if (pos < BUFFER_SIZE-1) {
					uart_write(shell, &c, 1);
					shell -> cmd_buffer[pos++] = c; //store
				}
			}
		}
		shell_exec(shell, shell -> cmd_buffer);
	}
	return 0;
}
