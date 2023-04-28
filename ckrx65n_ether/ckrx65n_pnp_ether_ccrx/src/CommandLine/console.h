/*
 * console.h
 *
 *  Created on: 15 Nov 2021
 *      Author: a5125422
 */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_
#include "r_smc_entry.h"
#define SCI_BUFFER_SIZE 2048


void console_write(const char *buffer);
void console_transmit_end(void);
void console_receive_end(void);
void printf_colour(const char *format, ...);
void start_key_check(void);
void clear_console(void);
char wait_for_keypress(void);
int8_t get_detected_key(void);
bool_t key_pressed(void);
void my_sci_callback(void *pArgs);

#endif /* CONSOLE_CONSOLE_H_ */
