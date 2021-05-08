/**************************************************************************************************
  Filename:       hal_drivers.h
  Revised:        $Date: 2012-07-09 13:23:30 -0700 (Mon, 09 Jul 2012) $
  Revision:       $Revision: 30873 $

  Description:    This file contains the interface to the Drivers service.

**************************************************************************************************/
#ifndef HAL_DRIVER_H
#define HAL_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include "hal_types.h"

/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/
#define PERIOD_RSSI_RESET_EVT               0x0040
  
#define HAL_LED_BLINK_EVENT                 0x0200

#if defined POWER_SAVING
#define HAL_SLEEP_TIMER_EVENT               0x0004
#define HAL_PWRMGR_HOLD_EVENT               0x0002
#define HAL_PWRMGR_CONSERVE_EVENT           0x0001
#endif

#define HAL_PWRMGR_CONSERVE_DELAY           10
#define PERIOD_RSSI_RESET_TIMEOUT           10
#define LED_CYCLE_PERIOD                    500
   
   

      
/**************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

extern uint8 Hal_TaskID;

/**************************************************************************************************
 * FUNCTIONS - API
 **************************************************************************************************/

extern void Hal_Init ( uint8 task_id );

/*
 * Process Serial Buffer
 */
extern uint16 Hal_ProcessEvent ( uint8 task_id, uint16 events );

/*
 * Process Polls
 */
extern void Hal_ProcessPoll (void);

/*
 * Initialize HW
 */
extern void HalDriverInit (void);

void uprint(uint8 *buf);
void uprintEx(uint8 *buf,uint16 len);
void dbg(char *fmt,...);
void LedBlink();
void LedStop();
#ifdef __cplusplus
}
#endif

#endif

/**************************************************************************************************
**************************************************************************************************/
