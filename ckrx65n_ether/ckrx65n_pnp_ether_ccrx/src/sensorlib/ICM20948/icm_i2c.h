/*
/*
 * I2C.h
 *
 *  Created on: 09-Dec-2021
 *      Author: vivek.b
 */

#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>
#include "r_smc_entry.h"
#include "r_comms_i2c_if.h"
#include "r_comms_i2c_rx_config.h"
#if COMMS_I2C_CFG_DRIVER_I2C
#include "r_riic_rx_if.h"
#endif
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
#include "r_sci_iic_rx_if.h"
#endif
///* Typedefs*/
//typedef enum
//{
//    ub_0 = 0 << 4,
//    ub_1 = 1 << 4,
//    ub_2 = 2 << 4,
//    ub_3 = 3 << 4
//} userbank;

void RmComDevice_init_Icm(void);
void icm20948_I2C_interface_open(void);
uint8_t write_i2c_reg(uint8_t Reg,uint8_t *val, char len);
uint8_t read_i2c_reg(uint8_t Reg,uint8_t *val, char len);

int8_t RmCom_I2C_r_icm(uint8_t reg, uint8_t *val, char num);
int8_t RmCom_I2C_w_icm(uint8_t reg, uint8_t *val, char num);

void comms_i2c_callback(rm_comms_callback_args_t *p_args);


void delayMicrosecondsicm(uint32_t us);
void delayicm(uint32_t ms);
#endif /* I2C_H_ */
