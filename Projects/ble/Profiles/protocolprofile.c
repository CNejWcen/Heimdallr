/**************************************************************************************************
  Filename:       ProtocolProfile.c
  Revised:        $Date: 2013-05-06 13:33:47 -0700 (Mon, 06 May 2013) $
  Revision:       $Revision: 34153 $

  Description:    Protocol Service.This is responsable for the security and communication protocol.
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

#include "protocolprofile.h"
#include "st_util.h"


/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 * #define PROTOCOL_SERV_UUID                  0x99C0 // F00099C0-0451-4000-B000-00000000-0000
 * #define PROTOCOL_FILTER_UUID                0x99C1
 * #define PROTOCOL_AUTH_UUID                  0x99C2
 * #define PROTOCOL_ADDR_UUID                  0x99C3
 * #define PROTOCOL_ACK_UUID                   0x99C4
 * #define PROTOCOL_SEQ_UUID                   0x99C5
 * #define PROTOCOL_CONF_UUID                  0x99C6
 * #define PROTOCOL_RCODES_UUID                0x99C7
 * #define PROTOCOL_CHECKSUM_UUID              0x99C8
 * #define PROTOCOL_RESERVED_UUID              0x99C9  
 * #define PROTOCOL_NAME_UUID                  0x99D0
 * SERV NAME FILTER AUTH ADDR ACK SEQ CONF RCODES CHECKSUM RESERVED    
 */
// Protocol GATT Profile Service UUID
CONST uint8 protocolServUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_SERV_UUID)
};

// NAME Characteristic UUID
CONST uint8 protocolNameUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_NAME_UUID)
};

// FILTER Characteristic UUID
CONST uint8 protocolFilterUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_FILTER_UUID)
};

// AUTH Characteristic UUID
CONST uint8 protocolAuthUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_AUTH_UUID)
};

// ADDR Characteristic UUID
CONST uint8 protocolAddrUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_ADDR_UUID)
};

// ACK Characteristic UUID
CONST uint8 protocolAckUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_ACK_UUID)
};

// SEQ Characteristic UUID
CONST uint8 protocolSeqUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_SEQ_UUID)
};

// CONF Characteristic UUID
CONST uint8 protocolConfUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_CONF_UUID)
};

// RCodes Characteristic UUID
CONST uint8 protocolRCodesUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_RCODES_UUID)
};

// Checksum Characteristic UUID
CONST uint8 protocolChecksumUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_CHECKSUM_UUID)
};

