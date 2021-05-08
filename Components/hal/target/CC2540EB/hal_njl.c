/**************************************************************************************************
  Filename:       hal_njl.c
  Revised:        $Date: 2014-4-20 20:49:46 +800 (Sun,20 April 2014) $
  Revision:       $Revision: 10005 $

  Description:    Driver for the HeartRate & Blood Oxygen Sensor.

**************************************************************************************************/

#include "hal_njl.h"

bool NJL_Init()
{

  return true;
  
}

//      HalAdcSetReference(HAL_ADC_REF_125V);
//      uint16 adc_data=HalAdcRead(HAL_ADC_CHN_AIN6,HAL_ADC_RESOLUTION_12);
//      then calc the rate by adc_data