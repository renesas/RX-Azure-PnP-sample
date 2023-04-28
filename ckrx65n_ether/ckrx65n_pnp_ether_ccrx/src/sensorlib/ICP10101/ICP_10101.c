/*
/*
 * ICP10101.c
 *
 *  Created on: Jan 24, 2022
 *      Author: Vivek
 */
#include "ICP_10101.h"
#include "stdint.h"
#include "stdio.h"
#include "r_comms_i2c_rx_config.h"
#include "r_smc_entry.h"
#include "../sensorsData.h"
#include "r_comms_i2c_if.h"
#include "r_comms_i2c_rx_config.h"
#include <demo_printf.h>
#if COMMS_I2C_CFG_DRIVER_I2C
#include "r_riic_rx_if.h"
#endif
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
#include "r_sci_iic_rx_if.h"
#endif
#include "string.h"
#define ICP_I2C_ID 0x63

#define ICP_CMD_READ_ID   0xefc8
#define ICP_CMD_SET_ADDR  0xc595
#define ICP_CMD_READ_OTP  0xc7f7
#define ICP_CMD_MEAS_LP   0x609c
#define ICP_CMD_MEAS_N    0x6825
#define ICP_CMD_MEAS_LN   0x70df
#define ICP_CMD_MEAS_ULN  0x7866

// constants for presure calculation
const float _pcal[3] = { 45000.0, 80000.0, 105000.0 };
const float _lut_lower = 3.5 * 0x100000;    // 1<<20
const float _lut_upper = 11.5 * 0x100000;   // 1<<20
const float _quadr_factor = 1 / 16777216.0;
const float _offst_factor = 2048.0;

#define I2C_TRANSMISSION_IN_PROGRESS        0
#define I2C_TRANSMISSION_COMPLETE           1
#define I2C_TRANSMISSION_ABORTED            2

static uint8_t ICP_transmit_complete_flag;
float gs_icp_temperature,gs_icp_pressure;
char Tempf[20];
char Tempficp[20],PressF[20];
float Temperature,Pressure,ChPressure;

float _scal[4];
uint16_t _raw_t;
uint32_t _raw_p;
float _temperature_C;
float _pressure_Pa;
unsigned long _meas_start;
uint8_t _meas_duration;
bool _data_ready;

uint8_t begin(void)
{
	R_Config_CMTW0_Create();         /*Create timer*/
	R_Config_CMTW0_Start();          /*Start timer*/
     //verify that the sensor is repsonding
    if (!isConnected())
    {
        return false;
    }

    // read sensor calibration data
    uint8_t addr_otp_cmd[5] = {
                               (ICP_CMD_SET_ADDR >> 8) & 0xff,
                               ICP_CMD_SET_ADDR & 0xff,
                               0x00, 0x66, 0x9c };
    uint8_t otp_buf[3];
    _sendCommands(addr_otp_cmd, 5);
    for (int i=0; i<4; i++) {
        _sendCommand(ICP_CMD_READ_OTP);
        _readResponse(otp_buf, 3);
        _scal[i] = (otp_buf[0] << 8) | otp_buf[1];
    }
    return true;
}

bool isConnected(void)
{
    uint8_t id_buf[2];
    _sendCommand(ICP_CMD_READ_ID);
    _readResponse(id_buf, 2);
    uint16_t id = (id_buf[0] << 8) | id_buf[1];
    if ((id & 0x03f) == 0x8)
    {

		return true;
	}
    else
    {

        return false;
    }
}


void measure(mmode mode) {
    delayicp(measureStart(mode));
    while (!dataReady());
}

uint8_t measureStart(mmode mode) {
    uint16_t cmd;
    switch (mode)
    {
        case FAST:
            cmd = ICP_CMD_MEAS_LP;
            _meas_duration = 3;
            break;
        case ACCURATE:
            cmd = ICP_CMD_MEAS_LN;
            _meas_duration = 24;
            break;
        case VERY_ACCURATE:
            cmd = ICP_CMD_MEAS_ULN;
            _meas_duration = 95;
            break;
        case NORMAL:
        default:
            cmd = ICP_CMD_MEAS_N;
            _meas_duration = 7;
            break;
    }
    _sendCommand(cmd);
    _data_ready = false;
    _meas_start = GetMilliTick();
    return _meas_duration;
}

bool dataReady(void)
{
    if (_data_ready)
    {
        return true;
    }
    uint32_t MilliTick;
    MilliTick = GetMilliTick();
    if (MilliTick - _meas_start < _meas_duration)
    {
        return false;
    }
    uint8_t res_buf[9];
    _readResponse(res_buf, 9);
    _raw_t = (res_buf[0] << 8) | res_buf[1];
    uint32_t L_res_buf3 = res_buf[3];   // expand result bytes to 32bit to fix issues on 8-bit MCUs
    uint32_t L_res_buf4 = res_buf[4];
    uint32_t L_res_buf6 = res_buf[6];
    _raw_p = (L_res_buf3 << 16) | (L_res_buf4 << 8) | L_res_buf6;
    _calculate();

    _data_ready = true;
    return true;
}

float getTemperatureC(void)
{
    return _temperature_C;
}

float getTemperatureF(void)
{
    return _temperature_C * 1.8 + 32;
}

float getPressurePa(void)
{
    return _pressure_Pa;
}

