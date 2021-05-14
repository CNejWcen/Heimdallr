/**************************************************************************************************
  Filename:       hal_dbflash.h
  Revised:        $Date: 2014-12-04 10:20:18 +0800 (Fri, 04 Apr 2014) $
  Revision:       $Revision: 10001 $

  Description:    This file contains the driver of 45DB081E.
**************************************************************************************************/

#ifndef __HAL_DBFLASH_H__
#define __HAL_DBFLASH_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_types.h"
#include "hal_timer.h"
#include "comdef.h"
#include "hal_drivers.h"
	
#define FLASH_RESET_BIT  0x08
#define FLASH_WP_BIT     0x02
#define FLASH_CS_BIT     0x10
#define FLASH_CS        P1_4
#define FLASH_RST       P1_3
#define FLASH_WP        P1_1       

#define SPI_ENABLE      0
#define SPI_DISABLE     1
#define FLASH_ENABLE	0
#define FLASH_DISABLE	1

#define DUMMY 	0xFF

//Read
	//Circular
#define DB_ARRAY_READ_HF4	0x1B				//2 Dummy 			Max 85Mhz	High Frequency
#define	DB_ARRAY_READ_HF1	0x0B				//1 Dummy 			Max 85Mhz	High Frequency
#define DB_ARRAY_READ_LF	0x03				//NO Dummy 			Max 50MHz 	Low Frequency
#define DB_ARRAY_READ_LP	0x01 				//NO Dummy 			Max 20MHz	Low Power Mode
	//Single page
#define DB_PAGE_READ 		0xD2

#define DB_BUFFER1_READ_HF	0xD1				//High frequency read buffer1
#define DB_BUFFER1_READ_LF	0xD4				//Low frequency read buffer1
#define DB_BUFFER2_READ_HF	0xD6 				//High frequency read buffer2
#define DB_BUFFER2_READ_LF	0xD3				//Low frequency read buffer2

#define DB_BUFFER1 DB_BUFFER1_READ_LF
#define DB_BUFFER2 DB_BUFFER2_READ_LF
//Write

#define DB_BUFFER1_W		0x84
#define DB_BUFFER2_W		0x87

#define DB_B1ToM_E 			0x83			//Buffer to Main Memory Page Program with Built-In Erase
#define DB_B2ToM_E 			0x86
#define DB_B1ToM_NE 		0x88			//Buffer to Main Memory Page Program without Built-In Erase
#define DB_B2ToM_NE 		0x89

#define DB_COM_B1ToM_E		0x82
#define DB_COM_B2ToM_E 		0x85

#define DB_WRITEBYTE2MEM	0x02			//Main Memory Byte/Page Program through Buffer 1 without Built-In Erase

#define DB_READMODIFYWRITE1	0x58 			//Read-Modify-Write
#define DB_READMODIFYWRITE2	0x59 			//Read-Modify-Write
//Erase
#define DB_PAGE_ERASE		0x81 			
#define DB_BLOCK_ERASE		0x50
#define DB_SECTOR_ERASE		0x7C

#define DB_CHIP_ERASE1		0xC7 			//Sequence
#define DB_CHIP_ERASE2		0x94
#define DB_CHIP_ERASE3		0x80
#define DB_CHIP_ERASE4		0x9A

//Control
#define DB_SUSPEND 		 	0xB0
#define DB_RESUME 			0xD0

//Security
#define DB_SECTOR_PROTECT1	0x3D
#define DB_SECTOR_PROTECT2	0x2A
#define DB_SECTOR_PROTECT3	0x7F
#define DB_SECTOR_PROTECT_ENABLE	0xA9 
#define DB_SECTOR_PROTECT_DISABLE 	0x9A

#define DB_ERASE_SECTOR_REG 0xCF
#define DB_SET_SECTOR_REG 0xFC
//0~15 protect
#define DBD_PROTECTED 		0xFF
#define DBD_UNPROTECT		0x00
//sector a b  protect
#define DBD_SECTOR_UNP		0x00
#define DBD_SECTOR_A_P 		0xC0
#define DBD_SECTOR_B_P		0x30 
#define DBD_SECTOR_AB_P		0xF0
//Not Recommand to use.The Sector Protection Register is subject to a limit of 10,000 erase/program cycles. 
const uint8 db_protected[16] = {DBD_SECTOR_A_P,DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED,\
										 	   DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED,\
											   DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED,\
											   DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED,\
											   DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED };

const uint8 db_normal[16] 	= {DBD_SECTOR_UNP, DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED,\
										 	   DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED,\
											   DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED,\
											   DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED,\
											   DBD_PROTECTED,DBD_PROTECTED,DBD_PROTECTED };


//Additional
#define DB_M2B1   		0x53 
#define DB_M2B2 		0x55 

#define DB_MvsB1		0x60
#define DB_MvsB2 		0x61

