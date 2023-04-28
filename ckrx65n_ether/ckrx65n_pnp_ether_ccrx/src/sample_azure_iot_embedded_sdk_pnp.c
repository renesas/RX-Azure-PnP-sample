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


#include <stdio.h>
#include "nx_api.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_hub_client_properties.h"
#include "nx_azure_iot_provisioning_client.h"

/* These are sample files, user can build their own certificate and ciphersuites.  */
#include "nx_azure_iot_cert.h"
#include "nx_azure_iot_ciphersuites.h"
#include "sample_config.h"
#include "hardware_setup.h"
#include "r_simple_filesystem_on_dataflash_if.h"
#include "CommandLine/menu_flash.h"
#include <demo_printf.h>
#include "softTimer.h"
#include "base64.h"
#ifndef SAMPLE_MAX_EXPONENTIAL_BACKOFF_IN_SEC
#define SAMPLE_MAX_EXPONENTIAL_BACKOFF_IN_SEC                           (10 * 60)
#endif /* SAMPLE_MAX_EXPONENTIAL_BACKOFF_IN_SEC */

#ifndef SAMPLE_INITIAL_EXPONENTIAL_BACKOFF_IN_SEC
#define SAMPLE_INITIAL_EXPONENTIAL_BACKOFF_IN_SEC                       (3)
#endif /* SAMPLE_INITIAL_EXPONENTIAL_BACKOFF_IN_SEC */

#ifndef SAMPLE_MAX_EXPONENTIAL_BACKOFF_JITTER_PERCENT
#define SAMPLE_MAX_EXPONENTIAL_BACKOFF_JITTER_PERCENT                   (60)
#endif /* SAMPLE_MAX_EXPONENTIAL_BACKOFF_JITTER_PERCENT */

#ifndef SAMPLE_WAIT_OPTION
#define SAMPLE_WAIT_OPTION                                              (NX_NO_WAIT)
#endif /* SAMPLE_WAIT_OPTION */

/* Sample events.  */
#define SAMPLE_ALL_EVENTS                                               ((ULONG)0xFFFFFFFF)
#define SAMPLE_CONNECT_EVENT                                            ((ULONG)0x00000001)
#define SAMPLE_INITIALIZATION_EVENT                                     ((ULONG)0x00000002)
#define SAMPLE_COMMAND_MESSAGE_EVENT                                    ((ULONG)0x00000004)
#define SAMPLE_DEVICE_PROPERTIES_GET_EVENT                              ((ULONG)0x00000008)
#define SAMPLE_DEVICE_DESIRED_PROPERTIES_EVENT                          ((ULONG)0x00000010)
#define SAMPLE_TELEMETRY_SEND_EVENT                                     ((ULONG)0x00000020)
#define SAMPLE_DEVICE_REPORTED_PROPERTIES_EVENT                         ((ULONG)0x00000040)
#define SAMPLE_DISCONNECT_EVENT                                         ((ULONG)0x00000080)
#define SAMPLE_RECONNECT_EVENT                                          ((ULONG)0x00000100)
#define SAMPLE_CONNECTED_EVENT                                          ((ULONG)0x00000200)

/* Sample states.  */
#define SAMPLE_STATE_NONE                                               (0)
#define SAMPLE_STATE_INIT                                               (1)
#define SAMPLE_STATE_CONNECTING                                         (2)
#define SAMPLE_STATE_CONNECT                                            (3)
#define SAMPLE_STATE_CONNECTED                                          (4)
#define SAMPLE_STATE_DISCONNECTED                                       (5)

#define SAMPLE_DEAFULT_START_TEMP_CELSIUS                               (22)
#define DOUBLE_DECIMAL_PLACE_DIGITS                                     (2)

#define SAMPLE_COMMAND_SUCCESS_STATUS                                   (200)
#define SAMPLE_COMMAND_NOT_FOUND_STATUS                                 (404)
#define SAMPLE_COMMAND_ERROR_STATUS                                     (500)

//TODO's: Change PNP model
#define SAMPLE_PNP_MODEL_ID                                             "dtmi:renesas:RX:CKRX65N;1"
#define SAMPLE_PNP_DPS_PAYLOAD                                          "{\"modelId\":\"" SAMPLE_PNP_MODEL_ID "\"}"

//TODO's: Add components this sample use.
#define SAMPLE_PNP_LED_COMPONENT_NAME                                   "LED_ckrx65n"
#define SAMPLE_PNP_AMBIENT_LIGHT_COMPONENT_NAME                         "AmbientLight"
#define SAMPLE_PNP_ENVIRONMENTAL_COMPONENT_NAME                         "Environmental"
#define SAMPLE_PNP_INERTIAL_COMPONENT_NAME                       		"Inertial"
#define SAMPLE_PNP_GAS_COMPONENT_NAME									"Gas"
#define SAMPLE_PNP_BIOMETRIC_COMPONENT_NAME								"Biometric"
void certfificateInit();
/* Define Sample context.  */
typedef struct SAMPLE_CONTEXT_STRUCT
{
    UINT                                state;
    UINT                                action_result;
    ULONG                               last_periodic_action_tick;

    TX_EVENT_FLAGS_GROUP                sample_events;

    /* Generally, IoTHub Client and DPS Client do not run at the same time, user can use union as below to
       share the memory between IoTHub Client and DPS Client.

       NOTE: If user can not make sure sharing memory is safe, IoTHub Client and DPS Client must be defined seperately.  */
    union SAMPLE_CLIENT_UNION
    {
        NX_AZURE_IOT_HUB_CLIENT             iothub_client;
#ifdef ENABLE_DPS_SAMPLE
        NX_AZURE_IOT_PROVISIONING_CLIENT    prov_client;
#endif /* ENABLE_DPS_SAMPLE */
    } client;

#define iothub_client client.iothub_client
#ifdef ENABLE_DPS_SAMPLE
#define prov_client client.prov_client
#endif /* ENABLE_DPS_SAMPLE */

} SAMPLE_CONTEXT;

//TODO`s: ADD Telemetry states
typedef enum TELEMETRY_STATE_ENUM
{
    TELEMETRY_STATE_ENVIRONMENTAL_HS3001,
    TELEMETRY_STATE_MOTION_SENSOR_ICM20948,
    TELEMETRY_STATE_GAS_IAQ,
	TELEMETRY_STATE_GAS_OAQ,
	TELEMETRY_STATE_BAROMETRIC_ICP10101,
	TELEMETRY_STATE_BIO,
    TELEMETRY_STATE_END
} TELEMETRY_STATE;