// RESERVED Characteristic UUID
CONST uint8 protocolReservedUUID[TI_UUID_SIZE] =
{
  TI_UUID(PROTOCOL_RESERVED_UUID)
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 *  
 */

static protocolCBs_t *protocol_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Protocol Profile Service attribute
static CONST gattAttrType_t protocolService = { TI_UUID_SIZE, protocolServUUID };

// Protocol Profile Data Characteristic Properties
static uint8 protocolNameProps          = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 protocolFilterProps        = GATT_PROP_READ;
static uint8 protocolAuthProps          = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 protocolAddrProps          = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 protocolAckProps           = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 protocolSeqProps           = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 protocolConfProps          = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 protocolRCodesProps        = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 protocolChecksumProps      = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 protocolReservedProps      = GATT_PROP_READ;

// Protocol Profile Data Characteristic Value
static uint8 protocolName[PROTOCOL_NAME_LEN]            = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint8 protocolFilter                             = 0x4D;
static uint8 protocolAuth[PROTOCOL_AUTH_LEN]            = {0,0,0,0};
static uint8 protocolAddr[PROTOCOL_ADDR_LEN]            = {0,0,0,0,0,0};
static uint8 protocolAck                                = 0x00;
static uint8 protocolSeq                                = 0x00;
static uint8 protocolConf                               = 0x00;
static uint8 protocolRCodes[PROTOCOL_RCODES_LEN]         = {0,0};
static uint8 protocolChecksum[PROTOCOL_CHECKSUM_LEN]    = {0,0};
static uint8 protocolReserved[PROTOCOL_RESERVED_LEN]    = {0,0,0,0};

// Protocol Characteristic Configuration
// static gattCharCfg_t protocolDataConfig[GATT_MAX_NUM_CONN];

// Protocol Profile Data Characteristic User Description
static uint8 protocolNameUserDesp[]             = "Protocol Name";
static uint8 protocolFilterUserDesp[]           = "Protocol Filter";
static uint8 protocolAuthUserDesp[]             = "Protocol Auth Password";
static uint8 protocolAddrUserDesp[]             = "Protocol Address";
static uint8 protocolAckUserDesp[]              = "Protocol Ack";
static uint8 protocolSeqUserDesp[]              = "Protocol Seq";
static uint8 protocolConfUserDesp[]             = "Protocol Conf";
static uint8 protocolRCodesUserDesp[]           = "Protocol RCodes";
static uint8 protocolChecksumUserDesp[]         = "Protocol Checksum";
static uint8 protocolReservedUserDesp[]         = "Protocol Reserved";

/*********************************************************************
 * Profile Attributes - Table
 *  NAME FILTER AUTH ADDR ACK SEQ CONF RCODES CHECKSUM RESERVED   
 */
static gattAttribute_t protocolAttrTbl[] =
{
  // Protocol Profile Service
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },           /* type */
    GATT_PERMIT_READ,                                   /* permissions */
    0,                                                  /* handle */
    (uint8 *)&protocolService                           /* pValue */
  }, 

    // Name Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolNameProps
    },

      // Name Characteristic Value
      {
        { TI_UUID_SIZE, protocolNameUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        protocolName
      },

      // Name Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolNameUserDesp
      },
    // Filter Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolFilterProps
    },

      // Filter Characteristic Value
      {
        { TI_UUID_SIZE, protocolFilterUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &protocolFilter
      },

      // Filter Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolFilterUserDesp
      },
    // Auth Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolAuthProps
    },

      // Auth Characteristic Value
      {
        { TI_UUID_SIZE, protocolAuthUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        protocolAuth
      },

      // Auth Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolAuthUserDesp
      },
    // Addr Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolAddrProps
    },

      // Addr Characteristic Value
      {
        { TI_UUID_SIZE, protocolAddrUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        protocolAddr
      },

      // Addr Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolAddrUserDesp
      },
    // Ack Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolAckProps
    },

      // Ack Characteristic Value
      {
        { TI_UUID_SIZE, protocolAckUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &protocolAck
      },

      // Ack Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolAckUserDesp
      },
    // Seq Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolSeqProps
    },

      // Seq Characteristic Value
      {
        { TI_UUID_SIZE, protocolSeqUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &protocolSeq
      },

      // Seq Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolSeqUserDesp
      },
    // Conf Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolConfProps
    },

      // Conf Characteristic Value
      {
        { TI_UUID_SIZE, protocolConfUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &protocolConf
      },

      // Conf Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolConfUserDesp
      },
    // RCodes Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolRCodesProps
    },

      // RCodes Characteristic Value
      {
        { TI_UUID_SIZE, protocolRCodesUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        protocolRCodes
      },

      // RCodes Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolRCodesUserDesp
      },
    // Checksum Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolChecksumProps
    },

      // Checksum Characteristic Value
      {
        { TI_UUID_SIZE, protocolChecksumUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        protocolChecksum
      },

      // Checksum Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolChecksumUserDesp
      },
    // Reserved Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &protocolReservedProps
    },

      // Reserved Characteristic Value
      {
        { TI_UUID_SIZE, protocolReservedUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        protocolReserved
      },

      // Reserved Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        protocolReservedUserDesp
      },
      
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 protocol_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t protocol_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset );
//static void protocol_HandleConnStatusCB( uint16 connHandle, uint8 changeType );

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Protocol Profile Service Callbacks
CONST gattServiceCBs_t protocolCBs =
{
  protocol_ReadAttrCB,  // Read callback function pointer
  protocol_WriteAttrCB, // Write callback function pointer
  NULL              // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Protocol_AddService
 *
 * @brief   Initializes the Protocol Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t Protocol_AddService( uint32 services )
{
  uint8 status = SUCCESS;

  // Initialize Client Characteristic Configuration attributes                  Use for Notify need be confirmed
  // GATTServApp_InitCharCfg( INVALID_CONNHANDLE, protocolDataConfig );

  // Register with Link DB to receive link status change callback
  // VOID linkDB_Register( protocol_HandleConnStatusCB );

  if ( services & PROTOCOL_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( protocolAttrTbl,
                                          GATT_NUM_ATTRS( protocolAttrTbl ),
                                          &protocolCBs );
  }

  return ( status );
}

/*********************************************************************
 * @fn      Protocol_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t Protocol_RegisterAppCBs( protocolCBs_t *appCallbacks )
{
  if ( protocol_AppCBs == NULL )
  {
    if ( appCallbacks != NULL )
    {
      protocol_AppCBs = appCallbacks;
    }

    return ( SUCCESS );
  }

  return ( bleAlreadyInRequestedMode );

}

/*********************************************************************
 * @fn      ProtocolProfile_SetParameter
 *
 * @brief   Set a Protocol Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *#define PROTOCOL_NAME_ATTR                      0      // RW uint8 - Profile Attribute value
#define PROTOCOL_FILTER_ATTR                    1      // RW uint8 - Profile Attribute value
#define PROTOCOL_AUTH_ATTR                      2      
#define PROTOCOL_ADDR_ATTR                      3      
#define PROTOCOL_ACK_ATTR                       4      
#define PROTOCOL_SEQ_ATTR                       5      
#define PROTOCOL_CONF_ATTR                      6      
#define PROTOCOL_RCODES_ATTR                    7      
#define PROTOCOL_CHECKSUM_ATTR                  8      
#define PROTOCOL_RESERVED_ATTR                  9      
 * @return  bStatus_t
 */
bStatus_t Protocol_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case PROTOCOL_NAME_ATTR:
      if ( len == PROTOCOL_NAME_LEN )
      {
        VOID osal_memcpy( protocolName, value, PROTOCOL_NAME_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
/*              //Read Only
    case PROTOCOL_FILTER_ATTR:
      if(len == sizeof ( uint8 ) )
      {
        protocolFilter = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
*/
    case PROTOCOL_AUTH_ATTR:
      if ( len == PROTOCOL_AUTH_LEN )
      {
        VOID osal_memcpy( protocolAuth, value, PROTOCOL_AUTH_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case PROTOCOL_ADDR_ATTR:
      if ( len == PROTOCOL_ADDR_LEN )
      {
        VOID osal_memcpy( protocolAddr, value, PROTOCOL_ADDR_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case PROTOCOL_ACK_ATTR:
      if(len == sizeof ( uint8 ) )
      {
        protocolAck = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case PROTOCOL_SEQ_ATTR:
      if(len == sizeof ( uint8 ) )
      {
        protocolSeq = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case PROTOCOL_CONF_ATTR:
      if(len == sizeof ( uint8 ) )
      {
        protocolConf = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case PROTOCOL_RCODES_ATTR:
      if ( len == PROTOCOL_RCODES_LEN )
      {
        VOID osal_memcpy( protocolRCodes, value, PROTOCOL_RCODES_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case PROTOCOL_CHECKSUM_ATTR:
      if ( len == PROTOCOL_CHECKSUM_LEN )
      {
        VOID osal_memcpy( protocolChecksum, value, PROTOCOL_CHECKSUM_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
/*                      //read-only   
    case PROTOCOL_RESERVED_ATTR:
      if ( len == PROTOCOL_RESERVED_LEN )
      {
        VOID osal_memcpy( protocolReserved, value, PROTOCOL_RESERVED_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
*/
    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      ProtocolProfile_GetParameter
 *
 * @brief   Get a Protocol Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Protocol_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case PROTOCOL_NAME_ATTR:
      VOID osal_memcpy (value, protocolName, PROTOCOL_NAME_LEN );
      break;
      
    case PROTOCOL_FILTER_ATTR:
      *((uint8*)value) = protocolFilter;
      break;
      
    case PROTOCOL_AUTH_ATTR:
      VOID osal_memcpy (value, protocolAuth, PROTOCOL_AUTH_LEN );
      break;
      
    case PROTOCOL_ADDR_ATTR:
      VOID osal_memcpy (value, protocolAddr, PROTOCOL_ADDR_LEN );
      break;
      
    case PROTOCOL_ACK_ATTR:
      *((uint8*)value) = protocolAck;
      break;
    
    case PROTOCOL_SEQ_ATTR:
      *((uint8*)value) = protocolSeq;
      break;
      
    case PROTOCOL_CONF_ATTR:
      *((uint8*)value) = protocolConf;
      break;

    case PROTOCOL_RCODES_ATTR:
      VOID osal_memcpy (value, protocolRCodes, PROTOCOL_RCODES_LEN );
      break;

    case PROTOCOL_CHECKSUM_ATTR:
      VOID osal_memcpy (value, protocolChecksum, PROTOCOL_CHECKSUM_LEN );
      break;
      
    case PROTOCOL_RESERVED_ATTR:
      VOID osal_memcpy (value, protocolReserved, PROTOCOL_RESERVED_LEN );
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn          protocol_ReadAttrCB
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
static uint8 protocol_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen )
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE)
  {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }

  switch ( uuid )
  {
    // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
    // gattserverapp handles those reads
    case PROTOCOL_NAME_UUID:
      *pLen = PROTOCOL_NAME_LEN;
      VOID osal_memcpy( pValue, pAttr->pValue, PROTOCOL_NAME_LEN );
      break;

    case PROTOCOL_FILTER_UUID:
    case PROTOCOL_ACK_UUID:
    case PROTOCOL_SEQ_UUID:
    case PROTOCOL_CONF_UUID:
      *pLen = 1;
      pValue[0] = *pAttr->pValue;
      break;

    case PROTOCOL_AUTH_UUID:
      *pLen = PROTOCOL_AUTH_LEN;
      VOID osal_memcpy( pValue, pAttr->pValue, PROTOCOL_AUTH_LEN );
      break;      

    case PROTOCOL_ADDR_UUID:
      *pLen = PROTOCOL_ADDR_LEN;
      VOID osal_memcpy( pValue, pAttr->pValue, PROTOCOL_ADDR_LEN );
      break;
      
    case PROTOCOL_RCODES_UUID:
      *pLen = PROTOCOL_RCODES_LEN;
      VOID osal_memcpy( pValue, pAttr->pValue, PROTOCOL_RCODES_LEN );
      break;
      
    case PROTOCOL_CHECKSUM_UUID:
      *pLen = PROTOCOL_CHECKSUM_LEN;
      VOID osal_memcpy( pValue, pAttr->pValue, PROTOCOL_CHECKSUM_LEN );
      break;
      
    case PROTOCOL_RESERVED_UUID:
      *pLen = PROTOCOL_RESERVED_LEN;
      VOID osal_memcpy( pValue, pAttr->pValue, PROTOCOL_RESERVED_LEN );
      break;
      
      
    default:
      // Should never get here!
      *pLen = 0;
      status = ATT_ERR_ATTR_NOT_FOUND;
      break;
  }

  return ( status );
}

/*********************************************************************
 * @fn      protocol_WriteAttrCB
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
static bStatus_t protocol_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset )
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  uint16 uuid;

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

  switch ( uuid )
  {

    case PROTOCOL_ACK_UUID: 
    case PROTOCOL_SEQ_UUID: 
    case PROTOCOL_CONF_UUID: 
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
      
      if ( status == SUCCESS )
      {
        uint8 *pCurValue = (uint8 *)pAttr->pValue;
        *pCurValue = pValue[0];

        if( pAttr->pValue == &protocolConf )
        {
          notifyApp = PROTOCOL_CONF_ATTR;
        }
        else if( pAttr->pValue == &protocolFilter )
        {
          notifyApp = PROTOCOL_FILTER_ATTR;
        }
        else if( pAttr->pValue == &protocolAck )
        {
          notifyApp = PROTOCOL_ACK_ATTR;
        }
        else if( pAttr->pValue == &protocolSeq )
        {
          notifyApp = PROTOCOL_SEQ_ATTR;
        }        
      }
      break;
    case PROTOCOL_FILTER_UUID: 
    case PROTOCOL_RESERVED_UUID:
      //should not be here
      break;
    case PROTOCOL_NAME_UUID: 
      if ( offset == 0 )
      {
        if ( len != PROTOCOL_NAME_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }
      
      if ( status == SUCCESS )
      {
        VOID osal_memcpy( pAttr->pValue, pValue , PROTOCOL_NAME_LEN );
        if( pAttr->pValue == protocolName )
        {
          notifyApp = PROTOCOL_NAME_ATTR;
        }
      }
      break;
      
    case PROTOCOL_AUTH_UUID: 
      if ( offset == 0 )
      {
        if ( len != PROTOCOL_AUTH_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }
      
      if ( status == SUCCESS )
      {
        VOID osal_memcpy( pAttr->pValue, pValue , PROTOCOL_AUTH_LEN );
        if( pAttr->pValue == protocolAuth )
        {
          notifyApp = PROTOCOL_AUTH_ATTR;
        }
      }
      break;
      
    case PROTOCOL_ADDR_UUID: 
      if ( offset == 0 )
      {
        if ( len != PROTOCOL_ADDR_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }
      
      if ( status == SUCCESS )
      {
        VOID osal_memcpy( pAttr->pValue, pValue , PROTOCOL_ADDR_LEN );
        if( pAttr->pValue == protocolAddr )
        {
          notifyApp = PROTOCOL_ADDR_ATTR;
        }
      }
      break;
      
    case PROTOCOL_RCODES_UUID: 
      if ( offset == 0 )
      {
        if ( len != PROTOCOL_RCODES_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }
      
      if ( status == SUCCESS )
      {
        VOID osal_memcpy( pAttr->pValue, pValue , PROTOCOL_RCODES_LEN );
        if( pAttr->pValue == protocolRCodes )
        {
          notifyApp = PROTOCOL_RCODES_ATTR;
        }
      }
      break;
      
    case PROTOCOL_CHECKSUM_UUID:
      if ( offset == 0 )
      {
        if ( len != PROTOCOL_CHECKSUM_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }
      
      if ( status == SUCCESS )
      {
        VOID osal_memcpy( pAttr->pValue, pValue , PROTOCOL_CHECKSUM_LEN );
        if( pAttr->pValue == protocolChecksum )
        {
          notifyApp = PROTOCOL_CHECKSUM_ATTR;
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

  // If a charactersitic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && protocol_AppCBs && protocol_AppCBs->pfnProtocolChange )
  {
    protocol_AppCBs->pfnProtocolChange( notifyApp );
  }

  return ( status );
}



/*********************************************************************
 * @fn          protocol_HandleConnStatusCB
 *
 * @brief       Protocol Profile link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none

static void protocol_HandleConnStatusCB( uint16 connHandle, uint8 changeType )
{
  // Make sure this is not loopback connection
  if ( connHandle != LOOPBACK_CONNHANDLE )
  {
    // Reset Client Char Config if connection has dropped
    if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
         ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) &&
           ( !linkDB_Up( connHandle ) ) ) )
    {
      GATTServApp_InitCharCfg( connHandle, protocolDataConfig );
    }
  }
}
 */

/*********************************************************************
*********************************************************************/
