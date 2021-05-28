/**************************************************************************************************
  Filename:       bsp_storage.c
  Revised:        $Date: 2014-12-04 10:20:18 +0800 (Fri, 04 Apr 2014) $
  Revision:       $Revision: 10001 $

  Description:    This file contains the board support package of storage data.
**************************************************************************************************/

#include "bsp_storage.h"


extern volatile DataPage _dataPg;
extern volatile UserPage _userPg;
extern volatile SecuPage _secuPg;
extern volatile TimePage _timePg;
extern volatile SettingsPage _settingsPg;
extern volatile HardWareInfo _hardPg;
extern volatile uint32 __currentTimeUnix;


uint8 UD_init(UnitData *unit,uint8 type,uint8 *data)
{
	uint8 datalen = getUDLen(type);
	unit = (UnitData *)malloc(sizeof(UnitData) + sizeof(uint8) * datalen );
	unit->type = type;
	unit->time = __currentTimeUnix;
	for (uint8 i = datalen - 1; i >= 0; --i)
	{
		unit->data[i] = data[i];
	}
	return datalen;
}
uint8 UD_setLength(uint8 type,uint8 len)
{
	return type | getUDLenCode(len);
}
uint8 UD_setType(uint8 type,uint8 sensor)
{
	return type | sensor;
}

uint32 UD_write(UnitData *unit,uint32 addr)
{
	uint8 datalen = getUDLen(unit->type);
	addr = bsp_write8b(addr,unit->type);
	addr = bsp_write32b(addr,unit->time);
	for (uint8 i = datalen -1; i >= 0 ; --i)
	{
		addr = bsp_write8b(addr,unit->data[i]);
	}
	return addr;
}

uint32 UD_read(UnitData *unit,uint32 addr)
{
	uint8 type;
	addr = bsp_read8b(addr,&type);
	uint8 datalen = getUDLen(type);
	unit = (UnitData *)malloc(sizeof(UnitData) + sizeof(uint8) * datalen );
	unit->type = type;
	addr = bsp_read32b(addr,&unit->time);
	addr = bsp_read(addr,unit->data,datalen);
	return addr;
}
uint8 *UD_toString(UnitData unit)
{
	uint8 unitlen = UDHeadLen + getUDLen(unit.type);
	static uint8 *string;
	string = malloc(unitlen * sizeof(uint8));
	string[0] = unit.type;
	string[1] = (unit.time & 0xff000000) >> 24;
	string[2] = (unit.time & 0x00ff0000) >> 16;
	string[3] = (unit.time & 0x0000ff00) >> 8;
	string[4] = (unit.time & 0x000000ff);
	uint8 datalen = unitlen - UDHeadLen;
	for (uint8 i = datalen - 1; i >=0 ; --i)
	{	//5 + (datalen -1 -i)
		string[4 + datalen - i] = unit.data[i];	
	}
	return string;
}
uint8 getUDLen(uint8 type)
{
	switch(type & 0xC0 )
	{
		case UDLen2:
			return 2;
		case UDLen4:
			return 4;
		case UDLen6:
			return 6;
		case UDLen8:
			return 8;
		default:
			break;
	}
	return 2;
}
uint8 getUDLenCode(uint8 len)
{
	switch(len)
	{
		case 2:
			return UDLen2;
		case 4:
			return UDLen4;
		case 6:
			return UDLen6;
		case 8:
			return UDLen8;
		default:
			break;
	}
	return UDLen2;
}
/*********************
 * 
 * MSB save unit32 data
 * @params: addr first 
 * @return: next addr
 *
*********************/
uint32 bsp_write32b(uint32 addr,uint32 data)
{
	uint8 buffer[4];
	buffer[3] = (data & 0xff000000) >> 24;
	buffer[2] = (data & 0x00ff0000) >> 16;
	buffer[1] = (data & 0x0000ff00) >> 8;
	buffer[0] = (data & 0x000000ff);
	DBWrite(addr,buffer,4);
	return addr + 4;
}
uint32 bsp_write8b(uint32 addr, uint8 data)
{
	DBWrite(addr,data,1);
	return addr + 1;
}
uint32 bsp_write(uint32 addr,uint8 *buffer,uint8 len)
{
	DBWrite(addr,buffer,len);
	return addr + len;
}
uint32 bsp_read32b(uint32 addr,uint32 *data)
{
	uint8 buffer[4];
	DBRead(addr,buffer,4);
	*data = ((buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0]);
	return addr + 4;
}
uint32 bsp_read8b(uint32 addr,uint8 *data)
{
	DBRead(addr,data,1);
	return addr + 1;
}
uint32 bsp_read(uint32 addr,uint8 *buffer,uint8 len)
{
	DBRead(addr,buffer,len);
	return addr + len;
}
	// uint32 datasize;
	// uint8 daycount;
	// uint32 iterAddr;
	// uint32 startAddr;
	// uint32 endAddr;
	// uint32 newAddr;
	// uint32 lastUpdateTime;
	// uint32 lastSyncTime;
