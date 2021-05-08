/**************************************************************************************************
  Filename:       FlashStruct.h
  Revised:        $Date: 2014-06-10 14:00:17 +0800 (Wen, 30 June 2014) $
  Revision:       $Revision: 00001 $

  Description:    The Flash Struct

**************************************************************************************************/

#ifndef __FLASH_STRUCT_H__
#define __FLASH_STRUCT_H__

#include "hal_types.h"
#include "comdef.h"
#include "osal_snv.h"
#include "OSAL_Clock.h"
#include "gatt.h"
#include "peripheral.h"

#include "hal_board.h"
#include "hal_drivers.h"
#include "hal_timer.h"
#include "hal_lis3dh.h"
#include "hal_tmp.h"
#include "hal_sht.h"
#include "hal_njl.h"

#include "hal_mxflash.h"


//INFO Last Subsection 
#define INFO_ENTRY_START_ADDR           0x1FF000
#define INFO_START_ADDR                 0x1FF2AA
#define INFO_SECTION_GAP                0x2AA


//INFO  Sections Define                   Commpany-Section        
#define DEVICE_INFO_CODE                0x4D01
#define USER_INFO_CODE                  0x4D02
#define ROM_INFO_CODE                   0x4D04
#define SECURITY_INFO_CODE              0x4D08
#define SENSOR_INFO_CODE                0x4D10

//INFO TYPE     common          device  rom 
#define INFO_NAME                       0x5401
#define INFO_CODE                       0x5402
#define INFO_VER                        0x5404
#define INFO_MANUFACTURE                0x5408
#define INFO_MODEL                      0x5410
#define INFO_UNIQUEID                   0x5420
#define INFO_PRODUCTION_DATE			0x5440
#define INFO_UPDATE_DATE				0x5480
#define INFO_NICKNAME					0x54AA
  
  //user
#define USER_NAME                       0x5801
#define USER_EMAIL                      0x5802
#define USER_PWD                        0x5804
#define USER_COMMET                     0x5808
  //Security use the common code
#define PROTOCOL_CODE                   0x5210

//Length   bytes
#define SECTION_LEN_SHORT               0x08
#define SECTION_LEN_LONG                0x0F

//Sensor TYPE Code
#define ACCEL_CODE                      0xAA00
#define FLASH_CODE                      0xBB11
#define HUMI_CODE                       0xCC22
#define TEMP_CODE                       0xDD33
#define IR_CODE                         0xEE44
#define HEART_CODE                      0xFF55
#define BATT_CODE                       0x8888

//Field
#define ACCEL_XYZ                       0x01
#define ACCEL_6D                        0x02
#define HUMI_DATA                       0x04
#define IRTEMP_DATA                     0x08
#define HEART_DATA                      0x10
#define BATT_DATA                       0x20

//Standard Manufacture IDs              JEDEC Standard
#define MAXTAIN                         0x4D
#define TI                              0x97
#define STM                             0x20
#define MICRON                          0x2C
#define SHT                             0xFF                    //self define
#define INVENSENSE                      0xFC                    //self define
#define NXP                             0x15

#define INFO_ESTABLISHED                0xAA
#define INFO_UNKNOWN                    0x00

#define BABY_TALK                       0xAA
/*************************************************
*       Info
*       0x1FF000  +--------------+
*                 | InfoEntry    |
*       0x1FF2AA  +--------------+
*                 | Device Info  |
*       0x1FF554  +--------------+
*                 | User Info    |
*       0x1F7FE   +--------------+
*                 | Rom Info     |
*       0x1FAA8   +--------------+
*                 | SecurityInfo |
*       0x1FD52   +--------------+
*                 | Sensor Info  |
*       0x1FFFC   +--------------+
*
*       Info Entry
*        0        1        2        3        4        5        6        7      
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       |   section ID    |   section Type  |def Len |data len|   sectionAddr   |
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*
*       Device Info
*        0        1        2        3        4        5        6        7      
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       | Device Manufacture                                                    
*       | Device Model
*       | Device Name
*       | Device Nickname
*       | Device Version
*       | Device Unique ID
*       | Device Production Date
*       | Device Update Date
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*
*       User Info
*        0        1        2        3        4        5        6        7      
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       | User Name                                                   
*       | User Email
*       | User Gender
*       | User Age
*       | User Weight
*       | User Height
*       | User Passwd
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*
*       Rom Info
*        0        1        2        3        4        5        6        7      
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       | Rom Code Name                                                    
*       | Rom Version
*       | Rom Release Date
*       | Rom Update Date
*       | Rom Update
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*
*       Security Info
*        0        1        2        3        4        5        6        7      
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       | Protocol Name                                                    
*       | Protocol Version
*       | Protocol Release Date
*       | Protocol Update Date
*       | User Name
*       | Password
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*
*       Sensor Info
*        0        1        2        3        4        5        6        7      
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       | sType  | sManu  |     sensor Name |   sensor Model  |  sensor Version |
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*
*************************************************/
typedef struct
{
  uint8 sectionID[2];
  uint8 sectionType[2];
  uint8 sectionDefaultLen;              //8 bytes or F bytes
  uint8 sectionDataLen;                 //Actual Len
  uint8 sectionAddr[2];                 
}InfoEntry;