void sample_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time));

#ifdef ENABLE_DPS_SAMPLE
static UINT sample_dps_entry(NX_AZURE_IOT_PROVISIONING_CLIENT *prov_client_ptr,
                             UCHAR **iothub_hostname, UINT *iothub_hostname_length,
                             UCHAR **iothub_device_id, UINT *iothub_device_id_length);
#endif /* ENABLE_DPS_SAMPLE */

/* Define Azure RTOS TLS info.  */
static NX_SECURE_X509_CERT root_ca_cert;
static UCHAR nx_azure_iot_tls_metadata_buffer[NX_AZURE_IOT_TLS_METADATA_BUFFER_SIZE];
static ULONG nx_azure_iot_thread_stack[NX_AZURE_IOT_STACK_SIZE / sizeof(ULONG)];

/* Using X509 certificate authenticate to connect to IoT Hub,
   set the device certificate as your device.  */
#if (USE_DEVICE_CERTIFICATE == 1)
//extern const UCHAR sample_device_cert_ptr[];
//extern const UINT sample_device_cert_len;
//extern const UCHAR sample_device_private_key_ptr[];
//extern const UINT sample_device_private_key_len;


NX_SECURE_X509_CERT device_certificate;
#endif /* USE_DEVICE_CERTIFICATE */

/* Define buffer for IoTHub info.  */
#ifdef ENABLE_DPS_SAMPLE
static UCHAR sample_iothub_hostname[SAMPLE_MAX_BUFFER];
static UCHAR sample_iothub_device_id[SAMPLE_MAX_BUFFER];
#endif /* ENABLE_DPS_SAMPLE */

/* Define the prototypes for AZ IoT.  */
static NX_AZURE_IOT nx_azure_iot;

static SAMPLE_CONTEXT sample_context;
static volatile UINT sample_connection_status = NX_NOT_CONNECTED;
static UINT exponential_retry_count;
static UCHAR scratch_buffer[256];
static UINT sample_pnp_default_properties_sent = 0;

extern char id_scope[256];
extern char device_id[256];
extern char primary_key[256];

char *az_deviceKey;
char *az_privateKey;

char *iothub_hostname;
char *iothub_device_id;
UINT iothub_hostname_length;
UINT iothub_device_id_length;

UCHAR sample_device_cert_ptr[1024];
unsigned int sample_device_cert_len=0;
UCHAR sample_device_private_key_ptr[2048];
unsigned int sample_device_private_key_len=0;


/* Default component functions.  */
extern UINT sample_pnp_default_reported_properties_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);
extern UINT sample_pnp_default_command_process(const UCHAR *pnp_command_name_ptr, UINT pnp_command_name_length,
                                               NX_AZURE_IOT_JSON_READER *json_reader_ptr,
                                               NX_AZURE_IOT_JSON_WRITER *json_response_ptr, UINT *status_code);
extern void sample_pnp_default_reboot_check(void);
extern UINT sample_pnp_default_reboot_check_return(void);

/* LED component functions.  */
extern UINT sample_pnp_led_desired_properties_process(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr, NX_AZURE_IOT_JSON_READER *json_reader, ULONG version);

/* Environmental sensor functions.  */
extern UINT sample_environmental_sensor_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);

/* Inertial sensor functions.  */
extern UINT sample_inertial_sensor_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);

/* Gas functions.  */
extern UINT sample_gas_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);

/* Biometric sensor functions.*/
extern UINT sample_biometric_telemetry_messages_send(NX_AZURE_IOT_HUB_CLIENT *pnp_client_ptr);

static UINT exponential_backoff_with_jitter()
{
double jitter_percent = (SAMPLE_MAX_EXPONENTIAL_BACKOFF_JITTER_PERCENT / 100.0) * (rand() / ((double)RAND_MAX));
UINT base_delay = SAMPLE_MAX_EXPONENTIAL_BACKOFF_IN_SEC;
uint64_t delay;

    if (exponential_retry_count < (sizeof(UINT) * 8))
    {
        delay = (uint64_t)((1 << exponential_retry_count) * SAMPLE_INITIAL_EXPONENTIAL_BACKOFF_IN_SEC);
        if (delay <= (UINT)(-1))
        {
            base_delay = (UINT)delay;
        }
    }

    if (base_delay > SAMPLE_MAX_EXPONENTIAL_BACKOFF_IN_SEC)
    {
        base_delay = SAMPLE_MAX_EXPONENTIAL_BACKOFF_IN_SEC;
    }
    else
    {
        exponential_retry_count++;
    }

    return((UINT)(base_delay * (1 + jitter_percent)) * NX_IP_PERIODIC_RATE) ;
}

static VOID exponential_backoff_reset()
{
    exponential_retry_count = 0;
}

static VOID connection_status_callback(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr, UINT status)
{
    NX_PARAMETER_NOT_USED(hub_client_ptr);

    sample_connection_status = status;

    if (status)
    {
        printf("Disconnected from IoTHub!: error code = 0x%08x\r\n", status);
        tx_event_flags_set(&(sample_context.sample_events), SAMPLE_DISCONNECT_EVENT, TX_OR);
    }
    else
    {
        printf("Connected to IoTHub.\r\n");
        tx_event_flags_set(&(sample_context.sample_events), SAMPLE_CONNECTED_EVENT, TX_OR);
        exponential_backoff_reset();
    }
}

static VOID message_receive_callback_properties(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr, VOID *context)
{
SAMPLE_CONTEXT *sample_ctx = (SAMPLE_CONTEXT *)context;

    NX_PARAMETER_NOT_USED(hub_client_ptr);
    tx_event_flags_set(&(sample_ctx -> sample_events),
                       SAMPLE_DEVICE_PROPERTIES_GET_EVENT, TX_OR);
}

static VOID message_receive_callback_command(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr, VOID *context)
{
SAMPLE_CONTEXT *sample_ctx = (SAMPLE_CONTEXT *)context;

    NX_PARAMETER_NOT_USED(hub_client_ptr);
    tx_event_flags_set(&(sample_ctx -> sample_events),
                       SAMPLE_COMMAND_MESSAGE_EVENT, TX_OR);
}

static VOID message_receive_callback_desire_property(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr, VOID *context)
{
SAMPLE_CONTEXT *sample_ctx = (SAMPLE_CONTEXT *)context;

    NX_PARAMETER_NOT_USED(hub_client_ptr);
    tx_event_flags_set(&(sample_ctx -> sample_events),
                       SAMPLE_DEVICE_DESIRED_PROPERTIES_EVENT, TX_OR);
}

