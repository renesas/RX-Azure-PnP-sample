/*
/*
 * RmcI2C.c
 *
 *  Created on: Jan 20, 2022
 *      Author: Vivek
 */

#include "icm_i2c.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
//#include "common_utils.h"
#include "r_smc_entry.h"
#include "r_cmt_rx_if.h"
#include "r_comms_i2c_if.h"
#include "r_comms_i2c_rx_config.h"
#if COMMS_I2C_CFG_DRIVER_I2C
#include "r_riic_rx_if.h"
#endif
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
#include "r_sci_iic_rx_if.h"
#endif
#include "sensorsData.h"
#include <demo_printf.h>

#define I2C_TRANSMISSION_IN_PROGRESS_ICM        0
#define I2C_TRANSMISSION_COMPLETE_ICM           1
#define I2C_TRANSMISSION_ABORTED_ICM            2


static uint8_t transmit_complete_flag_icm;




void RmComDevice_init_Icm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err=g_comms_i2c_device6.p_api->open(g_comms_i2c_device6.p_ctrl,g_comms_i2c_device6.p_cfg);
	if (FSP_SUCCESS == err)
	{
		demo_printf("ICM20948 open sensor instance successful: %d \r\n", err);
	}
	else
	{
		demo_printf("ICM20948 open sensor instance failed: %d \n\n", err);
	}
}

int8_t RmCom_I2C_r_icm(uint8_t reg, uint8_t *val, char num)
{

	uint16_t timeout = 1000;
	    fsp_err_t err;
	    transmit_complete_flag_icm = I2C_TRANSMISSION_IN_PROGRESS_ICM;
	    rm_comms_write_read_params_t write_read_params;
	    write_read_params.p_src=&reg;
	    write_read_params.src_bytes  = 1;
	    write_read_params.p_dest     = val;
	    write_read_params.dest_bytes = (uint8_t)num;
	    err=g_comms_i2c_device6.p_api->writeRead(g_comms_i2c_device6.p_ctrl,write_read_params);
	    if (err == FSP_SUCCESS)
	    {
	        while (transmit_complete_flag_icm == I2C_TRANSMISSION_IN_PROGRESS_ICM)
	        {
	            if (--timeout == 0)
	            {
	                break;
	            }
	            delayMicrosecondsicm(1);
	        }

	        transmit_complete_flag_icm = I2C_TRANSMISSION_IN_PROGRESS_ICM;
	    }

	    if (transmit_complete_flag_icm == I2C_TRANSMISSION_ABORTED_ICM) {
	        err = FSP_ERR_ABORTED;
	    }
	    return err;

}

int8_t RmCom_I2C_w_icm(uint8_t reg, uint8_t *val, char num)
{

	uint16_t timeout = 1000;
	    int8_t err;
	    static uint8_t data[50];
	    data[0] = reg;
	    memcpy(&(data[1]), val, (size_t)num);
	    transmit_complete_flag_icm = I2C_TRANSMISSION_IN_PROGRESS_ICM;
	    err=g_comms_i2c_device6.p_api->write(g_comms_i2c_device6.p_ctrl,data, (uint32_t)(num+1));
	    if (err == FSP_SUCCESS)
	    {
	//        APP_PRINT("** OB1203 Sensor I2C Write Successful ** %u \r\n",err);
	        while (transmit_complete_flag_icm == I2C_TRANSMISSION_IN_PROGRESS_ICM)
	        {
	            if (--timeout == 0)
	            {
	                break;
	            }
	            delayMicrosecondsicm(1);
	        }

	        transmit_complete_flag_icm = I2C_TRANSMISSION_IN_PROGRESS_ICM;
	    }

	    if (transmit_complete_flag_icm == I2C_TRANSMISSION_ABORTED_ICM) {
	        err = FSP_ERR_ABORTED;
	    }

	    return err;

}

void comms_i2c_callback_icm(rm_comms_callback_args_t *p_args)
{
    if (p_args->event == RM_COMMS_EVENT_OPERATION_COMPLETE)
    {
    	transmit_complete_flag_icm = I2C_TRANSMISSION_COMPLETE_ICM;
    }
    else {
    	transmit_complete_flag_icm = I2C_TRANSMISSION_ABORTED_ICM;
    }

}

void delayMicrosecondsicm(uint32_t us)
{
    R_BSP_SoftwareDelay(us, BSP_DELAY_MICROSECS);
}

void delayicm(uint32_t ms)
{
    R_BSP_SoftwareDelay(ms, BSP_DELAY_MILLISECS);

}
