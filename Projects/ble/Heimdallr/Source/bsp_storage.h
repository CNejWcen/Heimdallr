/**************************************************************************************************
  Filename:       bsp_storage.h
  Revised:        $Date: 2014-12-04 10:20:18 +0800 (Fri, 04 Apr 2014) $
  Revision:       $Revision: 10001 $

  Description:    This file contains the board support package of storage data.
**************************************************************************************************/

#ifndef __BSP_STORAGE_H__
#define __BSP_STORAGE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_types.h"
#include "hal_timer.h"
#include "comdef.h"
#include "hal_drivers.h"
#include "hal_dbflash.h"


#define UDHeadLen 0x05
#define UDLen2 	0x00
#define UDLen4 	0x40
#define UDLen6 	0x80
#define UDLen8 	0xC0

#define UDTHumi 0x00
#define UDTTemp 0x01
#define UDTSlep	0x02
#define UDTRsv1 0x03
#define UDTRsv2 0x04
#define UDTRsv3 0x05
#define UDTBatt 0x06
#define UDTTime 0x07
#define UDTWarn 0x10
#define UDTErro	0x17
typedef struct __data_info
{
	uint32 datasize;
	uint8 daycount;
	uint32 iterAddr;
	uint32 startAddr;
	uint32 endAddr;
	uint32 newAddr;
	uint32 lastUpdateTime;
	uint32 lastSyncTime;
}DataPage;
typedef struct __user_info
{
	uint8 name[20];
	uint8 fatherName[20];
	uint8 motherName[20];
	uint8 fatherPhone[20];
	uint8 matherPhone[20];
	uint8 email[20];
	uint8 address[100];
}UserPage;	//need to be auto refresh

typedef struct __secu_info
{
	uint8 username[64];
	uint8 password[128];
}SecuPage;

typedef struct __time_info
{
	uint8 time[4];
}TimePage;

typedef struct __setttings
{
	uint8 setting[32];
	uint32 intervalTime;
	uint8 type[32];
	uint8 position[32];
}SettingsPage;

typedef struct __hardware_info
{
	uint32 manufacture;
	uint32 version;
	uint32 firmversion;
	uint32 firstOn;
	uint32 firstSync;
	uint32 lastUpdate;
	uint32 deviceID;
}HardWareInfo;

typedef struct flashdata
{
	uint8 meta;
	uint32 time;
	uint8 data[];
}UnitData;

uint8 UD_init(UnitData *unit,uint8 type,uint8 *data);
uint8 UD_setLength(uint8 type,uint8 len);
uint8 UD_setType(uint8 type,uint8 sensor);
uint32 UD_write(UnitData *unit,uint32 addr);
uint32 UD_read(UnitData *unit,uint32 addr);
uint8 *UD_toString(UnitData unit);
uint8 getUDLen(uint8 type);
uint8 getUDLenCode(uint8 len);
uint32 bsp_write32b(uint32 addr,uint32 data);
uint32 bsp_write8b(uint32 addr, uint8 data);
uint32 bsp_write(uint32 addr,uint8 *buffer,uint8 len);
uint32 bsp_read32b(uint32 addr,uint32 *data);
uint32 bsp_read8b(uint32 addr,uint8 *data);
uint32 bsp_read(uint32 addr,uint8 *buffer,uint8 len);
void bsp_dataPg2Mem(void);
void bsp_userPg2Mem(void);
void bsp_secuPg2Mem(void);
void bsp_timePg2Mem(void);
void bsp_setPg2Mem(void);
void bsp_hwPg2Mem(void);


#ifdef __cplusplus
}
#endif
#endif


