/**************************************************************************************************
  Filename:       testservice.c
  Revised:        $Date: 2013-05-06 13:33:47 -0700 (Mon, 06 May 2013) $
  Revision:       $Revision: 34153 $

  Description:    Flash Profiles.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "flashprofile.h"
#include "st_util.h"


/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Test GATT Profile Service UUID
CONST uint8 flashServUUID[TI_UUID_SIZE] =
{
  TI_UUID(FLASH_SERV_UUID)
};

// Data Characteristic UUID
CONST uint8 flashDataUUID[TI_UUID_SIZE] =
{
  TI_UUID(FLASH_DATA_UUID)
};

// Config Characteristic UUID
CONST uint8 flashConfUUID[TI_UUID_SIZE] =
{
  TI_UUID(FLASH_CONF_UUID)
};


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static flashCBs_t *flash_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Test Profile Service attribute
static CONST gattAttrType_t flashService = { ATT_BT_UUID_SIZE, flashServUUID };

// Test Profile Data Characteristic Properties
static uint8 flashDataProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Test Profile Data Characteristic Value
static uint8 flashData[FLASH_DATA_LEN] = {0,0,0,0, 0,0,0,0};

// Test Characteristic Configuration
static gattCharCfg_t flashDataConfig[GATT_MAX_NUM_CONN];

// Test Profile Data Characteristic User Description
static uint8 flashDataUserDesp[] = "Flash Data\0";

// Test Profile Config Characteristic Properties
static uint8 flashConfProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Test Profile Config Characteristic Value
static uint8 flashConf = 0x00;

// Test Profile Config Characteristic User Description
static uint8 flashConfUserDesp[] = "Falsh Config\0";


/*********************************************************************
 * Profile Attributes - Table
 */
static gattAttribute_t flashAttrTbl[] =
{
  // Test Profile Service
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&flashService                     /* pValue */
  },

    // Data Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &flashDataProps
    },

      // Data Characteristic Value
      {
        { TI_UUID_SIZE, flashDataUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        flashData
      },

      // Data Characteristic configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)flashDataConfig
      },
      
      // Data Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        flashDataUserDesp
      },
      
    // Config Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &flashConfProps
    },

      // Config Characteristic Value
      {
        { TI_UUID_SIZE, flashConfUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &flashConf
      },

      // Config Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        flashConfUserDesp
      },
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 flash_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t flash_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset );
static void flash_HandleConnStatusCB( uint16 connHandle, uint8 changeType );

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Test Profile Service Callbacks
CONST gattServiceCBs_t flashCBs =
{
  flash_ReadAttrCB,  // Read callback function pointer
  flash_WriteAttrCB, // Write callback function pointer
  NULL              // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Test_AddService
 *
 * @brief   Initializes the Test Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t Flash_AddService( uint32 services )
{
  uint8 status = SUCCESS;

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, flashDataConfig );

  // Register with Link DB to receive link status change callback
  VOID linkDB_Register( flash_HandleConnStatusCB );

  if ( services & FLASH_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( flashAttrTbl,
                                          GATT_NUM_ATTRS( flashAttrTbl ),
                                          &flashCBs );
  }

  return ( status );
}

/*********************************************************************
 * @fn      Test_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t Flash_RegisterAppCBs( flashCBs_t *appCallbacks )
{
  if ( flash_AppCBs == NULL )
  {
    if ( appCallbacks != NULL )
    {
      flash_AppCBs = appCallbacks;
    }

    return ( SUCCESS );
  }

  return ( bleAlreadyInRequestedMode );

}

/*********************************************************************
 * @fn      FlashProfile_SetParameter
 *
 * @brief   Set a Test Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Flash_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case FLASH_DATA:
      if ( len == FLASH_DATA_LEN )
      {
        VOID osal_memcpy( flashData, value, FLASH_DATA_LEN );
        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( flashDataConfig, flashData, FALSE,
                                   flashAttrTbl, GATT_NUM_ATTRS( flashAttrTbl ),
                                   INVALID_TASK_ID );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case FLASH_CONF:
      if(len == sizeof ( uint8 ) )
      {
        flashConf = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      FlashProfile_GetParameter
 *
 * @brief   Get a FLASH Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Flash_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case FLASH_DATA:
      VOID osal_memcpy (value, flashData, FLASH_DATA_LEN );
      break;

    case FLASH_CONF:
      *((uint8*)value) = flashConf;
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn          test_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 *
 * @return      Success or Failure
 */
static uint8 flash_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen )
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

 /* // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
*/
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
/*
  if (utilExtractUuid16(pAttr,&uuid) == FAILURE)
  {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }
*/
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {    
    // 16-bit UUID
    uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads
      case FLASH_DATA_UUID:
        *pLen = FLASH_DATA_LEN;
        VOID osal_memcpy( pValue, pAttr->pValue, FLASH_DATA_LEN );
        break;

      case FLASH_CONF_UUID:
        *pLen = 1;
        pValue[0] = *pAttr->pValue;
        break;

      default:
        // Should never get here!
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}

/*********************************************************************
 * @fn      flash_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 *
 * @return  Success or Failure
 */
static bStatus_t flash_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset )
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
/*
  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE)
  {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }
*/
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
      uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
      switch ( uuid )
      {
        case FLASH_CONF_UUID:
          // Validate the value
          // Make sure it's not a blob oper
          if ( offset == 0 )
          {
            if ( len != 1 )
            {
              status = ATT_ERR_INVALID_VALUE_SIZE;
            }
          }
          else
          {
            status = ATT_ERR_ATTR_NOT_LONG;
          }

          // Write the value
          if ( status == SUCCESS )
          {
            uint8 *pCurValue = (uint8 *)pAttr->pValue;
            *pCurValue = pValue[0];

//            if( pAttr->pValue == &flashConf )
            {
              notifyApp = FLASH_CONF;
            }
          }
          break;

        case GATT_CLIENT_CHAR_CFG_UUID:
          status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                  offset, GATT_CLIENT_CFG_NOTIFY );
          break;

        default:
          // Should never get here! (characteristics 2 and 4 do not have write permissions)
          status = ATT_ERR_ATTR_NOT_FOUND;
          break;
      }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }  

  // If a charactersitic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && flash_AppCBs && flash_AppCBs->pfnFlashChange )
  {
    flash_AppCBs->pfnFlashChange( notifyApp );
  }

  return ( status );
}

/*********************************************************************
 * @fn          flash_HandleConnStatusCB
 *
 * @brief       Flash Profile link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
static void flash_HandleConnStatusCB( uint16 connHandle, uint8 changeType )
{
  // Make sure this is not loopback connection
  if ( connHandle != LOOPBACK_CONNHANDLE )
  {
    // Reset Client Char Config if connection has dropped
    if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
         ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) &&
           ( !linkDB_Up( connHandle ) ) ) )
    {
      GATTServApp_InitCharCfg( connHandle, flashDataConfig );
    }
  }
}


/*********************************************************************
*********************************************************************/
