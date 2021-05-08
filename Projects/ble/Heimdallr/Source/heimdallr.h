/**************************************************************************************************
  Filename:       keyfobdemo.h
  Revised:        $Date: 2012-10-11 12:11:30 -0700 (Thu, 11 Oct 2012) $
  Revision:       $Revision: 31784 $

  Description:    This file contains Key Fob Demo Application header file.
**************************************************************************************************/

#ifndef KEYFOBDEMO_H
#define KEYFOBDEMO_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
  MWH_HUMI_READ_EVT MWH_HEART_READ_EVT MWH_IRTEMP_READ_EVT MWH_FALSH_READ_EVT
 */


// Watcher Heimdallr Task Events
#define MWH_START_DEVICE_EVT                              0x0001
#define MWH_BATTERY_CHECK_EVT                             0x0002
#define MWH_ACCEL_READ_EVT                                0x0004
#define MWH_HUMI_READ_EVT                                 0x0008
#define MWH_ADV_IN_CONNECTION_EVT                         0x0010
#define MWH_HEART_READ_EVT                                0x0020
#define MWH_IRTEMP_READ_EVT                               0x0040
#define MWH_FLASH_READ_EVT                                0x0080

#define MWH_RESTART_SCAN_DEVICE                           0x0100
#define MWH_START_UP_SIM800L0                             0x0200
#define MWH_START_UP_SIM800L                              0x0400
#define MWH_ASSEMBLE_PACKAGE_EVT                          0x0800
   
//#define MWH_LED__EVT                       0x0020

  
// NV item Unique ID Application 0x80 ¨C 0xFE
  
#define HNV_ANY_TEST_DATA    0x80
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the BLE Application
 */
extern void HeimDallr_Init( uint8 task_id );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 HeimDallr_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* KEYFOBDEMO_H */
