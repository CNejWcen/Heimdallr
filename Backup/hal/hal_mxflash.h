/**************************************************************************************************
  Filename:       hal_mxflash.h
  Revised:        $Date: 2014-04-17 13:06:18 -0700 (Thu, 04 Apr 2014) $
  Revision:       $Revision: 10001 $

  Description:    This file contains Read and Write of MX25U1635E.

**************************************************************************************************/

/**************************************************************************************************
 *                                           Includes
 **************************************************************************************************/
#ifndef _HAL_MXFLASH_H_
#define _HAL_MXFLASH_H_
   
#include "hal_types.h"
#include "hal_timer.h"
#include "comdef.h"
/******************************************************************************
 * MACROS
 ******************************************************************************/

#define CS              P1_4

#define CS_DISABLED     1
#define CS_ENABLED      0

#define MX_DUMMY_BYTE   0xFF
/******************************************************************************
 * MXFlash Reg & Command
 ******************************************************************************/      


/*      Read Commands(Only SPI Mode)    */    
#define MX_NORMAL_READ  0x03            //4bytes 03 AD1 AD2 AD3                 33MHz

//#define MX_FAST_READ    0x0B            //5bytes 0B AD1 AD2 AD3 Dummy(8)        104MHz                                  a good choice
//#define MX_RDSFDP       0x5A            //Read SFDP mode (5bytes)              104Mhz
//#define MX_2READ        0xBB            //n bytres read out by 2xI/O until CS# goes high(5bytes)        84MHz
//#define MX_W4READ       0xE7            //Quad I/O read with 4 dummy cycle in 84MHz                     84MHz
//#define MX_4READ        0xEB            //Quad I/O read with 6 dummy cycle in 104MHz                    104Mhz
//#define MX_4PP          0x38            //quad page program  4bytes      
//#define MX_BE32K        0x52            //to erase selected 32k block,4byte
//#define MX_BE64K        0xD8            //to erase selected 64k block,4byte
//#define MX_CE           0x60            //to erase whole chip
   /*
//Before executing RST command, RSTEN command must be executed. If there is any other command to interfere, the reset operation will be disabled
#define MX_RSTEN        0x66            //Reset Enable                  
#define MX_RST          0x99            //Reset Memory
#define MX_EQIO         0x35            //Enter QPI Mode
#define MX_RSTQIO       0xF5            //Exit the QPI Mode -----  to enter 1-SO/SI mode(SPI mode)
#define MX_QPIID        0xAF            //ID in QPI interface
#define MX_SBL          0xC0            //2bytes     to set burst length
#define MX_WPSEL        0x68            //to enter and enable individal block protect mod
*/
/*      Program/Erase Commands          */  
#define MX_WREN         0x06            //write enable
#define MX_WRDI         0x04            //write disable   
#define MX_RDSR         0x05            //read status register
#define MX_WRSR         0x01            //write status register
   
#define MX_SUBSE        0x20
#define MX_SE           0xD8            //sector erase  4bytes

#define MX_CE           0xC7            //to erase whole chip alt. choice
#define MX_PP           0x02            //to program the selected page
/*      POWER MANAGEMENT                */
#define MX_DP           0xB9            //to enter deep power down mode                
#define MX_RDP          0xAB            //to release from deep power down mode
/*      PROGRAM and ERASER Operation    */
//#define MX_PE_SUSPEND   0xB0            //to suspend program/erase
//#define MX_PE_RESUME    0x30            //to resume program/erase     
//#define MX_RES          0xAB            // to read out 1-byte Device ID
//#define MX_REMS         0x9F            // to read electronic manufacture ID            
      
/*      Security/ID/Mode Setting/Reset Commands               */
#define MX_RDID         0x9F            // read identification      Manufacture ID
/*              Chip Management         */
#define MX_NOP          0x00            //NOP

#define MX_MANUFACTURE_ID 0x20
#define MX_MEM_TYPE       0x71
#define MX_MEM_CAP        0x15
      
  // Write Enable execute
#define MX_WREN_FUN()   \
  CS = CS_ENABLED;      \
  spiWriteByte(MX_WREN);\
  CS = CS_DISABLED      

    //goto Deep Power-down
#define MX_SHUTDOWN()    \
  CS = CS_ENABLED;      \
  spiWriteByte(MX_DP);  \
  CS = CS_DISABLED            
    
    // release for Deep Power-down
#define MX_START()    \
  CS = CS_ENABLED;      \
  spiWriteByte(MX_RDP);  \
  CS = CS_DISABLED    
 
#define WaitForWriteEnd()       \
      WAIT_1MS();

//SPI Configuration & SPI Read/Write
void spiWriteByte(uint8 write);
void spiReadByte(uint8 *read, uint8 write);
void spiInit();

//
bool MXFlash_Init();
void readMxFlash(uint8 *addr,uint8 *pBuf);
void writeMxFlash(uint8 *addr,uint8 *pBuf);
void writeMxFlash_Ex(uint8 *addr,uint8 len,uint8 *pBuf);
void readMxFlash_Ex(uint8 *addr,uint16 len,uint8 *pBuf);

void subsectorErase(uint8 *addr);
void sectorErase(uint8* addr);
void sectorErase_Ex(uint8 type,uint8* addr);
void chipErase();

bool MXFlash_TEST();

/************************Flash Structure******************************/
 
/*        OTP&Secure !important    */
/*
#define MX_ENSO         0xB1            // to enter the 4K-bit secured OTP MODE
#define MX_EXSO         0xC1            // to exit the  4K-bit secured OTP MODE
#define MX_RDSCUR       0x2B            //to read value of security register
#define MX_WRSCUR       0x2F            //to set the lock down bit as 1(once lock-down can't be updated)        1byte?? how to unlock?
#define MX_SBLK         0x36            //single block lock     individual block 64k byte or sector 4kbyte      4bytes
#define MX_SBULK        0x39            //unlock
#define MX_RDBLOCK      0x3C            //read individual block or sector wirte protect status
#define MX_GBLK         0x7E            //whole chip write protect
#define MX_GBULK        0x98            //whole chip unprotect
*/
#endif