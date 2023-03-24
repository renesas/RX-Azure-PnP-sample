#include "r_comms_i2c_if.h"
#if COMMS_I2C_CFG_DRIVER_I2C
#include "r_riic_rx_if.h"
#endif
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
#include "r_sci_iic_rx_if.h"
#endif
#include "r_ob1203_if.h"
#include "ob1203_bio.h"
#include "stdint.h"
#include "string.h"
//#include "serial_term_uart.h"
#include "sensorsData.h"
#include <demo_printf.h>
uint8_t tx_buffer[60];
float gpi;
typedef enum e_demo_sequence
{
    OB1203_DEMO_SEQUENCE_1 = (1),
	OB1203_DEMO_SEQUENCE_2,
	OB1203_DEMO_SEQUENCE_3,
	OB1203_DEMO_SEQUENCE_4,
	OB1203_DEMO_SEQUENCE_5,
	OB1203_DEMO_SEQUENCE_6,
	OB1203_DEMO_SEQUENCE_7,
	OB1203_DEMO_SEQUENCE_8,
	OB1203_DEMO_SEQUENCE_9,
	OB1203_DEMO_SEQUENCE_10,
	OB1203_DEMO_SEQUENCE_11,
	OB1203_DEMO_SEQUENCE_12,
} demo_sequence_t;



void ob1203_user_i2c_callback0(rm_ob1203_callback_args_t * p_args);
void ob1203_irq_callback(rm_ob1203_callback_args_t *p_args);
void demo_err(void);


void g_comms_i2c_bus0_quick_setup(void);
void start_demo(void);
void demo_err(void);
//TaskHandle_t	                  ob1203_sensor_thread;
static spo2_t gs_spo2;
static volatile ob1203_bio_data_t gs_ob1203_bio_data;

void start_ob1203_demo(void);

