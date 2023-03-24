/*
 * sample_pnp.h
 *
 *  Created on: Mar 16, 2023
 *      Author: a5111029
 */

#ifndef SRC_SAMPLE_PNP_H_
#define SRC_SAMPLE_PNP_H_

#include "demo_printf.h"

UINT sample_pnp_default_reported_properties_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);
UINT sample_pnp_default_command_process(const UCHAR *pnp_command_name_ptr, UINT pnp_command_name_length,
                                        NX_AZURE_IOT_JSON_READER *json_reader_ptr,
                                        NX_AZURE_IOT_JSON_WRITER *json_response_ptr, UINT *status_code);
void sample_pnp_default_reboot_check(void);
UINT sample_pnp_default_reboot_check_return(void);


UINT sample_environmental_sensor_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);
UINT sample_gas_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);
UINT sample_gas_oaq_sensor_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);

UINT sample_pnp_led_desired_properties_process(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr, NX_AZURE_IOT_JSON_READER *json_reader, ULONG version);
UINT sample_pnp_led_reported_properties_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr,
                                             UCHAR *property_name_ptr, UINT property_name_length,
                                             UINT property_value, UINT status_code,
                                             ULONG version, const CHAR *description);

#endif /* SRC_SAMPLE_PNP_H_ */