void bsp_dataPg2Mem(void)
{
	uint32 addr = DATA_INFO;
	addr = bsp_write32b(addr,_dataPg.datasize);
	addr = bsp_write8b(addr,_dataPg.daycount);
	addr = bsp_write32b(addr,_dataPg.iterAddr);
	addr = bsp_write32b(addr,_dataPg.startAddr);
	addr = bsp_write32b(addr,_dataPg.endAddr);
	addr = bsp_write32b(addr,_dataPg.newAddr);
	addr = bsp_write32b(addr,_dataPg.lastUpdateTime);
	addr = bsp_write32b(addr,_dataPg.lastSyncTime);
}
// uint8 name[20];
// uint8 fatherName[20];
// uint8 motherName[20];
// uint8 fatherPhone[20];
// uint8 matherPhone[20];
// uint8 email[20];
// uint8 address[100];
void bsp_userPg2Mem(void)
{
	uint32 addr = USER_INFO;
	addr = bsp_write(addr,_userPg.name,20);
	addr = bsp_write(addr,_userPg.fatherName,20);
	addr = bsp_write(addr,_userPg.motherName,20);
	addr = bsp_write(addr,_userPg.fatherPhone,20);
	addr = bsp_write(addr,_userPg.matherPhone,20);
	addr = bsp_write(addr,_userPg.email,20);
	addr = bsp_write(addr,_userPg.address,100);

}
	// uint8 username[64];
	// uint8 password[128];
void bsp_secuPg2Mem(void)
{
	uint32 addr = SECU_DATA;
	addr = bsp_write(addr,_userPg.username,64);
	addr = bsp_write(addr,_userPg.password,128);
}
void bsp_timePg2Mem(void)
{
	bsp_write32b(TIME_DATA,_timePg.time);
}
	// uint8 setting[32];
	// uint32 intervalTime;
	// uint8 type[32];
	// uint8 position[32];
void bsp_setPg2Mem(void)
{
	uint32 addr = SETTINGS;
	addr = bsp_write(addr,_userPg.username,64);
	addr = bsp_write(addr,_userPg.password,128);
}
	// uint32 manufacture;
	// uint32 version;
	// uint32 firmversion;
	// uint32 firstOn;
	// uint32 firstSync;
	// uint32 lastUpdate;
void bsp_hwPg2Mem(void)
{
	uint32 addr = HW_INFO;
	addr = bsp_write32b(addr,_hardPg.manufacture);
	addr = bsp_write32b(addr,_hardPg.version);
	addr = bsp_write32b(addr,_hardPg.firmversion);
	addr = bsp_write32b(addr,_hardPg.firstOn);
	addr = bsp_write32b(addr,_hardPg.firstSync);
	addr = bsp_write32b(addr,_hardPg.lastUpdate);
	addr = bsp_write32b(addr,_hardPg.deviceID);
}