typedef struct                  //8 bytes
{
  uint8 sensorType[2];
  uint8 sensorManufacture;
  uint8 sensorModel[3];
  uint8 sensorVersion[2];
}SensorInfo;

//INIT address
#define ENTRY_HEAD_ADDR                       0x00000000
#define ENTRY_ITEM_ADDR                       0x00000008

#define ENTRY_INFO_FLAG_ADDR                  0x0000000A

#define FRAME_ENTRY_START_ADDR                0x00001000
#define PACKAGE_START_ADDR                    0x00010000
#define PACKAGE_END_ADDR                      0x1FE000

#define DUMMY_ADDR                            0xFF

#define SYNC_NONEVENT                         0xAA
#define SYNC_FINISHED                         0x01
#define SYNC_INTERRUPT                        0x02

#define DELETED                               SYNC_FINISHED
#define KEEPIT                                SYNC_NONEVENT  

//Length define
#define COMMON_FLAG             0xFFF0
#define ENTRY_HEAD_FLAG         0xFFF1
#define ENTRY_ITEM_FLAG         0xFFF2
#define ENTRY_ALL_FLAG          0xFFF3
#define FRAME_HEAD_FLAG         0xFFF4
#define FRMAE_ITEM_FLAG         0xFFF5
#define FRMAE_ALL_FLAG          0xFFF6
#define PACKAGE_HEAD_FALG       0xFFF7
#define DATAGRAM_FLAG           0xFFF8

#define NORMAL_LEN      8
#define DOUBLE_LEN      16
#define ENTRY_HEAD_LEN  DOUBLE_LEN
#define ENTRY_ITEM_LEN  NORMAL_LEN
#define FRAME_HEAD_LEN  NORMAL_LEN
#define FRAME_ITEM_LEN  NORMAL_LEN
#define PACKAGE_HEAD_LEN DOUBLE_LEN

//gramType
#define GDATASHORT          0xA1
#define GDATALONG           0xA8
#define GDATASTREAM         0xAA

//Structure of Entry 
/********************
*       EntryHead
*        0        1        2        3        4        5        6        7      
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       |syncFlag|      height     |         crashAddr        |  year  |last[0] |
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       |PackageAddr      |infoDone|    infoEntryAddr         | infoStartAddr   |
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       EntryItem
*       0         1        2        3        4        5        6        7
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       |syncFlag|  month |  day   |      height     |         frameAddr        |
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*
*
*       FrameHead
*       0         1        2        3        4        5        6        7
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       |syncFlag|      height     |  month |  day   |      lastFrameItemAddr   |
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       FrameItem
*       0         1        2        3        4        5        6        7
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*       |syncFlag|      height     |  hour  | minute |       packageAddr        |
*       +--------+--------+--------+--------+--------+--------+--------+--------+
*
*
*       PackageHead
*       0         1        2        3        4        5        6        7        8        9        10       11       12       13       14       15       
*       +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
*       |syncFlag|    packageNum   |   deviceFlag    |      height     |   hour | minute | second |      prevPackageAddr     |  nextPackageAddr(ext)    |
*       +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
*       DataGram
*       +--------+--------+--------+--------+--------+...
*       |DataShort  or  DataLong  or DataStream
*       +--------+--------+--------+--------+--------+...
*       DataShort
*       0         1        2        3        4        5        6        7        8        9        10       11       12       13       14       15       
*       +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
*       |gramType|    sensorType   | sField | length |  hour  | minute | second |                     data[8]                                              |
*       +--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
*       DataLong
*       0         1          2        3        4        5        6        7        8        9        10       11       12       13       14       15       
*       +--------+----------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
*       |gramType|sensorType| sField | length |                                 data[12]                                                                      |
*       +--------+----------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
*       DataStream
*       0         1          2        3        4        5        6        7        8        9        10       11       12       13       14       15       
*       +--------+----------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
*       |gramType|sensorType| sField | height | flag   |                                    data[11]                                                      |
*       +--------+----------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
*       |                                                       data[1]                                                                                   |
*       |                                                         ...                                                                                     |
*       |                                                       data[height]                                                                              |
*       +--------+----------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
*
*******************/
typedef struct
{
    uint8       syncFlag;                       //total Flag
    uint16      height;                         //how many line
    uint8       crashAddr[3];
    uint8       year;
    uint8       lastFrameAddr[3];
    uint8       infoDone;
    uint8       infoEntryAddr[3];
    uint8       infoStartAddr[2];
}EntryHead;

typedef struct
{
    uint8       syncFlag;
    uint8       month;
    uint8       day;
    uint16      height;
    uint8       frameAddr[3];
}EntryItem;                                     //Frame Addr

typedef struct eitems
{
  EntryItem ei;
  struct eitems *next_eis;
}EntryItems;

typedef struct
{
  EntryHead eh;
  EntryItems *eis;
}EntryCache;

