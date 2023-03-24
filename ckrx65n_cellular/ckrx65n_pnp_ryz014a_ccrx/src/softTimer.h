/*
 * softTimer.h
 *
 *  Created on: 06-Apr-2022
 *      Author: 27779
 */

#ifndef APPLICATION_CODE_SOFTTIMER_H_
#define APPLICATION_CODE_SOFTTIMER_H_
#include <stdint.h>
#define MAX_TIMER 5
typedef struct
{
    uint32_t msec_timer;
    uint32_t timer_time;
    uint8_t timeout_flag;
    uint8_t timer_on;

}SOFT_TIMER;


extern SOFT_TIMER stimer_array[MAX_TIMER];

#define     MQTT_TIMER     &stimer_array[0]
#define     TIMER1         &stimer_array[1]
#define     TIMER2         &stimer_array[2]
#define     TIMER3         &stimer_array[3]
#define     TIMER4         &stimer_array[4]


void timerOperation(void);
void timerISRcallback (void);
void stopTimer(SOFT_TIMER *timer_data);
uint8_t  getTimeOutOccur(SOFT_TIMER *timer_data);
void startTimer(SOFT_TIMER *timer_data,uint32_t timeMs);

#endif /* APPLICATION_CODE_SOFTTIMER_H_ */
