/*
/*
 * ICM_20948.c
 *
 *  Created on: Jan 20, 2022
 *      Author: Vivek
 */

#include "icm20948.h"
#include "icm_i2c.h"
#include "math.h"
#include "sensorsData.h"
#include <demo_printf.h>
 xyzFloat gbAccActual;
 xyzFloat gbAccGvalActual;
 xyzFloat gbgyroActual;
 xyzFloat gbMagActual;

#define sq(x)  (x*x)
#define M_PI        3.14159265358979323846

//static void     select_user_bank(userbank ub);
//static uint8_t  read_single_icm20948_reg(userbank ub, uint8_t reg);
//static void     write_single_icm20948_reg(userbank ub, uint8_t reg, uint8_t val);
uint8_t currentBank;
//TwoWire *_wire;
int i2cAddress;

uint8_t buffer[20];
xyzFloat accOffsetVal;
xyzFloat accCorrFactor;
xyzFloat gyrOffsetVal;
uint8_t accRangeFactor;
uint8_t gyrRangeFactor;
uint8_t regVal;   // intermediate storage of register values
ICM20948_fifoType fifoType;


bool icm20948_init(void)
{
    reset_ICM20948();
    accOffsetVal.x = 0.0;
    accOffsetVal.y = 0.0;
    accOffsetVal.z = 0.0;

    accCorrFactor.x = 1.0;
    accCorrFactor.y = 1.0;
    accCorrFactor.z = 1.0;

    accRangeFactor = 1.0;

    gyrOffsetVal.x = 0.0;
    gyrOffsetVal.y = 0.0;
    gyrOffsetVal.z = 0.0;

    gyrRangeFactor = 1.0;

    fifoType = ICM20948_FIFO_ACC;
    sleep(false);

    writeRegister8(2, ICM20948_ODR_ALIGN_EN, 1);
    return 0;
}

void autoOffsets(void){
    xyzFloat accRawVal, gyrRawVal;
    accOffsetVal.x = 0.0;
    accOffsetVal.y = 0.0;
    accOffsetVal.z = 0.0;

    setGyrDLPF(ICM20948_DLPF_6); // lowest noise
    setGyrRange(ICM20948_GYRO_RANGE_250); // highest resolution
    setAccRange(ICM20948_ACC_RANGE_2G);
    setAccDLPF(ICM20948_DLPF_6);
    delayicm(100);

    for(int i=0; i<50; i++){
        readSensor();
        accRawVal = getAccRawValues();
        accOffsetVal.x += accRawVal.x;
        accOffsetVal.y += accRawVal.y;
        accOffsetVal.z += accRawVal.z;
        delayicm(10);
    }

    accOffsetVal.x /= 50;
    accOffsetVal.y /= 50;
    accOffsetVal.z /= 50;
    accOffsetVal.z -= 16384.0;

    for(int i=0; i<50; i++){
        readSensor();
        gyrRawVal = getGyrRawValues();
        gyrOffsetVal.x += gyrRawVal.x;
        gyrOffsetVal.y += gyrRawVal.y;
        gyrOffsetVal.z += gyrRawVal.z;
        delayicm(1);
    }

    gyrOffsetVal.x /= 50;
    gyrOffsetVal.y /= 50;
    gyrOffsetVal.z /= 50;

}

void setAccOffsets(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax){
    accOffsetVal.x = (xMax + xMin) * 0.5;
    accOffsetVal.y = (yMax + yMin) * 0.5;
    accOffsetVal.z = (zMax + zMin) * 0.5;
    accCorrFactor.x = (xMax + abs(xMin)) / 32768.0;
    accCorrFactor.y = (yMax + abs(yMin)) / 32768.0;
    accCorrFactor.z = (zMax + abs(zMin)) / 32768.0 ;
}

void setGyrOffsets(float xOffset, float yOffset, float zOffset){
    gyrOffsetVal.x = xOffset;
    gyrOffsetVal.y = yOffset;
    gyrOffsetVal.z = zOffset;
}

/* Sub Functions */
uint8_t whoAmI(void)
{
    uint8_t icm20948_id = readRegister8(0, ICM20948_WHO_AM_I);
    if (icm20948_id == 0xEA)
        return true;
    else
        return false;
}

void enableAcc(bool enAcc){
    regVal = readRegister8(0, ICM20948_PWR_MGMT_2);
    if(enAcc){
        regVal &= ~ICM20948_ACC_EN;
    }
    else{
        regVal |= ICM20948_ACC_EN;
    }
    writeRegister8(0, ICM20948_PWR_MGMT_2, regVal);
}

