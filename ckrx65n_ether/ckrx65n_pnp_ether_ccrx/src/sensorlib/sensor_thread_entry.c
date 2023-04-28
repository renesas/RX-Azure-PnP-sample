//#include "sensor_thread.h"
//#include "RX_HS3001.h"
//#include "RX_ZMOD4XXX_Common.h"

#include "r_smc_entry.h"
#include "ICP10101/ICP_10101.h"
#include "ICM20948/icm20948.h"
#include "ICM20948/icm_i2c.h"
#include "HS3001/RX_HS3001.h"
#include "ZMOD4x10/RX_ZMOD4XXX_Common.h"
#include <demo_printf.h>
//#include "OB1203.h"
//#include "ob1203Task.h"
//#include "oximeter.h"
#include "sensorsData.h"
#include "r_irq_rx_pinset.h"
#define HS3001_SENSOR 	1
#define ZMOD4410_SENSOR 0
#define ZMOD4510_SENSOR 1
#define OB1202_SENSOR 	0
#define ICP10101_SENSOR 1
#define ICM20948_SENSOR 0

#define UNUSED(x)  (void)(x)
static void reset_zmod_sensor(void);
uint8_t uart_printBuf[60];

void sensor_thread_entry1(void *pvParameters)
{

    FSP_PARAMETER_NOT_USED (pvParameters);

   // ledInit();     /*Board Led configuration*/

    reset_zmod_sensor();   /*Reset Zmod Sensors before Init*/

    R_ICU_PinSet();       /*Set Interrupt pins*/

    /*Open Enabled Sensor Instances*/

#if HS3001_SENSOR
	g_hs300x_sensor0_quick_setup();  /*HS Sensor i2c device Setup*/
#endif

#if ZMOD4410_SENSOR
	g_zmod4xxx_sensor0_quick_setup();  /*ZMOD4410 Sensor i2c device Setup*/
#endif

#if ZMOD4510_SENSOR
	g_zmod4xxx_sensor1_quick_setup();   /*ZMOD4510  Sensor i2c device Setup*/
#endif

#if OB1202_SENSOR
	//g_ob1203_sensor0_quick_setup();     /*OB1203 Sensor i2c device Setup*/
	//main_init();                       /*OB1203 init */
#endif

#if ICP10101_SENSOR
	RmComDevice_init();                 /*ICP10101 Sensor i2c device Setup*/
	begin();                            /*ICP init */
	measureStart(VERY_ACCURATE);        /*ICP start */
#endif

#if ICM20948_SENSOR
	RmComDevice_init_Icm();             /*ICM20948 Sensor i2c device Setup*/
	icmInit();                         /*ICM Init*/
#endif


	while(1)
	{
		/*Read Enabled Sensors data*/
#if HS3001_SENSOR
		hs3001_get();                   /*Read HS3001 Sensor Data*/
		//tx_thread_sleep (10);
#endif
#if ZMOD4410_SENSOR
		start_iaq_1st_gen_demo();		/*Read ZMOD4410 Sensor Data*/
		vTaskDelay(10);
#endif

#if ZMOD4510_SENSOR
		start_oaq_1st_gen_demo();       /*Read ZMOD4510 Sensor Data*/
		//tx_thread_sleep (10);
#endif

#if OB1202_SENSOR
		//start_ob1203_demo();             /*Read OB1203 Sensor Data*/
		//vTaskDelay(10);
#endif

#if ICP10101_SENSOR
		readICPdata(); 					/*Read ICP10101 Sensor Data*/
		//tx_thread_sleep (10);
#endif

#if ICM20948_SENSOR
		readicm20948dta();				/*Read ICM20948 Sensor Data*/

#endif
		tx_thread_sleep (10);
	}

}


//void ledInit()
//{
//    PORT2.PDR.BIT.B5 = 1;   /*RED LED PIN AS A OUTPUT*/
//	PORT2.PDR.BIT.B2 = 1;   /*GREEN LED PIN AS A OUTPUT*/
//	PORTA.PDR.BIT.B3 = 1;   /*BLUE LED PIN AS A OUTPUT*/
//
//	PORT1.PDR.BIT.B7=1;   /*Tri Color LED RED*/
//	PORTA.PDR.BIT.B5 = 1; /*Tri Color LED GREEN*/
//	PORTA.PDR.BIT.B7 = 1; /*Tri Color LED BLUE*/
//
//	PORT2.PODR.BIT.B5 = 1;  /*TURN OFF RED LED*/
//	PORT2.PODR.BIT.B2 = 1;  /*TURN OFF GREEN LED*/
//	PORTA.PODR.BIT.B3 = 1;  /*TURN OFF BLUE LED*/
//
//	PORT1.PODR.BIT.B7=1;   /*Tri Color LED RED*/
//    PORTA.PODR.BIT.B5 = 1; /*Tri Color LED GREEN*/
//	PORTA.PODR.BIT.B7 = 1; /*Tri Color LED BLUE*/
//}

static void reset_zmod_sensor(void)
{
	/* ZMOD Reset for CK-RX65N */
	PORT6.PDR.BIT.B2 = 1; /*ZMOD4410 RESET PIN AS A OUTPUT*/
	PORT6.PDR.BIT.B3 = 1; /*ZMOD4510 RESET PIN AS A OUTPUT*/
	PORT6.PODR.BIT.B2 = 1; /*HIGH*/
	PORT6.PODR.BIT.B3 = 1; /*HIGH*/
	//vTaskDelay(10);
	PORT6.PODR.BIT.B2 = 0; /*LOW*/
	PORT6.PODR.BIT.B3 = 0; /*LOW*/
	//vTaskDelay(10);
	PORT6.PODR.BIT.B2 = 1; /*HIGH*/
	PORT6.PODR.BIT.B3 = 1; /*HIGH*/
}