void start_ob1203_demo(void)
{
	R_BSP_SoftwareDelay(2000,BSP_DELAY_MILLISECS);
	bool result;
	rm_ob1203_raw_data_t raw_data;
	rm_ob1203_ppg_data_t ppg_data;
	ob1203_bio_t ob1203_bio;
	bool change = false;
	bool valid = false;
	bool update = false;
	bool ready = false;
	ob1203_bio_gain_currents_t gain_currents;

 /* Set the current thread */
	//ob1203_sensor_thread = xTaskGetCurrentTaskHandle();
	/* Set default gain and currents */
	gain_currents.gain.ppg_prox = g_ob1203_sensor1_extended_cfg.ppg_prox_gain;
	gain_currents.currents.ir_led = g_ob1203_sensor1_extended_cfg.ppg_ir_led_current;
	gain_currents.currents.red_led = g_ob1203_sensor1_extended_cfg.ppg_red_led_current;

	    /* Open OB1203 Bio extension */
	    result = ob1203_bio_open(&ob1203_bio,
	                             (rm_ob1203_instance_t*)&g_ob1203_sensor0, // Proximity mode
	                             (rm_ob1203_instance_t*)&g_ob1203_sensor1,  // PPG mode
	                             &gs_spo2);

	    if (false == result)
	    {
	    	demo_printf("\r\nOB1203 Device open failed\r\n");
	        demo_err();
	    }
	    else
	    {
	    	demo_printf("\r\nOB1203 Device open success\r\n");
	    }

	    demo_sequence_t obsequence = OB1203_DEMO_SEQUENCE_1;

  while(1)
    {


        switch (obsequence)
        {
            case OB1203_DEMO_SEQUENCE_1 :
            {
                /* Initialize an operation mode */
                result = ob1203_bio_operation_mode_init(&ob1203_bio);

                if (false == result)
                {
                	demo_printf("OB operation mode init Failed in Seq-1\r\n");
                    demo_err();
                }

                obsequence = OB1203_DEMO_SEQUENCE_2;
            }
            break;

            case OB1203_DEMO_SEQUENCE_2 :
            {
            	            gs_ob1203_bio_data.heart_rate =0;
            			    gs_ob1203_bio_data.respiration_rate =0;
            			    gs_ob1203_bio_data.spo2=0;
            			    gs_ob1203_bio_data.perfusion_index=0.00;
                /* Start a measurement */
                result = ob1203_bio_measurement_start(&ob1203_bio);
                if (false == result)
                {
                	demo_printf("\r\n Bio Measurement start Failed in Seq-2\r\n");
                    demo_err();
                }

                obsequence = OB1203_DEMO_SEQUENCE_3;
            }
            break;

            case OB1203_DEMO_SEQUENCE_3:
            {
                /* Wait measurement period */
            	/* Wait measurement period */
//			result = ob1203_bio_measurement_period_wait(&ob1203_bio);
//
//			if (false == result)
//			{
//				configPRINTF(("\r\n Bio Measurement wait Failed in Seq-3"));
//				demo_err();
//			}
//
//			obsequence = OB1203_DEMO_SEQUENCE_4;
            	result=	ob1203_bio_measurement_period_wait(&ob1203_bio);
            	if(result==true)
            	{
            		obsequence = OB1203_DEMO_SEQUENCE_4;
            	}
            	else
            	{
            		obsequence = OB1203_DEMO_SEQUENCE_3;
            	}
            	  R_BSP_SoftwareDelay(100, BSP_DELAY_MICROSECS);
            }
            break;

            case OB1203_DEMO_SEQUENCE_4 :
            {
                /* Check if an operation mode needs to be changed */
                result = ob1203_bio_mode_change_check(&ob1203_bio, &change);
                if (false == result)
                {
                	demo_printf("\r\n Bio mode change Failed in Seq-4\r\n");
                    demo_err();
                }
                if (false != change)
                {
                    /* Stop the measurement */
                	result = ob1203_bio_measurement_stop(&ob1203_bio);
                    if (false == result)
                    {
                    	demo_printf("\r\n Measurement stop Failed in Seq\r\n");
                        demo_err();
                    }

                    /* Change to another mode */
                    obsequence = OB1203_DEMO_SEQUENCE_1;
                }
                else
                {
                    /* No change */
                    obsequence = OB1203_DEMO_SEQUENCE_5;
                }
            }
            break;

            case OB1203_DEMO_SEQUENCE_5:
            {
                /* Read raw data */
                result = ob1203_bio_ppg_raw_data_read(&ob1203_bio, &raw_data);
                if (false == result)
                {
                	demo_printf("\r\n raw data read Failed in Seq-5\r\n");
                    demo_err();
                }

                obsequence = OB1203_DEMO_SEQUENCE_6;
            }
            break;

            case OB1203_DEMO_SEQUENCE_6:
            {
                /* Calculate PPG data from raw data */
                result = ob1203_bio_ppg_data_calculate(&ob1203_bio, &raw_data, &ppg_data, &valid);

                if (false == result)
                {
                	demo_printf("\r\n Data cal Failed in Seq-6\r\n");
                    demo_err();
                }

                if (false != valid)
                {
                    /* Valid data */
                	obsequence = OB1203_DEMO_SEQUENCE_7;
                }
                else
                {
                    /* Check if an operation mode needs to be changed */
                    result = ob1203_bio_mode_change_check(&ob1203_bio, &change);

                    if (false == result)
                    {
                    	demo_printf("\r\n Bio mode change failed in Seq-6\r\n");
                        demo_err();
                    }

                    if (false != change)
                    {
                        /* Stop the measurement */
                        result = ob1203_bio_measurement_stop(&ob1203_bio);
                        if (false == result)
                        {
                        	demo_printf("\r\n Measurement stop Failed in Seq-6\r\n");
                            demo_err();
                        }

                        /* Change to another mode */
                        obsequence = OB1203_DEMO_SEQUENCE_1;
                    }
                    else
                    {
                        /* Invalid data */
                    	obsequence = OB1203_DEMO_SEQUENCE_3;
                    }
                }
            }
            break;

            case OB1203_DEMO_SEQUENCE_7:
            {
                /* Auto gain and currents control */

                result = ob1203_bio_auto_gain_currents_control(&ob1203_bio,
                                                               &ppg_data,
                                                               &gain_currents,
                                                               &update);

                if (false == result)
                {
                	demo_printf("\r\n Gain control Failed in Seq-7\r\n");
                    demo_err();
                }

                if (false != update)
                {
                    /* Stop the measurement */
                    result = ob1203_bio_measurement_stop(&ob1203_bio);

                    if (false == result)
                    {
                    	demo_printf("\r\n Measurement stop Failed in Seq-7\r\n");
                        demo_err();
                    }

                    /* Reconfigure gain and currents */
                    result = ob1203_bio_gain_currents_reconfigure(&ob1203_bio, &gain_currents);

                    if (false == result)
                    {
                    	demo_printf("\r\n Gain reconfig Failed in Seq-7\r\n");
                        demo_err();
                    }

                    obsequence = OB1203_DEMO_SEQUENCE_2;
                }
                else
                {
                	obsequence = OB1203_DEMO_SEQUENCE_8;
                }

            }
            break;

            case OB1203_DEMO_SEQUENCE_8:
            {
                /* Check if the preparation for the algorithm is complete */
                result = ob1203_bio_algorithm_preparation_check(&ob1203_bio, &ready);

                if (false == result)
                {
                	demo_printf("\r\n Algorith Prep check Failed in Seq-8\r\n");
                    demo_err();
                }

                if (false == ready)
                {
                    /* Stop the measurement */
                    result = ob1203_bio_measurement_stop(&ob1203_bio);

                    if (false == result)
                    {
                    	demo_printf("\r\n Measurement stop Failed in Seq-8\r\n");
                        demo_err();
                    }

                    /* Reset the algorithm */
                    result = ob1203_bio_algorithm_reset(&ob1203_bio);

                    if (false == result)
                    {
                    	demo_printf("\r\n Algorithm reset failed in Seq-8\r\n");
                        demo_err();
                    }

                    /* Clear PPG samples */
                    result = ob1203_bio_samples_clear(&ob1203_bio);

                    if (false == result)
                    {
                    	demo_printf("\r\n Samples clear failed in Seq-8 is\r\n");
                        demo_err();
                    }

                    obsequence = OB1203_DEMO_SEQUENCE_2;
                }
                else
                {
                	obsequence = OB1203_DEMO_SEQUENCE_9;
                }
            }
            break;

            case OB1203_DEMO_SEQUENCE_9:
            {
                /* Add PPG samples */
                result = ob1203_bio_samples_add(&ob1203_bio, &ppg_data);

                if (false == result)
                {
                	demo_printf("\r\n Samples Add failed in Seq-9\r\n");
                    demo_err();
                }

                obsequence = OB1203_DEMO_SEQUENCE_10;
            }
            break;

            case OB1203_DEMO_SEQUENCE_10:
            {
                /* Calculate heart rate and SpO2 values */
                result = ob1203_bio_hr_spo2_calculate(&ob1203_bio, (ob1203_bio_data_t *)&gs_ob1203_bio_data);
                if (false == result)
                {
                	demo_printf("\r\n Actual data cal failed in Seq-10\r\n");
                    demo_err();
                }
                else
                {
#if ENABLE_SENSORS_SERIAL_PRINTS
                	demo_printf("\r\nHeart Rate : %d \r\n",gs_ob1203_bio_data.heart_rate);
                	demo_printf("SPO2 : %d \r\n",gs_ob1203_bio_data.spo2);
#endif

                }


                obsequence = OB1203_DEMO_SEQUENCE_11;
            }
            break;

            case OB1203_DEMO_SEQUENCE_11:
            {
                /* Calculate a respiration rate value */
                result = ob1203_bio_rr_calculate(&ob1203_bio,
                                                 (ob1203_bio_data_t *)&gs_ob1203_bio_data);

                if (false == result)
                {
                	demo_printf("\r\n RR cal failed in Seq-11\r\n");
                    demo_err();
                }
                else
                {
#if ENABLE_SENSORS_SERIAL_PRINTS
                	 demo_printf("Respiration Rate :%d \r\n",gs_ob1203_bio_data.respiration_rate);
#endif

                }


                obsequence = OB1203_DEMO_SEQUENCE_12;
            }
            break;

            case OB1203_DEMO_SEQUENCE_12:
            {
                /* Check perfusion index (PI) */
                result = ob1203_bio_perfusion_index_check(&ob1203_bio,&valid);

                if (false == result)
                {
                	demo_printf("\r\n PI cal failed in Seq-12\r\n");
                    demo_err();
                }
                else
                {
                 	//demo_printf("\r\nPI: %f",gs_ob1203_bio_data.perfusion_index);
#if ENABLE_SENSORS_SERIAL_PRINTS
                	demo_printf("Perfusion Index :%f \r\n",gs_ob1203_bio_data.perfusion_index);
#endif

                }


                if (false != valid)
                {
                	obsequence = OB1203_DEMO_SEQUENCE_3;
                }
                else
                {
                    /* Stop the measurement */
                    result = ob1203_bio_measurement_stop(&ob1203_bio);
                    if (false == result)
                    {
                    	demo_printf("\r\n Measurement stop Failed in Seq-12\r\n");
                        demo_err();
                    }

                    /* Reset the algorithm */
                    result = ob1203_bio_algorithm_reset(&ob1203_bio);

                    if (false == result)
                    {
                    	demo_printf("\r\n Algorithm reset failed in Seq-8\r\n");
                        demo_err();
                    }

                    obsequence = OB1203_DEMO_SEQUENCE_2;
                }
            }
            break;

            default:
            {
                demo_err();
            }
            break;
        }
       R_BSP_SoftwareDelay(4, BSP_DELAY_MILLISECS);
        tx_thread_sleep(1);
    }

}

void demo_err(void)
{

	demo_printf("\r\n................. OB1203-Demo Failed..................\r\n");
	//vTaskDelete(ob1203_sensor_thread);
//    while (1)
//    {
//        // nothing
//    }
}


void getOB1203Data(st_sensorsOB_t *obData)
{
	obData->ob_hr = gs_ob1203_bio_data.heart_rate;
	obData->ob_spo2=gs_ob1203_bio_data.spo2;
	obData->ob_rr=gs_ob1203_bio_data.respiration_rate;
	obData->ob_pi=gs_ob1203_bio_data.perfusion_index;
}

void ob1203_sensor_thread_entry(void *pvParameters)
{

    start_ob1203_demo();
    //R_BSP_SoftwareDelay(50, BSP_DELAY_MILLISECS);
}