void setAccRange(ICM20948_accRange accRange){
    regVal = readRegister8(2, ICM20948_ACCEL_CONFIG);
    regVal &= ~(0x06);
    regVal |= (accRange<<1);
    writeRegister8(2, ICM20948_ACCEL_CONFIG, regVal);
    accRangeFactor = 1<<accRange;
}

void setAccDLPF(ICM20948_dlpf dlpf)
{
    regVal = readRegister8(2, ICM20948_ACCEL_CONFIG);
    if(dlpf==ICM20948_DLPF_OFF){
        regVal &= 0xFE;
        writeRegister8(2, ICM20948_ACCEL_CONFIG, regVal);
        return;
    }
    else{
        regVal |= 0x01;
        regVal &= 0xC7;
        regVal |= (dlpf<<3);
    }
    writeRegister8(2, ICM20948_ACCEL_CONFIG, regVal);
}

void setAccSampleRateDivider(uint16_t accSplRateDiv){
    writeRegister16(2, ICM20948_ACCEL_SMPLRT_DIV_1, accSplRateDiv);
}

void enableGyr(bool enGyr){
    regVal = readRegister8(0, ICM20948_PWR_MGMT_2);
    if(enGyr){
        regVal &= ~ICM20948_GYR_EN;
    }
    else{
        regVal |= ICM20948_GYR_EN;
    }
    writeRegister8(0, ICM20948_PWR_MGMT_2, regVal);
}

void setGyrRange(ICM20948_gyroRange gyroRange){
    regVal = readRegister8(2, ICM20948_GYRO_CONFIG_1);
    regVal &= ~(0x06);
    regVal |= (gyroRange<<1);
    writeRegister8(2, ICM20948_GYRO_CONFIG_1, regVal);
    gyrRangeFactor = (1<<gyroRange);
}

void setGyrDLPF(ICM20948_dlpf dlpf){
    regVal = readRegister8(2, ICM20948_GYRO_CONFIG_1);
    if(dlpf==ICM20948_DLPF_OFF){
        regVal &= 0xFE;
        writeRegister8(2, ICM20948_GYRO_CONFIG_1, regVal);
        return;
    }
    else{
        regVal |= 0x01;
        regVal &= 0xC7;
        regVal |= (dlpf<<3);
    }
    writeRegister8(2, ICM20948_GYRO_CONFIG_1, regVal);
}

void setGyrSampleRateDivider(uint8_t gyrSplRateDiv){
    writeRegister8(2, ICM20948_GYRO_SMPLRT_DIV, gyrSplRateDiv);
}

void setTempDLPF(ICM20948_dlpf dlpf){
    writeRegister8(2, ICM20948_TEMP_CONFIG, dlpf);
}

void setI2CMstSampleRate(uint8_t rateExp){
    if(rateExp < 16){
        writeRegister8(3, ICM20948_I2C_MST_ODR_CFG, rateExp);
    }
}


void readSensor(){
    readAllData(&buffer);
}

xyzFloat getAccRawValues(){
    xyzFloat accRawVal;
    accRawVal.x = (int16_t)(((buffer[0]) << 8) | buffer[1]) * 1.0;
    accRawVal.y = (int16_t)(((buffer[2]) << 8) | (int16_t)buffer[3]) * 1.0;
    accRawVal.z = (int16_t)(((buffer[4]) << 8) | buffer[5]) * 1.0;
    return accRawVal;
}

xyzFloat getCorrectedAccRawValues(){
    xyzFloat accRawVal = getAccRawValues();
    accRawVal = correctAccRawValues(accRawVal);
    return accRawVal;
}


xyzFloat getGValues(){
    xyzFloat gVal, accRawVal;
    accRawVal = getCorrectedAccRawValues();

    gVal.x = accRawVal.x * accRangeFactor / 16384.0;
    gVal.y = accRawVal.y * accRangeFactor / 16384.0;
    gVal.z = accRawVal.z * accRangeFactor / 16384.0;
    return gVal;
}

xyzFloat getAccRawValuesFromFifo(){
    xyzFloat accRawVal = readICM20948xyzValFromFifo();
    return accRawVal;
}

xyzFloat getCorrectedAccRawValuesFromFifo(){
    xyzFloat accRawVal = getAccRawValuesFromFifo();
    accRawVal = correctAccRawValues(accRawVal);

    return accRawVal;
}

