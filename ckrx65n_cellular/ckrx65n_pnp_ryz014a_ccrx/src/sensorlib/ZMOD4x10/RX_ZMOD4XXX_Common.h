#ifndef RA_ZMOD4XXX_COMMON_H_
#define RA_ZMOD4XXX_COMMON_H_

#include "../sensorsData.h"
#define DEMO_IRQ_ENABLE (1)

#define DEMO_IRQ_ENABLE (1)

typedef enum e_demo_sequence_zmod4410
{
    ZMOD4410_DEMO_SEQUENCE_1 = (1),
	ZMOD4410_DEMO_SEQUENCE_2,
	ZMOD4410_DEMO_SEQUENCE_3,
	ZMOD4410_DEMO_SEQUENCE_4,
	ZMOD4410_DEMO_SEQUENCE_5,
	ZMOD4410_DEMO_SEQUENCE_6,
	ZMOD4410_DEMO_SEQUENCE_7,
	ZMOD4410_DEMO_SEQUENCE_8,
	ZMOD4410_DEMO_SEQUENCE_9,
} zmod4410_demo_sequence_t;

typedef enum e_demo_sequence_zmod4510
{
	ZMOD4510_DEMO_SEQUENCE_1 = (1),
	ZMOD4510_DEMO_SEQUENCE_2,
	ZMOD4510_DEMO_SEQUENCE_3,
	ZMOD4510_DEMO_SEQUENCE_4,
	ZMOD4510_DEMO_SEQUENCE_5,
	ZMOD4510_DEMO_SEQUENCE_6,
	ZMOD4510_DEMO_SEQUENCE_7,
	ZMOD4510_DEMO_SEQUENCE_8,
	ZMOD4510_DEMO_SEQUENCE_9,
} zmod4510_demo_sequence_t;

typedef enum e_demo_callback_status_zmod4410
{
	ZMOD4410_DEMO_CALLBACK_STATUS_WAIT = (0),
	ZMOD4410_DEMO_CALLBACK_STATUS_SUCCESS,
	ZMOD4410_DEMO_CALLBACK_STATUS_REPEAT,
} zmod4410_demo_callback_status_t;

typedef enum e_demo_callback_status_zmod4510
{
	ZMOD4510_DEMO_CALLBACK_STATUS_WAIT = (0),
	ZMOD4510_DEMO_CALLBACK_STATUS_SUCCESS,
	ZMOD4510_DEMO_CALLBACK_STATUS_REPEAT,
} zmod4510_demo_callback_status_t;

void g_zmod4xxx_sensor0_quick_setup(void);
/* Quick setup for g_zmod4xxx_sensor0. */
void g_zmod4xxx_sensor1_quick_setup(void);
void start_iaq_1st_gen_demo(void);
void start_oaq_1st_gen_demo(void);

extern volatile zmod4410_demo_callback_status_t  g_i2c_callback_status;
extern volatile zmod4410_demo_callback_status_t  g_irq_callback_status;

extern volatile zmod4510_demo_callback_status_t  g_i2c_callback_status1;
extern volatile zmod4510_demo_callback_status_t  g_irq_callback_status1;


void getZMOD4410Data(st_sensorsZmod4410_t *zmod4410Data);
void getZMOD4510Data(st_sensorsZmod4510_t *zmod4510Data);

#endif /* RA_ZMOD4XXX_COMMON_H_ */
