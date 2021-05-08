/**************************************************************************************************
  Filename:       hal_sht.h
  Revised:        $Date: 2014-04-20 15:01:15 +0800 (Sun, 20 April 2014) $
  Revision:       $Revision: 10002 $

  Description:    Interface to the humidity sensor driver SHT20
**************************************************************************************************/
#ifndef HAL_HUMI_H
#define HAL_HUMI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "hal_types.h"

/*********************************************************************
 * CONSTANTS
 */
#define SHT_MEAS_STATE_1         0    // Start temp. meas.
#define SHT_MEAS_STATE_2         1    // Read temp. meas, start hum.
#define SHT_MEAS_STATE_3         2    // Read humi. meas.

/*********************************************************************
 * FUNCTIONS
 */
bool SHT_Init(void);
bool SHT_ExecMeasurementStep(uint8 state);
bool SHT_ReadMeasurement(uint8 *pBuf);
bool SHT_TEST(void);


bool SHT_ReadTemp(uint8 *buf2);
bool SHT_ReadHumi(uint8 *buf2);
/*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HAL_HUMI_H */
