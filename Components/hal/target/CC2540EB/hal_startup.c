/**************************************************************************************************
  Filename:       hal_startup.c
  Revised:        $Date: 2012-06-12 12:38:53 -0700 (Tue, 12 Jun 2012) $
  Revision:       $Revision: 30729 $

  Description:    Contains code that needs to run before main()

**************************************************************************************************/

/**************************************************************************************************
 * INCLUDES
 *************************************************************************************************/
#include "hal_board.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma language=extended

//
// Locate low_level_init in the CSTART module
//
#pragma location="CSTART"
//
// If the code model is banked, low_level_init must be declared
// __near_func elsa a ?BRET is performed
//
#if (__CODE_MODEL__ == 2)
__near_func __root char
#else
__root char
#endif
__low_level_init(void);

/**************************************************************************************************
 * @fn          __low_level_init
 *
 * @brief      The function __low_level_init is called by the start-up code before doing
 *                the normal initialization of data segments. If the return value is zero,
 *                initialization is not performed.
 *
 * @param       None
 *
 * @return      0 - don't intialize data segments / 1 - do initialization
 **************************************************************************************************/
#if (__CODE_MODEL__ == 2)
__near_func __root char
#else
__root char
#endif
__low_level_init(void)
{
  /*==================================*/
  /*  Initialize hardware.            */
  /*==================================*/
  // Map flash bank with constants into XDATA for access to "ROM mapped as data".
#if defined HAL_IMAGE_A
  MEMCTR = (MEMCTR & 0xF8) | 0x05;
#elif defined HAL_IMAGE_B
  MEMCTR = (MEMCTR & 0xF8) | 0x04;
#else
  MEMCTR = (MEMCTR & 0xF8) | 0x01;
#endif

  /*==================================*/
  /* Choose if segment initialization */
  /* should be done or not.           */
  /* Return: 0 to omit seg_init       */
  /*         1 to run seg_init        */
  /*==================================*/
  return 1;
}

#pragma language=default

#ifdef __cplusplus
}
#endif
