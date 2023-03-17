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

#include <string.h>
//#include "nx_azure_iot_pnp_client.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_json_writer.h"

#include "sample_pnp.h"
#include "platform.h"
//#include "r_cmt_rx_if.h"


/* Command response status. */
#define SAMPLE_COMMAND_SUCCESS_STATUS                                   (200)
#define SAMPLE_COMMAND_NOT_FOUND_STATUS                                 (404)
#define SAMPLE_COMMAND_ERROR_STATUS                                     (500)

/* Reported property keys and values.  */
static const CHAR sample_pnp_default_manufacture_property_name[] = "manufacturer";
static const CHAR sample_pnp_default_manufacture_property_value[] = "Renesas_Electronics_Corporation"; 
static const CHAR sample_pnp_default_model_property_name[] = "model";
static const CHAR sample_pnp_default_model_property_value[] = "ETK5CKN0C00000BE";
static const CHAR sample_pnp_default_processor_property_name[] = "processorName";
static const CHAR sample_pnp_default_processor_property_value[] = "RX65N"; 
static const CHAR sample_pnp_default_sample_version_name[] = "version";
static const CHAR sample_pnp_default_sample_version_value[] = "Rev.1.00_CCRX";
static const CHAR sample_pnp_default_ipaddress_name[] = "IPAddress";
static const CHAR sample_pnp_default_macaddress_name[] = "MACAddress";

/* Command.  */
static const CHAR sample_pnp_default_reboot_command_name[] = "reboot";
static CHAR command_parameter_buffer[256];
static int32_t reboot_delay;
static ULONG reboot_action_tick = 0;

// TODO : TEMP
//extern uint8_t ipaddress_display[16];
//extern uint8_t mac_address_with_bar[18];
uint8_t ipaddress_display[16];
uint8_t mac_address_with_bar[18];

UINT sample_pnp_default_reported_properties_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr)
{
UINT status;
UINT response_status = 0;
NX_AZURE_IOT_JSON_WRITER json_writer;
NX_PACKET *packet_ptr;

    if ((status = nx_azure_iot_hub_client_reported_properties_create(pnp_client_ptr, &packet_ptr, NX_WAIT_FOREVER)))
    { 
        return(status);
    }

    if ((status = nx_azure_iot_json_writer_init(&json_writer, packet_ptr, NX_WAIT_FOREVER)))
    {
        LOG_TERMINAL("Failed init json writer: error code = 0x%08x\r\n", status);
        nx_packet_release(packet_ptr);
        return(status);
    }

    if ((status = nx_azure_iot_json_writer_append_begin_object(&json_writer)) ||
            nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
                                                                   (UCHAR *)sample_pnp_default_manufacture_property_name,
                                                                   sizeof(sample_pnp_default_manufacture_property_name) - 1,
                                                                   (UCHAR *)sample_pnp_default_manufacture_property_value,
                                                                   sizeof(sample_pnp_default_manufacture_property_value) - 1) ||
        nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
                                                                   (UCHAR *)sample_pnp_default_model_property_name,
                                                                   sizeof(sample_pnp_default_model_property_name) - 1,
                                                                   (UCHAR *)sample_pnp_default_model_property_value,
                                                                   sizeof(sample_pnp_default_model_property_value) - 1) ||
        nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
                                                                   (UCHAR *)sample_pnp_default_processor_property_name,
                                                                   sizeof(sample_pnp_default_processor_property_name) - 1,
                                                                   (UCHAR *)sample_pnp_default_processor_property_value,
                                                                   sizeof(sample_pnp_default_processor_property_value) - 1) ||
	   nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
																  (UCHAR *)sample_pnp_default_sample_version_name,
																  sizeof(sample_pnp_default_sample_version_name) - 1,
																  (UCHAR *)sample_pnp_default_sample_version_value,
																  sizeof(sample_pnp_default_sample_version_value) - 1) ||
	   nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
																  (UCHAR *)sample_pnp_default_ipaddress_name,
																  sizeof(sample_pnp_default_ipaddress_name) - 1,
																  (UCHAR *)ipaddress_display,
																  strlen(ipaddress_display)) ||
	   nx_azure_iot_json_writer_append_property_with_string_value(&json_writer,
																  (UCHAR *)sample_pnp_default_macaddress_name,
																  sizeof(sample_pnp_default_macaddress_name) - 1,
																  (UCHAR *)mac_address_with_bar,
																  strlen(mac_address_with_bar))||
		(status = nx_azure_iot_json_writer_append_end_object(&json_writer)))
                                                                  {
        status = NX_NOT_SUCCESSFUL;
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }

    if ((status = nx_azure_iot_hub_client_reported_properties_send(pnp_client_ptr,
                                                                   packet_ptr,
                                                                   NX_NULL, &response_status,
                                                                   NX_NULL,
                                                                   (5 * NX_IP_PERIODIC_RATE))))
    {
        nx_azure_iot_json_writer_deinit(&json_writer);
        return(status);
    }

    nx_azure_iot_json_writer_deinit(&json_writer);

    if ((response_status < 200) || (response_status >= 300))
    {
        return(NX_NOT_SUCCESSFUL);
    }

    return(NX_AZURE_IOT_SUCCESS);
}

