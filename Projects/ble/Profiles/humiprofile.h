/**************************************************************************************************
  Filename:       humidityservice.h
  Revised:        $Date: 2013-08-23 11:45:31 -0700 (Fri, 23 Aug 2013) $
  Revision:       $Revision: 35100 $

  Description:    Humidity service definitions and prototypes

**************************************************************************************************/

#ifndef HUMIDITYSERVICE_H
#define HUMIDITYSERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "st_util.h"
  
/*********************************************************************
 * CONSTANTS
 */

// Service UUID
#define HUMI_SERV_UUID                  0xAA20  // F000AA20-0451-4000-B000-00000000-0000
#define HUMIDITY_DATA_UUID              0xAA21  
#define HUMIDITY_CONF_UUID              0xAA22
#define HUMIDITY_PERI_UUID              0xAA23

// Sensor Profile Services bit fields
#define HUMIDITY_SERVICE                0x00000004

// Length of sensor data in bytes
#define HUMIDITY_DATA_LEN               4

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * API FUNCTIONS
 */


/*
 * Humi_AddService- Initializes the Sensor GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t Humi_AddService( uint32 services );

/*
 * Humi_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t Humi_RegisterAppCBs( sensorCBs_t *appCallbacks );

/*
 * Humi_SetParameter - Set a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t Humi_SetParameter( uint8 param, uint8 len, void *value );
extern bStatus_t Humi_AsmParameter(uint8 param, uint8 *buf,void *value);
/*
 * Humi_GetParameter - Get a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t Humi_GetParameter( uint8 param, void *value );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HUMIDITYSERVICE_H */

