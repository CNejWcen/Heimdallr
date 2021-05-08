/**************************************************************************************************
  Filename:       hal_mpu.c
  Revised:        $Date: 2014-04-18 10:20:18 +0800 (Fri, 04 Apr 2014) $
  Revision:       $Revision: 10001 $

  Description:    This file contains the driver of MPU6500.

**************************************************************************************************/


#include "hal_mpu.h"
#include "hal_drivers.h"


/*************************************************************************************************
*       @fun MPU_Init()
*       
*       @brief  MPU_CS useless!!!!!!!!!!!!!!!!!!!!!Test for low level ?!
*
*       @return success true
************************************************************************************************/
bool MPU_Init()
{
  P0DIR |= 0x10;
  P0SEL &= ~0x10;
 // MPU_CS = MPU_CS_ENABLED;
  
  MPU_Select();
  
/*if(!MPU6500_TEST())
  {
    return false;
  }
*/
  
//  writeMpuByte(MPU_FIFO_EN,     0x00);
  writeMpuByte(MPU_CONFIG,      0x00);                //FIFO disable Ext sync
  writeMpuByte(MPU_GYRO_CONFIG, 0x08);                //500dps;低量程用于高精度测量慢速运动，高量程则用于测量超快速的手势和运动。
  writeMpuByte(MPU_ACCEL_CONFIG,0x08);                //4g
  writeMpuByte(MPU_ACCEL_CONFIG2,0x01);               //0000 0001
  writeMpuByte(MPU_PWR_MGMT_1,  0x00);                  
  writeMpuByte(MPU_PWR_MGMT_2,  0x00);
  
  return true;
}


void MPU_Select()
{
  i2cClock_t clockRate = i2cClock_267KHZ;
  HalI2CInit(MPU6500_ADDR,clockRate);
}
/*************************************************************************************************
*       @fun getGYRO(uint16 *gyro)
*       
*       @brief  
*
*       @param  gyro uint16 gyro[3],x,y,z
*       
************************************************************************************************/
void getGYRO(uint16 *gyro)
{
  uint8 buf;
  readMpuByte(MPU_GYRO_XOUT_H,&buf);
  gyro[0] = buf << 8;
  readMpuByte(MPU_GYRO_XOUT_L,&buf);
  gyro[0]+= buf;
  readMpuByte(MPU_GYRO_YOUT_H,&buf);
  gyro[1] = buf << 8;
  readMpuByte(MPU_GYRO_YOUT_L,&buf);
  gyro[1]+= buf;
  readMpuByte(MPU_GYRO_ZOUT_H,&buf);
  gyro[2] = buf << 8;
  readMpuByte(MPU_GYRO_ZOUT_L,&buf);
  gyro[2]+= buf;
}
/*************************************************************************************************
*       @fun getACCEL(uint16 *accel)
*       
*       @brief  
*
*       @param  accel uint16 accel[3],x,y,z
*       
************************************************************************************************/
void getACCEL(uint16 *accel)
{
  uint8 buf;
  readMpuByte(MPU_ACCEL_XOUT_H,&buf);
  accel[0] = buf << 8;
  readMpuByte(MPU_ACCEL_XOUT_L,&buf);
  accel[0]+= buf;
  readMpuByte(MPU_ACCEL_YOUT_H,&buf);
  accel[1] = buf << 8;
  readMpuByte(MPU_ACCEL_YOUT_L,&buf);
  accel[1]+= buf; 
  readMpuByte(MPU_ACCEL_ZOUT_H,&buf);
  accel[2] = buf << 8;
  readMpuByte(MPU_ACCEL_ZOUT_L,&buf);
  accel[2]+= buf;
}
/*************************************************************************************************
*       @fun getTemp()
*       
*       @brief  get temp of the mpu6500 
*
*       @return temp_ret temp
************************************************************************************************/
uint16 getTemp()
{
  uint16 temp_ret;
  uint8 temp;
  readMpuByte(MPU_TEMP_OUT_H,&temp);
  temp_ret = temp << 8;
  readMpuByte(MPU_TEMP_OUT_H,&temp);
  temp_ret += temp;
  return temp_ret;
}

/*************************************************************************************************
*       @fun is_mpu6500()
*       
*       @brief  test is right?
*
*       @return who_am_i return 0x70 return true,or return false
************************************************************************************************/
bool MPU6500_TEST()
{
  MPU_Select();
  uint8 who;
  readMpuByte(MPU_WHO_AM_I,&who);
  MPU_Shutdown();
  
  if( who == 0x68 )
  {
    uprint("true");
    return true;
  }
  uprint("False");
  return false;
}


/*************************************************************************************************
*       @fun MPU_Shutdown()
*       
*       @brief  goto power save mode                    goto sleep
*
*       @return who_am_i return 0x70 return true,or return false
************************************************************************************************/
void MPU_Shutdown()
{
  writeMpuByte(MPU_LP_ACCEL_ODR,0x00);
  writeMpuByte(MPU_PWR_MGMT_1,  0x28);
  writeMpuByte(MPU_PWR_MGMT_2,  0x07);                  //not disable accel
  //writeMpuByte(MPU_PWR_MGMT_2,  0x2F);                //disable all
  
}


uint8 readMpuByte(uint8 reg_addr,uint8 *rbuf)
{
  HalI2CWrite(1,&reg_addr);
  HalI2CRead(1,rbuf);
  return 1;
}

void writeMpuByte(uint8 reg_addr,uint8 data)
{
  uint8 buf[]={reg_addr,data};
  HalI2CWrite(2,buf);
}

