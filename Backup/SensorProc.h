/**************************************************************************************************
  Filename:       SensorProc.h
  Revised:        $Date: 2014-04-16 16:06:18 -0700 (Thu, 04 Apr 2014) $
  Revision:       $Revision: 10001 $

  Description:    This file contains the init and test of Sensors.

**************************************************************************************************/

/**************************************************************************************************
 *                                           Includes
 **************************************************************************************************/

#ifndef _SENSOR_PROC_H_
#define _SENSOR_PROC_H_

#include "hal_types.h"
#include "comdef.h"
#include "osal_snv.h"
#include "gatt.h"
#include "peripheral.h"

#include "hal_board.h"
#include "hal_drivers.h"
#include "hal_mxflash.h"
#include "hal_timer.h"
#include "hal_i2c.h"
#include "hal_mpu.h"
#include "hal_lis3dh.h"

#include "hal_tmp.h"
#include "hal_sht.h"
#include "hal_njl.h"



bool SensorsInit();
void SensorsJoin();
bool SensorPOST();

bool SensorsSleep();




#endif