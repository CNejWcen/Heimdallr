/**************************************************************************************************
  Filename:       heartrateservice.h
  Revised:        $Date $
  Revision:       $Revision $

  Description:    This file contains the Heart Rate service definitions and
                  prototypes.
**************************************************************************************************/

#ifndef HEARTRATESERVICE_H
#define HEARTRATESERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "st_util.h"
/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
//#define HEART_SERV_UUID                 0x180D  // Heart Rate
#define HEARTBEAT_DATA_UUID                  HEARTRATE_MEAS_UUID
#define BLOOD_OXYGEN_DATA_UUID               BODY_SENSOR_LOC_UUID
#define HEART_CONF_UUID                      HEARTRATE_CTRL_PT_UUID
#define HEART_PERI_UUID                      0x2A3A

// Heart Rate Service Parameters
#define HEARTRATE_MEAS                      0
#define HEARTRATE_MEAS_CHAR_CFG             1
#define BLOOD_OXYGEN_MEAS                   2
#define HEARTRATE_COMMAND                   3
#define HEART_PERI                          4



// Maximum length of heart rate measurement characteristic
#define HEARTRATE_MEAS_MAX                  (ATT_MTU_SIZE -5)

// Values for flags
#define HEARTRATE_FLAGS_FORMAT_UINT16       0x01
#define HEARTRATE_FLAGS_CONTACT_NOT_SUP     0x00
#define HEARTRATE_FLAGS_CONTACT_NOT_DET     0x04
#define HEARTRATE_FLAGS_CONTACT_DET         0x06
#define HEARTRATE_FLAGS_ENERGY_EXP          0x08
#define HEARTRATE_FLAGS_RR                  0x10

// Values for sensor location
#define HEARTRATE_SENS_LOC_OTHER            0x00
#define HEARTRATE_SENS_LOC_CHEST            0x01
#define HEARTRATE_SENS_LOC_WRIST            0x02
#define HEARTRATE_SENS_LOC_FINGER           0x03
#define HEARTRATE_SENS_LOC_HAND             0x04
#define HEARTRATE_SENS_LOC_EARLOBE          0x05
#define HEARTRATE_SENS_LOC_FOOT             0x06

// Value for command characteristic
#define HEARTRATE_COMMAND_ENERGY_EXP        0x01

// ATT Error code
// Control point value not supported
#define HEARTRATE_ERR_NOT_SUP               0x80

// Heart Rate Service bit fields
#define HEARTRATE_SERVICE                   0x00000001

// Callback events
#define HEARTRATE_MEAS_NOTI_ENABLED         1
#define HEARTRATE_MEAS_NOTI_DISABLED        2
#define HEARTRATE_COMMAND_SET               3

/*********************************************************************
 * TYPEDEFS
 */

// Heart Rate Service callback function
//typedef void (*sensorCBs_t)(uint8 event);

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

/*********************************************************************
 * API FUNCTIONS 
 */

/*
 * Heart_AddService- Initializes the Heart Rate service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t Heart_AddService( uint32 services );

/*
 * Heart_Register - Register a callback function with the
 *          Heart Rate Service
 *
 * @param   pfnServiceCB - Callback function.
 */

extern bStatus_t Heart_RegisterAppCBs( sensorCBs_t *appCallbacks );

/*
 * Heart_SetParameter - Set a Heart Rate parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t Heart_SetParameter( uint8 param, uint8 len, void *value );
extern bStatus_t Heart_AsmParameter(uint8 param, uint8 *buf,void *value);
/*
 * Heart_GetParameter - Get a Heart Rate parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t Heart_GetParameter( uint8 param, void *value );

/*********************************************************************
 * @fn          Heart_MeasNotify
 *
 * @brief       Send a notification containing a heart rate
 *              measurement.
 *
 * @param       connHandle - connection handle
 * @param       pNoti - pointer to notification structure
 *
 * @return      Success or Failure
 */
extern bStatus_t Heart_MeasNotify( uint16 connHandle, attHandleValueNoti_t *pNoti );

/*********************************************************************
 * @fn          Heart_HandleConnStatusCB
 *
 * @brief       Heart Rate Service link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
extern void Heart_HandleConnStatusCB( uint16 connHandle, uint8 changeType );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HEARTRATESERVICE_H */
