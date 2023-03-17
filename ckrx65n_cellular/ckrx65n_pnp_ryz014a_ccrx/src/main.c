/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


#include "azurertos_object_init.h"
#include "sample_config.h"
#include "demo_printf.h"

void platform_setup(void);
void tx_application_define_user (void);
TX_THREAD        cli_thread;
UCHAR               cli_thread_memory_stack[2048];
void cli_thread_entry(void);
/* Define main entry point.  */
int main(void)
{   
    /* Setup the hardware. */
    platform_setup();
    demo_printf_init();
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
   // tx_application_define_user();
}

/* Define what the initial system looks like.  */
void    tx_application_define(void *first_unused_memory)
{

	//tx_application_define_user();
	UINT  status;

#if CUI_SETTING
	/* Initialize the demo printf implementation. */
	demo_printf_init();

	/* Initialize the NetX system.  */
	status = tx_thread_create(&cli_thread, "CLI Thread",
	                                  cli_thread_entry, 0,
									  cli_thread_memory_stack, 2048,
	    							  4, 4,
	    							  0, TX_AUTO_START);

	/* Check status.  */
	if (status)
	{
	     demo_printf("Demo helper thread creation fail: %u\r\n", status);
	     return;
	}
#else
	        start_app_mqtt();

#endif
}
