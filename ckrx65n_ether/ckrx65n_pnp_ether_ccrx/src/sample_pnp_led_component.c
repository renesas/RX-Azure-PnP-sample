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

#include "nx_api.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_hub_client_properties.h"

#include "r_gpio_rx_if.h"

/* LED component name.  */
#define SAMPLE_PNP_LED_COMPONENT_NAME                               "LED_ckrx65n"

/* LED status value. */
#define SAMPLE_LED_ON_STATUS                                        (1)
#define SAMPLE_LED_OFF_STATUS                                       (2)

#define SAMPLE_COMMAND_SUCCESS_STATUS                               (200)
#define SAMPLE_COMMAND_NOT_FOUND_STATUS                             (404)
#define SAMPLE_COMMAND_ERROR_STATUS                                 (500)

/* Desired property names.  */
static const CHAR sample_pnp_led_led1_property_name[] = "led1";
static const CHAR sample_pnp_led_led2_property_name[] = "led2";
static const CHAR sample_pnp_led_led3_property_name[] = "led3";
static const CHAR sample_pnp_led_led4_property_name[] = "led4";

static int32_t led1_status;
static int32_t led2_status;
static int32_t led3_status;
static int32_t led4_status;


UINT sample_pnp_led_desired_properties_process(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr, NX_AZURE_IOT_JSON_READER *json_reader, ULONG version);
UINT sample_pnp_led_reported_properties_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr,
                                             UCHAR *property_name_ptr, UINT property_name_length,
                                             UINT property_value, UINT status_code, 
                                             ULONG version, const CHAR *description);

