/**************************************************************************************************
  Filename:       hal_dbflash.c
  Revised:        $Date: 2014-12-04 10:20:18 +0800 (Fri, 04 Apr 2014) $
  Revision:       $Revision: 10001 $

  Description:    This file contains the driver of 45DB081E.
**************************************************************************************************/


#include "hal_dbflash.h"



void DBRead(uint32 addr,uint8 *buffer,uint8 len)
{
	DBOpStart(DB_ARRAY_READ_LP,addr);
	for (int i = 0; i < len; ++i)
	{
		SPIRead(&buffer[i],DUMMY);
	}
	DBOpEnd();
}
//DB_BUFFER1	DB_BUFFER2
//addr three address bytes comprised of 15 dummy bits and 9 buffer address bits (BFA8 - BFA0)
void DBReadBuffer(uint8 which,uint32 addr,uint8 *buffer,uint8 len)
{
	DBOpStart(which,addr);
	for (uint8 i = 0; i < len; ++i)
	{
		SPIRead(&buffer[i],DUMMY);
	}
	DBOpEnd();
}
void DBReadBuffer1(uint32 addr,uint8 *buffer,uint8 len)
{
	DBReadBuffer(DB_BUFFER1,addr,buffer,len);
}
void DBReadBuffer2(uint32 addr,uint8 *buffer,uint8 len)
{
	DBReadBuffer(DB_BUFFER2,addr,buffer,len);
}

void DBWriteBuffer(uint8 which,uint32 addr,uint8 *buffer,uint8 len)
{
	DBOpStart(which,addr);
	DBSendData(buffer,len);
	DBOpEnd();
}
void DBWriteBuffer1(uint32 addr,uint8 *buffer,uint8 len)
{
	DBWriteBuffer(DB_BUFFER1_W,addr,buffer,len);
}
void DBWriteBuffer2(uint32 addr,uint8 *buffer,uint8 len)
{
	DBWriteBuffer(DB_BUFFER2_W,addr,buffer,len);
}
void DBBufferPage2Mem(uint8 which,uint32 addr)
{
	DBOpStart(which,addr);
	DBOpEnd();
	//while(isBusy());
}
void DBBuffer1PageToMem(uint32 addr)
{
	DBBufferPage2Mem(DB_B1ToM_E,addr);
}
void DBBuffer2PageToMem(uint32 addr)
{
	DBBufferPage2Mem(DB_B2ToM_E,addr);
}
void DBWritePageThroughBuffer(uint8 which,uint32 addr,uint8 *buffer,uint8 len)
{	
	DBOpStart(which,addr);
	DBSendData(buffer,len);
	DBOpEnd();
	//while(isBusy());
}

void DBWritePageThroughBuffer1(uint32 addr,uint8 *buffer,uint8 len)
{
	DBWritePageThroughBuffer(DB_COM_B1ToM_E,addr,buffer,len);
}
void DBWritePageThroughBuffer2(uint32 addr,uint8 *buffer,uint8 len)
{
	DBWritePageThroughBuffer(DB_COM_B2ToM_E,addr,buffer,len);
}

void DBWriteByteThroughBuffer(uint32 addr,uint8 *buffer,uint8 len)
{
	DBOpStart(DB_WRITEBYTE2MEM,addr);
	DBSendData(buffer,len);
	DBOpEnd();
	//while(isBusy());
}
void DBWrite(uint32 addr,uint8 *data,uint8 len)
{
	DBWriteByteThroughBuffer(addr,data,len);
}

void DBModify(uint8 which,uint32 addr,uint8 *buffer,uint8 len) 
{
	if(which == 1){
		DBOpStart(DB_READMODIFYWRITE1,addr);
	}
	else
	{
		DBOpStart(DB_READMODIFYWRITE2,addr);
	}
	DBSendData(buffer,len);
	DBOpEnd();
	//while(isBusy());
}
void DBPageErase(uint32 addr)
{
	DBOpStart(DB_PAGE_ERASE,addr);
	DBOpEnd();
}
void DBBlockErase(uint32 addr)
{
	DBOpStart(DB_BLOCK_ERASE,addr);
	DBOpEnd();
}
void DBSectorErase(uint32 addr)
{
	DBOpStart(DB_SECTOR_ERASE,addr);
	DBOpEnd();
}
void DBChipErase(void)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_CHIP_ERASE1);
	SPIWrite(DB_CHIP_ERASE2);
	SPIWrite(DB_CHIP_ERASE3);
	SPIWrite(DB_CHIP_ERASE4);
	FLASH_CS = SPI_DISABLE;
}
/*********************
	*	
	*	Control
	*
*********************/
void DBSupend(void)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_SUSPEND); 
	FLASH_CS = SPI_DISABLE;
}
void DBResume(void)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_RESUME); 
	FLASH_CS = SPI_DISABLE;
}

