/**************************************************************************************************
  Filename:       st_util.h
  Revised:        $Date: 2013-09-24 07:55:13 -0700 (Tue, 24 Sep 2013) $
  Revision:       $Revision: 35432 $

  Description:    Utilties for Sensor Tag services


**************************************************************************************************/

#ifndef ST_UTIL_H
#define ST_UTIL_H

#include "bcomdef.h"
#include "gatt.h"
/*********************************************************************
 * MACROS
 */
#ifdef GATT_TI_UUID_128_BIT

// TI Base 128-bit UUID: F000XXXX-0451-4000-B000-000000000000
#define TI_UUID_SIZE        ATT_UUID_SIZE
#define TI_UUID(uuid)       TI_BASE_UUID_128(uuid)

#else

// Using 16-bit UUID
#define TI_UUID_SIZE        ATT_BT_UUID_SIZE
#define TI_UUID(uuid)       LO_UINT16(uuid), HI_UINT16(uuid)

#endif

// Profile Parameters
#define SENSOR_DATA               0       // RN uint8 - Profile Attribute value
#define SENSOR_CONF               1       // RW uint8 - Profile Attribute value
#define SENSOR_PERI               2       // RW uint8 - Profile Attribute value
#define SENSOR_CALB               3       // RW uint8 - Profile Attribute value

// Data readout periods (range 100 - 2550 ms)
#define SENSOR_MIN_UPDATE_PERIOD        100 // Minimum 100 milliseconds
#define SENSOR_PERIOD_RESOLUTION         10 // Resolution 10 milliseconds

// Common values for turning a sensor on and off + config/status
#define ST_CFG_SENSOR_DISABLE                 0x00
#define ST_CFG_SENSOR_ENABLE                  0x01
#define ST_CFG_CALIBRATE                      0x02
#define ST_CFG_ERROR                          0xFF


/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*sensorChange_t)( uint8 paramID );

typedef struct
{
  sensorChange_t        pfnSensorChange;  // Called when characteristic value changes
} sensorCBs_t;


/*-------------------------------------------------------------------
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      utilExtractUuid16
 *
 * @brief   Extracts a 16-bit UUID from a GATT attribute
 *
 * @param   pAttr - pointer to attribute
 *
 * @param   pValue - pointer to UUID to be extracted
 *
 * @return  Success or Failure
 */
bStatus_t utilExtractUuid16(gattAttribute_t *pAttr, uint16 *pValue);

#endif /* ST_UTIL_H */

