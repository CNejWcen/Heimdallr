/**************************************************************************************************
  Filename:       ProtocolProfile.h
  Revised:        $Date: 2013-09-05 15:48:17 -0700 (Thu, 05 Sep 2013) $
  Revision:       $Revision: 35223 $

  Description:    Protocol Service.This is responsable for the security and communication protocol.

**************************************************************************************************/

#ifndef _PROTOCOLPROFILE_H_
#define _PROTOCOLPROFILE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 *                              The Protocol
 * +--------------------------------------------------------------------------+
 * |                               Name                                       |
 * |                              16bytes                                     |
 * |                                                                          |
 * +--------------------------------------------+-----------------------------+
 * |Filter      |         Auth                  |       Addr                  |                                                            
 * | 1 byte RO  |         4bytes                |       6bytes                |                                    
 * +------------+-------------------------------+-----------------------------+
 * | ACK        |  Seq  | Conf  |     RCodes    |  Checksum |   Reserved      |
 * |1Byte       | 1Byte | 1byte |      2Bytes   |   2Bytes  |    4bytes  RO   |
 * +------------+-------+-------+---------------+-----------+-----------------+
 */
  
/*********************************************************************
 * CONSTANTS
 */

// Protocol Service Parameters
#define PROTOCOL_NAME_ATTR                      0      // RW uint8 - Profile Attribute value
#define PROTOCOL_FILTER_ATTR                    1      // RW uint8 - Profile Attribute value
#define PROTOCOL_AUTH_ATTR                      2      
#define PROTOCOL_ADDR_ATTR                      3      
#define PROTOCOL_ACK_ATTR                       4      
#define PROTOCOL_SEQ_ATTR                       5      
#define PROTOCOL_CONF_ATTR                      6      
#define PROTOCOL_RCODES_ATTR                    7      
#define PROTOCOL_CHECKSUM_ATTR                  8      
#define PROTOCOL_RESERVED_ATTR                  9      

// Service UUID
#define PROTOCOL_SERV_UUID                  0x99C0 // F00099C0-0451-4000-B000-00000000-0000
#define PROTOCOL_FILTER_UUID                0x99C1
#define PROTOCOL_AUTH_UUID                  0x99C2
#define PROTOCOL_ADDR_UUID                  0x99C3
#define PROTOCOL_ACK_UUID                   0x99C4
#define PROTOCOL_SEQ_UUID                   0x99C5
#define PROTOCOL_CONF_UUID                  0x99C6
#define PROTOCOL_RCODES_UUID                0x99C7
#define PROTOCOL_CHECKSUM_UUID              0x99C8
#define PROTOCOL_RESERVED_UUID              0x99C9  
#define PROTOCOL_NAME_UUID                  0x99D0
  
// Protocol Profile Services bit fields
#define PROTOCOL_SERVICE                    0x00000001

// Protocol Data Length
#define PROTOCOL_NAME_LEN                   16
#define PROTOCOL_AUTH_LEN                   4
#define PROTOCOL_ADDR_LEN                   6
#define PROTOCOL_RCODES_LEN                 2
#define PROTOCOL_CHECKSUM_LEN               2
#define PROTOCOL_RESERVED_LEN               4

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
typedef void (*protocolChange_t)( uint8 paramID );

typedef struct
{
  protocolChange_t        pfnProtocolChange;  // Called when characteristic value changes
} protocolCBs_t;


/*********************************************************************
 * API FUNCTIONS
 */

/*
 * Protocol_AddService- Initializes the Protocol Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t Protocol_AddService( uint32 services );

/*
 * Protocol_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t Protocol_RegisterAppCBs( protocolCBs_t *appCallbacks );

/*
 * Protocol_SetParameter - Set a Protocol Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    pValue - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t Protocol_SetParameter( uint8 param, uint8 len, void *pValue );

/*
 * Protocol_GetParameter - Get a Protocol Profile parameter.
 *
 *    param - Profile parameter ID
 *    pValue - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t Protocol_GetParameter( uint8 param, void *pValue );


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* TESTPROFILE_H */
