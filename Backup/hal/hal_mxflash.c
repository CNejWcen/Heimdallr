/**************************************************************************************************
  Filename:       hal_mxflash.c
  Revised:        $Date: 2014-04-17 13:06:18 -0700 (Thu, 04 Apr 2014) $
  Revision:       $Revision: 10001 $

  Description:    This file contains Read and Write of MX25U1635E.

Todo:   Check isProgFailed after MX_PP
        when unused, MX_SHUTDOWN()
**************************************************************************************************/


#include "hal_mxflash.h"
#include "hal_drivers.h"

#define isEraseFailed (mxReadSCUR()&0x40);
#define isProgFailed (mxReadSCUR()&0x20);
#define isEraseSuspended (mxReadSCUR()&0x08);
#define isProgSuspended (mxReadSCUR()&0x04);
 
// we will simulate spi  because the fault of mine     1011
#define spi_clk             P1_5
#define spi_miso            P1_6                        
#define spi_mosi            P1_7

uint8 getSR();
bool checkWIP();
uint8 checkWEL();
void waitingForNotBusy();

void spiInit_simulate()
{
  P1SEL &= ~0xF0;//1111 0000
  P1DIR |= 0xB0;//1011 0000
}

bool MXFlash_Init()
{
  spiInit();
  CS = CS_DISABLED;
  MX_START();
  MX_WREN_FUN();
  
  uint8 stat = checkWEL(); 
  if(stat == 0)
  {
      waitingForNotBusy();
  }     

  WAIT_1MS();

  return true;
  
}

bool MXFlash_Stop()
{
  waitingForNotBusy();
  MX_SHUTDOWN();
  return true;
}
/*********************************************************************
 * @fn      MXFlash_TEST(-)
 *
 * @brief   Only for Power on Self Test
 * @return  none
 *******************************************************************/
bool MXFlash_TEST()
{ 
  MX_WREN_FUN();
  CS = CS_ENABLED;

  uint8 ids[3]={0,0,0};
  spiWriteByte(MX_RDID);
  
  spiReadByte(&ids[0],0xFF);         //Manuafacture ID 
  spiReadByte(&ids[1],0xFF);         //Memory Type
  spiReadByte(&ids[2],0xFF);         //Memory Capacity
  WAIT_MS(5);
  CS = CS_DISABLED;
  //uprintEx(ids,3);
  if(ids[0] == MX_MANUFACTURE_ID && ids[1] == MX_MEM_TYPE && ids[2] == MX_MEM_CAP)
  {
    uprint("true");
    return true;
  }
  uprint("false");
  return false;
}


/*********************************************************************
 * @fn      readMxFlash_Ex(-)
 *
 * @brief   The first address byte can be at any location.
 *          U1GCR.Order bit order for transfers :MSB first
 *
 *          Can read more than one byte          
 *
 * @param   addr[0] addr[1]
 *           MSB     LSB
 * @return  none
 *******************************************************************/
void readMxFlash_Ex(uint8 *addr,uint16 len,uint8 *pBuf)
{
  CS = CS_ENABLED;
  spiWriteByte(MX_NORMAL_READ);
  spiWriteByte(addr[0]);                        //MSB
  spiWriteByte(addr[1]);
  spiWriteByte(addr[2]);
  while(len >= 1)
  {
    uint8 tmp = 0x00;
    spiReadByte(&tmp,0xFF);
    *pBuf += tmp;
    pBuf++;
    len--;
  }
  CS = CS_DISABLED;
}

void readMxFlash(uint8 *addr,uint8 *pBuf)
{
  readMxFlash_Ex(addr,8,pBuf);
}

  
/*********************************************************************
 * @fn     writeMxFlash_Ex(-)
 *
 * @brief       The last address byte (the 8 least significant address bits, A7-A0) 
 *              should be set to 0 for 256 bytes page program
 *
 *              !!If A7-A0 are not all zero, transmitted data that exceed
 *              page length are programmed from the starting address 
 *              (24-bit address that last 8 bit are all 0) of currently selected page.
 *              
 *              !!!!last 256 byte data
 *              !!If the data bytes sent to the device exceeds 256, the last 256 data byte is programmed at the request page
 *              and previous data will be disregarded. If the data bytes sent to the device has not exceeded 256, the data will be
 *              programmed at the request address of the page. There will be no effort on the other data bytes of the same page     
 *
 *              total page number : 8192 
 *              
 *              16 pages per sector
 *
 * @params  addr[1]  addr[0]    0x00
 *           MSB       LSB
 *
 * @return  none
 *******************************************************************/
void writeMxFlash(uint8 *addr,uint8 *pBuf)
{
  uint8 address[3];
  address[2] = 0x00;
  address[1] = addr[0];
  address[0] = addr[1];
  writeMxFlash_Ex(address,8,pBuf);
}