static VOID sample_connect_action(SAMPLE_CONTEXT *context)
{
    if (context -> state != SAMPLE_STATE_CONNECT)
    {
        return;
    }

    context -> action_result = nx_azure_iot_hub_client_connect(&(context -> iothub_client), NX_FALSE, SAMPLE_WAIT_OPTION);

    if (context -> action_result == NX_AZURE_IOT_CONNECTING)
    {
        context -> state = SAMPLE_STATE_CONNECTING;
    }
    else if (context -> action_result != NX_SUCCESS)
    {
        sample_connection_status = context -> action_result;
        context -> state = SAMPLE_STATE_DISCONNECTED;
    }
    else
    {
        context -> state = SAMPLE_STATE_CONNECTED;

        context -> action_result =
            nx_azure_iot_hub_client_device_twin_properties_request(&(context -> iothub_client), NX_WAIT_FOREVER);
    }
}

static VOID sample_disconnect_action(SAMPLE_CONTEXT *context)
{
    if (context -> state != SAMPLE_STATE_CONNECTED &&
        context -> state != SAMPLE_STATE_CONNECTING)
    {
        return;
    }

    context -> action_result = nx_azure_iot_hub_client_disconnect(&(context -> iothub_client));
    context -> state = SAMPLE_STATE_DISCONNECTED;
}

static VOID sample_connected_action(SAMPLE_CONTEXT *context)
{
    if (context -> state != SAMPLE_STATE_CONNECTING)
    {
        return;
    }

    context -> state = SAMPLE_STATE_CONNECTED;

    context -> action_result =
        nx_azure_iot_hub_client_device_twin_properties_request(&(context -> iothub_client), NX_WAIT_FOREVER);
}

static VOID sample_initialize_iothub(SAMPLE_CONTEXT *context)
{
UINT status;
#ifdef ENABLE_DPS_SAMPLE
UCHAR *iothub_hostname = NX_NULL;
UCHAR *iothub_device_id = NX_NULL;
UINT iothub_hostname_length = 0;
UINT iothub_device_id_length = 0;
#else

#endif /* ENABLE_DPS_SAMPLE */

uint8_t *tmp;
				uint32_t data_length;
				demo_printf("Start Provisioning Client...\r\n");


	    	    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)hostname_label, strlen(hostname_label)),&tmp,&data_length))
	    	    {
	    	    	data_length=data_length-2;
	    	    	demo_printf("\r\nHost Name Data Length:%d",data_length);
	    	    	iothub_hostname = malloc(data_length);
	    	        memcpy(iothub_hostname, tmp, data_length);
	    	        iothub_hostname_length = data_length;
	    	        iothub_hostname[data_length] = '\0';


	    	    }
	    	    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)deviceID_label, strlen(deviceID_label)),&tmp,&data_length))
	    	    {
	    	    	data_length=data_length-2;
	    	    	iothub_device_id = malloc(data_length);
	    	    	demo_printf("\r\nDevice ID Data Length:%d",data_length);
	    	        memcpy(iothub_device_id, tmp, data_length);
	    	        iothub_device_id_length =data_length;
	    	        iothub_device_id[data_length] = '\0';

	    	    }
	    	    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)device_cert_label, strlen(device_cert_label)),&tmp,&data_length))
	    	    {
	    	    	//data_length=data_length-2;
	    	    	az_deviceKey = malloc(data_length);
	    	    	demo_printf("\r\nCer Length:%d",data_length);
	    	     	memcpy(az_deviceKey, tmp, data_length);
	    	     	//az_deviceKey[data_length] = '\0';

	    	     }
	    	    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)private_key_label, strlen(private_key_label)),&tmp,&data_length))
	    	     {
	    	    	//data_length=data_length-2;
	    	    	az_privateKey = malloc(data_length);
	    	    	demo_printf("\r\Pvt Key Data Length:%d",data_length);
	    	     	memcpy(az_privateKey, tmp, data_length);
	    	     	//az_privateKey[data_length] = '\0';
	    	     }

	    	 sample_device_cert_len= DecodeBASE64(&az_deviceKey[0],&sample_device_cert_ptr[0]);
	    	 sample_device_private_key_len= DecodeBASE64(&az_privateKey[0],&sample_device_private_key_ptr[0]);

NX_AZURE_IOT_HUB_CLIENT* iothub_client_ptr = &(context -> iothub_client);

    if (context -> state != SAMPLE_STATE_INIT)
    {
        return;
    }

#ifdef ENABLE_DPS_SAMPLE

    /* Run DPS.  */
    if ((status = sample_dps_entry(&(context -> prov_client), &iothub_hostname, &iothub_hostname_length,
                                   &iothub_device_id, &iothub_device_id_length)))
    {
        printf("Failed on sample_dps_entry!: error code = 0x%08x\r\n", status);
        context -> action_result = status;
        return;
    }
#endif /* ENABLE_DPS_SAMPLE */
    /* Initialize IoTHub client.  */
    if ((status = nx_azure_iot_hub_client_initialize(iothub_client_ptr, &nx_azure_iot,
                                                     iothub_hostname, iothub_hostname_length,
                                                     iothub_device_id, iothub_device_id_length,
                                                     (UCHAR *)MODULE_ID, sizeof(MODULE_ID) - 1,
                                                     _nx_azure_iot_tls_supported_crypto,
                                                     _nx_azure_iot_tls_supported_crypto_size,
                                                     _nx_azure_iot_tls_ciphersuite_map,
                                                     _nx_azure_iot_tls_ciphersuite_map_size,
                                                     nx_azure_iot_tls_metadata_buffer,
                                                     sizeof(nx_azure_iot_tls_metadata_buffer),
                                                     &root_ca_cert)))

    {
        printf("Failed on nx_azure_iot_hub_client_initialize!: error code = 0x%08x\r\n", status);
        context -> action_result = status;
        return;
    }



#if (USE_DEVICE_CERTIFICATE == 1)

    /* Initialize the device certificate.  */
    if ((status = nx_secure_x509_certificate_initialize(&device_certificate,
                                                        (UCHAR *)sample_device_cert_ptr, (USHORT)sample_device_cert_len,
                                                        NX_NULL, 0,
                                                        (UCHAR *)sample_device_private_key_ptr, (USHORT)sample_device_private_key_len,
                                                        DEVICE_KEY_TYPE)))
    {
        printf("Failed on nx_secure_x509_certificate_initialize!: error code = 0x%08x\r\n", status);
    }

    /* Set device certificate.  */
    else if ((status = nx_azure_iot_hub_client_device_cert_set(iothub_client_ptr, &device_certificate)))
    {
        printf("Failed on nx_azure_iot_hub_client_device_cert_set!: error code = 0x%08x\r\n", status);
    }
