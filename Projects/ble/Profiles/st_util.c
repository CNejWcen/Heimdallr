/**************************************************************************************************
  Filename:       st_util.c
  Revised:        $Date: 2012-09-25 06:26:26 -0700 (Tue, 25 Sep 2012) $
  Revision:       $Revision: 31617 $

  Description:    Utilties for Sensor Tag services

**************************************************************************************************/

/*-------------------------------------------------------------------
 * INCLUDES
 */
#include "bcomdef.h"
#include "gatt.h"
#include "st_util.h"

/*-------------------------------------------------------------------
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      utilExtractUuid16
 *
 * @brief   Extracts a 16-bit UUID from a GATT attribute
 *
 * @param   pAttr - pointer to attribute
 *
 * @param   pUuid - pointer to UUID to be extracted
 *
 * @return  Success or Failure
 */
bStatus_t utilExtractUuid16(gattAttribute_t *pAttr, uint16 *pUuid)
{
  bStatus_t status = SUCCESS;

  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID direct
    *pUuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
#ifdef GATT_TI_UUID_128_BIT
  }
  else if (pAttr->type.len == ATT_UUID_SIZE)
  {
    // 16-bit UUID extracted bytes 12 and 13
    *pUuid = BUILD_UINT16( pAttr->type.uuid[12], pAttr->type.uuid[13]);
#endif
  } else {
    *pUuid = 0xFFFF;
    status = FAILURE;
  }

  return status;
}

/*********************************************************************
*********************************************************************/

