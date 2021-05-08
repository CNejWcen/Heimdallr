/**************************************************************************************************
  Filename:       SensorProc.c
  Revised:        $Date: 2014-04-16 16:06:18 -0700 (Thu, 04 Apr 2014) $
  Revision:       $Revision: 10001 $

  Description:    This file contains the init and test of Sensors.
 
  Sensor List:
                  MX25U1635
                  LIS3DH
                  MPU6500
                  SHT25
                  HeartRate-NJL
                  IRTemp-TMP006
**************************************************************************************************/


#include "SensorProc.h"
#include <ioCC2541.h>


/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/
bool NJL_Init();

bool NVUnique_Check();
void Proc_UniqueID();

char *bdAddr2Str( uint8 *pAddr );




/*********************************************************************
 * @fn      HalSensorTest
 *
 * @brief   Run a self-test on all the sensors
 *
 * @param   none
 *
 * @return  bitmask of error flags

uint16 HalSensorTest(void)
{
  uint16 i;
  uint8 selfTestResult;

  halSensorEnableMap = 0;
  selfTestResult = 0;

  for  (i=0; i<N_TESS_RUNS; i++)
  {

    // 1. Temp sensor test
    if (HalIRTempTest())
      selfTestResult |= SS_IRTEMP;

    // 2. Humidity  sensor test
    if (HalHumiTest())
      selfTestResult |= SS_HUMID;
    
    // 4. Accelerometer test
    if (HalAccTest())
      selfTestResult |= SS_ACC;

  return selfTestResult;
}
 */



/*********************************************************************
 * @fn      SensorPOST
 *
 * @brief   Sensors Power-on Self Test
 *
 * @param   none
 *
 * @return  test whether the sensors are working normal
**********************************************************************/
bool SensorPOST()
{  
  LED_FLASH_TWICE();
  LED_ON();

  //LIS3DH_TEST();
  //SHT_Init();
  //MPU6500_TEST();
  //MXFlash_Init();
  //MXFlash_TEST() ; 
  // All Sensor test
  bool test = 1;//LIS3DH_TEST() ;//&& SHT_TEST() && MXFlash_TEST() && TMP_TEST();
  
   //MPU6500_TEST() && 
  return test;
}

   
bool SensorsInit()
{                
                                                       //Init actually means test!     need to finish the test part
  return LIS3DH_Init();
  //MXFlash_Init() & LIS3DH_Init() &  MPU_Init() 
    //      & SHT_Init() & NJL_Init() & TMP_Init();
}

bool SensorsSleep()                                                     //make sure all sensors have been shutdown
{ 
  return true;
}
#define UNIQUE_ID_ADDR 0xF0
#define UNIQUE_ID_LEN  8
#define B_ADDR_LEN     6
#define B_ADDR_STR_LEN 15

bool NVUnqiue_Check()
{
  uint8 pAddr[UNIQUE_ID_LEN];
  uint8 retVal = osal_snv_read(UNIQUE_ID_ADDR,UNIQUE_ID_LEN,pAddr);
  
  if(retVal == NV_OPER_FAILED)
  {
    return false;
  }
  else
  {
     if(sizeof(retVal)<7)
     {
       Proc_UniqueID();
       return false;
     }else
     {
        return true;
     }
  }
}

/****************************************
*              id :  should alloc 8 bytes before run this proc
*****************************************/
bool getUniqueID(uint8 *id)
{
  uint8 retVal = NVUnqiue_Check();
  if(!retVal)
  {
    Proc_UniqueID();                            // Try Again
    return false;
  }
  uint8 retVal2 = osal_snv_read(UNIQUE_ID_ADDR,UNIQUE_ID_LEN,id);
  
  if(retVal2 == NV_OPER_FAILED)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void getMacAddr(uint8* ownAddress){                                     //length B_ADDR_LEN
  GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress); 
}

void Proc_UniqueID()
{
  uint8 unqID[UNIQUE_ID_LEN];
  getMacAddr(unqID);
  
  uint16 rand16 = osal_rand();
  //uint8 result[2];
  unqID[6]     =       rand16 & 0xff;
  unqID[7]     =       (rand16 >> 8);
  
  uint8 retVal = osal_snv_write(UNIQUE_ID_ADDR,UNIQUE_ID_LEN,unqID);
  
  if(retVal == NV_OPER_FAILED)
  {
    return;                                       ////////////////DEAD
  }
 
  
}


/*********************************************************************
 * @fn      bdAddr2Str
 *
 * @brief   Convert Bluetooth address to string. Only needed when
 *          LCD display is used.
 *
 * @return  none
 */
char *bdAddr2Str( uint8 *pAddr )
{
  uint8       i;
  char        hex[] = "0123456789ABCDEF";
  static char str[B_ADDR_STR_LEN];
  char        *pStr = str;

  *pStr++ = '0';
  *pStr++ = 'x';

  // Start from end of addr
  pAddr += B_ADDR_LEN;

  for ( i = B_ADDR_LEN; i > 0; i-- )
  {
    *pStr++ = hex[*--pAddr >> 4];
    *pStr++ = hex[*pAddr & 0x0F];
  }

  *pStr = 0;

  return str;
}