#else


#if !ENABLE_SERIAL_PARAM
    /* Set symmetric key.  */
    if ((status = nx_azure_iot_hub_client_symmetric_key_set(iothub_client_ptr,
                                                             (UCHAR *)az_key,
                                                             strlen(az_key))))
    {
        printf("Failed on nx_azure_iot_hub_client_symmetric_key_set!\r\n");
    }
    #else
    /* Set symmetric key.  */
    if ((status = nx_azure_iot_hub_client_symmetric_key_set(iothub_client_ptr,
        primary_key,
                                                            strlen(primary_key))))
    {
        printf("Failed on nx_azure_iot_pnp_client_symmetric_key_set!\r\n");
    }

#endif


#endif /* USE_DEVICE_CERTIFICATE */

    /* Set connection status callback.  */
    else if ((status = nx_azure_iot_hub_client_connection_status_callback_set(iothub_client_ptr,
                                                                              connection_status_callback)))
    {
        printf("Failed on connection_status_callback!\r\n");
    }
    else if ((status = nx_azure_iot_hub_client_direct_method_enable(iothub_client_ptr)))
    {
        printf("Direct method receive enable failed!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_device_twin_enable(iothub_client_ptr)))
    {
        printf("device twin enabled failed!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_receive_callback_set(iothub_client_ptr,
                                                                    NX_AZURE_IOT_HUB_PROPERTIES,
                                                                    message_receive_callback_properties,
                                                                    (VOID *)context)))
    {
        printf("device twin callback set!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_receive_callback_set(iothub_client_ptr,
                                                                    NX_AZURE_IOT_HUB_COMMAND,
                                                                    message_receive_callback_command,
                                                                    (VOID *)context)))
    {
        printf("device method callback set!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_receive_callback_set(iothub_client_ptr,
                                                                    NX_AZURE_IOT_HUB_WRITABLE_PROPERTIES,
                                                                    message_receive_callback_desire_property,
                                                                    (VOID *)context)))
    {
        printf("device twin desired property callback set!: error code = 0x%08x\r\n", status);
    }
//TODO`s: ADD components
    else if ((status = nx_azure_iot_hub_client_model_id_set(iothub_client_ptr, (UCHAR *)SAMPLE_PNP_MODEL_ID, sizeof(SAMPLE_PNP_MODEL_ID) - 1)))
    {
        printf("digital twin modelId set!: error code = 0x%08x\r\n", status);
    }else if ((status = nx_azure_iot_hub_client_component_add(iothub_client_ptr, 
                                                             SAMPLE_PNP_LED_COMPONENT_NAME,
                                                             sizeof(SAMPLE_PNP_LED_COMPONENT_NAME) - 1)))
    {
        printf("led component add!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_component_add(iothub_client_ptr, 
                                                             SAMPLE_PNP_AMBIENT_LIGHT_COMPONENT_NAME,
                                                             sizeof(SAMPLE_PNP_AMBIENT_LIGHT_COMPONENT_NAME) - 1)))
    {
        printf("Ambient component component add!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_component_add(iothub_client_ptr,
    															SAMPLE_PNP_ENVIRONMENTAL_COMPONENT_NAME,
                                                             sizeof(SAMPLE_PNP_ENVIRONMENTAL_COMPONENT_NAME) - 1)))
    {
        printf("ENVIRONMENTAL component add!: error code = 0x%08x\r\n", status);
    }
    else if ((status = nx_azure_iot_hub_client_component_add(iothub_client_ptr,
    															SAMPLE_PNP_INERTIAL_COMPONENT_NAME,
                                                             sizeof(SAMPLE_PNP_INERTIAL_COMPONENT_NAME) - 1)))
    {
        printf("INERTIAL component add!: error code = 0x%08x\r\n", status);
    }
     else if ((status = nx_azure_iot_hub_client_component_add(iothub_client_ptr,
    		 	 	 	 	 	 	 	 	 	 	 	 	 	 	 SAMPLE_PNP_BIOMETRIC_COMPONENT_NAME,
                                                                  sizeof(SAMPLE_PNP_BIOMETRIC_COMPONENT_NAME) - 1)))
	 {
	 	printf("Biometric component add!: error code = 0x%08x\r\n", status);
	 }
     else if ((status = nx_azure_iot_hub_client_component_add(iothub_client_ptr,
    		 	 	 	 	 	 	 	 	 	 	 	 	 	 SAMPLE_PNP_GAS_COMPONENT_NAME,
																sizeof(SAMPLE_PNP_GAS_COMPONENT_NAME) - 1)))
	{
    	 printf("Gas component add!: error code = 0x%08x\r\n", status);
	}

    if (status)
    {
        nx_azure_iot_hub_client_deinitialize(iothub_client_ptr);
    }

    context -> action_result = status;

    if (status == NX_AZURE_IOT_SUCCESS)
    {
        context -> state = SAMPLE_STATE_CONNECT;
    }
}

static VOID sample_connection_error_recover(SAMPLE_CONTEXT *context)
{
    if (context -> state != SAMPLE_STATE_DISCONNECTED)
    {
        return;
    }

    switch (sample_connection_status)
    {
        case NX_AZURE_IOT_SUCCESS:
        {
            printf("already connected\r\n");
        }
        break;

        /* Something bad has happened with client state, we need to re-initialize it.  */
        case NX_DNS_QUERY_FAILED :
        case NXD_MQTT_ERROR_BAD_USERNAME_PASSWORD :
        case NXD_MQTT_ERROR_NOT_AUTHORIZED :
        {
            printf("re-initializing iothub connection, after backoff\r\n");

            tx_thread_sleep(exponential_backoff_with_jitter());
            nx_azure_iot_hub_client_deinitialize(&(context -> iothub_client));
            context -> state = SAMPLE_STATE_INIT;
        }
        break;

        default :
        {
            printf("reconnecting iothub, after backoff\r\n");

            tx_thread_sleep(exponential_backoff_with_jitter());
            context -> state = SAMPLE_STATE_CONNECT;
        }
        break;
    }
}