xyzFloat getGValuesFromFifo(){
    xyzFloat gVal, accRawVal;
    accRawVal = getCorrectedAccRawValuesFromFifo();

    gVal.x = accRawVal.x * accRangeFactor / 16384.0;
    gVal.y = accRawVal.y * accRangeFactor / 16384.0;
    gVal.z = accRawVal.z * accRangeFactor / 16384.0;
    return gVal;
}

float getResultantG(xyzFloat gVal){
    float resultant = 0.0;
    resultant = sqrt(sq(gVal.x) + sq(gVal.y) + sq(gVal.z));

    return resultant;
}

float getTemperature(){
    int16_t rawTemp = (int16_t)(((buffer[12]) << 8) | buffer[13]);
    float tmp = (rawTemp*1.0 - ICM20948_ROOM_TEMP_OFFSET)/ICM20948_T_SENSITIVITY + 21.0;
    return tmp;
}

xyzFloat getGyrRawValues(){
    xyzFloat gyrRawVal;

    gyrRawVal.x = (int16_t)(((buffer[6]) << 8) | buffer[7]) * 1.0;
    gyrRawVal.y = (int16_t)(((buffer[8]) << 8) | buffer[9]) * 1.0;
    gyrRawVal.z = (int16_t)(((buffer[10]) << 8) | buffer[11]) * 1.0;

    return gyrRawVal;
}

xyzFloat getCorrectedGyrRawValues(){
    xyzFloat gyrRawVal = getGyrRawValues();
    gyrRawVal = correctGyrRawValues(gyrRawVal);
    return gyrRawVal;
}

xyzFloat getGyrValues(){
    xyzFloat gyrVal = getCorrectedGyrRawValues();

    gyrVal.x = gyrVal.x * gyrRangeFactor * 250.0 / 32768.0;
    gyrVal.y = gyrVal.y * gyrRangeFactor * 250.0 / 32768.0;
    gyrVal.z = gyrVal.z * gyrRangeFactor * 250.0 / 32768.0;

    return gyrVal;
}

xyzFloat getGyrValuesFromFifo(){
    xyzFloat gyrVal;
    xyzFloat gyrRawVal = readICM20948xyzValFromFifo();

    gyrRawVal = correctGyrRawValues(gyrRawVal);
    gyrVal.x = gyrRawVal.x * gyrRangeFactor * 250.0 / 32768.0;
    gyrVal.y = gyrRawVal.y * gyrRangeFactor * 250.0 / 32768.0;
    gyrVal.z = gyrRawVal.z * gyrRangeFactor * 250.0 / 32768.0;

    return gyrVal;
}

xyzFloat getMagValues(){
    int16_t x,y,z;
    xyzFloat mag;

    x = (int16_t)((buffer[15]) << 8) | buffer[14];
    y = (int16_t)((buffer[17]) << 8) | buffer[16];
    z = (int16_t)((buffer[19]) << 8) | buffer[18];

    mag.x = x * AK09916_MAG_LSB;
    mag.y = y * AK09916_MAG_LSB;
    mag.z = z * AK09916_MAG_LSB;

    return mag;
}

/********* Power, Sleep, Standby *********/

void enableCycle(ICM20948_cycle cycle){
    regVal = readRegister8(0, ICM20948_LP_CONFIG);
    regVal &= 0x0F;
    regVal |= cycle;

    writeRegister8(0, ICM20948_LP_CONFIG, regVal);
}

void enableLowPower(bool enLP){    //
    regVal = readRegister8(0, ICM20948_PWR_MGMT_1);
    if(enLP){
        regVal |= ICM20948_LP_EN;
    }
    else{
        regVal &= ~ICM20948_LP_EN;
    }
    writeRegister8(0, ICM20948_PWR_MGMT_1, regVal);
}

void setGyrAverageInCycleMode(ICM20948_gyroAvgLowPower avg){
    writeRegister8(2, ICM20948_GYRO_CONFIG_2, avg);
}

void setAccAverageInCycleMode(ICM20948_accAvgLowPower avg){
    writeRegister8(2, ICM20948_ACCEL_CONFIG_2, avg);
}

void sleep(bool sleep){
    regVal = readRegister8(0, ICM20948_PWR_MGMT_1);
    if(sleep){
        regVal |= ICM20948_SLEEP;
    }
    else{
        regVal &= ~ICM20948_SLEEP;
    }
    writeRegister8(0, ICM20948_PWR_MGMT_1, regVal);
}

/******** Angles and Orientation *********/

