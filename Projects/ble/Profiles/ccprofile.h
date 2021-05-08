/**************************************************************************************************
  Filename:       ccservice.h
  Revised:        $Date: 2010-08-06 08:56:11 -0700 (Fri, 06 Aug 2010) $
  Revision:       $Revision: 23333 $

  Description:    This file contains the proprietary Connection Control Service
				          Prototypes.
**************************************************************************************************/

#ifndef CCSERVICE_H
#define CCSERVICE_H

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

// Service Parameters
#define CCSERVICE_CHAR1                 0  // R   - connection parameters (6 bytes)
#define CCSERVICE_CHAR2                 1  // W   - requested connection parameters
#define CCSERVICE_CHAR3                 2  // W   - disconnect request

// Service UUID
#define CCSERVICE_SERV_UUID             0xCCC0
#define CCSERVICE_CHAR1_UUID            0xCCC1 // F000CCC1-0451-4000-B000-00000000-0000
#define CCSERVICE_CHAR2_UUID            0xCCC2
#define CCSERVICE_CHAR3_UUID            0xCCC3

// Connection Control Services bit fields
#define CCSERVICE                       0x00000001

// Length of Characteristics in bytes
#define CCSERVICE_CHAR1_LEN             6
#define CCSERVICE_CHAR2_LEN             8
#define CCSERVICE_CHAR3_LEN             1

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*ccChange_t)( uint8 paramID );

typedef struct
{
  ccChange_t        pfnCcChange;  // Called when characteristic value changes
} ccCBs_t;



/*********************************************************************
 * API FUNCTIONS
 */


/*
 * CcService_AddService- Initializes the Simple GATT Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t CcService_AddService( uint32 services );

/*
 * CcService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t CcService_RegisterAppCBs( ccCBs_t *appCallbacks );

/*
 * CcService_SetParameter - Set a Simple GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t CcService_SetParameter( uint8 param, uint8 len, void *value );

/*
 * CcService_GetParameter - Get a Simple GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t CcService_GetParameter( uint8 param, void *value );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* CCSERVICE_H */