#define DB_AutoPageRewrite1 0x58
#define DB_AutoPageRewrite2 0x59

#define DB_GET_STATUS   0xD7
#define DB_DEEP_POWER_DOWN		0xB9
#define DB_EXIT_DEEP_POWER_DOWN		0xAB
#define DB_ULTRA_DEEP_POWER_DOWN	0x79
#define DB_EXIT_ULTRA_DEEP_POWER_DOWN	0x79

#define DB256  0x01
#define DB264  0x00

#define DB_PAGE_SIZE1 0x3D
#define DB_PAGE_SIZE2 0x2A
#define DB_PAGE_SIZE3 0x80

#define DB_PAGE_AS_256 0xA6
#define DB_PAGE_AS_264 0xA7
//Address es


void SPI_Init(void);
void SPIWrite(uint8 write);
void SPIRead(uint8 *read, uint8 write);

// Flash Read
void DBRead(uint32 addr,uint8 *buffer,uint8 len);
void DBReadBuffer(uint8 which,uint32 addr,uint8 *buffer,uint8 len);
void DBReadBuffer1(uint32 addr,uint8 *buffer,uint8 len);
void DBReadBuffer2(uint32 addr,uint8 *buffer,uint8 len);

// Write to Buffer
void DBWriteBuffer(uint8 which,uint32 addr,uint8 *buffer,uint8 len);
void DBWriteBuffer1(uint32 addr,uint8 *buffer,uint8 len);
void DBWriteBuffer2(uint32 addr,uint8 *buffer,uint8 len);
// Buffer to Memory
void DBBufferPage2Mem(uint8 which,uint32 addr);
void DBBuffer1PageToMem(uint32 addr);
void DBBuffer2PageToMem(uint32 addr);

// Write Through With Erase whole Page
void DBWritePageThroughBuffer(uint8 which,uint32 addr,uint8 *buffer,uint8 len);
void DBWritePageThroughBuffer1(uint32 addr,uint8 *buffer,uint8 len);
void DBWritePageThroughBuffer2(uint32 addr,uint8 *buffer,uint8 len);

// Write Through Without  Erase whole Page
void DBWriteByteThroughBuffer(uint32 addr,uint8 *buffer,uint8 len);
void DBWrite(uint32 addr,uint8 *data,uint8 len);		// abbr. of DBWriteByteThroughBuffer

// Modify part of Page
void DBModify(uint8 which,uint32 addr,uint8 *buffer,uint8 len); 

// Erase Data
void DBPageErase(uint32 addr);
void DBBlockErase(uint32 addr);
void DBSectorErase(uint32 addr);
void DBChipErase(void);

// Control
void DBSupend(void);
void DBResume(void);

// Memory to Buffer
void DBMemPage2Buffer(uint8 which,uint32 addr);
void DBMemPage2Buffer1(uint32 addr);
void DBMemPage2Buffer2(uint32 addr);

/****
*
*To confirm successfully programmed after a Buffer to Main Memory Page Program command.
*
****/
void DBCompareMemVsBuffer(uint8 which,uint32 addr);
void DBCompareMemVsBuffer1(uint32 addr);
void DBCompareMemVsBuffer2(uint32 addr);
void DBConfirmWrite1(uint32 addr);
void DBConfirmWrite2(uint32 addr);
/****
	* To preserve data integrity of a sector, each page within a sector must be updated/rewritten at least once within every 50,000 cumulative page
	* erase/program operations within that sector. 
	* The operation is internally self-timed and should take place in a maximum time of tEP.
****/
void DBAutoPageRewriteEx(uint8 which,uint32 addr);
void DBAutoPageRewrite(uint32 addr);						// Static Data need to be Rewrite

uint16 DBGetStatus(void);

bool isBusy(void);				
bool isSame(void);				// After use DBCompareMemVsBuffer or DBConfirmWrite1
bool isProtectionDisabled(void);
bool isError(void);				// Erase/Program Error

// Power Management
void DBDeepPowerDown(void);
void DBExitDeepPowerDown(void);
void DBUltraDeepPowerDown(void);
void DBExitUltraDeepPowerDown(void);

// Configure PageSize    ---- No USE
void DBPageSizeConfig(uint8 type);

// Basic Op
void DBOpStart(uint8 opcode,uint32 addr);
void DBOpEnd();
void DBSendData(uint8 *buffer,uint8 len);

void DBReset(bool val);
void DBWriteProtect(bool val);
void DBTest(void);

/***********************
	*
	*  DB___________XXXXX function is dangerous. NOT recommanded.
	*
*********************/
void DB___________SoftProtection(bool val);
void DB___________EraseSectorProtecReg(void);
void DB___________SetSectorReg(uint8 *settings);
void DB___________FrezzeLockDown(void);

#ifdef __cplusplus
}
#endif

#endif