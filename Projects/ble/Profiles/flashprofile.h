/**************************************************************************************************
  Filename:       testservice.h
  Revised:        $Date: 2013-09-05 15:48:17 -0700 (Thu, 05 Sep 2013) $
  Revision:       $Revision: 35223 $

  Description:    Test service definitions and prototypes

**************************************************************************************************/

#ifndef TESTSERVICE_H
#define TESTSERVICE_H

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

// Test Service Parameters
#define FLASH_DATA                        0      // RW uint8 - Profile Attribute value
#define FLASH_CONF                        1      // RW uint8 - Profile Attribute value

// Service UUID
#define FLASH_SERV_UUID                  0xFFF0 // 0000FFF0-0451-4000-B000-00000000-0000
#define FLASH_DATA_UUID                  0xFFF1
#define FLASH_CONF_UUID                  0xFFF2

// Test Profile Services bit fields
#define FLASH_SERVICE                    0x00000001

  
#define FL_STOP_TRANSFER                0
#define FL_START_TRANSFER               1
  
// Test Data Length
#define FLASH_DATA_LEN                   8

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */
/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*flashChange_t)( uint8 paramID );

typedef struct
{
  flashChange_t        pfnFlashChange;  // Called when characteristic value changes
} flashCBs_t;


/*********************************************************************
 * API FUNCTIONS
 */

/*
 * Test_AddService- Initializes the Test Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t Flash_AddService( uint32 services );

/*
 * Test_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t Flash_RegisterAppCBs( flashCBs_t *appCallbacks );

/*
 * Test_SetParameter - Set a Test Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    pValue - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t Flash_SetParameter( uint8 param, uint8 len, void *pValue );

/*
 * Test_GetParameter - Get a Test Profile parameter.
 *
 *    param - Profile parameter ID
 *    pValue - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t Flash_GetParameter( uint8 param, void *pValue );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* TESTPROFILE_H */
