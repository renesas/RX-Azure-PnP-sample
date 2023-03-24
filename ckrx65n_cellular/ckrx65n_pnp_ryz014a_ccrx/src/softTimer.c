/*
 * softTimer.c
 *
 *  Created on: 06-Apr-2022
 *      Author: 27779
 */

#include "softTimer.h"
#include <stdint.h>
#include "stdbool.h"
bool milliSecondflag=false;

SOFT_TIMER stimer_array[MAX_TIMER];

//static uint32_t guiTimeoutCounter=0U;

void softTimerISRcallback (void)
{
	timerOperation();
}

void timerOperation(void)
{
    uint8_t count =0;

    for(count =0;count<MAX_TIMER;count++)
    {
        if(stimer_array[count].timer_on == 1)
        {
         stimer_array[count].msec_timer++;
         if(stimer_array[count].msec_timer >= stimer_array[count].timer_time)
         {
            stimer_array[count].timeout_flag = 1;
            stimer_array[count].timer_on= 0;
            stimer_array[count].msec_timer = 0;
         }
        }
        else
        {
           stimer_array[count].msec_timer = 0;
        }
    }
}

void startTimer(SOFT_TIMER *timer_data,uint32_t timeMs)
{
    if(timer_data->timer_on == 0)
    {
        timer_data->timeout_flag=0;
        timer_data->timer_time = timeMs;
        timer_data->timer_on=1;
        timer_data->msec_timer = 0;
    }
}

void stopTimer(SOFT_TIMER *timer_data)
{
    timer_data->timeout_flag=0;
    timer_data->timer_on=0;
    timer_data->msec_timer = 0;
}

uint8_t getTimeOutOccur(SOFT_TIMER *timer_data)
{
    return timer_data->timeout_flag;
}
