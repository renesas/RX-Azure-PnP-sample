/*
 * console.c
 *
 *  Created on: 15th November 2021
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "stdbool.h"
//#include "FreeRTOS.h"
//#include "semphr.h"
#include "r_smc_entry.h"
#include "r_typedefs.h"
#include "sensorsData.h"
//#include "hal_data.h"
//#include "console_thread.h"
#include "console.h"
#include "r_smc_cgc.h"
#include "../sensorsData.h"
#define MINIMUM_TOKEN_LENGTH      ( 5)   // minimum length of a colour token
#define MAXIMUM_TOKEN_LENGTH      (10)   // maximum length of a colour token
#define PRINT_BUFFER              (8*1024)
#define TRANSFER_LENGTH           (4096)
//static QueueHandle_t xQueue;
// #define USE_DEBUG_INFO_CONSOLE    (1) // Enable to see task info on print_colour() calls

static uint8_t  g_out_of_band_received[TRANSFER_LENGTH];
static uint32_t g_transfer_complete = 0;
static uint32_t g_receive_complete  = 0;
static uint32_t g_out_of_band_index = 0;
bool_t transmit_complete;
bool receive_complete;
char g_write_buffer[2048];//= {0};
uint8_t g_start_read_flag = 0;
uint16_t g_counter = 0;


/*****************************************************************************
 * Function Name: get_colour
 *                Get the escape code string for the supplied colour tag
 * @param[in] char *string : the escape code
 * @param[out] bool_t *found : true if the tag was found, false if not
 * @retval the escape code for the colour tag, or the original string if there
 *         was no match
 ******************************************************************************/
static const char *get_colour(const char *string, bool_t *found)
{
    const char *colour_codes[] = {"[BLACK]", "\x1B[30m", "[RED]", "\x1B[91m", "[GREEN]", "\x1B[92m", "[YELLOW]", "\x1B[93m",
                                  "[BLUE]", "\x1B[94m", "[MAGENTA]", "\x1B[95m", "[CYAN]", "\x1B[96m", "[WHITE]", "\x1B[97m",
                                  "[ORANGE]", "\x1B[38;5;208m", "[PINK]", "\x1B[38;5;212m", "[BROWN]", "\x1B[38;5;94m",
                                  "[PURPLE]", "\x1B[35m"};
	uint8_t i;

	for (i = 0; i < 12; i++)
	{
		if (0 == strcmp(string, colour_codes[i << 1]))
		{
			*found = true;
			return colour_codes[(i << 1) + 1];
		}
	}

	*found = false;
	return (string);
}
/******************************************************************************
 * End of function get_colour
 ******************************************************************************/

/* ************************************************************************** */
/* Function Name: detokenise                                                  */
/*                Replace colour tokens with terminal colour escape codes     */
/* @param[in] input : input string possibly containing colour tokens          */
/* @param[out] output : string with colour tokens replaced with escape codes  */
/* @retval none                                                               */
/* ************************************************************************** */
static void detokenise(const char * input, char *output)
{
    int16_t start_bracket_index;
    int16_t end_bracket_index;
    int16_t start_bracket_output_index;
    size_t token_length;
    int16_t i;
    int16_t o;
    bool_t token_found;
    bool_t token_replaced;
    const char_t *colour_code;
    char_t token[MAXIMUM_TOKEN_LENGTH + 1];

    start_bracket_index = -1;
    end_bracket_index = -1;
    start_bracket_output_index = 0;
    o = 0;

    /* scan the input string */
    for (i = 0; '\0' != input[i]; i++)
    {
        token_replaced = false;

        /* token start? */
        if ('[' == input[i])
        {
            start_bracket_index = i;
            start_bracket_output_index = o;
        }

        /* token end? */
        if (']' == input[i])
        {
            end_bracket_index = i;

            /* check to see if we have a token */
            if (start_bracket_index >= 0)
            {
                token_length = (size_t)(end_bracket_index - start_bracket_index + 1);

                if ((token_length >= MINIMUM_TOKEN_LENGTH) && (token_length <= MAXIMUM_TOKEN_LENGTH))
                {
                    /* copy the token to a buffer */
                    strncpy(token, &input[start_bracket_index], token_length);
                    token[token_length] = '\0';

                    /* check for a valid token */
                    colour_code = get_colour(token, &token_found);

                    /* if we have a colour token, then replace it in the output with the associated escape code */
                    if (token_found)
                    {
                        strcpy(&output[start_bracket_output_index], colour_code);
                        o = (int16_t)(start_bracket_output_index + (int16_t)strlen(colour_code));
                        token_replaced = true;
                    }
                }
            }

            /* reset and start looking for another token */
            start_bracket_index = -1;
            end_bracket_index = -1;
        }

        /* if we didn't replace a token, then carry on copying input to output */
        if (!token_replaced)
        {
            output[o] = input[i];
            o++;
        }
    }

    /* terminate the output string */
    output[o] = '\0';
}
/******************************************************************************
 * End of function detokenise
 ******************************************************************************/