UINT sample_pnp_default_command_process(const UCHAR *pnp_command_name_ptr, UINT pnp_command_name_length,
                                        NX_AZURE_IOT_JSON_READER *json_reader_ptr,
                                        NX_AZURE_IOT_JSON_WRITER *json_response_ptr, UINT *status_code)
{
UINT dm_status;
UINT bytes_copied;
uint32_t ch;
bool ret;
INT delay;

    if (pnp_command_name_length != (sizeof(sample_pnp_default_reboot_command_name) - 1) ||
        strncmp((CHAR *)pnp_command_name_ptr, (CHAR *)sample_pnp_default_reboot_command_name, pnp_command_name_length) != 0)
    {
        return(NX_AZURE_IOT_FAILURE);
    }
    else
    {

        /* Yes. it is reboot command. */
        memset(command_parameter_buffer, 0, sizeof(command_parameter_buffer));
        bytes_copied = 0;

        /* Try to parse the command parameter.  */
        if ((nx_azure_iot_json_reader_next_token(json_reader_ptr)) == NX_AZURE_IOT_SUCCESS)
        {
			nx_azure_iot_json_reader_token_string_get(json_reader_ptr, (UCHAR *)command_parameter_buffer, sizeof(command_parameter_buffer), &bytes_copied);
        }

        /* Process reboot command.  */
        reboot_delay = atoi(command_parameter_buffer);

        /* MSFT: Record the current time, and check the delay in sample_pnp_default_reboot_check().  */
        reboot_action_tick = tx_time_get();

        /* Build the command response payload.  */
        if (nx_azure_iot_json_writer_append_string(json_response_ptr, "commandResponse", sizeof("commandResponse") - 1))
        {
            dm_status = SAMPLE_COMMAND_ERROR_STATUS;
        }
        else
        {
            dm_status = SAMPLE_COMMAND_SUCCESS_STATUS;
        }

        dm_status = SAMPLE_COMMAND_SUCCESS_STATUS;

    }

    *status_code = dm_status;

    return(NX_AZURE_IOT_SUCCESS);
}


void sample_pnp_default_reboot_check(void)
{

	/* Check if set the reboot flag.  */
	if (reboot_action_tick)
	{

		/* Check if reboot it immediately.  */
	    if ((tx_time_get() - reboot_action_tick) >= (reboot_delay * NX_IP_PERIODIC_RATE))
	    {

	    	/* Reboot.  */
	        printf("\r\n\nRebooting...\r\n");

	    	reboot_action_tick = 0;
	        R_BSP_SoftwareReset();
            while(1)
            ;
	    }
	}
}

UINT sample_pnp_default_reboot_check_return(void)
{
    	/* Check if set the reboot flag.  */
	if (reboot_action_tick)
	{
        return 0;
    }
    else
    {
        return 1;
    }
}


