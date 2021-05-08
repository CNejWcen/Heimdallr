/**************************************************************************************************
  Filename:       hal_sensor.c
  Revised:        $Date: 2012-09-21 06:30:38 -0700 (Fri, 21 Sep 2012) $
  Revision:       $Revision: 31581 $

  Description:    This file contains code that is common to all sensor drivers.

**************************************************************************************************/

/* ------------------------------------------------------------------------------------------------
*                                          Includes
* ------------------------------------------------------------------------------------------------
*/
#include "hal_board_cfg.h"
#include "hal_drivers.h"
#include "hal_sensor.h"
#include "hal_i2c.h"
#include "hal_sht.h"
#include "hal_mpu.h"
#include "hal_dbflash.h"

/* ------------------------------------------------------------------------------------------------
*                                           Macros and constants
* ------------------------------------------------------------------------------------------------
*/
#define N_TESS_RUNS                           10

/* ------------------------------------------------------------------------------------------------
*                                           Local Variables
* ------------------------------------------------------------------------------------------------
*/      
static uint8 buffer[24];
static uint8 halSensorEnableMap;

/**************************************************************************************************
 * @fn          HalSensorReadReg
 *
 * @brief       This function implements the I2C protocol to read from a sensor. The sensor must
 *              be selected before this routine is called.
 *
 * @param       addr - which register to read
 * @param       pBuf - pointer to buffer to place data
 * @param       nBytes - numbver of bytes to read
 *
 * @return      TRUE if the required number of bytes are reveived
 **************************************************************************************************/
bool HalSensorReadReg(uint8 addr, uint8 *pBuf, uint8 nBytes)
{
  uint8 i = 0;
  if ( HalI2CWrite(1,&addr) == 1)
  {
    i = HalI2CRead(nBytes,pBuf);
  }

  return i == nBytes;
}

/**************************************************************************************************
* @fn          HalSensorWriteReg
* @brief       This function implements the I2C protocol to write to a sensor. he sensor must
*              be selected before this routine is called.
*
* @param       addr - which register to write
* @param       pBuf - pointer to buffer containing data to be written
* @param       nBytes - number of bytes to write
*
* @return      TRUE if successful write
*/
bool HalSensorWriteReg(uint8 addr, uint8 *pBuf, uint8 nBytes)
{
  uint8 i;
  uint8 *p = buffer;

  /* Copy address and data to local buffer for burst write */
  *p++ = addr;
  for (i = 0; i < nBytes; i++)
  {
    *p++ = *pBuf++;
  }
  nBytes++;

  /* Send address and data */
  i = HalI2CWrite(nBytes, buffer);

  return (i == nBytes);
}


/*********************************************************************
 * @fn      HalDcDcControl
 *
 * @brief   Control the DCDC converter based on what sensors are enabled.
 *
 * @descr   This applies to IR Temp, Humidity. If any of those
 *          are used, the DCDC-converter must be bypassed.
 *
 * @param   sensorID - bit indicating one sensor
 *
 * @param   powerOn - indicate whether the sensor is turned on or off
 *
 * @return  none
 ******************************************************************/
void HalDcDcControl(uint8 sensorID, bool powerOn)
{
  if (powerOn)
  {
    halSensorEnableMap |= sensorID;
  }
  else
  {
    halSensorEnableMap &= ~sensorID;
  }

  if (halSensorEnableMap & HIGH_SUPPLY_SENSOR_MAP)
  {
    /* Bypass DCDC */
    DCDC_SBIT = 0;
  }
  else
  {
    /*          Enable DCDC        */
    //  DCDC_SBIT = 1;
    DCDC_SBIT = 0;                      //      Liber:  First to test whether 2.1V is available for DCDC
  }
}

/*********************************************************************
*********************************************************************/
