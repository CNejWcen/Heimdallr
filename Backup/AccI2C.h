/*************
 * 加速度计Accelerator通过I2C模式驱动
*/
#ifndef __ACC_I2C_
#define __ACC_I2C_

#include "hal_types.h"
#ifdef BMA250

#define DEVICE_ADDR     0x18
//#define DEVICE_WRITE    0x30
//#define DEVICE_READ     0x31

//3-axis accelerator
#define ACCD_AXIS       0x02
#define ACC_X_LSB       0x02
#define ACC_X_MSB       0x03
#define ACC_Y_LSB       0x04
#define ACC_Y_MSB       0x05
#define ACC_Z_LSB       0x06
#define ACC_Z_MSB       0x07

//read register
#define ACCD_TEMP       0x08            //0x00 ~ 23摄氏度
#define ACCD_INTST0     0x09
#define ACCD_INTST1     0x0A
#define ACCD_INTST2     0x0B
#define ACCD_INTST3     0x0C
#define ACCD_INTST1     0x0A

#endif

#ifdef BMA250
void AccGetAllAxis(uint8 *data);
#endif






#define MC3230
#ifdef MC3230


#define DEVICE_ADDR     0x4C
//#define DEVICE_WRITE    0x98
//#define DEVICE_READ     0x99

//3-axis accelerator
#define XOUT_REG        0x00
#define YOUT_REG        0x01
#define ZOUT_REG        0x02
#define TILT_REG        0x03
#define OP_STAT         0x04

//write register
#define SC_REG          0x05
#define INT_REG         0x06
#define MODE_REG        0x07
#define SAMPR_REG       0x08
#define TAPEN_REG       0x09
#define TAPP_REG        0x0A
#define DROP_REG        0x0B
#define SHDB_REG        0x0C

#define XOFFL           0x21
#define XOFFH           0x22
#define YOFFL           0x23
#define YOFFH           0x24
#define ZOFFL           0x25
#define ZOFFH           0x26

#define PCCODE          0x3B        

void AccInit();
void AccStop();

uint8 AccRead(uint8 reg_addr,uint8 *rbuf);
void AccWrite(uint8 reg_addr,uint8 data);

uint8 AccGetOPSTAT();
void AccSetting();


uint8 AccGetXAxis();
uint8 AccGetYAxis();
uint8 AccGetZAxis();
uint8 AccGetTilt();

#endif
#endif