void _calculate(void)
{
    // calculate temperature
    _temperature_C = -45.f + 175.f / 65536.f * _raw_t;

    // calculate pressure
    float t = (float)(_raw_t - 32768);
    float s1 = _lut_lower + (float)(_scal[0] * t * t) * _quadr_factor;
    float s2 = _offst_factor * _scal[3] + (float)(_scal[1] * t * t) * _quadr_factor;
    float s3 = _lut_upper + (float)(_scal[2] * t * t) * _quadr_factor;
    float c = (s1 * s2 * (_pcal[0] - _pcal[1]) +
            s2 * s3 * (_pcal[1] - _pcal[2]) +
            s3 * s1 * (_pcal[2] - _pcal[0])) /
                    (s3 * (_pcal[0] - _pcal[1]) +
                            s1 * (_pcal[1] - _pcal[2]) +
                            s2 * (_pcal[2] - _pcal[0]));
    float a = (_pcal[0] * s1 - _pcal[1] * s2 - (_pcal[1] - _pcal[0]) * c) / (s1 - s2);
    float b = (_pcal[0] - a) * (s1 + c);
    _pressure_Pa = a + b / (c + _raw_p);
}

void _sendCommand(uint16_t cmd)
{
    uint8_t WriteBuff[2]={0};
    WriteBuff[0] = ((cmd>>8) & 0xFF);
    WriteBuff[1] = cmd & 0xFF;
    ICP_RmCom_I2C_w(WriteBuff, 2);
}

void _sendCommands(uint8_t *cmd_buf, uint8_t cmd_len)
{

    ICP_RmCom_I2C_w(cmd_buf, cmd_len);
}

void _readResponse(uint8_t *res_buf, uint8_t res_len)
{
    ICP_RmCom_I2C_r(res_buf,res_len);
}

volatile uint32_t milliTicks;
volatile uint32_t milliTicks2;
uint32_t GetMilliTick(void)
{
    return milliTicks;
}

void readICPdata()
{
	if (dataReady())
	 {
		Temperature = getTemperatureC();
		gs_icp_temperature =Temperature;
		gs_icp_temperature = (gs_icp_temperature * (float)1.8)+(float)32.0; //Deg C to Deg F
		Pressure = getPressurePa();
		gs_icp_pressure =Pressure;

#if ENABLE_SENSORS_SERIAL_PRINTS
		demo_printf("\r\nICP10101-Temperature : %f \r\n",gs_icp_temperature);
		demo_printf("ICP10101-Pressure    : %f \r\n",Pressure );
#endif


	measureStart(VERY_ACCURATE);
	}
}



void TimerCallback( )
{
    milliTicks++;
    milliTicks2++;
    if(milliTicks2==1000)
    {
    	PORT2.PODR.BIT.B2 = ~PORT2.PODR.BIT.B2;
    	milliTicks2=0;
    }
}
void RmComDevice_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err=g_comms_i2c_device5.p_api->open(g_comms_i2c_device5.p_ctrl,g_comms_i2c_device5.p_cfg);
    if(FSP_SUCCESS==err)
    {
    	demo_printf("ICP10101 open sensor instance successful: %d \r\n", err);
    }
    else
    {
    	demo_printf("ICP10101 open sensor instance failed: %d \r\n", err);
    }
}


int8_t ICP_RmCom_I2C_r(uint8_t *val, char num)
{
    uint16_t timeout = 1000;
    fsp_err_t err;
    ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
//    rm_comms_write_read_params_t write_read_params;
//    write_read_params.p_src=&reg;
//    write_read_params.src_bytes  = 1;
//    write_read_params.p_dest     = val;
//    write_read_params.dest_bytes = (uint8_t)num;
    err = g_comms_i2c_device5.p_api->read(g_comms_i2c_device5.p_ctrl,val,num);
//    err=g_comms_i2c_device1.p_api->writeRead(g_comms_i2c_device1.p_ctrl,write_read_params);
    if (err == FSP_SUCCESS)
    {
        while (ICP_transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            if (--timeout == 0)
            {
                break;
            }
            delayMicrosecondsicp(1);
        }

        ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (ICP_transmit_complete_flag == I2C_TRANSMISSION_ABORTED)
    {
        err = FSP_ERR_ABORTED;
    }
    return err;

}


int8_t ICP_RmCom_I2C_w( uint8_t *val, char num)
{

    uint16_t timeout = 1000;
    int8_t err;
    static uint8_t data[50];
//    data[0] = reg;
    memcpy(data, val, (size_t)num);
    ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    err=g_comms_i2c_device5.p_api->write(g_comms_i2c_device5.p_ctrl,data, (uint32_t)(num));
    if (err == FSP_SUCCESS)
    {
        //APP_PRINT("** OB1203 Sensor I2C Write Successful ** %u \r\n",err);
        while (ICP_transmit_complete_flag == I2C_TRANSMISSION_IN_PROGRESS)
        {
            if (--timeout == 0)
            {
                break;
            }
            delayMicrosecondsicp(1);
        }

        ICP_transmit_complete_flag = I2C_TRANSMISSION_IN_PROGRESS;
    }

    if (ICP_transmit_complete_flag == I2C_TRANSMISSION_ABORTED)
    {
        err = FSP_ERR_ABORTED;
    }

    return err;
}

void comms_i2c_callback_icp(rm_comms_callback_args_t *p_args)
{
    if (p_args->event == RM_COMMS_EVENT_OPERATION_COMPLETE)
    {
        ICP_transmit_complete_flag = I2C_TRANSMISSION_COMPLETE;
    }
    else
    {
        ICP_transmit_complete_flag = I2C_TRANSMISSION_ABORTED;
    }

}

void getICP10101Data(st_sensorsICP_t *icpData)
{
	icpData->temperatureicp = gs_icp_temperature;
	icpData->pressureicp =gs_icp_pressure;
}

void delayMicrosecondsicp(uint32_t us)
{
    R_BSP_SoftwareDelay(us, BSP_DELAY_MICROSECS);
}

void delayicp(uint32_t ms)
{
    R_BSP_SoftwareDelay(ms, BSP_DELAY_MILLISECS);

}