static VOID sample_trigger_action(SAMPLE_CONTEXT *context)
{
    switch (context -> state)
    {
        case SAMPLE_STATE_INIT:
        {
            tx_event_flags_set(&(context -> sample_events), SAMPLE_INITIALIZATION_EVENT, TX_OR);
        }
        break;

        case SAMPLE_STATE_CONNECT:
        {
            tx_event_flags_set(&(context -> sample_events), SAMPLE_CONNECT_EVENT, TX_OR);
        }
        break;

        case SAMPLE_STATE_CONNECTED:
        {
            /* Only report default properties once.  */
            if (sample_pnp_default_properties_sent == 0)
            {
                tx_event_flags_set(&(context -> sample_events), SAMPLE_DEVICE_REPORTED_PROPERTIES_EVENT, TX_OR);
            }

            /* Set telemetry event every 5s.  */
            if ((tx_time_get() - context -> last_periodic_action_tick) >= (5 * NX_IP_PERIODIC_RATE))
            {
                context -> last_periodic_action_tick = tx_time_get();
                tx_event_flags_set(&(context -> sample_events), SAMPLE_TELEMETRY_SEND_EVENT, TX_OR);
            }
        }
        break;

        case SAMPLE_STATE_DISCONNECTED:
        {
            tx_event_flags_set(&(context -> sample_events), SAMPLE_RECONNECT_EVENT, TX_OR);
        }
        break;
        
        default:
        	break;
    }
}

static VOID sample_command_action(SAMPLE_CONTEXT *sample_context_ptr)
{

    UINT status;
    USHORT context_length;
    VOID *context_ptr;
    USHORT component_name_length;	
    const UCHAR *component_name_ptr;
    USHORT pnp_command_name_length_short;	
    UINT pnp_command_name_length;
    const UCHAR *pnp_command_name_ptr;
    NX_AZURE_IOT_JSON_WRITER json_writer;
    NX_AZURE_IOT_JSON_READER json_reader;
    UINT status_code;
    UINT response_length;
    NX_PACKET *packet_ptr;

    /* Loop to receive command message.  */
    while (1)
    {

        if (sample_context_ptr -> state != SAMPLE_STATE_CONNECTED)
        {
            return;
        }

        /* Receive command.  */
        if ((status = nx_azure_iot_hub_client_command_message_receive(&(sample_context_ptr -> iothub_client),
                                                                        &component_name_ptr, &component_name_length,
                                                                        &pnp_command_name_ptr, &pnp_command_name_length_short,
                                                                        &context_ptr, &context_length,
                                                                        &packet_ptr, NX_NO_WAIT)))
        {
            return;
        }

        pnp_command_name_length = (UINT)pnp_command_name_length_short;	
        printf("Received command: %.*s", (INT)pnp_command_name_length, (CHAR *)pnp_command_name_ptr);
        printf("\r\n");

        if ((status = nx_azure_iot_json_reader_init(&json_reader,
                                                    packet_ptr)))
        {
            printf("Failed to initialize json reader \r\n");
            nx_packet_release(packet_ptr);
            return;
        }

        /* Initialize response json.  */
        if ((status = nx_azure_iot_json_writer_with_buffer_init(&json_writer,
                scratch_buffer,
                sizeof(scratch_buffer))))
        {
            printf("Failed to initialize json builder response \r\n");
            nx_azure_iot_json_reader_deinit(&json_reader);
            return;
        }

        /* Process command.  */
        if ((status = sample_pnp_default_command_process(pnp_command_name_ptr, pnp_command_name_length,
                &json_reader, &json_writer, &status_code)) == NX_AZURE_IOT_SUCCESS)
        {
            printf("Successfully executed command %.*s on default\r\n", pnp_command_name_length, pnp_command_name_ptr);
            response_length = nx_azure_iot_json_writer_get_bytes_used(&json_writer);
            status_code = SAMPLE_COMMAND_SUCCESS_STATUS;
        }
        else
        {
            printf("Failed to find any handler for command %.*s\r\n", pnp_command_name_length, pnp_command_name_ptr);
            status_code = SAMPLE_COMMAND_NOT_FOUND_STATUS;
            response_length = 0;
        }

        nx_packet_release(packet_ptr);
        nx_azure_iot_json_reader_deinit(&json_reader);

        if ((status = nx_azure_iot_hub_client_command_message_response(&(sample_context_ptr -> iothub_client), status_code,
                context_ptr, context_length, scratch_buffer,
                response_length, NX_WAIT_FOREVER)))
        {
            printf("Command response failed!: error code = 0x%08x\r\n", status);
        }

        nx_azure_iot_json_writer_deinit(&json_writer);
    }
}

/* Parses device properties document.  */
static UINT sample_parse_desired_temp_property(SAMPLE_CONTEXT *context,
                                               NX_AZURE_IOT_JSON_READER *json_reader_ptr,
                                               UINT message_type, ULONG version)
{
UINT status;
const UCHAR *component_name_ptr;
USHORT component_length = 0;
demo_printf("RX_message In--sample_parse_desired_temp_property\r\n");
    while ((status = nx_azure_iot_hub_client_properties_component_property_next_get(&(context -> iothub_client),
                                                                                                json_reader_ptr,
                                                                                                message_type,
                                                                                                NX_AZURE_IOT_HUB_CLIENT_PROPERTY_WRITABLE,
                                                                                                &component_name_ptr, &component_length)) == NX_AZURE_IOT_SUCCESS)
    {
    	demo_printf("LED------------->\r\n");
    	demo_printf("component_name_ptr %s\r\n",component_name_ptr);

        /* Check the component name.  */
        if ((component_length == (sizeof(SAMPLE_PNP_LED_COMPONENT_NAME) - 1)) &&
            (memcmp(component_name_ptr, SAMPLE_PNP_LED_COMPONENT_NAME, component_length) == 0))
        {
        	sample_pnp_led_desired_properties_process(&(context -> iothub_client), json_reader_ptr, version);
        }
        else
        {
        	/* The JSON reader must be advanced regardless of whether the property is of interest or not.  */
			nx_azure_iot_json_reader_next_token(json_reader_ptr);

			/* Skip children in case the property value is an object.  */
			if (nx_azure_iot_json_reader_token_type(json_reader_ptr) == NX_AZURE_IOT_READER_TOKEN_BEGIN_OBJECT)
			{
				nx_azure_iot_json_reader_skip_children(json_reader_ptr);
			}
			nx_azure_iot_json_reader_next_token(json_reader_ptr);
        }

    }

    return(NX_AZURE_IOT_SUCCESS);
}