xyzFloat getAngles(){
    xyzFloat angleVal;
    xyzFloat gVal = getGValues();
    if(gVal.x > 1.0){
        gVal.x = 1.0;
    }
    else if(gVal.x < -1.0){
        gVal.x = -1.0;
    }
    angleVal.x = (asin(gVal.x)) * 57.296;

    if(gVal.y > 1.0){
        gVal.y = 1.0;
    }
    else if(gVal.y < -1.0){
        gVal.y = -1.0;
    }
    angleVal.y = (asin(gVal.y)) * 57.296;

    if(gVal.z > 1.0){
        gVal.z = 1.0;
    }
    else if(gVal.z < -1.0){
        gVal.z = -1.0;
    }
    angleVal.z = (asin(gVal.z)) * 57.296;

    return angleVal;
}

ICM20948_orientation getOrientation(){
    xyzFloat angleVal = getAngles();
    ICM20948_orientation orientation = ICM20948_FLAT;
    if(abs(angleVal.x) < 45){      // |x| < 45
        if(abs(angleVal.y) < 45){      // |y| < 45
            if(angleVal.z > 0){          //  z  > 0
                orientation = ICM20948_FLAT;
            }
            else{                        //  z  < 0
                orientation = ICM20948_FLAT_1;
            }
        }
        else{                         // |y| > 45
            if(angleVal.y > 0){         //  y  > 0
                orientation = ICM20948_XY;
            }
            else{                       //  y  < 0
                orientation = ICM20948_XY_1;
            }
        }
    }
    else{                           // |x| >= 45
        if(angleVal.x > 0){           //  x  >  0
            orientation = ICM20948_YX;
        }
        else{                       //  x  <  0
            orientation = ICM20948_YX_1;
        }
    }
    return orientation;
}

//String getOrientationAsString(){
//    ICM20948_orientation orientation = getOrientation();
//    String orientationAsString = "";
//    switch(orientation){
//        case ICM20948_FLAT:      orientationAsString = "z up";   break;
//        case ICM20948_FLAT_1:    orientationAsString = "z down"; break;
//        case ICM20948_XY:        orientationAsString = "y up";   break;
//        case ICM20948_XY_1:      orientationAsString = "y down"; break;
//        case ICM20948_YX:        orientationAsString = "x up";   break;
//        case ICM20948_YX_1:      orientationAsString = "x down"; break;
//    }
//    return orientationAsString;
//}

float getPitch(){
    xyzFloat angleVal = getAngles();
    float pitch = (atan2(angleVal.x, sqrt(abs((angleVal.x*angleVal.y + angleVal.z*angleVal.z))))*180.0)/M_PI;
    return pitch;
}

float getRoll(){
    xyzFloat angleVal = getAngles();
    float roll = (atan2(angleVal.y, angleVal.z)*180.0)/M_PI;
    return roll;
}

/************** Interrupts ***************/

void setIntPinPolarity(ICM20948_intPinPol pol){
    regVal = readRegister8(0, ICM20948_INT_PIN_CFG);
    if(pol){
        regVal |= ICM20948_INT1_ACTL;
    }
    else{
        regVal &= ~ICM20948_INT1_ACTL;
    }
    writeRegister8(0, ICM20948_INT_PIN_CFG, regVal);
}

void enableIntLatch(bool latch){
    regVal = readRegister8(0, ICM20948_INT_PIN_CFG);
    if(latch){
        regVal |= ICM20948_INT_1_LATCH_EN;
    }
    else{
        regVal &= ~ICM20948_INT_1_LATCH_EN;
    }
    writeRegister8(0, ICM20948_INT_PIN_CFG, regVal);
}

void enableClearIntByAnyRead(bool clearByAnyRead){
    regVal = readRegister8(0, ICM20948_INT_PIN_CFG);
    if(clearByAnyRead){
        regVal |= ICM20948_INT_ANYRD_2CLEAR;
    }
    else{
        regVal &= ~ICM20948_INT_ANYRD_2CLEAR;
    }
    writeRegister8(0, ICM20948_INT_PIN_CFG, regVal);
}

void setFSyncIntPolarity(ICM20948_intPinPol pol){
    regVal = readRegister8(0, ICM20948_INT_PIN_CFG);
    if(pol){
        regVal |= ICM20948_ACTL_FSYNC;
    }
    else{
        regVal &= ~ICM20948_ACTL_FSYNC;
    }
    writeRegister8(0, ICM20948_INT_PIN_CFG, regVal);
}

