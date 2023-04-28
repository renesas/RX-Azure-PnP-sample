/*
/*
 * ICM10101.h
 *
 *  Created on: Jan 24, 2022
 *      Author: Vivek
 */

#ifndef ICP_10101_H_
#define ICP_10101_H_

#include "stdbool.h"
#include "stdint.h"
//#include "hal_data.h"
#include "sensorsData.h"

typedef enum
{
FAST=0,
NORMAL,
ACCURATE,
VERY_ACCURATE
}mmode;


uint8_t begin();
bool isConnected(void);
void measure(mmode mode);
uint8_t measureStart(mmode mode);
bool dataReady();
float getTemperatureC(void);
float getTemperatureF(void);
float getPressurePa(void);

void getICP10101Data(st_sensorsICP_t *icpData);
void _calculate(void);
void _sendCommand(uint16_t cmd);
void _sendCommands(uint8_t *cmd_buf, uint8_t cmd_len);
void _readResponse(uint8_t *res_buf, uint8_t res_len );
int8_t ICP_RmCom_I2C_r(uint8_t *val, char num);
int8_t ICP_RmCom_I2C_w( uint8_t *val, char num);
int8_t I2C_w(uint8_t addr, uint8_t reg, uint8_t *val, char num);
int8_t I2C_r(uint8_t addr, uint8_t reg, uint8_t *val, char num);
void g_comms_i2c_bus0_quick_setup(void);
void RmComDevice_init(void);
void delayMicrosecondsicp(uint32_t us);
void delayicp(uint32_t ms);
uint32_t GetMilliTick(void);
void TimerCallback( );
void readICPdata();
#endif /* ICP_10101_H_ */
