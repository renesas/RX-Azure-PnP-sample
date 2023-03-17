
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <tx_api.h>

#include "r_cg_macrodriver.h"
#include <platform.h>
#include "r_sci_rx_pinset.h"

#include "demo_printf.h"
#include "sensorsData.h"
#include "CommandLine/menu_flash.h"
#define DEMO_PRINTF_BUF_LEN (256U)

static void demo_printf_transmit_end(void * pArgs);
static void demo_flash_printf_interrupt(void * pArgs);

static char demo_printf_buf[DEMO_PRINTF_BUF_LEN + 1u];

TX_MUTEX demo_printf_mutex;
TX_SEMAPHORE demo_printf_semaphore;
sci_hdl_t Console;

void demo_printf_init(void)
{
    UINT res;
    sci_cfg_t config;
    sci_err_t err;

    /* Initialize SCI to communicate with PC terminal */
    config.async.baud_rate = 115200;
    config.async.clk_src = SCI_CLK_INT;
    config.async.data_size = SCI_DATA_8BIT;
    config.async.parity_en = SCI_PARITY_OFF;
    config.async.parity_type = SCI_EVEN_PARITY;
    config.async.stop_bits = SCI_STOPBITS_1;
    config.async.int_priority = 15;
    err = R_SCI_Open(SCI_CH5, SCI_MODE_ASYNC, &config, demo_printf_transmit_end, &Console);


    R_SCI_PinSet_SCI5();

    res = tx_mutex_create(&demo_printf_mutex, "Demo printf mutex", TX_INHERIT);
    if(res != TX_SUCCESS) {
        for(;;) {}
    }

    res = tx_semaphore_create(&demo_printf_semaphore, "Demo printf semaphore", 0u);
    if(res != TX_SUCCESS) {
        for(;;) {}
    }

}

void demo_printf(char *format, ...)
{
    va_list v_args;
    size_t len;

    tx_mutex_get(&demo_printf_mutex, TX_WAIT_FOREVER);

    va_start(v_args, format);

    len = (size_t)vsnprintf(&demo_printf_buf[0], sizeof(demo_printf_buf), format, v_args);

    R_SCI_Send(Console,(uint8_t *)demo_printf_buf,len);

    va_end(v_args);

    tx_semaphore_get(&demo_printf_semaphore, TX_WAIT_FOREVER);

    tx_mutex_put(&demo_printf_mutex);
}

static void demo_printf_transmit_end(void * pArgs)
{
//	sci_cb_args_t *p_args = (sci_cb_args_t*) pArgs;
//
//	if (SCI_EVT_TEI == p_args->event)
//	{
//		tx_semaphore_put(&demo_printf_semaphore);
//	}
	sci_cb_args_t *args;
	sci_err_t sci_err;
	uint8_t tmp;
	/* We have not woken a task at the start of the ISR. */

	args = (sci_cb_args_t*) pArgs;

	if (args->event == SCI_EVT_RX_CHAR)
	{
		/* From RXI interrupt; received character data is in args->byte */
		nop();
		sci_err = R_SCI_Receive(Console, &tmp, 1);
		if (sci_err == SCI_SUCCESS)
		{
			g_write_buffer[uiInWritebufferIndex] = (char) tmp;
			uiInWritebufferIndex++;
			receive_complete = true;
		}

	}
	else if (args->event == SCI_EVT_RXBUF_OVFL)
	{
		/* From RXI interrupt; rx queue is full; 'lost' data is in args->byte
		 You will need to increase buffer size or reduce baud rate */
		nop();
	}
	else if (args->event == SCI_EVT_OVFL_ERR)
	{
		/* From receiver overflow error interrupt; error data is in args->byte
		 Error condition is cleared in calling interrupt routine */
		nop();
	}
	else if (args->event == SCI_EVT_FRAMING_ERR)
	{
		/* From receiver framing error interrupt; error data is in args->byte
		 Error condition is cleared in calling interrupt routine */
		nop();
	}
	else if (args->event == SCI_EVT_PARITY_ERR)
	{
		/* From receiver parity error interrupt; error data is in args->byte
		 Error condition is cleared in calling interrupt routine */
		nop();
	}
	else if (args->event == SCI_EVT_TEI)
	{
		tx_semaphore_put(&demo_printf_semaphore);
	}

}

void my_sw_charput_function(uint8_t c)
{
    tx_mutex_get(&demo_printf_mutex, TX_WAIT_FOREVER);
    
    R_SCI_Send(Console,&c,1);
    
    tx_semaphore_get(&demo_printf_semaphore, TX_WAIT_FOREVER);

    tx_mutex_put(&demo_printf_mutex);

	return;
}

void demo_flash_printf_init(void)
{
	sci_cfg_t config;
	sci_err_t err;

	/* Initialize SCI to communicate with PC terminal */
	config.async.baud_rate = 115200;
	config.async.clk_src = SCI_CLK_INT;
	config.async.data_size = SCI_DATA_8BIT;
	config.async.parity_en = SCI_PARITY_OFF;
	config.async.parity_type = SCI_EVEN_PARITY;
	config.async.stop_bits = SCI_STOPBITS_1;
	config.async.int_priority = 15;
	err = R_SCI_Open(SCI_CH5, SCI_MODE_ASYNC, &config, demo_flash_printf_interrupt, &Console);

	R_SCI_PinSet_SCI5();
}

static uint8_t demo_flash_prinf_transend;
static void demo_flash_printf_interrupt(void * pArgs)
{
	sci_cb_args_t *p_args = (sci_cb_args_t*) pArgs;

	if (SCI_EVT_TEI == p_args->event)
	{
		demo_flash_prinf_transend = 1;
	}

}

sci_err_t demo_flash_printf_SCI_Send(uint8_t *p_src, uint16_t const length)
{
	sci_err_t sci_err_send;

	demo_flash_prinf_transend = 0;
	sci_err_send = R_SCI_Send(Console,p_src,length);

	while(demo_flash_prinf_transend != 1)
	{
		R_BSP_NOP();
	}

	return sci_err_send;
}

sci_err_t demo_flash_printf_SCI_Receive(uint8_t *p_src, uint16_t const length)
{
	return R_SCI_Receive(Console, p_src,length);
}

void demo_flash_printf_close(void)
{
	R_SCI_Close(Console);

	return;
}

void uart_string_printf(char *pString)
{
    uint16_t str_length = 0;
    uint16_t transmit_length = 0;
    sci_err_t sci_err;
    uint32_t retry = 0xFFFF;

    str_length = (uint16_t)strlen(pString);

    while ((retry > 0) && (str_length > 0))
    {

        R_SCI_Control(Console, SCI_CMD_TX_Q_BYTES_FREE, &transmit_length);

        if(transmit_length > str_length)
        {
            transmit_length = str_length;
        }

        sci_err = R_SCI_Send(Console, (uint8_t *) pString,
                             transmit_length);
        R_BSP_SoftwareDelay(1,BSP_DELAY_MILLISECS);

        if ((sci_err == SCI_ERR_XCVR_BUSY) || (sci_err == SCI_ERR_INSUFFICIENT_SPACE))
        {
            retry--; // retry if previous transmission still in progress or tx buffer is insufficient.
            continue;
        }

        str_length -= transmit_length;
        pString += transmit_length;

    }

    if (SCI_SUCCESS != sci_err)
    {
        R_BSP_NOP(); //TODO error handling code
    }

}
