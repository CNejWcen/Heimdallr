/**************************************************************************************************
  Filename:       accelerometer.c
  Revised:        $Date: 2011-11-21 15:26:10 -0800 (Mon, 21 Nov 2011) $
  Revision:       $Revision: 28439 $

  Description:    Accelerometer Profile


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
#include "hal_drivers.h"
#include "accelprofile.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        19

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Accelerometer Service UUID
CONST uint8 accServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_SERV_UUID), HI_UINT16(ACCEL_SERV_UUID)
};

// Accelerometer Enabler UUID
CONST uint8 accEnablerUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_ENABLER_UUID), HI_UINT16(ACCEL_ENABLER_UUID)
};

// Accelerometer Range UUID
CONST uint8 rangeUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_RANGE_UUID), HI_UINT16(ACCEL_RANGE_UUID)
};

// Accelerometer XYZ-Axis Data UUID
CONST uint8 xyzUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_XYZ_UUID), HI_UINT16(ACCEL_XYZ_UUID)
};

// Accelerometer Y-Axis Data UUID
CONST uint8 yUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_6D_UUID), HI_UINT16(ACCEL_6D_UUID)
};

// Accelerometer Z-Axis Data UUID
CONST uint8 zUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_CLICK_UUID), HI_UINT16(ACCEL_CLICK_UUID)
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
static accelCBs_t *accel_AppCBs = NULL;


/*********************************************************************
 * Profile Attributes - variables
 */

// Accelerometer Service attribute
static CONST gattAttrType_t accelService = { ATT_BT_UUID_SIZE, accServUUID };


// Enabler Characteristic Properties
static uint8 accelEnabledCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Enabler Characteristic Value
static uint8 accelEnabled = FALSE;

// Enabler Characteristic user description
static uint8 accelEnabledUserDesc[14] = "Accel Enable\0";


// Range Characteristic Properties
static uint8 accelRangeCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Range Characteristic Value
static uint16 accelRange = ACCEL_RANGE_2G;

// Range Characteristic user description
static uint8 accelRangeUserDesc[13] = "Accel Range\0";


// Accel Coordinate Characteristic Properties
static uint8 accelXYZCharProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
static uint8 accel6DCharProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
static uint8 accelClickProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Accel Coordinate Characteristics
static uint8 accelXYZCoordinates[6] = {0,0,0,0,0,0};
static int8 accel6Direction = 0;
static int8 accelClickAction = 0;

// Client Characteristic configuration. Each client has its own instantiation
// of the Client Characteristic Configuration. Reads of the Client Characteristic
// Configuration only shows the configuration for that client and writes only
// affect the configuration of that client.

// Accel Coordinate Characteristic Configs
static gattCharCfg_t accelXYZConfigCoordinates[GATT_MAX_NUM_CONN];
static gattCharCfg_t accel6DirectionConfig[GATT_MAX_NUM_CONN];
static gattCharCfg_t accelClickConfig[GATT_MAX_NUM_CONN];

// Accel Coordinate Characteristic user descriptions
static uint8 accelXYZCharUserDesc[22] = "Accel XYZ-Coordinate\0";
static uint8 accel6DCharUserDesc[20] = "Accel Y-Coordinate\0";
static uint8 accelClickCharUserDesc[20] = "Accel Z-Coordinate\0";


/*********************************************************************
 * Profile Attributes - Table
 */