typedef struct
{
    uint8       syncFlag;
    uint16      height;                                 //how many line items
    uint8       month;
    uint8       day;
    uint8       lastPackageItemAddr[3];                 //use for fast read
}FrameHead;

typedef struct
{ 
    uint8       syncFlag;
    uint16      height;
    uint8       hour;
    uint8       minute;
    uint8       packageAddr[3];
}FrameEntryItem;

typedef struct
{
    uint8       syncFlag;                               //repeat for fast read
    uint16      packageNum;
    uint8       deviceFlag[2];
    uint16      height;                                 //how many lines
    uint8       hour;
    uint8       minute;
    uint8       second;
    uint8       prevPackageAddr[3];
    union
    {
      uint8       frameAddr[3];
      uint8       nextPackageAddr[3];
      uint8       ext[3];
    }_arg;
}PackageHead;

typedef struct
{
    uint8       gramType;
    uint8       sensorType[2];                          //with company
    uint8       sensorField;
    uint8       length;
    uint8       hour;
    uint8       minute;
    uint8       second;
    uint8       data[8];
}DataShort;

typedef struct
{
    uint8       gramType;
    uint8       sensorType;                             //One Byte
    uint8       sensorField;
    uint8       length;
    uint8       data[12];
}DataLong;

typedef struct
{
   uint8        gramType;
   uint8        sensorType;
   uint8        sensorField;
   uint8        height;
   uint8        flag;
   uint8        *data;
}DataStream;

typedef struct
{
  void* data;
}Datagram;

typedef struct datag
{
  Datagram dg;
  struct datag *next_datagram;
}Datagrams;

typedef struct
{
  PackageHead ph;
  Datagrams *dgs;
}Package;
//info
bool createInfoEntry();
bool insertInfoItem(uint16 sid,uint16 stype,uint8 deflen,uint8 len,uint16 saddr);
bool lockInfoSection();
//create
bool createDeviceInfo(uint32 addr);
bool createUserInfo(uint32 addr);
bool createRomInfo(uint32 addr);
bool createSecurityInfo(uint32 addr);
bool createSensorInfo(uint32 addr);
bool updateByIDandType(uint16 sid,uint16 stype,uint8 len,uint8 *data);


//parse
bool parseBySectionId(uint16 sid,uint8 *pBuf,uint8 *lens,uint8 type);
bool updateByIDandType(uint16 sid,uint16 stype,uint8 len,uint8 *data);


//update info done
bool updateEntryHeadInfoDone();
bool updateInfoEntryLen(uint32 addr,uint8 len);

//Has?
bool hasEntryHead();
bool hasInfo();


//common data 
bool initFlash();


EntryCache getEntry();

//Add
bool createEntryHead();
EntryItem createEntryItem(UTCTimeStruct utctime,uint16 height,uint8 *addr);
bool insertEntryItem();

FrameHead createFrameHead(uint16 height,uint8 *addr,UTCTimeStruct utctime);
bool insertFrameItem(uint32 _addr,uint8 hour,uint8 minute,uint16 height,uint8 *packageAddr);

PackageHead createPackageHead(uint8 *deviceFlag,uint16 height,UTCTimeStruct utctime);
Datagram insertDatagram(uint16 sensorType, uint8 sensorField,uint8 *data);


Package assemblePackage(Datagram *dg,uint16 height,uint8 *deviceFlag,UTCTimeStruct utctime);

void write_back(EntryCache *ec,Package pack,UTCTimeStruct utctime);
void write_back_package(Package *pack,uint32 _addr);
void write_back_entry_cache(EntryCache ec,bool force_all);
void write_back_frame(uint32 head_addr,FrameHead *fh);

//Update
bool updateEntryHead();
bool updateEntryItem();
bool updateFrameHead();
bool updateFrameItem();
bool updatePackageHead();
bool updateEntryCache(EntryCache *ec,EntryItem ei);

//write back
bool generateCacheByPackage(EntryCache *ec,FrameHead *fh,Package pack,uint32 pack_addr, uint32 frame_head_addr,UTCTimeStruct utctime);
uint32 updatePackage(EntryCache *ec,Package *pack,UTCTimeStruct utctime,FrameHead *fh0,uint32 *frame_head_addr_p);
//Select
bool syncData();
bool syncInterruptted();
bool syncAllData();
bool syncFromInterrupt();

bool readByDay();
bool readByTime();
bool readTraverse();

//Parse

bool parseEntryInfo();
bool parseEntry();

bool parseDatagram();
bool parsePackageHead();
bool parseFrameHead();

//get unit
EntryHead getEntryHead();
bool getEntryList();
FrameHead getFrameHead(uint32 _addr);
bool getFrameItem();

bool getPackageHeads();
PackageHead getPackageHead(uint32 _addr);
bool getPackageItem();



//clear
bool clearFlashSection();


//flash packaging
uint16 readf(uint32 addr,uint16 lenType,uint8 *pBuf);
void writef(void *val,uint16 len,uint32 addr);
void readt(uint32 addr,uint8 lenType,uint8 *pBuf,void *stval);

//misc
bool isEqual(uint8 val1,uint8 val2);
bool hasBit(uint8 val,uint8 bit);

#endif