void enableInterrupt(ICM20948_intType intType){
    switch(intType){
        case ICM20948_FSYNC_INT:
            regVal = readRegister8(0, ICM20948_INT_PIN_CFG);
            regVal |= ICM20948_FSYNC_INT_MODE_EN;
            writeRegister8(0, ICM20948_INT_PIN_CFG, regVal); // enable FSYNC as interrupt pin
            regVal = readRegister8(0, ICM20948_INT_ENABLE);
            regVal |= 0x80;
            writeRegister8(0, ICM20948_INT_ENABLE, regVal); // enable wake on FSYNC interrupt
            break;

        case ICM20948_WOM_INT:
            regVal = readRegister8(0, ICM20948_INT_ENABLE);
            regVal |= 0x08;
            writeRegister8(0, ICM20948_INT_ENABLE, regVal);
            regVal = readRegister8(2, ICM20948_ACCEL_INTEL_CTRL);
            regVal |= 0x02;
            writeRegister8(2, ICM20948_ACCEL_INTEL_CTRL, regVal);
            break;

        case ICM20948_DMP_INT:
            regVal = readRegister8(0, ICM20948_INT_ENABLE);
            regVal |= 0x02;
            writeRegister8(0, ICM20948_INT_ENABLE, regVal);
            break;

        case ICM20948_DATA_READY_INT:
            writeRegister8(0, ICM20948_INT_ENABLE_1, 0x01);
            break;

        case ICM20948_FIFO_OVF_INT:
            writeRegister8(0, ICM20948_INT_ENABLE_2, 0x01);
            break;

        case ICM20948_FIFO_WM_INT:
            writeRegister8(0, ICM20948_INT_ENABLE_3, 0x01);
            break;
    }
}

void disableInterrupt(ICM20948_intType intType){
    switch(intType){
        case ICM20948_FSYNC_INT:
            regVal = readRegister8(0, ICM20948_INT_PIN_CFG);
            regVal &= ~ICM20948_FSYNC_INT_MODE_EN;
            writeRegister8(0, ICM20948_INT_PIN_CFG, regVal);
            regVal = readRegister8(0, ICM20948_INT_ENABLE);
            regVal &= ~(0x80);
            writeRegister8(0, ICM20948_INT_ENABLE, regVal);
            break;

        case ICM20948_WOM_INT:
            regVal = readRegister8(0, ICM20948_INT_ENABLE);
            regVal &= ~(0x08);
            writeRegister8(0, ICM20948_INT_ENABLE, regVal);
            regVal = readRegister8(2, ICM20948_ACCEL_INTEL_CTRL);
            regVal &= ~(0x02);
            writeRegister8(2, ICM20948_ACCEL_INTEL_CTRL, regVal);
            break;

        case ICM20948_DMP_INT:
            regVal = readRegister8(0, ICM20948_INT_ENABLE);
            regVal &= ~(0x02);
            writeRegister8(0, ICM20948_INT_ENABLE, regVal);
            break;

        case ICM20948_DATA_READY_INT:
            writeRegister8(0, ICM20948_INT_ENABLE_1, 0x00);
            break;

        case ICM20948_FIFO_OVF_INT:
            writeRegister8(0, ICM20948_INT_ENABLE_2, 0x00);
            break;

        case ICM20948_FIFO_WM_INT:
            writeRegister8(0, ICM20948_INT_ENABLE_3, 0x00);
            break;
    }
}

uint8_t readAndClearInterrupts(){
    uint8_t intSource = 0;
    regVal = readRegister8(0, ICM20948_I2C_MST_STATUS);
    if(regVal & 0x80){
        intSource |= 0x01;
    }
    regVal = readRegister8(0, ICM20948_INT_STATUS);
    if(regVal & 0x08){
        intSource |= 0x02;
    }
    if(regVal & 0x02){
        intSource |= 0x04;
    }
    regVal = readRegister8(0, ICM20948_INT_STATUS_1);
    if(regVal & 0x01){
        intSource |= 0x08;
    }
    regVal = readRegister8(0, ICM20948_INT_STATUS_2);
    if(regVal & 0x01){
        intSource |= 0x10;
    }
    regVal = readRegister8(0, ICM20948_INT_STATUS_3);
    if(regVal & 0x01){
        intSource |= 0x20;
    }
    return intSource;
}

bool checkInterrupt(uint8_t source, ICM20948_intType type){
    source &= type;
    return source;
}
void setWakeOnMotionThreshold(uint8_t womThresh, ICM20948_womCompEn womCompEn){
    regVal = readRegister8(2, ICM20948_ACCEL_INTEL_CTRL);
    if(womCompEn){
        regVal |= 0x01;
    }
    else{
        regVal &= ~(0x01);
    }
    writeRegister8(2, ICM20948_ACCEL_INTEL_CTRL, regVal);
    writeRegister8(2, ICM20948_ACCEL_WOM_THR, womThresh);
}