void DBMemPage2Buffer(uint8 which,uint32 addr)
{
	DBOpStart(which,addr);
	DBOpEnd();
}
void DBMemPage2Buffer1(uint32 addr)
{
	DBMemPage2Buffer(DB_M2B1,addr);
}
void DBMemPage2Buffer2(uint32 addr)
{
	DBMemPage2Buffer(DB_M2B2,addr);
}
//To confirm successfully programmed after a Buffer to Main Memory Page Program command.
void DBCompareMemVsBuffer(uint8 which,uint32 addr)
{
	DBOpStart(which,addr);
	DBOpEnd();
}
void DBCompareMemVsBuffer1(uint32 addr)
{
	DBCompareMemVsBuffer(DB_MvsB1,addr);
}
void DBCompareMemVsBuffer2(uint32 addr)
{
	DBCompareMemVsBuffer(DB_MvsB2,addr);
}
void DBConfirmWrite1(uint32 addr)
{
	DBCompareMemVsBuffer1(addr);
}
void DBConfirmWrite2(uint32 addr)
{
	DBCompareMemVsBuffer2(addr);
}

/****
	* To preserve data integrity of a sector, each page within a sector must be updated/rewritten at least once within every 50,000 cumulative page
	* erase/program operations within that sector. 
	* The operation is internally self-timed and should take place in a maximum time of tEP.
****/
void DBAutoPageRewriteEx(uint8 which,uint32 addr)
{
	DBOpStart(which,addr);
	DBOpEnd();
}
void DBAutoPageRewrite(uint32 addr)
{
	DBAutoPageRewriteEx(DB_AutoPageRewrite2,addr);
}

uint16 DBGetStatus(void)
{
	uint16 status;
	uint8 st[2];
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_GET_STATUS);
	SPIRead(&st[1],0xFF);
	SPIRead(&st[0],0xFF);
	FLASH_CS = SPI_DISABLE;
	status = st[1] << 8 + st[0];
	return status;
}

bool isBusy(void)
{
	if (DBGetStatus() & 0x8000)
	{
		return false;
	}
	return true;   //when bit 7 is 0 ,is busy
}
bool isSame(void)
{
	if (DBGetStatus() & 0x4000)
	{
		return false;
	}
	return true;    		//when bit 6 is 0, Main memory page data matches buffer data.
}
bool isProtectionDisabled(void)
{
	if (DBGetStatus() & 0x0200)
	{
		return false;
	}
	return true;		//when bit 1 is 0, Sector protection is disabled,so true
}
bool isError(void)
{
	if (DBGetStatus() & 0x0020)
	{
		return true;				//if bit 5 is 1 ,Erase or program error detected
	}
	return false;
}

//Power Management
void DBDeepPowerDown(void)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_DEEP_POWER_DOWN);
	FLASH_CS = SPI_DISABLE;
	WAIT_MS(3);
}
void DBExitDeepPowerDown(void)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_DEEP_POWER_DOWN);
	FLASH_CS = SPI_DISABLE;
	WAIT_MS(3);
}
void DBUltraDeepPowerDown(void)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_ULTRA_DEEP_POWER_DOWN);
	FLASH_CS = SPI_DISABLE;
	WAIT_MS(3);	
}
void DBExitUltraDeepPowerDown(void)
{
	FLASH_CS = SPI_ENABLE;
	WAIT_MS(1);
	FLASH_CS = SPI_DISABLE;
	WAIT_MS(3);
}

void DBPageSizeConfig(uint8 type)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_PAGE_SIZE1);
	SPIWrite(DB_PAGE_SIZE2);
	SPIWrite(DB_PAGE_SIZE3);
	if (type == DB256)
	{
		SPIWrite(DB_PAGE_AS_256);
	}else if (type == DB264)
	{
		SPIWrite(DB_PAGE_AS_264);
	}
	FLASH_CS = SPI_DISABLE;
	WAIT_MS(3);	
}
/***********************
	*
	*  DB___________XXXXX function is dangerous. NOT recommanded.
	*
*********************/
void DB___________SoftProtection(bool val)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	if (val == FLASH_DISABLE)
	{
		FLASH_CS = SPI_ENABLE;
		SPIWrite(DB_SECTOR_PROTECT1);
		SPIWrite(DB_SECTOR_PROTECT2);
		SPIWrite(DB_SECTOR_PROTECT3);
		SPIWrite(DB_SECTOR_PROTECT_DISABLE);
		FLASH_CS = SPI_DISABLE;
	}
	if (val == FLASH_ENABLE)
	{
		FLASH_CS = SPI_ENABLE;
		SPIWrite(DB_SECTOR_PROTECT1);
		SPIWrite(DB_SECTOR_PROTECT2);
		SPIWrite(DB_SECTOR_PROTECT3);
		SPIWrite(DB_SECTOR_PROTECT_ENABLE);
		FLASH_CS = SPI_DISABLE;
	}
}
void DB___________EraseSectorProtecReg(void)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_SECTOR_PROTECT1);
	SPIWrite(DB_SECTOR_PROTECT2);
	SPIWrite(DB_SECTOR_PROTECT3);
	SPIWrite(DB_ERASE_SECTOR_REG);
	FLASH_CS = SPI_DISABLE;
}
void DB___________SetSectorReg(uint8 *settings)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(DB_SECTOR_PROTECT1);
	SPIWrite(DB_SECTOR_PROTECT2);
	SPIWrite(DB_SECTOR_PROTECT3);
	SPIWrite(DB_SET_SECTOR_REG);
	for (uint8 i = 0; i < 16; ++i)
	{
		SPIWrite(settings[i]);
	}
	FLASH_CS = SPI_DISABLE;
}
void DB___________FrezzeLockDown(void)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(0x34);
	SPIWrite(0x55);
	SPIWrite(0xAA);
	SPIWrite(0x40); 
	FLASH_CS = SPI_DISABLE;
}

