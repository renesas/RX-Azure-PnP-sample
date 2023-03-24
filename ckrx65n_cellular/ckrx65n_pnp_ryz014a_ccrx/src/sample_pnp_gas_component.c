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
#include "RX_ZMOD4XXX_Common.h"
#include "sample_pnp.h"
#include <demo_printf.h>
#define SAMPLE_PNP_GAS_COMPONENT_NAME									"gas"
#define DOUBLE_DECIMAL_PLACE_DIGITS                                     (2)
#define THREE_DECIMAL_PLACE_DIGITS                                      (3)

/* TODO : Add property name */
static const CHAR sample_pnp_iaq_tvoc_property_name[] = "iaq_tvoh";
static const CHAR sample_pnp_iaq_etoh_property_name[] = "iaq_etoh";
static const CHAR sample_pnp_iaq_eco2_property_name[] = "iaq_eco2";
static const CHAR sample_pnp_gas_oaq_name_property_name[] = "iaq";

static UCHAR telemetry_buffer[256];

UINT sample_gas_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr)
{
UINT status;
NX_PACKET *packet_ptr;
UINT data_length;
NX_AZURE_IOT_JSON_WRITER json_writer;
st_sensorsZmod4510_t zmod4510_Data;
st_sensorsZmod4410_t zmod4410_Data;

    /* Create a telemetry message packet.  */
    if ((status = nx_azure_iot_hub_client_telemetry_message_create(pnp_client_ptr, &packet_ptr, NX_WAIT_FOREVER)))
    {
        return(status);
    }

    if ((status = nx_azure_iot_hub_client_telemetry_component_set(packet_ptr, SAMPLE_PNP_GAS_COMPONENT_NAME, sizeof(SAMPLE_PNP_GAS_COMPONENT_NAME) - 1, NX_WAIT_FOREVER)))
    {
        return(status);
    }


    /* Build telemetry JSON payload.  */
    if (nx_azure_iot_json_writer_with_buffer_init(&json_writer, telemetry_buffer, sizeof(telemetry_buffer)))
    {
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return(NX_NOT_SUCCESSFUL);
    }

        getZMOD4410Data(&zmod4410_Data);
        getZMOD4510Data(&zmod4510_Data);

    /* Append ambient light property name and value.  */
    /* TODO : Replace real data from temp.*/
    if (nx_azure_iot_json_writer_append_begin_object(&json_writer) ||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                   (UCHAR *)sample_pnp_iaq_tvoc_property_name,
                                                                   sizeof(sample_pnp_iaq_tvoc_property_name) - 1,
																   zmod4410_Data.zmod4410Data.tvoc,
																   THREE_DECIMAL_PLACE_DIGITS) ||
		nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																   (UCHAR *)sample_pnp_iaq_etoh_property_name,
																   sizeof(sample_pnp_iaq_etoh_property_name) - 1,
																   zmod4410_Data.zmod4410Data.etoh,
																   THREE_DECIMAL_PLACE_DIGITS) ||
		nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	(UCHAR*) sample_pnp_iaq_eco2_property_name,
																	sizeof(sample_pnp_iaq_eco2_property_name) - 1,
																	zmod4410_Data.zmod4410Data.ec02,
																	THREE_DECIMAL_PLACE_DIGITS)||
        nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
                                                                      (UCHAR *)sample_pnp_gas_oaq_name_property_name,
                                                                      sizeof(sample_pnp_gas_oaq_name_property_name) - 1,
                                                                      zmod4510_Data.zmod410_oaq,
                                                                      THREE_DECIMAL_PLACE_DIGITS) ||
        nx_azure_iot_json_writer_append_end_object(&json_writer))
    {
    	demo_printf("Telemetry message failed to build message\r\n");
        nx_azure_iot_json_writer_deinit(&json_writer);
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return(NX_NOT_SUCCESSFUL);
    }

    data_length = nx_azure_iot_json_writer_get_bytes_used(&json_writer);
    if ((status = nx_azure_iot_hub_client_telemetry_send(pnp_client_ptr, packet_ptr,
                                                         (UCHAR *)telemetry_buffer, data_length, NX_WAIT_FOREVER)))
    {
    	demo_printf("Telemetry message send failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_json_writer_deinit(&json_writer);
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return(status);
    }

    nx_azure_iot_json_writer_deinit(&json_writer);
    nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
    /* TODO : Replace real data from temp.*/
#if ENABLE_SENSORS_MQTT_PRINTS
    demo_printf("\r\nTelemetry message sent:  OAQ = %f,IAQ-TVOC = %f,IAQ-ETOH = %f, IAQ-ECO2 = %f\r\n", zmod4510_Data.zmod410_oaq,zmod4410_Data.zmod4410Data.tvoc, zmod4410_Data.zmod4410Data.etoh,zmod4410_Data.zmod4410Data.ec02);
#endif
    return(NX_AZURE_IOT_SUCCESS);
}