/***************** FIFO ******************/

void enableFifo(bool fifo){
    regVal = readRegister8(0, ICM20948_USER_CTRL);
    if(fifo){
        regVal |= ICM20948_FIFO_EN;
    }
    else{
        regVal &= ~ICM20948_FIFO_EN;
    }
    writeRegister8(0, ICM20948_USER_CTRL, regVal);
}

void setFifoMode(ICM20948_fifoMode mode){
    if(mode){
        regVal = 0x01;
    }
    else{
        regVal = 0x00;
    }
    writeRegister8(0, ICM20948_FIFO_MODE, regVal);
}

void startFifo(ICM20948_fifoType fifo){
    fifoType = fifo;
    writeRegister8(0, ICM20948_FIFO_EN_2, fifoType);
}

void stopFifo(){
    writeRegister8(0, ICM20948_FIFO_EN_2, 0);
}

void resetFifo(){
    writeRegister8(0, ICM20948_FIFO_RST, 0x01);
    writeRegister8(0, ICM20948_FIFO_RST, 0x00);
}

int16_t getFifoCount(){
    int16_t regVal16 = (int16_t) readRegister16(0, ICM20948_FIFO_COUNT);
    return regVal16;
}

int16_t getNumberOfFifoDataSets(){
    int16_t numberOfSets = getFifoCount();

    if((fifoType == ICM20948_FIFO_ACC) || (fifoType == ICM20948_FIFO_GYR)){
        numberOfSets /= 6;
    }
    else if(fifoType==ICM20948_FIFO_ACC_GYR){
        numberOfSets /= 12;
    }

    return numberOfSets;
}

void findFifoBegin(){
    int16_t count = getFifoCount();
    int16_t start = 0;

    if((fifoType == ICM20948_FIFO_ACC) || (fifoType == ICM20948_FIFO_GYR)){
        start = count%6;
        for(int i=0; i<start; i++){
            readRegister8(0, ICM20948_FIFO_R_W);
        }
    }
    else if(fifoType==ICM20948_FIFO_ACC_GYR){
        start = count%12;
        for(int i=0; i<start; i++){
            readRegister8(0, ICM20948_FIFO_R_W);
        }
    }
}

/************** Magnetometer **************/

bool initMagnetometer(){
    enableI2CMaster();
    resetMag();
    reset_ICM20948();
    sleep(false);
    writeRegister8(2, ICM20948_ODR_ALIGN_EN, 1); // aligns ODR
    enableI2CMaster();


    if(!(whoAmIMag() == AK09916_WHO_AM_I)){
        return false;
    }

    setMagOpMode(AK09916_CONT_MODE_100HZ);

    return true;
}

int16_t whoAmIMag(){
    return readAK09916Register16(AK09916_WIA_1);
}

void setMagOpMode(AK09916_opMode opMode){
    writeAK09916Register8(AK09916_CNTL_2, opMode);
    delayicm(10);
    if(opMode!=AK09916_PWR_DOWN){
        enableMagDataRead(AK09916_HXL, 0x08);
    }
}

void resetMag(){
    writeAK09916Register8(AK09916_CNTL_3, 0x01);
    delayicm(100);
}


/************************************************
     Private Functions
 *************************************************/


void setClockToAutoSelect(){
    regVal = readRegister8(0, ICM20948_PWR_MGMT_1);
    regVal |= 0x01;
    writeRegister8(0, ICM20948_PWR_MGMT_1, regVal);
    delayicm(10);
}

xyzFloat correctAccRawValues(xyzFloat accRawVal){
    accRawVal.x = (accRawVal.x -(accOffsetVal.x / accRangeFactor)) / accCorrFactor.x;
    accRawVal.y = (accRawVal.y -(accOffsetVal.y / accRangeFactor)) / accCorrFactor.y;
    accRawVal.z = (accRawVal.z -(accOffsetVal.z / accRangeFactor)) / accCorrFactor.z;

    return accRawVal;
}

xyzFloat correctGyrRawValues(xyzFloat gyrRawVal){
    gyrRawVal.x -= (gyrOffsetVal.x / gyrRangeFactor);
    gyrRawVal.y -= (gyrOffsetVal.y / gyrRangeFactor);
    gyrRawVal.z -= (gyrOffsetVal.z / gyrRangeFactor);

    return gyrRawVal;
}


