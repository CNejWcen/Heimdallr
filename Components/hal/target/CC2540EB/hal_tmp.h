/**************************************************************************************************
  Filename:       hal_tmp.h
  Revised:        $Date: 2014-4-20 16:58:46 +800 (Sun,20 April 2014) $
  Revision:       $Revision: 10007 $

  Description:    Interface to the IR temperature sensor driver TMP006

**************************************************************************************************/

#ifndef HAL_IRTEMP_H
#define HAL_IRTEMP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "comdef.h"

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
  TMP006_OFF,                           // IR Temperature Sleeping
  TMP006_IDLE,			        // IR Temperature On and Configured
  TMP006_DATA_READY                     // IR Temperature On, Configured and Data is Ready	
} TMP_States_t;


/*********************************************************************
 * FUNCTIONS
 *********************************************************************/
bool TMP_Init(void);
void TMP_Start(void);
void TMP_Shutdown(void);
bool TMP_Read(uint8 *irTempData);
bool TMP_TEST(void);
TMP_States_t TMP_Status(void);


#ifdef __cplusplus
}
#endif

#endif /* HAL_IRTEMP_H */