/* console_write */
void console_write(const char *buffer)
{
	uart_string_printf(&buffer[0]);
}

void console_transmit_end(void)
{
	transmit_complete = true;
}

void console_receive_end(void)
{
	receive_complete = true;
}
static uint8_t s_rx_buf = 0;

void start_key_check(void)
{
//    s_rx_buf = 0;
//    g_receive_complete = false;
//
//    //R_SCI_UART_Read(&g_console_uart_ctrl, &s_rx_buf, 1);
//    R_Config_SCI5_Serial_Receive(&s_rx_buf, 1);
//    //printf("\r\nRx data: %c",s_rx_buf);
}

int8_t get_detected_key(void)
{
    return ((int8_t)s_rx_buf);
}

bool_t key_pressed(void)
{
    return (g_receive_complete);
}
//uint16_t uiInWritebufferIndex;
char wait_for_keypress(void)
{
	uint8_t rx_buf;
	uiInWritebufferIndex=0;
	receive_complete = false;
	while (!receive_complete)
	{
		R_BSP_NOP();
	}
	rx_buf=g_write_buffer[0];
	memset(g_write_buffer, 0, 2048);
	uiInWritebufferIndex =0;
	receive_complete = false;

	if(rx_buf == 0xd) // '/n' then read 'r'
	{
		//uiInWritebufferIndex=0;
		while (!receive_complete)
		{
		   R_BSP_NOP();
		}
	    rx_buf=g_write_buffer[0];
	    memset(g_write_buffer, 0, 2048);
	    uiInWritebufferIndex = 0;
	}
	return (rx_buf);
}
/******************************************************************************
 * End of function wait_for_keypress
 ******************************************************************************/

void clear_console(void)
{
	console_write("\x1b[2J\x1b[H");
}
/******************************************************************************
 * End of function clear_console
 ******************************************************************************/

#ifdef USE_DEBUG_CONSOLE
static void printf_colour_debug(const char *format, ...)
{
    va_list arglist;
    static char colour_format[PRINT_BUFFER];
    static char final_buffer[PRINT_BUFFER];

    va_start(arglist, format);

    /* replace colour tokens with terminal colour escape codes */
    detokenise(format, colour_format);

    vsprintf(final_buffer, colour_format, arglist);
    va_end(arglist);

    console_write(final_buffer);
}
#endif // USE_DEBUG_CONSOLE

/*****************************************************************************
 * Function Name: printf_colour
 *                As printf, but replaces colour tags with escape codes
 * @param[in] char *format : the format string
 * @param[in] ... : argument list, 0 or more parameters
 * @retval None
 ******************************************************************************/
void printf_colour(const char *format, ...)
{
	va_list arglist;
    static char colour_format[PRINT_BUFFER];
    static char final_buffer[PRINT_BUFFER];

    va_start(arglist, format);

    /* replace colour tokens with terminal colour escape codes */
    detokenise(format, colour_format);

    vsprintf(final_buffer, colour_format, arglist);
    va_end(arglist);

    console_write(final_buffer);
}
/******************************************************************************
 * End of function printf_colour
 ******************************************************************************/
