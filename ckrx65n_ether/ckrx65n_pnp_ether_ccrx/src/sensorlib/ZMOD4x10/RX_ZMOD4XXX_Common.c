//#include "hal_data.h"
#include "RX_HS3001.h"
//#include "common_utils.h"
#include "r_zmod4xxx_if.h"
#include "r_comms_i2c_if.h"

#include "RX_ZMOD4XXX_Common.h"
#if COMMS_I2C_CFG_DRIVER_I2C
#include "r_riic_rx_if.h"
#endif
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
#include "r_sci_iic_rx_if.h"
#endif
#include <demo_printf.h>

volatile zmod4410_demo_callback_status_t  g_i2c_callback_status;
volatile zmod4410_demo_callback_status_t  g_irq_callback_status;
volatile zmod4510_demo_callback_status_t  g_i2c_callback_status1;
volatile zmod4510_demo_callback_status_t  g_irq_callback_status1;

/* Quick setup for g_zmod4xxx_sensor0. */
void g_zmod4xxx_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Open ZMOD4XXX sensor instance, this must be done before calling any ZMOD4XXX API */
    err = g_zmod4xxx_sensor0.p_api->open(g_zmod4xxx_sensor0.p_ctrl, g_zmod4xxx_sensor0.p_cfg);
	if (FSP_SUCCESS == err)
	{
		demo_printf("\r\nZMOD4410 open sensor instance successful: %d\r\n", err);
	}
	else
	{
		demo_printf("\r\nZMOD4410 open sensor instance failed: %d \r\n", err);
	}

    /* Set the pin for IRQ */
//   #if RM_ZMOD4XXX_CFG_DEVICE0_IRQ_ENABLE
//       R_ICU_PinSet();
//   #endif
}

/* Quick setup for g_zmod4xxx_sensor0. */
void g_zmod4xxx_sensor1_quick_setup(void)
{
    fsp_err_t err;

    /* Open ZMOD4XXX sensor instance, this must be done before calling any ZMOD4XXX API */
    err = g_zmod4xxx_sensor1.p_api->open(g_zmod4xxx_sensor1.p_ctrl, g_zmod4xxx_sensor1.p_cfg);
	if (FSP_SUCCESS == err)
	{
		demo_printf("\r\nZMOD4510 open sensor instance successful: %d\r\n", err);
	}
	else
	{
		demo_printf("\r\nZMOD4510 open sensor instance successful: %d\r\n", err);
	}
}

void zmod4xxx_user_i2c_callback0(rm_zmod4xxx_callback_args_t * p_args)
{
    if (RM_ZMOD4XXX_EVENT_ERROR != p_args->event)
    {
    	g_i2c_callback_status = ZMOD4410_DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else
    {
    	g_i2c_callback_status = ZMOD4410_DEMO_CALLBACK_STATUS_REPEAT;
    }
   // xSemaphoreGiveFromISR (g_allsensors_semaphore, &xHigherPriorityTaskWoken1);
}

void zmod4xxx_user_i2c_callback1(rm_zmod4xxx_callback_args_t * p_args)
{
    if (RM_ZMOD4XXX_EVENT_ERROR != p_args->event)
    {
    	g_i2c_callback_status1 = ZMOD4510_DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else
    {
    	g_i2c_callback_status1 = ZMOD4510_DEMO_CALLBACK_STATUS_REPEAT;
    }
   // xSemaphoreGiveFromISR (g_allsensors_semaphore, &xHigherPriorityTaskWoken2);
}


void zmod4xxx_user_irq_callback0(rm_zmod4xxx_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    g_irq_callback_status = ZMOD4410_DEMO_CALLBACK_STATUS_SUCCESS;

}

void zmod4xxx_user_irq_callback1(rm_zmod4xxx_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    g_irq_callback_status1 = ZMOD4510_DEMO_CALLBACK_STATUS_SUCCESS;

}


