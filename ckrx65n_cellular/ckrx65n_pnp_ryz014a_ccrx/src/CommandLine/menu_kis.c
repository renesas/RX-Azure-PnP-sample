/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : menu_kis.c
 * Description  : Kit Information Screen.
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>

//#include "FreeRTOS.h"
//#include "FreeRTOSconfig.h"
//#include "semphr.h"
//#include "queue.h"
//#include "task.h"

#include "common_init.h"
#include "common_utils.h"
#include "common_init.h"
#include "menu_kis.h"
#include "console.h"
#include "menu_flash.h"
#include "nx_api.h"
void application_thread_entry(ULONG entry_input);
#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define APP_VERSION_MAJOR    1
#define APP_VERSION_MINOR    0
#define APP_VERSION_BUILD    0
static TX_THREAD        application_thread;
static char_t s_print_buffer[BUFFER_LENGTH_SHORT];// = { };

test_fn get_version(void)
{
    int8_t c = -1;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_VERSION, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer, "   %d.%d.%d",APP_VERSION_MAJOR,APP_VERSION_MINOR,APP_VERSION_BUILD);
    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

     sprintf (s_print_buffer, MENU_RETURN_INFO);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);
    while (CONNECTION_ABORT_CRTL != c)
    {
        c = wait_for_keypress ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }

    return (0);
}

test_fn get_uuid (void)
{

	volatile uint32_t UUID_buf[4];
	uint8_t send_buff[64];

	UUID_buf[0] = FLASHCONST.UIDR0;
	UUID_buf[1] = FLASHCONST.UIDR1;
	UUID_buf[2] = FLASHCONST.UIDR2;
	UUID_buf[3] = FLASHCONST.UIDR3;

	//printf(send_buff);
	int8_t c = -1;
//	bsp_unique_id_t const * p_uid = R_BSP_UniqueIdGet();

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_UUID, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer, UUID_TAG);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

	memset (s_print_buffer, 0, BUFFER_LENGTH_SHORT);
	sprintf(s_print_buffer,"%08x-%08x-%08x-%08x\n",FLASHCONST.UIDR0,FLASHCONST.UIDR1,FLASHCONST.UIDR2,FLASHCONST.UIDR3);
	printf_colour(s_print_buffer);

	printf_colour(MENU_RETURN_INFO);

	    while (CONNECTION_ABORT_CRTL != c)
	    {
	        c = wait_for_keypress ();
	        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
	        {
	            break;
	        }
	    }
	return (0);
}

test_fn help (void)
{
	int8_t c = -1;
	//bsp_unique_id_t const * p_uid = R_BSP_UniqueIdGet();

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_HELP, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer," %s\r\n %s\r\n %s\r\n %s\r\n %s", HELP_TAG1, HELP_TAG2, HELP_TAG3,HELP_TAG4,HELP_TAG5);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

	printf_colour(MENU_RETURN_INFO);

	    while (CONNECTION_ABORT_CRTL != c)
	    {
	        c = wait_for_keypress ();
	        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
	        {
	            break;
	        }
	    }

	return 0;
}

test_fn start_app(void)
{
	UINT  status;
	g_comms_i2c_bus0_quick_setup();
	tx_application_define_user();
	status=tx_thread_terminate(&cli_thread);
    return (0);
}
UCHAR               application_thread_memory_stack[4096];
test_fn start_app_mqtt(void)
{
   UINT status;
   int8_t c = -1;
  // if(FSP_SUCCESS==check_credentials_stored())
   if(1)
   {

	g_comms_i2c_bus0_quick_setup();

	//telemetryTask();
	status = tx_thread_create(&application_thread, "Demo Thread",
		                              application_thread_entry, 0,
									  application_thread_memory_stack, 4096,
									  3, 1,
									  2, TX_AUTO_START);

	//g_comms_i2c_bus0_quick_setup();
	tx_thread_sleep(100);
	tx_application_define_user();

	tx_thread_terminate(&cli_thread);
	return (0);
   }
   else
   {
	   printf_colour(MENU_RETURN_INFO);

	   	    while (CONNECTION_ABORT_CRTL != c)
	   	    {
	   	        c = wait_for_keypress ();
	   	        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
	   	        {
	   	            break;
	   	        }
	   	    }
	   	return (0);
   }
	return (0);
}

/**********************************************************************************************************************
 End of function kis_display_menu
 *********************************************************************************************************************/

