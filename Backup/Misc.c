/**************************************************************************************************
  Filename:       misc.h
  Revised:        $Date: 2014-04-30 20:43:17 +0800 (Wen, 30 Apr 2014) $
  Revision:       $Revision: 00001 $

  Description:    The Misc function of all

**************************************************************************************************/

#include "hal_drivers.h"

#include "battprofile.h"
#include "accelprofile.h" 
#include "humiprofile.h"
#include "irtempprofile.h"
#include "heartprofile.h"
#include "flashprofile.h"

#include "hal_lis3dh.h"
#include "FlashStruct.h"
#include "Misc.h"
bool isConnected(gaprole_States_t state);
static Datagram dgs[6];
static uint8 index = 0;
bStatus_t transferData(gaprole_States_t state,paramCallback *cb,uint8 *flag,EntryCache *ec)
{
  uint8 status = SUCCESS;
 
  /*if(isConnected(state) && cb->setParam != Flash_SetParameter)                            //setParam with or without * are same?     //state == GAPROLE_CONNECTED &&
  {
    uprint("\n\ndd\n\n");
    cb->setParam(cb->param,cb->len,cb->value);
  }

  else
  */
  {
    uprint("\n\nff\n\n");
    uint16 sensorType = 0;
    uint8 sensorField = 0;

    //cb->asmParam(cb->param,cb->buf,cb->value);
    //assemble sensor
    
    if( cb->asmParam == Accel_AsmParameter && cb->param == ACCEL_XYZ_ATTR)
    {
      UPDATE_PROCESS(flag[1],ACCEL_XYZ_DONE);
      sensorType  = ACCEL_CODE;
      sensorField = ACCEL_XYZ;
    }else if( cb->asmParam == Accel_AsmParameter && cb->param == ACCEL_6D_ATTR)
    {
      UPDATE_PROCESS(flag[1],ACCEL_6D_DONE);
      sensorType  = ACCEL_CODE;
      sensorField = ACCEL_6D;
    }else if( cb->asmParam == Humi_AsmParameter)
    {
      UPDATE_PROCESS(flag[1],HUMI_DONE);
      sensorType  = HUMI_CODE;
      sensorField = HUMI_DATA;
    }else if( cb->asmParam == IRTemp_AsmParameter)
    {
       UPDATE_PROCESS(flag[1],IR_DONE);
       sensorType  = IR_CODE;
       sensorField = IRTEMP_DATA;
    }else if( cb->asmParam == Heart_AsmParameter)
    {
       UPDATE_PROCESS(flag[1],HEART_DONE);
       sensorType  = HEART_CODE;
       sensorField = HEART_DATA;
    }else if( cb->asmParam == Batt_AsmParameter)
    {
       UPDATE_PROCESS(flag[1],BATT_DONE);
       sensorType  = BATT_CODE;
       sensorField = BATT_DATA;
    }else
    {
      bool isAsmDone = isEqual(ASS_DONE,flag[1]);
      while(!isAsmDone)
      {
        isAsmDone = isEqual(ASS_DONE,flag[1]);
      }
      if(isAsmDone)
      {
        UPDATE_FLASH_FLAG(flag[0],PACKAGE_ASM_DONE);
        transferFromFlash(state,cb,flag,ec);
        index = 0;
        return status;
      }
    }
    dgs[index] = insertDatagram(sensorType,sensorField,cb->value);
    index ++ ;
  }
  return status;        
}
bool isConnected(gaprole_States_t state)
{
  if(state == GAPROLE_CONNECTED  || state == GAPROLE_CONNECTED_ADV)
  {
    return true;
  }
  return false;
}
bStatus_t transferFromFlash(gaprole_States_t state,paramCallback *cb,uint8 *flag,EntryCache *ec)
{
  uint8 status = SUCCESS;
  if(isConnected(state) && (cb->setParam) == Flash_SetParameter)
  {
    //read From Flash& transfer
    //read
    //Flash_SetParameter()

    Flash_SetParameter( FLASH_DATA, FLASH_DATA_LEN, cb->value);
    //cb->setParam(cb->param,cb->len,cb->value);
  }
  else if(!isConnected(state) && (cb->setParam) != Flash_SetParameter){
    uprint("sf");
    status = saveToFlash((uint8 *)cb->value,flag,ec);
  }
  else                          // NOT CON and flash_setParam
  {
    //Should not in here
  }
  return status;
}

bStatus_t saveToFlash(uint8 *data,uint8 *flag,EntryCache *ec)
{
  uint8 status = SUCCESS;
  //ASSEMBLE Structure
  //ssemblePackage(dg,height,package_addr,uint8 No,uint8 *deviceFlag,uint8 *prevPackageAddr); 
  MXFlash_Init();
  uint8 deviceFlag[2] = {MAXTAIN,BABY_TALK};

  UTCTime tt = osal_getClock();
  UTCTimeStruct utctime;
  osal_ConvertUTCTime(&utctime,tt);
  
  Package pack = assemblePackage(dgs,index,deviceFlag,utctime);
  
  UPDATE_FLASH_FLAG(flag[0],PACKAGE_WRITE_BACK_START);
  //write back
  write_back(ec,pack,utctime);
  
  
  UPDATE_FLASH_FLAG(flag[0],PACKAGE_WRITE_BACK_DONE);
  UPDATE_PROCESS(flag[1],FLASH_WRITE_BACK_DONE);
  MXFlash_Stop();
  
  return status;
}

/*********************************************************************
 * @fn      Accel read xyz 
 *
 * @brief   get an accel 
 *
 * @param   xyz 6 bytes XH XL YH YL ZH ZL
 *
 * @return  void
 */
void accReadXYZ(uint8 xyz[])
{
  int16 new_x, new_y, new_z;
  AxesRaw_t buf;
  bStatus_t stat;
  
  stat = LIS3DH_GetAccAxesRaw(&buf);
  if (stat == MEMS_ERROR)
  {
    uprint("err\n\n");
  }  
  new_x = buf.AXIS_X;
  new_y = buf.AXIS_Y;
  new_z = buf.AXIS_Z;

  xyz[0] = (new_x >> 8) & 0xff;
  xyz[1] = new_x & 0xff;
  xyz[2] = (new_y >> 8) & 0xff;
  xyz[3] = new_y & 0xff;
  xyz[4] = (new_z >> 8) & 0xff;
  xyz[5] = new_z & 0xff;
  
}