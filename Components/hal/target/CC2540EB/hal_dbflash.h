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

//Address es


void SPI_Init(void);
void SPIWrite(uint8 write);
void SPIRead(uint8 *read, uint8 write);


void DbTest(void);

void DbWriteProtect(bool val);
void DbReset(bool val);


#ifdef __cplusplus
}
#endif

#endif