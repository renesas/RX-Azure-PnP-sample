
#include "r_smc_entry.h"

#include <platform.h>
#include <stdio.h>              // For sprintf
#include <stdlib.h>
#include <string.h>

#include "platform.h"           // Located in the FIT BSP module
#include "r_cmt_rx_if.h"
#include "r_gpio_rx_if.h"
#include "r_byteq_if.h"         // The BYTEQ module API interface file.

#include "r_flash_rx_if.h"
#include "r_simple_filesystem_on_dataflash_if.h"
#include "sample_config.h"
#include "r_sci_rx_if.h"
#include "r_sci_rx_pinset.h"
#include "r_cmt_rx_if.h"
#include "r_ether_rx_if.h"
#include "r_ether_rx_pinset.h"

#include <tx_api.h>

#include <demo_printf.h>
#include "RX_HS3001.h"
#include "RX_ZMOD4XXX_Common.h"

void _tx_timer_interrupt(void);
static void parameter_registration(void);
static void display_serial_terminal_putstring_with_uart( char *string);
static int32_t get_command_code(uint8_t *command);
static uint8_t copy_param(void);

//uint8_t *temp;

const char id_scope_flash[] = DATAFLASH_LABEL_NAME_ID_SCOPE;
const char device_id_flash[] = DATAFLASH_LABEL_NAME_DEVICE_ID;
const char primary_key_flash[] = DATAFLASH_LABEL_NAME_PRIMARY_KEY;

char id_scope[256];
char device_id[256];
char primary_key[256];

/* Handle storage. Needs to persist as long as SCI calls are going to be made.*/
static char message_buffer[MESSAGE_BUFFER_SIZE];

/* CMT Timer callback used as the system tick. */
void timer_callback(void * pdata)
{
    _tx_timer_interrupt();
}


void platform_setup(void)
{
    uint32_t chan;
    ether_param_t eth_param = {0};
    uint32_t bank_info;
    uint8_t ret;

    /* flash initialization */
    R_FLASH_Open();
    R_FLASH_Control(FLASH_CMD_BANK_GET, &bank_info);
    demo_printf("Bank_info: %d\r\n", bank_info);
    R_SFD_Open();

  //  g_comms_i2c_bus0_quick_setup();

	/* User Switch on CK RX65N*/
	R_GPIO_PinDirectionSet(GPIO_PORT_D_PIN_1, GPIO_DIRECTION_INPUT);

#if ENABLE_SERIAL_PARAM
	demo_flash_printf_init();
	if(R_GPIO_PinRead(GPIO_PORT_D_PIN_1) == GPIO_LEVEL_HIGH)
	{
		if(copy_param() != 0)
		{
			display_serial_terminal_putstring_with_uart(  "--- To set the Azure connecting information to hold Press SW2. ---\r\n");
			while(R_GPIO_PinRead(GPIO_PORT_D_PIN_1) == GPIO_LEVEL_HIGH)
			{
				nop();
			}
			parameter_registration();
		}
	}
	else
	{
		parameter_registration();
	}
	demo_flash_printf_close();
#endif

    /* Create periodic timer for the system tick. */
    R_CMT_CreatePeriodic(100u, timer_callback, &chan);
    
    /* Setup Ethernet hardware. */
    R_ETHER_Initial();
    R_ETHER_PinSet_ETHERC0_RMII();
    eth_param.channel = 0u;
    R_ETHER_Control(CONTROL_POWER_ON, eth_param);

    /* Initialize the Option Board sensors */
    /* TODO : ADD Sensor Initialize*/


    /* Initialize LED2, LED3, LED4, LED6 on CK RX65N*/
    R_GPIO_PinDirectionSet(GPIO_PORT_1_PIN_7, GPIO_DIRECTION_OUTPUT); //LED2_R
    R_GPIO_PinDirectionSet(GPIO_PORT_A_PIN_5, GPIO_DIRECTION_OUTPUT); //LED2_G
    R_GPIO_PinDirectionSet(GPIO_PORT_A_PIN_7, GPIO_DIRECTION_OUTPUT); //LED2_B

    R_GPIO_PinDirectionSet(GPIO_PORT_2_PIN_2, GPIO_DIRECTION_OUTPUT); //LED3
    R_GPIO_PinDirectionSet(GPIO_PORT_A_PIN_3, GPIO_DIRECTION_OUTPUT); //LED4
    R_GPIO_PinDirectionSet(GPIO_PORT_2_PIN_5, GPIO_DIRECTION_OUTPUT); //LED6

    R_GPIO_PinWrite(GPIO_PORT_1_PIN_7, GPIO_LEVEL_HIGH); //LED2_R
    R_GPIO_PinWrite(GPIO_PORT_A_PIN_5, GPIO_LEVEL_HIGH); //LED2_G
    R_GPIO_PinWrite(GPIO_PORT_A_PIN_7, GPIO_LEVEL_HIGH); //LED2_B

    R_GPIO_PinWrite(GPIO_PORT_2_PIN_2, GPIO_LEVEL_HIGH); //LED3
    R_GPIO_PinWrite(GPIO_PORT_A_PIN_3, GPIO_LEVEL_HIGH); //LED4
    R_GPIO_PinWrite(GPIO_PORT_2_PIN_5, GPIO_LEVEL_HIGH); //LED6

	//g_hs300x_sensor0_quick_setup();
//	g_zmod4xxx_sensor0_quick_setup();
//	g_zmod4xxx_sensor1_quick_setup();

	/* Set the warm start to Cold/Warm Start Determination Flag */
    SYSTEM.RSTSR1.BIT.CWSF = 1;
}

