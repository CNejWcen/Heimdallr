/**************************************************************************************************
  Filename:       hal_sleep.h
  Revised:        $Date: 2013-02-27 11:32:02 -0800 (Wed, 27 Feb 2013) $
  Revision:       $Revision: 33315 $

  Description:    This file contains the interface to the power management service.
**************************************************************************************************/

#ifndef HAL_SLEEP_H
#define HAL_SLEEP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Execute power management procedure
 */
extern void halSleep( uint32 osal_timer );

/*
 * Used in mac_mcu
 */
extern void halSleepWait(uint16 duration);

/*
 * Used in hal_drivers, AN044 - DELAY EXTERNAL INTERRUPTS
 */
extern void halRestoreSleepLevel( void );

/*
 * Used by the interrupt routines to exit from sleep.
 */
extern void halSleepExit(void);

/*
 * Set the max sleep loop time lesser than the T2 rollover period.
 */
extern void halSetMaxSleepLoopTime(uint32 rolloverTime);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