UINT sample_pnp_led_desired_properties_process(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr, NX_AZURE_IOT_JSON_READER *json_reader, ULONG version)
{
UINT status;
UINT response_status = 0;
NX_AZURE_IOT_JSON_WRITER json_writer;
int32_t led1_value;
int32_t led2_value;
int32_t led3_value;
int32_t led4_value;
    demo_printf("RX_message In--sample_pnp_led_desired_properties_process");
    while (1)
    {

        /* led1 property.  */
        if (nx_azure_iot_json_reader_token_is_text_equal(json_reader,
                                                            (UCHAR *)sample_pnp_led_led1_property_name,
                                                            sizeof(sample_pnp_led_led1_property_name) - 1))
        {
            if ((status = nx_azure_iot_json_reader_next_token(json_reader)) ||
                (status = nx_azure_iot_json_reader_token_int32_get(json_reader, &led1_value)))
            {
                return(status);
            }

            nx_azure_iot_json_reader_next_token(json_reader);

            /* Update the state.  */
            led1_status = led1_value;
            
            /* Update LED1 property with led1_value.  */
            switch (led1_status)
            {
            case 1:
                R_GPIO_PinWrite(GPIO_PORT_2_PIN_2, GPIO_LEVEL_LOW);
                break;
            case 2:
                R_GPIO_PinWrite(GPIO_PORT_2_PIN_2, GPIO_LEVEL_HIGH);
                break;
            default:
                break;
            }

            /* Response.  */
            sample_pnp_led_reported_properties_send(pnp_client_ptr,
                                                    (UCHAR *)sample_pnp_led_led1_property_name,
                                                    sizeof(sample_pnp_led_led1_property_name) - 1, 
                                                    led1_status, SAMPLE_COMMAND_SUCCESS_STATUS, version, "");

        }

        /* led2 property.  */
        else if (nx_azure_iot_json_reader_token_is_text_equal(json_reader,
                                                                (UCHAR *)sample_pnp_led_led2_property_name,
                                                                sizeof(sample_pnp_led_led2_property_name) - 1))
        {
            if ((status = nx_azure_iot_json_reader_next_token(json_reader)) ||
                (status = nx_azure_iot_json_reader_token_int32_get(json_reader, &led2_value)))
            {
                return(status);
            }

            nx_azure_iot_json_reader_next_token(json_reader);

            /* Update the state.  */
            led2_status = led2_value;

            /* Update LED2 property with led2_value.  */
            switch (led2_status)
            {
            case 1:
                R_GPIO_PinWrite(GPIO_PORT_2_PIN_5, GPIO_LEVEL_LOW);
                break;
            case 2:
                R_GPIO_PinWrite(GPIO_PORT_2_PIN_5, GPIO_LEVEL_HIGH);
                break;
            default:
                break;
            }

            /* Response.  */
            sample_pnp_led_reported_properties_send(pnp_client_ptr,
                                                    (UCHAR *)sample_pnp_led_led2_property_name,
                                                    sizeof(sample_pnp_led_led2_property_name) - 1, 
                                                    led2_status, SAMPLE_COMMAND_SUCCESS_STATUS, version, "");
        }
        /* led3 property.  */
		else if (nx_azure_iot_json_reader_token_is_text_equal(json_reader,
																(UCHAR *)sample_pnp_led_led3_property_name,
																sizeof(sample_pnp_led_led3_property_name) - 1))
		{
			if ((status = nx_azure_iot_json_reader_next_token(json_reader)) ||
				(status = nx_azure_iot_json_reader_token_int32_get(json_reader, &led3_value)))
			{
				return(status);
			}

			nx_azure_iot_json_reader_next_token(json_reader);

			/* Update the state.  */
			led3_status = led3_value;

			/* Update LED3 property with led3_value.  */
			switch (led3_status)
			{
			case 1:
				R_GPIO_PinWrite(GPIO_PORT_A_PIN_3, GPIO_LEVEL_LOW);
				break;
			case 2:
				R_GPIO_PinWrite(GPIO_PORT_A_PIN_3, GPIO_LEVEL_HIGH);
				break;
			default:
				break;
			}

			/* Response.  */
			sample_pnp_led_reported_properties_send(pnp_client_ptr,
													(UCHAR *)sample_pnp_led_led3_property_name,
													sizeof(sample_pnp_led_led3_property_name) - 1,
													led3_status, SAMPLE_COMMAND_SUCCESS_STATUS, version, "");
		}
        /* led4 property.  */
		else if (nx_azure_iot_json_reader_token_is_text_equal(json_reader,
																(UCHAR *)sample_pnp_led_led4_property_name,
																sizeof(sample_pnp_led_led4_property_name) - 1))
		{
			if ((status = nx_azure_iot_json_reader_next_token(json_reader)) ||
				(status = nx_azure_iot_json_reader_token_int32_get(json_reader, &led4_value)))
			{
				return(status);
			}

			nx_azure_iot_json_reader_next_token(json_reader);

			/* Update the state.  */
			led4_status = led4_value;

			/* Update LED4 property with led4_value.  */
			switch (led4_status)
			{
			case 1:
			    R_GPIO_PinWrite(GPIO_PORT_1_PIN_7, GPIO_LEVEL_LOW); //LED2_R
			    R_GPIO_PinWrite(GPIO_PORT_A_PIN_5, GPIO_LEVEL_HIGH); //LED2_G
			    R_GPIO_PinWrite(GPIO_PORT_A_PIN_7, GPIO_LEVEL_HIGH); //LED2_B
				break;
			case 2:
			    R_GPIO_PinWrite(GPIO_PORT_1_PIN_7, GPIO_LEVEL_HIGH); //LED2_R
			    R_GPIO_PinWrite(GPIO_PORT_A_PIN_5, GPIO_LEVEL_LOW); //LED2_G
			    R_GPIO_PinWrite(GPIO_PORT_A_PIN_7, GPIO_LEVEL_HIGH); //LED2_B
				break;
			case 3:
			    R_GPIO_PinWrite(GPIO_PORT_1_PIN_7, GPIO_LEVEL_HIGH); //LED2_R
			    R_GPIO_PinWrite(GPIO_PORT_A_PIN_5, GPIO_LEVEL_HIGH); //LED2_G
			    R_GPIO_PinWrite(GPIO_PORT_A_PIN_7, GPIO_LEVEL_LOW); //LED2_B
				break;
			default: //Turn off
				R_GPIO_PinWrite(GPIO_PORT_1_PIN_7, GPIO_LEVEL_HIGH); //LED2_R
				R_GPIO_PinWrite(GPIO_PORT_A_PIN_5, GPIO_LEVEL_HIGH); //LED2_G
				R_GPIO_PinWrite(GPIO_PORT_A_PIN_7, GPIO_LEVEL_HIGH); //LED2_B
				break;
			}

			/* Response.  */
			sample_pnp_led_reported_properties_send(pnp_client_ptr,
													(UCHAR *)sample_pnp_led_led4_property_name,
													sizeof(sample_pnp_led_led4_property_name) - 1,
													led4_status, SAMPLE_COMMAND_SUCCESS_STATUS, version, "");
		}
        else
        {
        	/* The JSON reader must be advanced regardless of whether the property is of interest or not.  */
			nx_azure_iot_json_reader_next_token(json_reader);

			/* Skip children in case the property value is an object.  */
			if (nx_azure_iot_json_reader_token_type(json_reader) == NX_AZURE_IOT_READER_TOKEN_BEGIN_OBJECT)
			{
				nx_azure_iot_json_reader_skip_children(json_reader);
			}
			nx_azure_iot_json_reader_next_token(json_reader);
            return(NX_AZURE_IOT_NOT_SUPPORTED);
        }
    }

    return(NX_AZURE_IOT_SUCCESS);
}