void switchBank(uint8_t newBank){
    if(newBank != currentBank)
    {
        currentBank = newBank;
        currentBank = currentBank<<4;
        RmCom_I2C_w_icm(ICM20948_REG_BANK_SEL, &currentBank, 1);
        //        _wire->beginTransmission(i2cAddress);
        //        _wire->write(ICM20948_REG_BANK_SEL);
        //        _wire->write(currentBank<<4);
        //        _wire->endTransmission();
    }
}

uint8_t writeRegister8(uint8_t bank, uint8_t reg, uint8_t val){
    switchBank(bank);
    uint8_t Ret=0;
    Ret = RmCom_I2C_w_icm(reg,&val,1);
    return Ret;
    //    _wire->beginTransmission(i2cAddress);
    //    _wire->write(reg);
    //    _wire->write(val);
    //
    //    return _wire->endTransmission();
}

uint8_t writeRegister16(uint8_t bank, uint8_t reg, int16_t val){

    uint8_t Ret=0;
    switchBank(bank);
    int8_t MSByte = (int8_t)((val>>8) & 0xFF);
    uint8_t LSByte = val & 0xFF;
    uint8_t WriteByte[2];

    WriteByte[0]=MSByte;
    WriteByte[1]=LSByte;
    Ret = RmCom_I2C_w_icm(reg,WriteByte,2);
    return Ret;

}

uint8_t readRegister8(uint8_t bank, uint8_t reg){
    switchBank(bank);
    uint8_t regValue = 0;

    RmCom_I2C_r_icm(reg,&regValue,1);
//    _wire->beginTransmission(i2cAddress);
//    _wire->write(reg);
//    _wire->endTransmission(false);
//    _wire->requestFrom(i2cAddress,1);
//    if(_wire->available()){
//        regValue = _wire->read();
//    }
    return regValue;
}

int16_t readRegister16(uint8_t bank, uint8_t reg){
    switchBank(bank);
    uint8_t MSByte = 0, LSByte = 0,regValue[2];
    int16_t reg16Val = 0;
    RmCom_I2C_r_icm(reg,&regValue,2);
    MSByte = regValue[0];
    LSByte = regValue[1];
//    _wire->beginTransmission(i2cAddress);
//    _wire->write(reg);
//    _wire->endTransmission(false);
//    _wire->requestFrom(i2cAddress,2);
//    if(_wire->available()){
//        MSByte = _wire->read();
//        LSByte = _wire->read();
//    }
    reg16Val = (MSByte<<8) + LSByte;
    return reg16Val;
}

void readAllData(uint8_t* data){
    switchBank(0);
    RmCom_I2C_r_icm(ICM20948_ACCEL_OUT, data, 20);

//    _wire->beginTransmission(i2cAddress);
//    _wire->write(ICM20948_ACCEL_OUT);
//    _wire->endTransmission(false);
//    _wire->requestFrom(i2cAddress,20);
//    if(_wire->available()){
//        for(int i=0; i<20; i++){
//            data[i] = _wire->read();
//        }
//    }
}

xyzFloat readICM20948xyzValFromFifo(){
    uint8_t MSByte = 0, LSByte = 0;
    xyzFloat xyzResult = {0.0, 0.0, 0.0};
    MSByte = readRegister8(0, ICM20948_FIFO_R_W);
    LSByte = readRegister8(0, ICM20948_FIFO_R_W);
    xyzResult.x = ((int16_t)((MSByte<<8) + LSByte)) * 1.0;
    MSByte = readRegister8(0, ICM20948_FIFO_R_W);
    LSByte = readRegister8(0, ICM20948_FIFO_R_W);
    xyzResult.y = ((int16_t)((MSByte<<8) + LSByte)) * 1.0;
    MSByte = readRegister8(0, ICM20948_FIFO_R_W);
    LSByte = readRegister8(0, ICM20948_FIFO_R_W);
    xyzResult.z = ((int16_t)((MSByte<<8) + LSByte)) * 1.0;
    return xyzResult;
}

void writeAK09916Register8(uint8_t reg, uint8_t val){
    writeRegister8(3, ICM20948_I2C_SLV0_ADDR, AK09916_ADDRESS); // write AK09916
    writeRegister8(3, ICM20948_I2C_SLV0_REG, reg); // define AK09916 register to be written to
    writeRegister8(3, ICM20948_I2C_SLV0_DO, val);
}


uint8_t readAK09916Register8(uint8_t reg){
    enableMagDataRead(reg, 0x01);
    enableMagDataRead(AK09916_HXL, 0x08);
    regVal = readRegister8(0, ICM20948_EXT_SLV_SENS_DATA_00);
    return regVal;
}

