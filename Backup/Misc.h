/**************************************************************************************************
  Filename:       misc.h
  Revised:        $Date: 2014-04-30 20:43:17 +0800 (Wen, 30 Apr 2014) $
  Revision:       $Revision: 00001 $

  Description:    The Misc function of all

**************************************************************************************************/


#ifndef __MISC__H__
#define __MISC__H__

#include "bcomdef.h"
#include "peripheral.h"
//bStatus_t Accel_SetParameter( uint8 param, uint8 len, void *value );

#define FLASH_DOWN                      0x00
#define FLASH_INIT                      0x01
#define PACKAGE_ASM_START               0x02
#define PACKAGE_ASM_DONE                0x04
#define PACKAGE_WRITE_BACK_START        0x08
#define PACKAGE_WRITE_BACK_DONE         0x10
#define FLASH_SYNCING                   0x20
#define FLASH_SYNC_INT                  0x40
#define FLASH_SYNC_DONE                 0x80

#define UPDATE_FLASH_FLAG(x,flag)       x = flag;

#define FULL_PROCESS(x)                    x |= 0x80;
#define PART_PROCESS(x)                    x &= ~0x80;

#define UPDATE_PROCESS(x,pro)             x |= pro;
#define CLEAR_PROCESS(x)                  x  = 0x00;

#define ASSEMBLE_START                    0x00
#define ACCEL_XYZ_DONE                    0x01
#define ACCEL_6D_DONE                     0x02
#define HUMI_DONE                         0x04
#define IR_DONE                           0x08
#define HEART_DONE                        0x10
#define BATT_DONE                         0x20
#define FLASH_WRITE_BACK_DONE             0x40

//1011 1111
#define ASS_DONE                          0xBF
#define ALL_DONE                          0xFF


typedef struct {
  bStatus_t (*setParam)(uint8 , uint8 , void *);
  bStatus_t (*asmParam)(uint8 , uint8*, void *);
  uint8 param;
  uint8 len;
  void *buf;
  void *value;
}paramCallback;


// determine whether use realtime transfer or  read from flash
//Transfer Real-time            determine which UUID 
bStatus_t transferData(gaprole_States_t state,paramCallback *cb,uint8 *flag,EntryCache *ec);

//read from Flash               And use the tunnul of Flash Data

bStatus_t transferFromFlash(gaprole_States_t state,paramCallback *cb,uint8 *flag,EntryCache *ec);

bStatus_t saveToFlash(uint8 *data,uint8 *flag,EntryCache *ec);


void accReadXYZ(uint8 xyz[]);

#endif