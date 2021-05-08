/**************************************************************************************************
  Filename:       proxreporter.h
  Revised:        $Date: 2013-09-05 15:48:17 -0700 (Thu, 05 Sep 2013) $
  Revision:       $Revision: 35223 $

  Description:    This file contains Proximity - Reporter header file.

**************************************************************************************************/

#ifndef PROXIMITYREPORTER_H
#define PROXIMITYREPORTER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Profile Parameters
#define PP_LINK_LOSS_ALERT_LEVEL         0  // RW uint8 - Profile Attribute value
#define PP_IM_ALERT_LEVEL                1  // RW uint8 - Profile Attribute value
#define PP_TX_POWER_LEVEL                2  // RW int8 - Profile Attribute value

// Alert Level Values
#define PP_ALERT_LEVEL_NO               0x00
#define PP_ALERT_LEVEL_LOW              0x01
#define PP_ALERT_LEVEL_HIGH             0x02
  
// Proximity Profile Services bit fields
#define PP_LINK_LOSS_SERVICE            0x00000001 // Link Loss Service
#define PP_IM_ALETR_SERVICE             0x00000002 // Immediate Alert Service
#define PP_TX_PWR_LEVEL_SERVICE         0x00000004 // Tx Power Level Service

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when the device has been started.  Callback event to 
// the Notify of an attribute change.
typedef void (*ppAttrChange_t)( uint8 attrParamID );

typedef struct
{
  ppAttrChange_t        pfnAttrChange;  // Whenever the Link Loss Alert attribute changes
} proxReporterCBs_t;

/*********************************************************************
 * API FUNCTIONS 
 */
 
/*
 * ProxReporter_InitService- Initializes the Proximity Reporter service by
 *          registering GATT attributes with the GATT server. Only call
 *          this function once.

 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t ProxReporter_AddService( uint32 services );

/*
 * ProxReporter_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t ProxReporter_RegisterAppCBs( proxReporterCBs_t *appCallbacks );


/*
 * ProxReporter_SetParameter - Set a Proximity Reporter parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t ProxReporter_SetParameter( uint8 param, uint8 len, void *value );
  
/*
 * ProxReporter_GetParameter - Get a Proximity Reporter parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t ProxReporter_GetParameter( uint8 param, void *value );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PROXIMITYREPORTER_H */