static void sample_device_desired_property_action(SAMPLE_CONTEXT *context)
{
    UINT status = 0;
    NX_PACKET *packet_ptr;
    NX_AZURE_IOT_JSON_READER json_reader;
    ULONG properties_version = 0;

        if (context -> state != SAMPLE_STATE_CONNECTED)
        {
            return;
        }

        if ((status = nx_azure_iot_hub_client_writable_properties_receive(&(context -> iothub_client),
                                                                          &packet_ptr,
                                                                          NX_WAIT_FOREVER)))
        {
            printf("Receive writable property receive failed!: error code = 0x%08x\r\n", status);
            return;
        }

        printf("Received writable property");
        printf("\r\n");

        if ((status = nx_azure_iot_json_reader_init(&json_reader, packet_ptr)))
        {
            printf("Init json reader failed!: error code = 0x%08x\r\n", status);
            nx_packet_release(packet_ptr);
            return;
        }

        /* Get the version.  */
        if ((status = nx_azure_iot_hub_client_properties_version_get(&(context -> iothub_client), &json_reader,
                                                                     NX_AZURE_IOT_HUB_WRITABLE_PROPERTIES,
                                                                     &properties_version)))
        {
            printf("Properties version get failed!: error code = 0x%08x\r\n", status);
            nx_packet_release(packet_ptr);
            return;
        }

        if ((status = nx_azure_iot_json_reader_init(&json_reader, packet_ptr)))
        {
            printf("Init json reader failed!: error code = 0x%08x\r\n", status);
            nx_packet_release(packet_ptr);
            return;
        }

        status = sample_parse_desired_temp_property(context, &json_reader, NX_AZURE_IOT_HUB_WRITABLE_PROPERTIES, properties_version);
        if (status && (status != NX_AZURE_IOT_NOT_FOUND))
        {
            printf("Failed to parse value\r\n");
        }

        nx_packet_release(packet_ptr);
        nx_azure_iot_json_reader_deinit(&json_reader);
}

static void sample_device_reported_property_action(SAMPLE_CONTEXT *context)
{

    if (context -> state != SAMPLE_STATE_CONNECTED)
    {
        return;
    }


        /* Send reported properties.  */
    if (sample_pnp_default_reported_properties_send(&(context -> iothub_client)))
    {
        printf("Default reported properties send failed\r\n");
    }
    else
    {
        sample_pnp_default_properties_sent++;
        printf("Default reported properties send successfully\r\n");
    }
}

static void sample_device_properties_get_action(SAMPLE_CONTEXT *context)
{
UINT status = 0;
NX_PACKET *packet_ptr;
NX_AZURE_IOT_JSON_READER json_reader;
ULONG properties_version = 0;

    if (context -> state != SAMPLE_STATE_CONNECTED)
    {
        return;
    }

    if ((status = nx_azure_iot_hub_client_device_twin_properties_receive(&(context -> iothub_client), &packet_ptr,
                                                                         NX_WAIT_FOREVER)))
    {
        printf("Twin receive failed!: error code = 0x%08x\r\n", status);
        return;
    }

    printf("Received all properties\r\n");


    if ((status = nx_azure_iot_json_reader_init(&json_reader, packet_ptr)))
    {
        printf("Init json reader failed!: error code = 0x%08x\r\n", status);
        nx_packet_release(packet_ptr);
        return;
    }
    else
    {
    	 printf("Init json reader success!: error code = 0x%08x\r\n", status);
    }

    if ((status = sample_parse_desired_temp_property(context, &json_reader, NX_AZURE_IOT_HUB_PROPERTIES, properties_version)))
    {
    	printf("Success to parse value\r\n");
    }
    else
    {
    	printf("Failed to parse value\r\n");
    }

    nx_azure_iot_json_reader_deinit(&json_reader);
}

typedef enum
{
	APP_MQTT_PUBLISH_STATE,
	APP_MQTT_WAIT_FOR_TIMEOUT_STATE,

}MQTT_STATE;

#define DATA_PUBLISH_INTERVAL_IN_SEC 10U

MQTT_STATE state =APP_MQTT_PUBLISH_STATE;
static void sample_telemetry_action(SAMPLE_CONTEXT * context)
{

	static uint16_t telemetry_state = TELEMETRY_STATE_ENVIRONMENTAL_HS3001;

    if (context->state != SAMPLE_STATE_CONNECTED)
    {
        return;
    }

    	switch(state)
           {
			 case APP_MQTT_PUBLISH_STATE:
			   {
				   startTimer(MQTT_TIMER,((DATA_PUBLISH_INTERVAL_IN_SEC-2)*1000));

				/* Send environmental sensor telemetry message HS3001 Temperature and Humidity.  */
				if (sample_environmental_sensor_telemetry_messages_send( &(context->iothub_client)))
				{
					printf("Telemetry message send failed!\r\n");
					return;
				}



				/* Send inertial sensor telemetry message Accelerometer Gyro and Magnetometer Data.  */
				if (sample_inertial_sensor_telemetry_messages_send( &(context->iothub_client)))
				{
					printf("Telemetry message send failed!\r\n");
					return;
				}


				/* Send Gas telemetry message Indoor Air Quality.  */
				if (sample_gas_telemetry_messages_send(&(context -> iothub_client)))
				{
					printf("Telemetry message send failed!\r\n");
					return;
				}


			/* Send Gas telemetry message Outdoor Air Quality.  */
				if (sample_gas_oaq_sensor_telemetry_messages_send(&(context->iothub_client)))
				{
					printf("Telemetry message send failed!\r\n");
					return;
				}


			/* Send Barometric sensor telemetry message Pressure and Temperature Data .  */
				if (sample_barometric_sensor_telemetry_messages_send(&(context->iothub_client)))
				{
					printf("Telemetry message send failed!\r\n");
					return;
				}


				/* Send Biometric sensor telemetry message SPO2, HR,RR and PI data.  */
				if (sample_biometric_telemetry_messages_send(&(context -> iothub_client)))
				{
					printf("Telemetry message send failed!\r\n");
					return;
				}
				state=APP_MQTT_WAIT_FOR_TIMEOUT_STATE;
				break;
			   }
    	 case APP_MQTT_WAIT_FOR_TIMEOUT_STATE:
    	   {

    	       if(1==getTimeOutOccur(MQTT_TIMER))
    	         {
    	       		stopTimer(MQTT_TIMER);
    	       		state = APP_MQTT_PUBLISH_STATE;
    	         }
    	    break;
    	    }


    }


telemetry_state = (telemetry_state + 1) % TELEMETRY_STATE_END;

}