static gattAttribute_t accelAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  // Accelerometer Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                   /* permissions */
    0,                                  /* handle */
    (uint8 *)&accelService                /* pValue */
  },
  
    // Accel Enabler Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accelEnabledCharProps 
    },

      // Accelerometer Enable Characteristic Value
      { 
        { ATT_BT_UUID_SIZE, accEnablerUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0,
        &accelEnabled 
      },

      // Accelerometer Enable User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0,
        (uint8*)&accelEnabledUserDesc 
      },

    // Accel Range Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accelRangeCharProps 
    },

      // Accelerometer Range Char Value
      { 
        { ATT_BT_UUID_SIZE, rangeUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0,
        (uint8*)&accelRange 
      },

      // Accelerometer Range User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0,
        accelRangeUserDesc 
      },
      
    // XYZ-Coordinate Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accelXYZCharProps 
    },
  
      // XYZ-Coordinate Characteristic Value
      { 
        { ATT_BT_UUID_SIZE, xyzUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        accelXYZCoordinates
      },
      
      //  XYZ-Coordinate  Characteristic configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)accelXYZConfigCoordinates 
      },

      // XYZ-Coordinate Characteristic User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        accelXYZCharUserDesc
      },  

   // 6D Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accel6DCharProps 
    },
  
      //6D Characteristic Value
      { 
        { ATT_BT_UUID_SIZE, yUUID },
         GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&accel6Direction 
      },
      
      // 6D Characteristic configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)accel6DirectionConfig
      },

      // Y6D Characteristic User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        accel6DCharUserDesc
      },

   // Click Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accelClickProps 
    },
  
      // Click Characteristic Value
      { 
        { ATT_BT_UUID_SIZE, zUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&accelClickAction
      },
      
      // Click Characteristic configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)accelClickConfig 
      },

      // Click Characteristic User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        accelClickCharUserDesc
      },  

};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 accel_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                               uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t accel_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                    uint8 *pValue, uint8 len, uint16 offset );

static void accel_HandleConnStatusCB( uint16 connHandle, uint8 changeType );

/*********************************************************************
 * PROFILE CALLBACKS
 */
//  Accelerometer Service Callbacks
CONST gattServiceCBs_t  accelCBs =
{
  accel_ReadAttrCB,  // Read callback function pointer
  accel_WriteAttrCB, // Write callback function pointer
  NULL               // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Accel_AddService
 *
 * @brief   Initializes the Accelerometer service by
 *          registering GATT attributes with the GATT server. Only
 *          call this function once.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t Accel_AddService( uint32 services )
{
  uint8 status = SUCCESS;

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, accelXYZConfigCoordinates );
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, accel6DirectionConfig );
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, accelClickConfig );

  // Register with Link DB to receive link status change callback
  VOID linkDB_Register( accel_HandleConnStatusCB );  

  if ( services & ACCEL_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( accelAttrTbl, 
                                          GATT_NUM_ATTRS( accelAttrTbl ),
                                          &accelCBs );
  }

  return ( status );
}

/*********************************************************************
 * @fn      Accel_RegisterAppCBs
 *
 * @brief   Does the profile initialization.  Only call this function
 *          once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t Accel_RegisterAppCBs( accelCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    accel_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

bStatus_t Accel_AsmParameter(uint8 param, uint8 *buf,void *value)
{
    bStatus_t ret = SUCCESS;
    switch ( param )
    {
        case ACCEL_XYZ_ATTR:
          VOID osal_memcpy( buf, value, 6);
          break;
        case ACCEL_6D_ATTR:
            *buf =  *((uint8*)value);
            break;
    }
    return ret;
}

/*********************************************************************
 * @fn      Accel_SetParameter
 *
 * @brief   Set an Accelerometer Profile parameter.
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
bStatus_t Accel_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case ACCEL_ENABLER:
      if ( len == sizeof ( uint8 ) ) 
      {
        accelEnabled = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case ACCEL_RANGE:
      if ( (len == sizeof ( uint16 )) && ((*((uint8*)value)) <= ACCEL_RANGE_8G) ) 
      {
        accelRange = *((uint16*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case ACCEL_XYZ_ATTR:
      if ( len == 6 ) 
      {
        //accelXYZCoordinates = *((int8*)value);
        VOID osal_memcpy( accelXYZCoordinates, value, 6);
        
        // See if Notification has beevan enabled
        GATTServApp_ProcessCharCfg( accelXYZConfigCoordinates, accelXYZCoordinates,
                                    FALSE, accelAttrTbl, GATT_NUM_ATTRS( accelAttrTbl ),
                                    INVALID_TASK_ID );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ACCEL_6D_ATTR:
      if ( len == 1 ) 
      {      
        accel6Direction = *((int8*)value);

        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( accel6DirectionConfig, (uint8 *)&accel6Direction,
                                    FALSE, accelAttrTbl, GATT_NUM_ATTRS( accelAttrTbl ),
                                    INVALID_TASK_ID );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ACCEL_CLICK_ATTR:
      if ( len == sizeof ( int8 ) ) 
      {      
        accelClickAction = *((int8*)value);

        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( accelClickConfig, (uint8 *)&accelClickAction,
                                    FALSE, accelAttrTbl, GATT_NUM_ATTRS( accelAttrTbl ),
                                    INVALID_TASK_ID );
        
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
 * @fn      Accel_GetParameter
 *
 * @brief   Get an Accelerometer Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Accel_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case ACCEL_ENABLER:
      *((uint8*)value) = accelEnabled;
      break;
      
    case ACCEL_RANGE:
      *((uint16*)value) = accelRange;
      break;

    case ACCEL_XYZ_ATTR:
      VOID osal_memcpy(value,accelXYZCoordinates,6);
      break;

    case ACCEL_6D_ATTR:
      *((int8*)value) = accel6Direction;
      break;

    case ACCEL_CLICK_ATTR:
      *((int8*)value) = accelClickAction;
      break;
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          accel_ReadAttr
 *
 * @brief       Read an attribute.
 *
 * @param       pAttr - pointer to attribute
 * @param       pLen - length of data to be read
 * @param       pValue - pointer to data to be read
 * @param       signature - whether to include Authentication Signature
 *
 * @return      Success or Failure
 */