void DBOpStart(uint8 opcode,uint32 addr)
{
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);
	FLASH_CS = SPI_ENABLE;
	SPIWrite(opcode);
	SPIWrite((addr & 0xff0000 ) >> 16);
	SPIWrite((addr & 0xff00 ) >> 8);
	SPIWrite(addr & 0xff);
}
void DBOpEnd()
{
	FLASH_CS = SPI_DISABLE;
}
void DBSendData(uint8 *buffer,uint8 len)
{
	for (uint8 i = 0; i < len; ++i)
	{
		SPIWrite(buffer[i]);
	}
}
void DBReset(bool val)
{
	P1DIR |= FLASH_RESET_BIT;       
	P1SEL &= ~FLASH_RESET_BIT; 
	FLASH_RST = val;
}
void DBWriteProtect(bool val)
{
	P1DIR |= FLASH_WP_BIT;       
	P1SEL &= ~FLASH_WP_BIT; 
	FLASH_WP = val;
}
void DBTest(void)
{
	SPI_Init();
	DBReset(FLASH_DISABLE);
	DBWriteProtect(FLASH_DISABLE);

	FLASH_CS = SPI_ENABLE;
    uint8 buf[3];
	SPIWrite(0x9F);
	SPIRead(&buf[0],0xFF);
	SPIRead(&buf[1],0xFF);
	SPIRead(&buf[2],0xFF);
	dbg("\nFlash ID:%02x %02x %02x\n\r",buf[0],buf[1],buf[2]);
	FLASH_CS = SPI_DISABLE;
}

void SPI_Init(void)
{
    // USART 1 at alternate location 2
    PERCFG |= 0x02;
    //  0100 0000  For USART1 has priority
    P2SEL |= 0x60;
    P2DIR |= 0x40;         //0100 0000
    // Peripheral function on SCK, MISO and MOSI (P1_4-7)11100000
    P1SEL = (P1SEL & ~0x10) | 0xE0;
    // Configure CS (P1_2) as output
    P1DIR |= 0x10;
      
    FLASH_CS = SPI_DISABLE;
    //*** Setup the SPI interface ***
    // SPI master mode
    U1CSR = 0x00;
    // Positive clock polarity, Phase: data out on CPOL -> CPOL-inv
    //                                 data in on CPOL-inv -> CPOL
    // MSB first
    // 1010 0000
    U1GCR = 0x20;
    
    // SCK frequency = 33MHz (CC254x max = 4MHz)
    //U1GCR |= 0x10;//0x10;
    //U1BAUD = 0x00 ;//0x80; 
    
    // SCK frequency = 3MHz (MBA250 max=10MHz, CC254x max = 4MHz)
    U1GCR |= 0x10;
    U1BAUD = 0x80;
    
    // Wait 2ms for Chip Select to power up and settle
    WAIT_MS(2);
    FLASH_CS = SPI_ENABLE;
}


/**************************************************************************//**
* @fn       SPIWrite(uint8 write)
*
* @brief    Write one byte to SPI interface
*
* @param    write   Value to write
******************************************************************************/
void SPIWrite(uint8 write)
{
        U1CSR &= ~0x02;                 // Clear TX_BYTE
        U1DBUF = write;
        while (!(U1CSR & 0x02));        // Wait for TX_BYTE to be set
}
/**************************************************************************//**
* @fn       SPIRead(uint8 *read, uint8 write)
*
* @brief    Read one byte from SPI interface
*
* @param    read    Read out value
* @param    write   Value to write
******************************************************************************/
void SPIRead(uint8 *read, uint8 write)
{
        U1CSR &= ~0x02;                 // Clear TX_BYTE
        U1DBUF = write;                 // Write address
        while (!(U1CSR & 0x02));        // Wait for TX_BYTE to be set
        *read = U1DBUF;                 // Save returned value
}