#ifdef ENABLE_DPS_SAMPLE
static UINT sample_dps_entry(NX_AZURE_IOT_PROVISIONING_CLIENT *prov_client_ptr,
                             UCHAR **iothub_hostname, UINT *iothub_hostname_length,
                             UCHAR **iothub_device_id, UINT *iothub_device_id_length)
{
			UINT status;
			uint8_t *tmp;
			uint32_t data_length;
			demo_printf("Start Provisioning Client...\r\n");


    	    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)endpoint_label, strlen(endpoint_label)),&tmp,&data_length))
    	    {
    	    	data_length=data_length-2;
    	    	demo_printf("\r\nEndpoint Data Length:%d",data_length);
    	    	az_endPoint = malloc(data_length);
    	        memcpy(az_endPoint, tmp, data_length);
    	        az_endPoint[data_length] = '\0';

    	    }
    	    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)id_scope_label, strlen(id_scope_label)),&tmp,&data_length))
    	    {
    	    	data_length=data_length-2;
    	    	az_id_scope = malloc(data_length);
    	    	demo_printf("\r\nID_Scope Data Length:%d",data_length);
    	        memcpy(az_id_scope, tmp, data_length);
    	        az_id_scope[data_length] = '\0';

    	    }
    	    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)registration_id_label, strlen(registration_id_label)),&tmp,&data_length))
    	    {
    	    	data_length=data_length-2;
    	    	az_regID = malloc(data_length);
    	    	demo_printf("\r\nRegID Length:%d",data_length);
    	     	memcpy(az_regID, tmp, data_length);
    	     	az_regID[data_length] = '\0';

    	     }
    	    if(SFD_SUCCESS == R_SFD_GetObjectValue(R_SFD_FindObject((uint8_t *)device_symetric_key_label, strlen(device_symetric_key_label)),&tmp,&data_length))
    	     {
    	    	data_length=data_length-2;
    	    	az_key = malloc(data_length);
    	    	demo_printf("\r\Key Data Length:%d",data_length);
    	     	memcpy(az_key, tmp, data_length);
    	     	az_key[data_length] = '\0';
    	     }

#if !ENABLE_SERIAL_PARAM
    /* Initialize IoT provisioning client.  */
    if ((status = nx_azure_iot_provisioning_client_initialize(prov_client_ptr, &nx_azure_iot,
                                                              (UCHAR *)az_endPoint, strlen(az_endPoint),
                                                              (UCHAR *)az_id_scope, strlen(az_id_scope),
                                                              (UCHAR *)az_regID, strlen(az_regID),
                                                              _nx_azure_iot_tls_supported_crypto,
                                                              _nx_azure_iot_tls_supported_crypto_size,
                                                              _nx_azure_iot_tls_ciphersuite_map,
                                                              _nx_azure_iot_tls_ciphersuite_map_size,
                                                              nx_azure_iot_tls_metadata_buffer,
                                                              sizeof(nx_azure_iot_tls_metadata_buffer),
                                                              &root_ca_cert)))
    {
        printf("Failed on nx_azure_iot_provisioning_client_initialize!: error code = 0x%08x\r\n", status);
        return(status);
    }
    demo_printf("\r\nEndpoint: %s Len :%d",az_endPoint,strlen(az_endPoint));
    demo_printf("\r\nID_Scope %s Len :%d",az_id_scope,strlen(az_id_scope));
    demo_printf("\r\nReg ID %s Len :%d",az_regID,strlen(az_regID));
    demo_printf("\r\nKey %s Len :%d",az_key,strlen(az_key));


#else
    /* Initialize IoT provisioning client.  */
    if ((status = nx_azure_iot_provisioning_client_initialize(prov_client_ptr, &nx_azure_iot,
	                                                              (UCHAR *)ENDPOINT, sizeof(ENDPOINT) - 1,
																  id_scope, strlen(id_scope),
																  device_id, strlen(device_id),
	                                                              _nx_azure_iot_tls_supported_crypto,
	                                                              _nx_azure_iot_tls_supported_crypto_size,
	                                                              _nx_azure_iot_tls_ciphersuite_map,
	                                                              _nx_azure_iot_tls_ciphersuite_map_size,
	                                                              nx_azure_iot_tls_metadata_buffer,
	                                                              sizeof(nx_azure_iot_tls_metadata_buffer),
	                                                              &root_ca_cert)))
    {
        printf("Failed on nx_azure_iot_provisioning_client_initialize!: error code = 0x%08x\r\n", status);
        return(status);
    }
#endif

    /* Initialize length of hostname and device ID.  */
    *iothub_hostname_length = sizeof(sample_iothub_hostname);
    *iothub_device_id_length = sizeof(sample_iothub_device_id);

#if (USE_DEVICE_CERTIFICATE == 1)

    /* Initialize the device certificate.  */
    if ((status = nx_secure_x509_certificate_initialize(&device_certificate, (UCHAR *)sample_device_cert_ptr, (USHORT)sample_device_cert_len, NX_NULL, 0,
                                                        (UCHAR *)sample_device_private_key_ptr, (USHORT)sample_device_private_key_len, DEVICE_KEY_TYPE)))
    {
        printf("Failed on nx_secure_x509_certificate_initialize!: error code = 0x%08x\r\n", status);
    }

    /* Set device certificate.  */
    else if ((status = nx_azure_iot_provisioning_client_device_cert_set(prov_client_ptr, &device_certificate)))
    {
        printf("Failed on nx_azure_iot_provisioning_client_device_cert_set!: error code = 0x%08x\r\n", status);
    }
#else

#if !ENABLE_SERIAL_PARAM
    /* Set symmetric key.  */
    if ((status = nx_azure_iot_provisioning_client_symmetric_key_set(prov_client_ptr, (UCHAR *)az_key,strlen(az_key))))
    {
        printf("Failed on nx_azure_iot_hub_client_symmetric_key_set!: error code = 0x%08x\r\n", status);
    }
    //demo_printf("\r\nKey %s Len :%d",DEVICE_SYMMETRIC_KEY,strlen(DEVICE_SYMMETRIC_KEY));
#else
    if ((status = nx_azure_iot_provisioning_client_symmetric_key_set(prov_client_ptr, primary_key, strlen(primary_key))))
        {
            printf("Failed on nx_azure_iot_pnp_client_symmetric_key_set!: error code = 0x%08x\r\n", status);
        }
#endif

