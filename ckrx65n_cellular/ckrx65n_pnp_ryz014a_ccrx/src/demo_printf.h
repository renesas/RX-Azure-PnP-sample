
#ifndef DEMO_PRINTF_H
#define DEMO_PRINTF_H

#include "hardware_setup.h"
#include "r_sci_rx_if.h"

#define LOG_TERMINAL_INIT demo_printf_init
#define LOG_TERMINAL demo_printf

void demo_printf_init(void);

void demo_printf(char *format, ...);

void demo_flash_printf_init(void);
sci_err_t demo_flash_printf_SCI_Send(uint8_t *p_src, uint16_t const length);
sci_err_t demo_flash_printf_SCI_Receive(uint8_t *p_src, uint16_t const length);
void demo_flash_printf_close(void);

#endif
