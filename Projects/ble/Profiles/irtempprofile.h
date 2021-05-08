/**************************************************************************************************
  Filename:       irtempservice.h
  Revised:        $Date: 2013-08-23 11:45:31 -0700 (Fri, 23 Aug 2013) $
  Revision:       $Revision: 35100 $

  Description:    IR temperature service definitions and prototypes

**************************************************************************************************/

#ifndef IRTEMPSERVICE_H
#define IRTEMPSERVICE_H

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
#define IRTEMP_SERV_UUID                0xAA00  // F000AA00-0451-4000-B000-00000000-0000
#define IRTEMPERATURE_DATA_UUID         0xAA01
#define IRTEMPERATURE_CONF_UUID         0xAA02
#define IRTEMPERATURE_PERI_UUID         0xAA03

// Sensor Profile Services bit fields
#define IRTEMPERATURE_SERVICE           0x00000001

// Length of sensor data in bytes
#define IRTEMPERATURE_DATA_LEN          4

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
 * IRTemp_AddService- Initializes the Sensor GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t IRTemp_AddService( uint32 services );

/*
 * IRTemp_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t IRTemp_RegisterAppCBs( sensorCBs_t *appCallbacks );

/*
 * IRTemp_SetParameter - Set a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t IRTemp_SetParameter( uint8 param, uint8 len, void *value );
extern bStatus_t IRTemp_AsmParameter(uint8 param, uint8 *buf,void *value);

/*
 * IRTemp_GetParameter - Get a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t IRTemp_GetParameter( uint8 param, void *value );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* IRTEMPSERVICE_H */