UINT sample_pnp_led_reported_properties_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr,
                                             UCHAR *property_name_ptr, UINT property_name_length,
                                             UINT property_value, UINT status_code, 
                                             ULONG version, const CHAR *description)
{
UINT status;
UINT response_status = 0;
NX_AZURE_IOT_JSON_WRITER json_writer;
UINT request_id;
NX_PACKET *packet_ptr;

    if ((status = nx_azure_iot_hub_client_reported_properties_create(pnp_client_ptr,
                                                                     &packet_ptr, NX_WAIT_FOREVER)))
    { 
        return(status);
    }
    

    if ((status = nx_azure_iot_json_writer_init(&json_writer, packet_ptr, NX_WAIT_FOREVER)))
    {
        printf("Failed to build reported response\r\n");
        nx_packet_release(packet_ptr);
        return (status);
    }

    if (nx_azure_iot_json_writer_append_begin_object(&json_writer) ||
		nx_azure_iot_hub_client_reported_properties_component_begin(pnp_client_ptr,&json_writer,(const UCHAR *)SAMPLE_PNP_LED_COMPONENT_NAME,sizeof(SAMPLE_PNP_LED_COMPONENT_NAME) - 1) ||
        nx_azure_iot_hub_client_reported_properties_status_begin(pnp_client_ptr,
                                                                 &json_writer, property_name_ptr,
                                                                 property_name_length,
                                                                 status, version,
                                                                 (const UCHAR *)description, strlen(description)) ||
        nx_azure_iot_json_writer_append_int32(&json_writer, property_value) ||
		nx_azure_iot_hub_client_reported_properties_status_end(pnp_client_ptr, &json_writer) ||
		nx_azure_iot_hub_client_reported_properties_component_end(pnp_client_ptr, &json_writer)||
        nx_azure_iot_json_writer_append_end_object(&json_writer))
    {
        printf("Failed to build reported response\r\n");
        nx_packet_release(packet_ptr);
    }
    else
    {
        if ((status = nx_azure_iot_hub_client_reported_properties_send(pnp_client_ptr,
                                                             packet_ptr, &request_id,
                                                             &response_status, NX_NULL,
                                                             (5 * NX_IP_PERIODIC_RATE))))
        {
            printf("Failed to send reported response\r\n");
            nx_packet_release(packet_ptr);
        }
        nx_azure_iot_json_writer_deinit(&json_writer);
        if ((response_status < 200) || (response_status >= 300))
        {
            return(NX_NOT_SUCCESSFUL);
        }
    }

    return(status);
}