int16_t readAK09916Register16(uint8_t reg){
    int16_t regValue = 0;
    enableMagDataRead(reg, 0x02);
    regValue = readRegister16(0, ICM20948_EXT_SLV_SENS_DATA_00);
    enableMagDataRead(AK09916_HXL, 0x08);
    return regValue;
}

uint8_t reset_ICM20948(){
    uint8_t ack = writeRegister8(0, ICM20948_PWR_MGMT_1, ICM20948_RESET);
    delayicm(10);  // wait for registers to reset
    return (ack == 0);
}

void enableI2CMaster(){
    writeRegister8(0, ICM20948_USER_CTRL, ICM20948_I2C_MST_EN); //enable I2C master
    writeRegister8(3, ICM20948_I2C_MST_CTRL, 0x07); // set I2C clock to 345.60 kHz
    delayicm(10);
}

void enableMagDataRead(uint8_t reg, uint8_t bytes){
    writeRegister8(3, ICM20948_I2C_SLV0_ADDR, AK09916_ADDRESS | AK09916_READ); // read AK09916
    writeRegister8(3, ICM20948_I2C_SLV0_REG, reg); // define AK09916 register to be read
    writeRegister8(3, ICM20948_I2C_SLV0_CTRL, 0x80 | bytes); //enable read | number of byte
    delayicm(10);
}

void readicm20948dta()
{
	        readSensor();
	        xyzFloat accRaw = getAccRawValues();
	        xyzFloat corrAccRaw = getCorrectedAccRawValues();       //Acc Corrected value
	        xyzFloat gVal = getGValues();
	        float resultantG = getResultantG(gVal);
	        //xyzFloat gyrRaw = getCorrectedGyrRawValues();
	        xyzFloat gyr = getGyrValues();                          //Gyro Corrected value
	        xyzFloat magValue = getMagValues();                     // returns magnetic flux density [µT]


	        gbAccActual.x= corrAccRaw.x;
	        gbAccActual.y= corrAccRaw.y;
	        gbAccActual.z= corrAccRaw.z;

	        gbAccGvalActual.x=gVal.x;
	        gbAccGvalActual.y=gVal.y;
	        gbAccGvalActual.z=gVal.z;

	        gbgyroActual.x =gyr.x;
	        gbgyroActual.y =gyr.y;
	        gbgyroActual.z =gyr.z;

	        gbMagActual.x =magValue.x;
	        gbMagActual.y =magValue.y;
	        gbMagActual.z =magValue.z;


#if ENABLE_SENSORS_SERIAL_PRINTS
	       // demo_printf(  "Acc Corrected : x: %f, y: %f,  z: %f \r\n",corrAccRaw.x,corrAccRaw.y,corrAccRaw.z );
	        demo_printf(  "\r\nG values : x: %f, y: %f,  z: %f \r\n",gVal.x,gVal.y,gVal.z );
	       // demo_printf(  "Resultant G : %f\r\n",resultantG );
	        demo_printf(  "Gyro Data : x: %f, y: %f,  z: %f \r\n",gyr.x,gyr.y,gyr.z );
	        demo_printf(  "Mag Data : x: %f, y: %f,  z: %f \r\n",magValue.x,magValue.y,magValue.z );
#endif

	tx_thread_sleep(100);
	//R_BSP_SoftwareDelay(100,BSP_DELAY_MILLISECS);
}

void icmInit()
{
	    whoAmI();
	    icm20948_init();
	    initMagnetometer();
	    //    setAccOffsets(-16330.0, 16450.0, -16600.0, 16180.0, -16640.0, 16560.0);
	    delayicm(1000);
	    autoOffsets();
	    setMagOpMode(AK09916_CONT_MODE_20HZ);
	    setAccRange(ICM20948_ACC_RANGE_4G);
	    setAccDLPF(ICM20948_DLPF_6);

	    setAccSampleRateDivider(10);

	    setGyrRange(ICM20948_GYRO_RANGE_250);
	    setGyrDLPF(ICM20948_DLPF_6);

}


void getICMData(st_icm20948_data_t *icmData)
{
	icmData->accel.xaxis =gbAccGvalActual.x;
	icmData->accel.yaxis=gbAccGvalActual.y;
	icmData->accel.zaxis=gbAccGvalActual.z;

    icmData->gyro.xaxis =gbgyroActual.x;
    icmData->gyro.yaxis =gbgyroActual.y;
    icmData->gyro.zaxis =gbgyroActual.z;

    icmData->mag.xaxis =gbMagActual.x;
    icmData->mag.yaxis =gbMagActual.y;
    icmData->mag.zaxis =gbMagActual.z;
}