// locate a exact address begin to write
void writeMxFlash_Ex(uint8 *addr,uint8 len,uint8 *pBuf)
{
  MX_WREN_FUN();
  CS = CS_ENABLED;
  spiWriteByte(MX_PP);
  spiWriteByte(addr[0]);
  spiWriteByte(addr[1]);
  spiWriteByte(addr[2]);

  while (len>=1)
  {
      spiWriteByte(*pBuf++);
      len--;
  }
  CS = CS_DISABLED;
  waitingForNotBusy();
}

void subsectorErase(uint8 *addr)
{
  sectorErase_Ex(MX_SUBSE,addr);
}
/*********************************************************************
 * @fn      sectorErase()
 *
 * @brief   Erase 4KB;
 *          Any address of the block is a valid address for Block Erase (BE) instruction.
 *
 * @param   addr addr[2] addr[1] addr[0]
 *                MSB              LSB
 *
 * @return  none
 *******************************************************************/
void sectorErase(uint8* addr)
{
  sectorErase_Ex(MX_SE,addr);
}

/*********************************************************************
 * @fn      sectorErase_Ex()
 *
 * @brief   Erase 4KB/32KB/64KB;
 *          Any address of the block is a valid address for Block Erase (BE) instruction.
 *
 * @param   addr addr[2] addr[1] addr[0]
 *                MSB              LSB
 *          type 
 *                MX_SE 
 *                MX_SE32K 
 *                MX_SE64K
 * @return  none
 *******************************************************************/
void sectorErase_Ex(uint8 type,uint8* addr)
{
  MX_WREN_FUN();
  CS = CS_ENABLED;
  spiWriteByte(type);
  spiWriteByte(addr[0]);
  spiWriteByte(addr[1]);
  spiWriteByte(addr[2]);
  CS = CS_DISABLED;
  waitingForNotBusy();
}

/*********************************************************************
 * @fn      chipErase()
 *
 * @brief  Erase whole Chip
 *
 * @return  none
 *******************************************************************/
void chipErase()
{
  MX_WREN_FUN();
  CS = CS_ENABLED;
  spiWriteByte(MX_CE);
  CS = CS_DISABLED;
  waitingForNotBusy();
}

/**************************************************************************//**
* @fn       getSR()
*
* @brief    Get Status Register
* 
*       7       6       5       4       3       2       1       0
*       SRWD    0      BP3(TB) BP2     BP1     BP0     WEL     WIP
*
*       SRWD=1 register write disabled
*       QE  =1 Quad Enabled
*       BP3~0  Protected block Addr
*       WEL =1 Write Enable 
*       WIP =1 Write in Progress
*
* @return status register       
*******************************************************************************/
uint8 getSR()
{
  CS = CS_ENABLED;
  uint8 stat;
  spiWriteByte(MX_RDSR);
  spiReadByte(&stat,0xFF);
  CS = CS_DISABLED;
  //uprintEx(&stat,1);
  //uprint("=-");
  return stat;
}
/******************************************************************************
* @function     checkWIP()
*
* @return       true is busy
*               false not busy
******************************************************************************/
bool checkWIP()                 //check the Write in Progress bit
{
  uint8 stat = getSR();
  if( stat& 0x01 )
  {
    return true;
  }
  return false;
}


void waitingForNotBusy()
{
  while(checkWIP())
  {
      WaitForWriteEnd();
  }
}
/******************************************************************************
* @function     checkWEL()
*
* @return       0 write is busy  !=0 write is not in progress
*               1 write enable
*               2 write disable
******************************************************************************/
uint8 checkWEL()
{
  uint8 stat = getSR();
  if( stat & 0x01 )
  {
    return 0;
  }
  if( stat & 0x02 )
  {
    return 1;
  }
  else
  {
    return 2;
  }
}
/**************************************************************************//**
* @fn       spiWriteByte(uint8 write)
*
* @brief    Write one byte to SPI interface
*
* @param    write   Value to write
******************************************************************************/
void spiWriteByte(uint8 write)
{
        U1CSR &= ~0x02;                 // Clear TX_BYTE
        U1DBUF = write;
        while (!(U1CSR & 0x02));        // Wait for TX_BYTE to be set
}
/**************************************************************************//**
* @fn       spiReadByte(uint8 *read, uint8 write)
*
* @brief    Read one byte from SPI interface
*
* @param    read    Read out value
* @param    write   Value to write
******************************************************************************/
void spiReadByte(uint8 *read, uint8 write)
{
        U1CSR &= ~0x02;                 // Clear TX_BYTE
        U1DBUF = write;                 // Write address
        while (!(U1CSR & 0x02));        // Wait for TX_BYTE to be set
        *read = U1DBUF;                 // Save returned value
}
