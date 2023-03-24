//#include "hal_data.h"

//#include "common_utils.h"
#include "r_zmod4xxx_if.h"
#include "r_comms_i2c_if.h"
#include "stdio.h"
#include "RX_ZMOD4XXX_Common.h"
#if COMMS_I2C_CFG_DRIVER_I2C
#include "r_riic_rx_if.h"
#endif
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
#include "r_sci_iic_rx_if.h"
#endif
#include "../sensorsData.h"
#include <demo_printf.h>
void start_oaq_1st_gen_demo(void);
static zmod4510_demo_sequence_t sequence_no = ZMOD4510_DEMO_SEQUENCE_1;
static volatile float gs_demo_air_quality;

static rm_zmod4xxx_raw_data_t raw_data;
void start_oaq_1st_gen_demo(void)
{
    fsp_err_t err;
   // rm_zmod4xxx_raw_data_t raw_data;
    rm_zmod4xxx_oaq_1st_data_t zmod4xxx_data;
   // zmod4410sens->zmod410_oaq=0;
    switch(sequence_no)
    {
        case ZMOD4510_DEMO_SEQUENCE_1 :
        {
            /* Clear status */
            g_i2c_callback_status1 = ZMOD4510_DEMO_CALLBACK_STATUS_WAIT;
#if DEMO_IRQ_ENABLE
            g_irq_callback_status1 = ZMOD4510_DEMO_CALLBACK_STATUS_WAIT;
#endif

            /* Start measurement */
            err = g_zmod4xxx_sensor1.p_api->measurementStart(g_zmod4xxx_sensor1.p_ctrl);
            if (FSP_SUCCESS == err)
            {
            	sequence_no = ZMOD4510_DEMO_SEQUENCE_2;
               // printf("\r\nTask zmod4510 measurement Success:%d\r\n",err);
            }
            else
            {
            	demo_printf ("Task zmod4510 task delete in seq 1:%d\r\n",err);
            }
        }
        break;

        case ZMOD4510_DEMO_SEQUENCE_2 :
        {
            /* Check I2C callback status */

            switch (g_i2c_callback_status1)
            {
                case ZMOD4510_DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case ZMOD4510_DEMO_CALLBACK_STATUS_SUCCESS :
                	sequence_no= ZMOD4510_DEMO_SEQUENCE_3;
                    break;
                case ZMOD4510_DEMO_CALLBACK_STATUS_REPEAT :
                	sequence_no = ZMOD4510_DEMO_SEQUENCE_1;
                    break;
                default :
                	printf ("Task zmod4510 task delete in seq 2:%d\r\n",err);
                    break;
            }
            /* Wait 50ms */
            //R_BSP_SoftwareDelay(50,BSP_DELAY_MILLISECS);
            tx_thread_sleep(10);
        }
        break;

#if DEMO_IRQ_ENABLE
        case ZMOD4510_DEMO_SEQUENCE_3 :
        {
            /* Check IRQ callback status */
            switch (g_irq_callback_status1)
            {
                case ZMOD4510_DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case ZMOD4510_DEMO_CALLBACK_STATUS_SUCCESS :
                	sequence_no = ZMOD4510_DEMO_SEQUENCE_5;
                    break;
                default :
                    break;
            }
        }
        break;
#else
        case DEMO_SEQUENCE_3 :
        {
            /* Clear status */
            g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_WAIT;

            /* Get status */
            err = g_zmod4xxx_sensor1.p_api->statusCheck(g_zmod4xxx_sensor1.p_ctrl);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_4;
            }
            else
            {
              //  demo_err();
            }
        }
        break;

        case DEMO_SEQUENCE_4 :
        {
            /* Check I2C callback status */
            switch (g_i2c_callback_status1)
            {
                case DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case DEMO_CALLBACK_STATUS_SUCCESS :
                    sequence = DEMO_SEQUENCE_5;
                    break;
                case DEMO_CALLBACK_STATUS_REPEAT :
                    sequence = DEMO_SEQUENCE_3;
                    break;
                default :
                //    demo_err();
                    break;
            }

            /* Wait 50ms */
            vTaskDelay(50);
        }
        break;
#endif
        case ZMOD4510_DEMO_SEQUENCE_5 :
        {
            /* Clear status */
            g_i2c_callback_status1 = ZMOD4510_DEMO_CALLBACK_STATUS_WAIT;

            /* Read data */
            err = g_zmod4xxx_sensor1.p_api->read(g_zmod4xxx_sensor1.p_ctrl, &raw_data);
            if (FSP_SUCCESS == err)
            {
               // printf ("\r\nTask zmod4510 read Successful:%d\r\n",err);
            	sequence_no = ZMOD4510_DEMO_SEQUENCE_6;
            }
            else if (FSP_ERR_SENSOR_MEASUREMENT_NOT_FINISHED == err)
            {
            	sequence_no = ZMOD4510_DEMO_SEQUENCE_3;

                /* Delay 50ms */
            	//R_BSP_SoftwareDelay(50,BSP_DELAY_MILLISECS);
            }
            else
            {
            	printf ("Task zmod4510 task delete in seq 5:%d\r\n",err);
            }
        }
        break;

        case ZMOD4510_DEMO_SEQUENCE_6 :
        {
            /* Check I2C callback status */

            switch (g_i2c_callback_status1)
            {
                case ZMOD4510_DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case ZMOD4510_DEMO_CALLBACK_STATUS_SUCCESS :
                	sequence_no = ZMOD4510_DEMO_SEQUENCE_7;
                    break;
                case ZMOD4510_DEMO_CALLBACK_STATUS_REPEAT :
                	sequence_no = ZMOD4510_DEMO_SEQUENCE_5;
                    break;
                default :
                	demo_printf ("Task zmod4510 task delete in seq 6:%d\r\n",err);
                    break;
            }

            /* Wait 50ms */
           // R_BSP_SoftwareDelay(50,BSP_DELAY_MILLISECS);
        }
        break;

        case ZMOD4510_DEMO_SEQUENCE_7 :
        {
            /* Calculate data */
            err = g_zmod4xxx_sensor1.p_api->oaq1stGenDataCalculate(g_zmod4xxx_sensor1.p_ctrl, &raw_data, &zmod4xxx_data);
            if (FSP_SUCCESS == err)
            {
            	sequence_no = ZMOD4510_DEMO_SEQUENCE_1;

                /* Set data */
                gs_demo_air_quality     = zmod4xxx_data.aiq;


#if ENABLE_SENSORS_SERIAL_PRINTS
                demo_printf(  "\r\nZMOD4510-OAQ  : %f \r\n",gs_demo_air_quality );
#endif

            }
            else if (FSP_ERR_SENSOR_IN_STABILIZATION == err)
            {
            	//demo_printf ("ZMOD4510 in stabilization:%d\r\n",err);
                sequence_no = ZMOD4510_DEMO_SEQUENCE_1;
            }
            else
            {
            	demo_printf ("Task zmod4510 task delete in seq 7:%d\r\n",err);
            }

            /* Delay required time */
           //  vTaskDelay(RM_ZMOD4XXX_ALG_REQ_DELAY_IN_MS);
        }
        break;

        default :
        {
        	demo_printf ("Task zmod4410 task delete in default case:%d\r\n",err);
        }
        break;
    }
}

void getZMOD4510Data(st_sensorsZmod4510_t *zmod4510Data)
{
	zmod4510Data->zmod410_oaq =gs_demo_air_quality;
}