static uint8 accel_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                               uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen )
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

  // Make sure it's not a blob operation
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }

  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {    
    // 16-bit UUID
    uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those types for reads
      case ACCEL_RANGE_UUID:
        *pLen = 2;
        pValue[0] = LO_UINT16( *((uint16 *)pAttr->pValue) );
        pValue[1] = HI_UINT16( *((uint16 *)pAttr->pValue) ); 
        break;
  
      case ACCEL_ENABLER_UUID:
        *pLen = 1;
        pValue[0] = *pAttr->pValue;
        break;
        
      case ACCEL_XYZ_UUID:
        *pLen = 6;
        VOID osal_memcpy( pValue, pAttr->pValue, 6 );
        break;
        
      case ACCEL_6D_UUID:
      case ACCEL_CLICK_UUID:
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
 * @fn      accel_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle – connection message was received on
 * @param   pReq - pointer to request
 *
 * @return  Success or Failure
 */
static bStatus_t accel_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                    uint8 *pValue, uint8 len, uint16 offset )
{
  bStatus_t status = SUCCESS;
  uint8 notify = 0xFF;

  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case ACCEL_ENABLER_UUID:
        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len > 1 )
            status = ATT_ERR_INVALID_VALUE_SIZE;
         // else if ( pValue[0] != FALSE && pValue[0] != TRUE )
           // status = ATT_ERR_INVALID_VALUE;
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;
          
          *pCurValue = pValue[0];
          notify = ACCEL_ENABLER;        
        }
             
        break;
          
      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        break;      
          
      default:
          // Should never get here!
          status = ATT_ERR_ATTR_NOT_FOUND;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }  

  // If an attribute changed then callback function to notify application of change
  if ( (notify != 0xFF) && accel_AppCBs && accel_AppCBs->pfnAccelEnabler )
    accel_AppCBs->pfnAccelEnabler();  
  
  return ( status );
}

/*********************************************************************
 * @fn          accel_HandleConnStatusCB
 *
 * @brief       Accelerometer Service link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
static void accel_HandleConnStatusCB( uint16 connHandle, uint8 changeType )
{ 
  // Make sure this is not loopback connection
  if ( connHandle != LOOPBACK_CONNHANDLE )
  {
    // Reset Client Char Config if connection has dropped
    if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
         ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) && 
           ( !linkDB_Up( connHandle ) ) ) )
    { 
      GATTServApp_InitCharCfg( connHandle, accelXYZConfigCoordinates );
      GATTServApp_InitCharCfg( connHandle, accel6DirectionConfig );
      GATTServApp_InitCharCfg( connHandle, accelClickConfig );
    }
  }
}


/*********************************************************************
*********************************************************************/
