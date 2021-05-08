/**************************************************************************************************
  Filename:       hal_sensor.h
  Revised:        $Date: 2013-03-26 07:47:25 -0700 (Tue, 26 Mar 2013) $
  Revision:       $Revision: 33597 $

  Description:    Interface to sensor driver shared code.

**************************************************************************************************/

#ifndef HAL_SENSOR_H
#define HAL_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "hal_types.h"

/*********************************************************************
 * CONSTANTS and MACROS
 */

/* Sensor bit values for self-test and DCDC conbtrol */
#define SS_IRTEMP                             0x01
#define SS_HUMID                              0x02
#define SS_ACC                                0x04
#define SS_GYRO                               0x08
#define SS_HeartBeat                          0x10

#define HIGH_SUPPLY_SENSOR_MAP                ( SS_IRTEMP | SS_HUMID | SS_GYRO)

/* Self test assertion; return FALSE (failed) if condition is not met */
#define SS_ASSERT(cond) st( if (!(cond)) return FALSE; )

/* Ative delay: 125 cycles ~1 msec */
#define SS_HAL_DELAY(n) st( { volatile uint32 i; for (i=0; i<(n); i++) { }; } )

/*********************************************************************
 * FUNCTIONS
 */
bool   HalSensorReadReg(uint8 addr, uint8 *pBuf, uint8 nBytes);
bool   HalSensorWriteReg(uint8 addr, uint8 *pBuf, uint8 nBytes);

void   HalDcDcControl(uint8 sensorID, bool powerOn);

/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HAL_SENSOR_H */
