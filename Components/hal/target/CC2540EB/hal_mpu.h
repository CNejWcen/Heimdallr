/**************************************************************************************************
  Filename:       hal_mpu.h
  Revised:        $Date: 2014-04-18 10:20:18 +0800 (Fri, 04 Apr 2014) $

  Description:    This file contains the driver of MPU6050.
**************************************************************************************************/

#ifndef __HAL_MPU_H__
#define __HAL_MPU_H__

#ifdef __cplusplus
extern "C"
{
#endif
/**************************************************************************************************
 *                                           Includes
 **************************************************************************************************/
#include "hal_types.h"
#include "hal_timer.h"
#include "comdef.h"
#include "hal_drivers.h"
#include "hal_i2c.h"

/******************************************************************************
 * MACROS
 ******************************************************************************/
#define MPU6050 
#define MPU_ADDR 0x68

#define MPU6050_CHIPID 0x68
#define MPU6500_CHIPID 0x70

#ifdef MPU6500
	/*            Register Map             */
	#define MPU_SELF_TEST_X_GYRO            0x00
	#define MPU_SELF_TEST_Y_GYRO            0x01
	#define MPU_SELF_TEST_Z_GYRO            0x02
	#define MPU_SELF_TEST_X_ACCEL           0x0D
	#define MPU_SELF_TEST_Y_ACCEL           0x0E
	#define MPU_SELF_TEST_Z_ACCEL           0x0F
#elif defined(MPU6050)
	#define MPU_SELF_TEST_X            0x0D
	#define MPU_SELF_TEST_Y            0x0E
	#define MPU_SELF_TEST_Z            0x0F
	#define MPU_SELF_TEST_A            0x10
#endif
#ifdef MPU6050
	/*              MPU OFFSET              */              //The value in this register is added to the gyroscope sensor value before going into the sensor register.
	#define MPU_XG_OFFSET_H                 0x13
	#define MPU_XG_OFFSET_L                 0x14
	#define MPU_YG_OFFSET_H                 0x15
	#define MPU_YG_OFFSET_L                 0x16
	#define MPU_ZG_OFFSET_H                 0x17
	#define MPU_ZG_OFFSET_L                 0x18
	#define MPU_XA_OFFSET_H                 0x77
	#define MPU_XA_OFFSET_L                 0x78
	#define MPU_YA_OFFSET_H                 0x7A
	#define MPU_YA_OFFSET_L                 0x7B
	#define MPU_ZA_OFFSET_H                 0x7D
	#define MPU_ZA_OFFSET_L                 0x7E
#endif

/*              MPU Configuration       */
#define MPU_SMPLRT_DIV                  0x19                                    //Sample rate divider
#define MPU_CONFIG                      0x1A
#define MPU_GYRO_CONFIG                 0x1B
#define MPU_ACCEL_CONFIG                0x1C
#ifdef MPU6500
	#define MPU_ACCEL_CONFIG2               0x1D
	#define MPU_LP_ACCEL_ODR                0x1E
	#define MPU_WOM_THR                     0x1F
#endif
#define MPU_FIFO_EN                     0x23

/*              Auxiliary Sensor        */
#define MPU_I2C_MST_CTRL                0x24
#define MPU_I2C_MST_STATUS              0x36

/*              MPU Interrupt           */
#define MPU_INT_PIN_CFG                 0x37
#define MPU_INT_ENABLE                  0x38
#define MPU_INT_STATUS                  0x3A

/*              MPU Data                */
#define MPU_ACCEL_XOUT_H                0x3B
#define MPU_ACCEL_XOUT_L                0x3C
#define MPU_ACCEL_YOUT_H                0x3D
#define MPU_ACCEL_YOUT_L                0x3E
#define MPU_ACCEL_ZOUT_H                0x3F
#define MPU_ACCEL_ZOUT_L                0x40

#define MPU_TEMP_OUT_H                  0x41
#define MPU_TEMP_OUT_L                  0x42

#define MPU_GYRO_XOUT_H                 0x43
#define MPU_GYRO_XOUT_L                 0x44
#define MPU_GYRO_YOUT_H                 0x45
#define MPU_GYRO_YOUT_L                 0x46
#define MPU_GYRO_ZOUT_H                 0x47
#define MPU_GYRO_ZOUT_L                 0x48

/*              MPU Power Management    */
#define MPU_SIGNAL_PATH_RESET           0x68
#ifdef MPU6500
	#define MPU_ACCEL_INTEL_CTRL            0x69
#endif
#define MPU_USER_CTRL                   0x6A
#define MPU_PWR_MGMT_1                  0x6B
#define MPU_PWR_MGMT_2                  0x6C

/*              MPU Useless             */
#define MPU_FIFO_COUNT_H                0x72
#define MPU_FIFO_COUNT_L                0x73
#define MPU_FIFO_R_W                    0x74
#define MPU_WHO_AM_I                    0x75

bool MPU_Init();
void MPU_Select();

void MPU_Shutdown();

bool MPU_TEST();

uint16 getTemp();
void getACCEL(uint16 *accel);
void getGYRO(uint16 *gyro);

uint8 I2CRead(uint8 reg_addr,uint8 *rbuf);
void I2CWrite(uint8 reg_addr,uint8 data);

#ifdef __cplusplus
}
#endif

#endif


