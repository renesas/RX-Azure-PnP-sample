
#include "RX_HS3001.h"
#include "stdbool.h"
//#include "stdint.h"
#include "r_smc_entry.h"
#include "r_hs300x_if.h"
#include "r_comms_i2c_if.h"
#include "r_comms_i2c_rx_config.h"

#if COMMS_I2C_CFG_DRIVER_I2C
#include "r_riic_rx_if.h"
#endif
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
#include "r_sci_iic_rx_if.h"
#endif
//#include "assert.h"
#include "sensorsData.h"
#include <demo_printf.h>
/* Utility MACRO to loop 50ms delays until expression becomes false*/
//#define WAIT_WHILE_FALSE(e) while(!e) { __asm(" nop"); }
#define WAIT_WHILE_FALSE(e) while(!e) { nop(); }
///* Utility struct for sensor data*/
//typedef struct
//{
//	float       gs_demo_humidity; /* Global variable to hold humidity reading*/
//	float       gs_demo_temperature; /* Global variable to hold temperature reading*/
//}

/* Variable declarations*/
static volatile fsp_err_t   g_err;    /* FSP Error variable*/
static volatile sensor_demo_data_t g_demo_data; /* Demo data struct*/

/* Quick setup for g_hs300x_sensor0. */
void g_hs300x_sensor0_quick_setup(void)
{
	fsp_err_t err;
	/* Open HS300X sensor instance, this must be done before calling any HS300X API */
	err = g_hs300x_sensor0.p_api->open(g_hs300x_sensor0.p_ctrl, g_hs300x_sensor0.p_cfg);
	if (FSP_SUCCESS == err)
	{
		//demo_printf(("HS3001 open sensor instance successful: %d \r\n", err));
	}
	else
	{
		//demo_printf(("HS3001 open sensor instance failed: %d \r\n", err));
	}

}

volatile bool g_hs300x_completed = false;

/* Function used to read the hs3001 sensor*/
void hs3001_get(void)
{
	rm_hs300x_raw_data_t TH_rawdata;
	rm_hs300x_data_t hs300x_data;

	/* start measurement */
	g_hs300x_completed = false;
	g_err = g_hs300x_sensor0.p_api->measurementStart(g_hs300x_sensor0.p_ctrl);
	if(g_err!=FSP_SUCCESS)
	{
		printf("HS-Measurement Failed: %d\r\n",g_err);
	}
	//assert(FSP_SUCCESS == g_err);
	WAIT_WHILE_FALSE(g_hs300x_completed);

	/* Keep attempting to read the data until it stabilises.
	 * Section 6.6 of the HS3001 datasheet stipulates a range of
	 * measurement times with corresponding to the range of
	 * sensor resolutions. */
	do
	{
		/* read the data */
		g_hs300x_completed = false;
		g_err = g_hs300x_sensor0.p_api->read(g_hs300x_sensor0.p_ctrl,&TH_rawdata);
		//assert(FSP_SUCCESS == g_err);

		WAIT_WHILE_FALSE(g_hs300x_completed);

		/* Calculate results */
		g_err = g_hs300x_sensor0.p_api->dataCalculate(g_hs300x_sensor0.p_ctrl, &TH_rawdata, &hs300x_data);
		//assert((FSP_SUCCESS == g_err) || (g_err == FSP_ERR_SENSOR_INVALID_DATA));
	}
	while(g_err == FSP_ERR_SENSOR_INVALID_DATA);

	g_demo_data.gs_demo_humidity     = (float)hs300x_data.humidity.integer_part + (float)hs300x_data.humidity.decimal_part * 0.01F;
	g_demo_data.gs_demo_temperature  = (float)hs300x_data.temperature.integer_part + (float)hs300x_data.temperature.decimal_part * 0.01F;

	g_demo_data.gs_demo_temperature = (g_demo_data.gs_demo_temperature * (float)1.8)+(float)32.0; //Deg C to Deg F

#if ENABLE_SENSORS_SERIAL_PRINTS
	demo_printf( "\r\nHS3001-Humidity   : %f \r\n",g_demo_data.gs_demo_humidity);
	demo_printf( "HS3001-Temperature: %f \r\n",g_demo_data.gs_demo_temperature);
#endif


	tx_thread_sleep (30);
	//R_BSP_SoftwareDelay(40, BSP_DELAY_MILLISECS);
}
/* END OF FUNCTION*/

void hs300x_callback(rm_hs300x_callback_args_t * p_args)
{
	if (RM_HS300X_EVENT_SUCCESS == p_args->event)
	{
		g_hs300x_completed = true;
	}
	FSP_PARAMETER_NOT_USED(p_args);

}

void getHS3001Data(st_sensorsHS_t *hsData)
{
	hsData->humidity = g_demo_data.gs_demo_humidity;
	hsData->temperature =g_demo_data.gs_demo_temperature;
}




void g_comms_i2c_bus0_quick_setup(void)
{

    i2c_master_instance_t * p_driver_instance = (i2c_master_instance_t *) g_comms_i2c_bus0_extended_cfg.p_driver_instance;
    /* Open I2C driver, this must be done before calling any COMMS API */
	if (COMMS_DRIVER_I2C == p_driver_instance->driver_type)
	{
		riic_return_t ret;
		riic_info_t *p_i2c_info = (riic_info_t*) p_driver_instance->p_info;
		p_i2c_info->ch_no = (uint8_t) p_driver_instance->driver_channel;
		ret = R_RIIC_Open(p_i2c_info);
		if (RIIC_SUCCESS != ret)
		{
			//demo_err();
			//demo_printf("\r\nError in opening I2C bus:%d \r\n", ret);
		}
		else
		{
			//demo_printf("\r\nI2C bus setup success:%d \r\n", ret);
		}
	}

#if 0
    /* Create a semaphore for blocking if a semaphore is not NULL */
        if (NULL != g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore)
        {
            *(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle)
            	= xSemaphoreCreateCounting((UBaseType_t) 1, (UBaseType_t) 0);
        }

        /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
        if (NULL != g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex)
        {
            *(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle)
            	= xSemaphoreCreateRecursiveMutex();
        }
#endif


#if BSP_CFG_RTOS
    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore)
    {
 #if BSP_CFG_RTOS == 1                 // AzureOS
        tx_semaphore_create(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle,
        		g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_name,
                            (ULONG) 0);
 #elif BSP_CFG_RTOS == 2               // FreeRTOS
        *(p_extend->p_blocking_semaphore->p_semaphore_handle) =
            xSemaphoreCreateCountingStatic((UBaseType_t) 1,
                                           (UBaseType_t) 0,
                                           p_extend->p_blocking_semaphore->p_semaphore_memory);
 #endif
    }
    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex)
    {
 #if BSP_CFG_RTOS == 1                 // AzureOS
        tx_mutex_create(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle,
        		g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_name,
                        TX_INHERIT);
 #elif BSP_CFG_RTOS == 2               // FreeRTOS
        *(p_extend->p_bus_recursive_mutex->p_mutex_handle) =
            xSemaphoreCreateRecursiveMutexStatic(p_extend->p_bus_recursive_mutex->p_mutex_memory);
 #endif
    }
#endif
}