#endif /* USE_DEVICE_CERTIFICATE */
    else if ((status = nx_azure_iot_provisioning_client_registration_payload_set(prov_client_ptr, (UCHAR *)SAMPLE_PNP_DPS_PAYLOAD,
                                                                                 sizeof(SAMPLE_PNP_DPS_PAYLOAD) - 1)))
    {
        printf("Failed on nx_azure_iot_provisioning_client_registration_payload_set!: error code = 0x%08x\r\n", status);
    }
    /* Register device.  */
    else if ((status = nx_azure_iot_provisioning_client_register(prov_client_ptr, NX_WAIT_FOREVER)))
    {
        printf("Failed on nx_azure_iot_provisioning_client_register!: error code = 0x%08x\r\n", status);
    }

    /* Get Device info.  */
    else if ((status = nx_azure_iot_provisioning_client_iothub_device_info_get(prov_client_ptr,
                                                                               sample_iothub_hostname, iothub_hostname_length,
                                                                               sample_iothub_device_id, iothub_device_id_length)))
    {
        printf("Failed on nx_azure_iot_provisioning_client_iothub_device_info_get!: error code = 0x%08x\r\n", status);
    }
    else
    {
        *iothub_hostname = sample_iothub_hostname;
        *iothub_device_id = sample_iothub_device_id;
        printf("Registered Device Successfully.\r\n");
    }

    /* Destroy Provisioning Client.  */
    nx_azure_iot_provisioning_client_deinitialize(prov_client_ptr);

    return(status);
}
#endif /* ENABLE_DPS_SAMPLE */

/**
 *
 * Sample Event loop
 *
 *
 *       +--------------+           +--------------+      +--------------+       +--------------+
 *       |              |  INIT     |              |      |              |       |              |
 *       |              | SUCCESS   |              |      |              |       |              +--------+
 *       |    INIT      |           |    CONNECT   |      |  CONNECTING  |       |   CONNECTED  |        | (TELEMETRY |
 *       |              +----------->              +----->+              +------->              |        |  COMMAND |
 *       |              |           |              |      |              |       |              <--------+  PROPERTIES)
 *       |              |           |              |      |              |       |              |
 *       +-----+--------+           +----+---+-----+      +------+-------+       +--------+-----+
 *             ^                         ^   |                   |                        |
 *             |                         |   |                   |                        |
 *             |                         |   |                   |                        |
 *             |                         |   | CONNECT           | CONNECTING             |
 *             |                         |   |  FAIL             |   FAIL                 |
 * REINITIALIZE|                RECONNECT|   |                   |                        |
 *             |                         |   |                   v                        |  DISCONNECT
 *             |                         |   |        +----------+-+                      |
 *             |                         |   |        |            |                      |
 *             |                         |   +------->+            |                      |
 *             |                         |            | DISCONNECT |                      |
 *             |                         |            |            +<---------------------+
 *             |                         +------------+            |
 *             +--------------------------------------+            |
 *                                                    +------------+
 *
 *
 *
 */
static VOID sample_event_loop(SAMPLE_CONTEXT *context)
{
ULONG app_events;
UINT loop = NX_TRUE;

    while (loop)
    {

        /* Pickup IP event flags.  */
        if (tx_event_flags_get(&(context -> sample_events), SAMPLE_ALL_EVENTS, TX_OR_CLEAR, &app_events, 5 * NX_IP_PERIODIC_RATE))
        {
            if (context -> state == SAMPLE_STATE_CONNECTED)
            {
                sample_trigger_action(context);
            }

            continue;
        }

        if (app_events & SAMPLE_CONNECT_EVENT)
        {
            sample_connect_action(context);
        }

        if (app_events & SAMPLE_INITIALIZATION_EVENT)
        {
            sample_initialize_iothub(context);
        }

        if (app_events & SAMPLE_DEVICE_PROPERTIES_GET_EVENT)
        {
            sample_device_properties_get_action(context);
        }

        if (app_events & SAMPLE_COMMAND_MESSAGE_EVENT)
        {
            sample_command_action(context);
        }

        if (app_events & SAMPLE_DEVICE_DESIRED_PROPERTIES_EVENT)
        {
            sample_device_desired_property_action(context);
        }

        if (app_events & SAMPLE_TELEMETRY_SEND_EVENT)
        {
            sample_telemetry_action(context);
        }

        if (app_events & SAMPLE_DEVICE_REPORTED_PROPERTIES_EVENT)
        {
            sample_device_reported_property_action(context);
        }

        if (app_events & SAMPLE_DISCONNECT_EVENT)
        {
            sample_disconnect_action(context);
        }

        if (app_events & SAMPLE_CONNECTED_EVENT)
        {
            sample_connected_action(context);
        }

        if (app_events & SAMPLE_RECONNECT_EVENT)
        {
            sample_connection_error_recover(context);
        }

        sample_trigger_action(context);
        
        sample_pnp_default_reboot_check();
    }
}

static VOID sample_context_init(SAMPLE_CONTEXT *context)
{
    memset(context, 0, sizeof(SAMPLE_CONTEXT));
    tx_event_flags_create(&(context->sample_events), (CHAR*)"sample_app");
}

static void log_callback(az_log_classification classification, UCHAR *msg, UINT msg_len)
{
    if (classification == AZ_LOG_IOT_AZURERTOS)
    {
        printf("%.*s", msg_len, (CHAR *)msg);
    }
}

void sample_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time))
{
UINT status = 0;

    nx_azure_iot_log_init(log_callback);

    /* Create Azure IoT handler.  */
    if ((status = nx_azure_iot_create(&nx_azure_iot, (UCHAR *)"Azure IoT", ip_ptr, pool_ptr, dns_ptr,
                                      nx_azure_iot_thread_stack, sizeof(nx_azure_iot_thread_stack),
                                      NX_AZURE_IOT_THREAD_PRIORITY, unix_time_callback)))
    {
        printf("Failed on nx_azure_iot_create!: error code = 0x%08x\r\n", status);
        return;
    }

    /* Initialize CA certificate.  */
    if ((status = nx_secure_x509_certificate_initialize(&root_ca_cert, (UCHAR *)_nx_azure_iot_root_cert,
                                                        (USHORT)_nx_azure_iot_root_cert_size,
                                                        NX_NULL, 0, NULL, 0, NX_SECURE_X509_KEY_TYPE_NONE)))
    {
        printf("Failed to initialize ROOT CA certificate!: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return;
    }

    sample_context_init(&sample_context);

    sample_context.state = SAMPLE_STATE_INIT;
    tx_event_flags_set(&(sample_context.sample_events), SAMPLE_INITIALIZATION_EVENT, TX_OR);

    /* Handle event loop.  */
    sample_event_loop(&sample_context);

    nx_azure_iot_delete(&nx_azure_iot);
}
