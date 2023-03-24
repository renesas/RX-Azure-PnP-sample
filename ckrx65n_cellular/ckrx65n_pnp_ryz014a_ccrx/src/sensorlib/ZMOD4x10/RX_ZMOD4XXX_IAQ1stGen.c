//#include "hal_data.h"

#include "r_zmod4xxx_if.h"
#include "r_comms_i2c_if.h"

#include "RX_ZMOD4XXX_Common.h"
#if COMMS_I2C_CFG_DRIVER_I2C
#include "r_riic_rx_if.h"
#endif
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
#include "r_sci_iic_rx_if.h"
#endif
#include "rm_zmod4xxx.h"
#include "../sensorsData.h"
#include <demo_printf.h>
static volatile float gs_demo_iaq=0.00;
static volatile float gs_demo_eco2=0.00;
static volatile float gs_demo_tvoc=0.00;
static volatile float gs_demo_etoh =0.00;
zmod4410_demo_sequence_t sequence = ZMOD4410_DEMO_SEQUENCE_1;
//TickType_t Semphr_wait_ticks0 = pdMS_TO_TICKS ( 500 );//MS
static rm_zmod4xxx_raw_data_t raw_data;
void start_iaq_1st_gen_demo(void)
{

	//demo_sequence_t sequence = DEMO_SEQUENCE_1;
    fsp_err_t err;
//    rm_zmod4xxx_raw_data_t raw_data;
    rm_zmod4xxx_iaq_1st_data_t zmod4xxx_data;
    switch(sequence)
    {
        case ZMOD4410_DEMO_SEQUENCE_1 :
        {
            /* Clear status */
            g_i2c_callback_status = ZMOD4410_DEMO_CALLBACK_STATUS_WAIT;
#if DEMO_IRQ_ENABLE
            g_irq_callback_status = ZMOD4410_DEMO_CALLBACK_STATUS_WAIT;
#endif

            /* Start measurement */
            err = g_zmod4xxx_sensor0.p_api->measurementStart(g_zmod4xxx_sensor0.p_ctrl);
            if (FSP_SUCCESS == err)
            {
                sequence = ZMOD4410_DEMO_SEQUENCE_2;
               // demo_printf ("Task zmod4410 measurement Success:%d\r\n",err);
            }
            else
            {
            	demo_printf ("Task zmod4410 task delete in seq 1:%d\r\n",err);
            }

        }
        break;

        case ZMOD4410_DEMO_SEQUENCE_2 :
        {
            /* Check I2C callback status */
//        if( pdPASS==xSemaphoreTake( g_allsensors_semaphore , Semphr_wait_ticks0 ))
//        {
            switch (g_i2c_callback_status)
            {
                case ZMOD4410_DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case ZMOD4410_DEMO_CALLBACK_STATUS_SUCCESS :
                    sequence = ZMOD4410_DEMO_SEQUENCE_3;
                    break;
                case ZMOD4410_DEMO_CALLBACK_STATUS_REPEAT :
                    sequence = ZMOD4410_DEMO_SEQUENCE_1;
                    break;
                default :
                	printf ("Task zmod4410 task delete in seq 2:%d\r\n",err);
                    break;
            }
        	// }

            /* Wait 50ms */
          //  vTaskDelay(50);
        }
        break;

#if DEMO_IRQ_ENABLE
        case ZMOD4410_DEMO_SEQUENCE_3 :
        {
            /* Check IRQ callback status */
            switch (g_irq_callback_status)
            {
                case ZMOD4410_DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case ZMOD4410_DEMO_CALLBACK_STATUS_SUCCESS :
                    sequence = ZMOD4410_DEMO_SEQUENCE_5;
                    break;
                default :
                    break;
            }
            /* Wait 50ms */
          //  vTaskDelay(50);
        }
        break;
#else
        case DEMO_SEQUENCE_3 :
        {
            /* Clear status */
            g_i2c_callback_status = DEMO_CALLBACK_STATUS_WAIT;

            /* Get status */
            err = g_zmod4xxx_sensor0.p_api->statusCheck(g_zmod4xxx_sensor0.p_ctrl);
            if (FSP_SUCCESS == err)
            {
                sequence = DEMO_SEQUENCE_4;
                APP_PRINT ("\r\nTask zmod4410 status Successful:%d\r\n",err);
            }
            else
            {
                APP_PRINT ("\r\nTask zmod4410 task delete in seq 3:%d\r\n",err);
                vTaskDelete(zmod4410_thread);
            }
        }
        break;

        case DEMO_SEQUENCE_4 :
        {
            /* Check I2C callback status */
            switch (g_i2c_callback_status)
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
                    APP_PRINT ("\r\nTask zmod4410 task delete in seq 4:%d\r\n",err);
                    vTaskDelete(zmod4410_thread);
                    break;
            }

            /* Wait 50ms */
            vTaskDelay(50);
        }
        break;
#endif
        case ZMOD4410_DEMO_SEQUENCE_5 :
        {
            /* Clear status */
            g_i2c_callback_status = ZMOD4410_DEMO_CALLBACK_STATUS_WAIT;

            /* Read data */
            err = g_zmod4xxx_sensor0.p_api->read(g_zmod4xxx_sensor0.p_ctrl, &raw_data);
            if (FSP_SUCCESS == err)
            {
                sequence = ZMOD4410_DEMO_SEQUENCE_6;
                //printf ("\r\nTask zmod4410 read Successful:%d\r\n",err);
            }
            else if (FSP_ERR_SENSOR_MEASUREMENT_NOT_FINISHED == err)
            {
                 sequence = ZMOD4410_DEMO_SEQUENCE_3;
               //  printf ("\r\nTask zmod4410 measurement not finished 5:%d\r\n",err);
                /* Delay 50ms */
              // tx_thread_sleep(10);
             //  R_BSP_SoftwareDelay(50, BSP_DELAY_MILLISECS);
            }
            else
            {
            	printf ("Task zmod4410 task delete in seq 5:%d\r\n",err);
            }
        }
        break;

        case ZMOD4410_DEMO_SEQUENCE_6 :
        {
            /* Check I2C callback status */
//        if( pdPASS==xSemaphoreTake( g_allsensors_semaphore , Semphr_wait_ticks0 ))
//        	{
            switch (g_i2c_callback_status)
            {
                case ZMOD4410_DEMO_CALLBACK_STATUS_WAIT :
                    break;
                case ZMOD4410_DEMO_CALLBACK_STATUS_SUCCESS :
                    sequence = ZMOD4410_DEMO_SEQUENCE_7;
                    break;
                case ZMOD4410_DEMO_CALLBACK_STATUS_REPEAT :
                    sequence = ZMOD4410_DEMO_SEQUENCE_5;
                    break;
                default :
                	printf ("Task zmod4410 task delete in seq 6:%d\r\n",err);
                    break;
            }
        	//}

            /* Wait 50ms */
           // vTaskDelay(50);
        }
        break;

        case ZMOD4410_DEMO_SEQUENCE_7 :
        {
            /* Calculate data */
            err = g_zmod4xxx_sensor0.p_api->iaq1stGenDataCalculate(g_zmod4xxx_sensor0.p_ctrl, &raw_data, &zmod4xxx_data);
            if (FSP_SUCCESS == err)
            {
#if RM_ZMOD4XXX_CFG_OPERATION_MODE == 1
                sequence = DEMO_SEQUENCE_3;
#if DEMO_IRQ_ENABLE
                g_irq_callback_status = DEMO_CALLBACK_STATUS_WAIT;
#endif
#elif RM_ZMOD4XXX_CFG_OPERATION_MODE == 2
                sequence = DEMO_SEQUENCE_1;
#endif

                /* Set data */
                gs_demo_iaq     = zmod4xxx_data.iaq;
                gs_demo_eco2    = zmod4xxx_data.eco2;
                gs_demo_tvoc    = zmod4xxx_data.tvoc;
                gs_demo_etoh    = zmod4xxx_data.etoh;

#if ENABLE_SENSORS_SERIAL_PRINTS
          demo_printf(  "\r\nZMOD4410-IAQ : %f \r\n",gs_demo_iaq);
         // demo_printf(  "ZMOD4410-ECO2: %f \r\n",gs_demo_eco2 );
          demo_printf(  "ZMOD4410-TVOC: %f \r\n",gs_demo_tvoc );
#endif

            }
            else if (FSP_ERR_SENSOR_IN_STABILIZATION == err)
            {
#if RM_ZMOD4XXX_CFG_OPERATION_MODE == 1
                sequence = DEMO_SEQUENCE_3;
                APP_PRINT ("\r\nTask zmod4410 in stabilization:%d\r\n",err);
#if DEMO_IRQ_ENABLE
                g_irq_callback_status = DEMO_CALLBACK_STATUS_WAIT;
#endif
#elif RM_ZMOD4XXX_CFG_OPERATION_MODE == 2
                sequence = DEMO_SEQUENCE_1;
#endif
                //demo_printf ("ZMOD4410 in stabilization:%d\r\n",err);

            }
            else
            {
            	demo_printf ("Task zmod4410 task delete in seq 7:%d\r\n",err);
            }

            /* Delay required time */
           // vTaskDelay(RM_ZMOD4XXX_ALG_REQ_DELAY_IN_MS);
           // tx_thread_sleep(1);
            sequence = ZMOD4410_DEMO_SEQUENCE_3;
        }
        break;

        default :
        {
        	demo_printf ("Task zmod4410 task delete in default case:%d\r\n",err);
        }
        break;
    }
}

void getZMOD4410Data(st_sensorsZmod4410_t *zmod4410Data)
{
	zmod4410Data->zmod4410Data.iaq=gs_demo_iaq;
	zmod4410Data->zmod4410Data.ec02 = 0;//gs_demo_eco2;
	zmod4410Data->zmod4410Data.tvoc=gs_demo_tvoc;
	zmod4410Data->zmod4410Data.etoh=gs_demo_etoh;
}

void zmod4410_thread(void *pvParameters)
{
	R_BSP_SoftwareDelay(2000,BSP_DELAY_MILLISECS);
	g_zmod4xxx_sensor0_quick_setup();
   while(1)
   {
	   start_iaq_1st_gen_demo();
	   R_BSP_SoftwareDelay(50, BSP_DELAY_MILLISECS);
	  //  tx_thread_sleep (50);
   }
}
