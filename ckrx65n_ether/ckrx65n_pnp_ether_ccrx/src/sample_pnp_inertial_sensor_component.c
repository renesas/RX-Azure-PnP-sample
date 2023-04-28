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

#include <math.h>
#include "nx_api.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_hub_client_properties.h"
#include "RX_ZMOD4XXX_Common.h"
#include "ICM20948/icm20948.h"
#include "ICM20948/icm_i2c.h"
#include <demo_printf.h>
#define THREE_DECIMAL_PLACE_DIGITS                                     (6)

#define SAMPLE_PNP_INERTIAL_COMPONENT_NAME                       "Inertial"

/* TODO : Add property name */
static const CHAR sample_pnp_accelerometerx_property_name[] = "accelerometerX";
static const CHAR sample_pnp_accelerometery_property_name[] = "accelerometerY";
static const CHAR sample_pnp_accelerometerz_property_name[] = "accelerometerZ";
static const CHAR sample_pnp_gyroscopex_property_name[] = "gyroscopeX";
static const CHAR sample_pnp_gyroscopey_property_name[] = "gyroscopeY";
static const CHAR sample_pnp_gyroscopez_property_name[] = "gyroscopeZ";
static const CHAR sample_pnp_compassx_property_name[] = "magX";
static const CHAR sample_pnp_compassy_property_name[] = "magY";
static const CHAR sample_pnp_compassz_property_name[] = "magZ";

static CHAR telemetry_buffer[256];
//axises my_gyro;
//axises my_accel;

UINT sample_inertial_sensor_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);

UINT sample_inertial_sensor_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr)
{
UINT status;
NX_PACKET *packet_ptr;
UINT data_length;
NX_AZURE_IOT_JSON_WRITER json_writer;
st_icm20948_data_t icm_Data;
//INT temp = 20;

    /* Create a telemetry message packet.  */
    if ((status = nx_azure_iot_hub_client_telemetry_message_create(pnp_client_ptr, &packet_ptr, NX_WAIT_FOREVER)))
    {
        return (status);
    }

    if ((status = (nx_azure_iot_hub_client_telemetry_component_set(packet_ptr, SAMPLE_PNP_INERTIAL_COMPONENT_NAME, sizeof(SAMPLE_PNP_INERTIAL_COMPONENT_NAME) - 1, NX_WAIT_FOREVER))))
    {
        return(status);
    }

     /* Build telemetry JSON payload.  */
     if (nx_azure_iot_json_writer_with_buffer_init(&json_writer, telemetry_buffer, sizeof(telemetry_buffer)))
     {
         nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
         return(NX_NOT_SUCCESSFUL);
     }

     /* Get the inertial sensor data */
     getICMData(&icm_Data);
     /* TODO : Add function to get the itertial sensor data */
     /* Append ambient light property name and value.  */
     /* TODO : Replace real data from temp.*/
     if (nx_azure_iot_json_writer_append_begin_object(&json_writer) ||
    		nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	  (UCHAR *)sample_pnp_accelerometerx_property_name,
																	  sizeof(sample_pnp_accelerometerx_property_name) - 1,
																	  icm_Data.accel.xaxis,THREE_DECIMAL_PLACE_DIGITS) ||
		   nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	  (UCHAR *)sample_pnp_accelerometery_property_name,
																	  sizeof(sample_pnp_accelerometery_property_name) - 1,
																	  icm_Data.accel.yaxis,THREE_DECIMAL_PLACE_DIGITS) ||
		   nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	  (UCHAR *)sample_pnp_accelerometerz_property_name,
																	  sizeof(sample_pnp_accelerometerz_property_name) - 1,
																	  icm_Data.accel.zaxis,THREE_DECIMAL_PLACE_DIGITS) ||
		   nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	  (UCHAR *)sample_pnp_gyroscopex_property_name,
																	  sizeof(sample_pnp_gyroscopex_property_name) - 1,
																	  icm_Data.gyro.xaxis,THREE_DECIMAL_PLACE_DIGITS) ||
		   nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	  (UCHAR *)sample_pnp_gyroscopey_property_name,
																	  sizeof(sample_pnp_gyroscopey_property_name) - 1,
																	  icm_Data.gyro.yaxis,THREE_DECIMAL_PLACE_DIGITS) ||
		   nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	  (UCHAR *)sample_pnp_gyroscopez_property_name,
																	  sizeof(sample_pnp_gyroscopez_property_name) - 1,
																	  icm_Data.gyro.zaxis,THREE_DECIMAL_PLACE_DIGITS) ||
		   nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	  (UCHAR *)sample_pnp_compassx_property_name,
																	  sizeof(sample_pnp_compassx_property_name) - 1,
																	  icm_Data.mag.xaxis,THREE_DECIMAL_PLACE_DIGITS) ||
		   nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	  (UCHAR *)sample_pnp_compassy_property_name,
																	  sizeof(sample_pnp_compassy_property_name) - 1,
																	  icm_Data.mag.yaxis,THREE_DECIMAL_PLACE_DIGITS) ||
		   nx_azure_iot_json_writer_append_property_with_double_value(&json_writer,
																	  (UCHAR *)sample_pnp_compassz_property_name,
																	  sizeof(sample_pnp_compassz_property_name) - 1,
																	  icm_Data.mag.zaxis,THREE_DECIMAL_PLACE_DIGITS) ||
//		   nx_azure_iot_json_writer_append_property_with_int32_value(&json_writer,
//																	  (UCHAR *)sample_pnp_compassz_property_name,
//																	  sizeof(sample_pnp_compassz_property_name) - 1,
//																	  temp) ||
         nx_azure_iot_json_writer_append_end_object(&json_writer))
     {
         nx_azure_iot_json_writer_deinit(&json_writer);
         nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
         return(NX_NOT_SUCCESSFUL);
     }

     data_length = nx_azure_iot_json_writer_get_bytes_used(&json_writer);
     if ((status = nx_azure_iot_hub_client_telemetry_send(pnp_client_ptr, packet_ptr,
                                                          (UCHAR *)telemetry_buffer, data_length, NX_WAIT_FOREVER)))
     {
         nx_azure_iot_json_writer_deinit(&json_writer);
         nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
         return(status);
     }

     nx_azure_iot_json_writer_deinit(&json_writer);
     /* TODO : Replace real data from temp. */
#if ENABLE_SENSORS_MQTT_PRINTS
     demo_printf("\r\nTelemetry message sent: accel_x = %f, accel_y = %f, accel_z = %f, ", icm_Data.accel.xaxis, icm_Data.accel.yaxis, icm_Data.accel.zaxis);
     demo_printf("gyro_x = %f, gyro_y = %f, gyro_z = %f ", icm_Data.gyro.xaxis, icm_Data.gyro.yaxis, icm_Data.gyro.zaxis);
     demo_printf("mag_x = %f, mag = %f, mag = %f\r\n", icm_Data.mag.xaxis, icm_Data.mag.yaxis, icm_Data.mag.zaxis);
#endif
    return(NX_AZURE_IOT_SUCCESS);
}
