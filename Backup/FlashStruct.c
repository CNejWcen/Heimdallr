/**************************************************************************************************
  Filename:       FlashStruct.h
  Revised:        $Date: 2014-06-10 14:00:17 +0800 (Wen, 30 June 2014) $
  Revision:       $Revision: 00001 $

  Description:    The Flash Struct

**************************************************************************************************/


#include "FlashStruct.h"



#define         MSB_ADDR(val)        ((val & 0x00ff0000 ) >> 16)
#define         MLSB_ADDR(val)       ((val & 0x0000ff00 ) >> 8 )
#define         LSB_ADDR(val)        ( val & 0x000000ff )
#define         FUSION32(msb,mlsb,lsb)    ((msb & 0x000000ff << 16) + (mlsb & 0x000000ff << 8) + lsb)
#define         FUSION32_ARRAY(arr)       FUSION32(arr[0],arr[1],arr[2])
#define         FUSION16(msb,lsb)         ((msb & 0x00ff << 8) + lsb)
//Has?
bool hasEntryHead()
{
  uint8 buf=0x00;
  readf(ENTRY_HEAD_ADDR,0x01,&buf);
  bool has = isEqual(buf,SYNC_NONEVENT) || isEqual(buf,SYNC_FINISHED) || isEqual(buf,SYNC_INTERRUPT);
  if(has)
  {
    return true;
  }
  return false;
}
bool hasInfo()
{
  uint8 buf=0x00;
  readf(ENTRY_INFO_FLAG_ADDR,0x01,&buf);
  bool has = isEqual(buf,INFO_ESTABLISHED);
  if ( has )
  {
    return true;
  }
  return false;
}

//Create
bool insertInfoItem(uint16 sid,uint16 stype,uint8 deflen,uint8 len,uint16 saddr)
{
  static uint32 entryAddr  = 0x1FF000;
  InfoEntry ie;
  ie.sectionID[0] = MLSB_ADDR(sid);
  ie.sectionID[1] = LSB_ADDR(sid);                          //shouldn't
  ie.sectionType[0] = MLSB_ADDR(stype);
  ie.sectionType[1] = LSB_ADDR(stype); 
  ie.sectionDefaultLen = deflen;
  ie.sectionDataLen = len;
  ie.sectionAddr[0] = MLSB_ADDR(saddr); 
  ie.sectionAddr[1] = LSB_ADDR(saddr); 
  writef(&ie,NORMAL_LEN,entryAddr);
  entryAddr = entryAddr + 8;
  return true;
}
bool createDeviceInfo(uint32 addr)
{
  uint8 *manufacture = "Maxtain";
  uint8 *model = "Baby Tag";
  uint8 *name = "Heimdallr";
  uint8 *verison = "0.0001";
  uint8 *uniqueId = "";
  uint8 *productiondate = "2014-06-13";
  uint8 *updateDate  = "2014-06-13";
  uint8 *nickname = "Baby";
  //manu
  writef(manufacture,7,addr);
  insertInfoItem(DEVICE_INFO_CODE,INFO_MANUFACTURE,16,7,addr);
  addr = addr + 16;
  //model
  writef(model,8,addr);
  insertInfoItem(DEVICE_INFO_CODE,INFO_MODEL,16,8,addr);
  addr = addr + 16;
  //name
  writef(name,9,addr);
  insertInfoItem(DEVICE_INFO_CODE,INFO_NAME,16,9,addr);
  addr = addr + 16;
  //verison
  writef(verison,6,addr);
  insertInfoItem(DEVICE_INFO_CODE,INFO_VER,16, 6,addr);
  addr = addr + 16;
  //uniqueId
  writef(uniqueId,16,addr);
  insertInfoItem(DEVICE_INFO_CODE,INFO_UNIQUEID,32, 16,addr);
  addr = addr + 32;
  //productiondate
  writef(productiondate,10,addr);
  insertInfoItem(DEVICE_INFO_CODE,INFO_PRODUCTION_DATE,32, 10,addr);
  addr = addr + 32;
  //updateDate
  writef(updateDate,10,addr);
  insertInfoItem(DEVICE_INFO_CODE,INFO_UPDATE_DATE,32, 10,addr);
  addr = addr + 32;
  //nickname
  writef(nickname,4,addr);
  insertInfoItem(DEVICE_INFO_CODE,INFO_NICKNAME,0xFF, 4,addr);
  return true;
}
bool createUserInfo(uint32 addr)
{
  uint8 *name = "Name";
  uint8 *email = "email@gmail.com";
  uint8 *gender_age_weight_height = "00022040";
  // uint8 *weight = "20";
  // uint8 *height = "40";
  uint8	*passwd = "maxtainpassword";
  //name
  writef(name,4,addr);
  insertInfoItem(USER_INFO_CODE,USER_NAME,32,4,addr);
  addr = addr + 32;

  //email
  writef(email,14,addr);
  insertInfoItem(USER_INFO_CODE,USER_EMAIL,48,14,addr);
  addr = addr + 48;

  //gender
  writef(gender_age_weight_height,8,addr);
  insertInfoItem(USER_INFO_CODE,USER_COMMET,16,8,addr);
  addr = addr + 16;

  //password
  writef(passwd,15,addr);
  insertInfoItem(USER_INFO_CODE,USER_PWD,0xFF,15,addr);
  return true;

}
bool createRomInfo(uint32 addr)
{
  uint8 *code_name = "Heimdallr";
  uint8 *version = "00.00.00.01";
  uint8 *release_date = "2014-06-14";
  uint8 *update_date = "2014-06-15";
  uint8 *update ="...";
  //code_name
  writef(code_name,9,addr);
  insertInfoItem(ROM_INFO_CODE,INFO_CODE,16,9,addr);
  addr = addr + 16;
  //version
  writef(version,11,addr);
  insertInfoItem(ROM_INFO_CODE,INFO_VER,16,11,addr);
  addr = addr + 16;
  //release date
  writef(release_date,10,addr);
  insertInfoItem(ROM_INFO_CODE,INFO_PRODUCTION_DATE,16,10,addr);
  addr = addr + 16;
  //update date
  writef(update_date,10,addr);
  insertInfoItem(ROM_INFO_CODE,INFO_UPDATE_DATE,16,10,addr);
  addr = addr + 16;
  //update
  writef(update,3,addr);
  insertInfoItem(ROM_INFO_CODE,INFO_CODE,0xFF,3,addr);
  return true;

}
bool createSecurityInfo(uint32 addr)
{
  uint8 *protocol_name = "Homeland";
  uint8 *protocol_ver = "00.00.00.01";
  uint8 *release_date = "2014-06-14";
  uint8 *update_date = "2014-06-14";
  uint8 *user_name = "maxtain";
  uint8 *user_pwd = "maxtainpassword";
  //protocol name
  writef(protocol_name,8,addr);
  insertInfoItem(SECURITY_INFO_CODE,INFO_NAME,16,8,addr);
  addr = addr + 16;
  //protocol ver
  writef(protocol_ver,11,addr);
  insertInfoItem(SECURITY_INFO_CODE,INFO_VER,16,11,addr);
  addr = addr + 16;
  //release date
  writef(release_date,10,addr);
  insertInfoItem(SECURITY_INFO_CODE,INFO_PRODUCTION_DATE,16,10,addr);
  addr = addr + 16;
  
  //update date
  writef(update_date,10,addr);
  insertInfoItem(SECURITY_INFO_CODE,INFO_UPDATE_DATE,16,10,addr);
  addr = addr + 16;
  
  //user name
  writef(user_name,7,addr);
  insertInfoItem(SECURITY_INFO_CODE,USER_NAME,32,7,addr);
  addr = addr + 32;
  
  //user passwd
  writef(user_pwd,15,addr);
  insertInfoItem(SECURITY_INFO_CODE,USER_PWD,0xFF,15,addr);
  return true;
}
bool createSensorInfo(uint32 addr)
{
  //ACCEL     LIS   
  SensorInfo tmp1 = {{MLSB_ADDR(ACCEL_CODE),LSB_ADDR(ACCEL_CODE)},STM,{0x4c,0x49,0x53},{0x33,0x44}};
  writef(&tmp1,8,addr);
  insertInfoItem(SENSOR_INFO_CODE,ACCEL_CODE,8,8,addr);
  addr += 8;
  //HUMI      SHT 53 48 54
  SensorInfo tmp2 = {{MLSB_ADDR(HUMI_CODE),LSB_ADDR(HUMI_CODE)},SHT,{0x53,0x48,0x54},{0x32,0x30}};
  writef(&tmp2,8,addr);
  insertInfoItem(SENSOR_INFO_CODE,HUMI_CODE,8,8,addr);
  addr += 8;
  //IR        TMP 54 4d 50
  SensorInfo tmp3 = {{MLSB_ADDR(IR_CODE),LSB_ADDR(IR_CODE)},TI,{0x54,0x4d,0x50},{0x30,0x36}};
  writef(&tmp3,8,addr);
  insertInfoItem(SENSOR_INFO_CODE,IR_CODE,8,8,addr);
  addr += 8;
  //HEART     HRS 48 52 53
  SensorInfo tmp4 = {{MLSB_ADDR(HEART_CODE),LSB_ADDR(HEART_CODE)},MAXTAIN,{0x48,0x52,0x53},{0x30,0x31}};
  writef(&tmp4,8,addr);
  insertInfoItem(SENSOR_INFO_CODE,HEART_CODE,8,8,addr);
  addr += 8;
  //FLASH     M25PX  4d 32 35 50 58
  SensorInfo tmp5 = {{MLSB_ADDR(FLASH_CODE),LSB_ADDR(FLASH_CODE)},MICRON,{0x4d,0x32,0x35},{0x50,0x58}};
  writef(&tmp5,8,addr);
  insertInfoItem(SENSOR_INFO_CODE,FLASH_CODE,8,8,addr);
  return true;
}
//parse
bool parseBySectionId(uint16 sid,uint8 *pBuf,uint8 *lens, uint8 type)
{
  
  uint32 _addr = INFO_ENTRY_START_ADDR;
  //loop
  while(type >= 1)
  {
    uint8 buf[8] = {0};
    readf(_addr,NORMAL_LEN,buf);
    _addr += 8;
    uint16 tmp_sid = FUSION16(buf[0],buf[1]);
    if(tmp_sid == sid)
    {
       uint8 __len                =       buf[5];
       uint32 __addr               =      FUSION32(0x1F,buf[6],buf[7]);
       pBuf = (uint8 *)osal_mem_alloc(__len);
       readf(__addr,__len,pBuf);

       *lens = __len;
       lens ++;
       pBuf += __len;
       type --;
    }
  }
  
  return true;
}
bool updateByIDandType(uint16 sid,uint16 stype,uint8 len,uint8 *data)
{
  uint32 _addr = INFO_ENTRY_START_ADDR;
  //loop
  while(_addr < 0x1FF2AA){
    uint8 buf[8] = {0};
    readf(_addr,NORMAL_LEN,buf);
    _addr += 8;
    uint16 tmp_sid = FUSION16(buf[0],buf[1]);
    uint16 tmp_type = FUSION16(buf[2],buf[3]);
    if((tmp_sid == sid) && (tmp_type == stype))
    {
       uint32 __addr               =       FUSION32(0x1F,buf[6],buf[7]);
       writef(data,len,__addr);
       updateInfoEntryLen(_addr,len);
       return true;
    }
  }
  return false;
}
bool updateInfoEntryLen(uint32 addr,uint8 len)
{
  addr += 0x05;
  writef(&len,0x01,addr);
  return false;
}
//update info done
bool updateEntryHeadInfoDone()
{
  uint32 addr = ENTRY_INFO_FLAG_ADDR;
  uint8 stat = INFO_ESTABLISHED;
  writef(&stat,0x01,addr);
  return true;
}
bool createInfoEntry()
{
  uint32 addr = INFO_START_ADDR;
  createDeviceInfo(addr);
  addr += INFO_SECTION_GAP;
  createUserInfo(addr);
  addr += INFO_SECTION_GAP;
  createRomInfo(addr);
  addr += INFO_SECTION_GAP;
  createSecurityInfo(addr);
  addr += INFO_SECTION_GAP;
  createSensorInfo(addr);

  updateEntryHeadInfoDone();
  return true;
}

           
//Common Data
bool initFlash()
{
  return true;
}
//Entry 
bool createEntryHead()
{
   EntryHead eh;
   eh.syncFlag = SYNC_NONEVENT;
   eh.height = 0;
   eh.crashAddr[0] = DUMMY_ADDR;
   eh.crashAddr[1] = DUMMY_ADDR;
   eh.crashAddr[2] = DUMMY_ADDR;
   eh.year = 14;
   eh.lastFrameAddr[0] = DUMMY_ADDR;
   eh.lastFrameAddr[1] = DUMMY_ADDR;
   eh.lastFrameAddr[2] = DUMMY_ADDR;
   eh.infoDone = INFO_UNKNOWN;
   eh.infoEntryAddr[0] = MSB_ADDR(INFO_ENTRY_START_ADDR);
   eh.infoEntryAddr[1] = MLSB_ADDR(INFO_ENTRY_START_ADDR);
   eh.infoEntryAddr[2] = LSB_ADDR(INFO_ENTRY_START_ADDR);
   eh.infoStartAddr[0] = MLSB_ADDR(INFO_START_ADDR);
   eh.infoStartAddr[1] = LSB_ADDR(INFO_START_ADDR);
   writef(&eh,ENTRY_HEAD_LEN,ENTRY_HEAD_ADDR);
   return true;
}
EntryItem createEntryItem(UTCTimeStruct utctime,uint16 height,uint8 *addr)
{
  EntryItem ei;
  ei.syncFlag   =       SYNC_NONEVENT;
  ei.month      =       utctime.month;
  ei.day        =       utctime.day;
  ei.height     =       height;
  ei.frameAddr[0]=      addr[0];
  ei.frameAddr[1]=      addr[1];
  ei.frameAddr[2]=      addr[2];
  return ei;
}

FrameHead createFrameHead(uint16 height,uint8 *addr,UTCTimeStruct utctime)
{
    FrameHead fh;
    fh.syncFlag         =       SYNC_NONEVENT;
    fh.month            =       utctime.month;
    fh.day              =       utctime.day;
    fh.height           =       height;
    fh.lastPackageItemAddr[0]=       addr[0];
    fh.lastPackageItemAddr[1]=       addr[1];
    fh.lastPackageItemAddr[2]=       addr[2];
    return fh;
}

bool insertFrameItem(uint32 _addr,uint8 hour,uint8 minute,uint16 height,uint8 *packageAddr)
{
  
  FrameEntryItem fi;
  fi.syncFlag           =               SYNC_NONEVENT;
  fi.hour               =               hour;
  fi.minute             =               minute;
  fi.height             =               height;
  fi.packageAddr[0]     =               packageAddr[0];
  fi.packageAddr[1]     =               packageAddr[1];
  fi.packageAddr[2]     =               packageAddr[2];
  
  writef(&fi,NORMAL_LEN,_addr);
  
  return true;
}

PackageHead createPackageHead(uint8 *deviceFlag,uint16 height,UTCTimeStruct utctime)
{//uint8 No,,uint8 *frameAddr
  PackageHead ph;
  ph.syncFlag           =               SYNC_NONEVENT;
  ph.deviceFlag[0]      =               deviceFlag[0];
  ph.deviceFlag[1]      =               deviceFlag[1];
  ph.height             =               height;

  ph.hour               =               utctime.hour;
  ph.minute             =               utctime.minutes;
  ph.second             =               utctime.seconds;
  /*
  ph.prevPackageAddr[0] =               prevPackageAddr[0];
  ph.prevPackageAddr[0] =               prevPackageAddr[0];
  ph.prevPackageAddr[0] =               prevPackageAddr[0];
  */
  /*                            BUG!!!!!!!!!!!!!!!!!!!!   need update to be frame addr
  uint32 _next_addr     =               _addr + height * DOUBLE_LEN;
  ph._arg.nextPackageAddr[0]    =       MSB_ADDR(_next_addr);
  ph._arg.nextPackageAddr[1]    =       MLSB_ADDR(_next_addr);
  ph._arg.nextPackageAddr[2]    =       LSB_ADDR(_next_addr);
  */
  
  //detect whether is the first!
  {
    //if first insert Frame
  }
  {
    //else update Frame
  }
  return ph;
}

Datagram insertDatagram(uint16 sensorType, uint8 sensorField,uint8 *data)
{
  uint8 len = osal_strlen(data);
  Datagram dg;
  if( len >0  && len <= 8)
  {
    UTCTime tt = osal_getClock();
    UTCTimeStruct utctime;
    osal_ConvertUTCTime(&utctime,tt);
    DataShort ds;
    ds.gramType = GDATASHORT;
    ds.sensorType[0] = MLSB_ADDR(sensorType);
    ds.sensorType[1] = LSB_ADDR(sensorType);
    ds.sensorField   = sensorField;
    ds.length        = len;
    ds.hour          = utctime.hour;
    ds.minute        = utctime.minutes;
    ds.second        = utctime.seconds;
    
    VOID osal_memcpy(ds.data,data,len);
    
    dg.data = &ds;
    
    /*
    {GDATASHORT,MLSB_ADDR(sensorType),LSB_ADDR(sensorType), len;,utctime.hour,utctime.minutes,utctime.seconds,data};
    */
  }else if(len > 8)
  {
    DataLong dl;
    dl.gramType = GDATALONG;
    dl.sensorType = LSB_ADDR(sensorType);
    dl.sensorField = sensorField;
    dl.length = len;
    VOID osal_memcpy(dl.data,data,len);
    
    dg.data = &dl;
  }
  return dg;
}

Package assemblePackage(Datagram *dg,uint16 height,uint8 *deviceFlag,UTCTimeStruct utctime)
{
  uint16 i=0;
  
  Package pack ;
  pack.ph = createPackageHead(deviceFlag,height,utctime);
  
  pack.dgs = (Datagrams *)osal_mem_alloc(sizeof(Datagrams));
  pack.dgs->dg = dg[0];
  Datagrams *tmp_dgs = pack.dgs;
  
  for(i=1;i<height;i++)
  {
    Datagrams *dgs =  (Datagrams *)osal_mem_alloc(sizeof(Datagrams));
    dgs->dg = dg[i];
    tmp_dgs->next_datagram = dgs;
    tmp_dgs = dgs;
  }
  
  return pack;
}
uint32 updatePackage(EntryCache *ec,Package *pack,UTCTimeStruct utctime,FrameHead *fh0,uint32 *frame_head_addr_p)
{
  if(ec->eh.lastFrameAddr[0] == 0xFF && ec->eh.lastFrameAddr[1] == 0xFF && ec->eh.lastFrameAddr[2] == 0xFF )
  {
    uint32 first_package_addr = PACKAGE_START_ADDR;
    uint8 first_pack_addr_array[3] = {MSB_ADDR(first_package_addr),MLSB_ADDR(first_package_addr),LSB_ADDR(first_package_addr)};
    //update pack
    pack->ph.prevPackageAddr[0] = 0xFF;
    pack->ph.prevPackageAddr[1] = 0xFF;
    pack->ph.prevPackageAddr[2] = 0xFF;
    pack->ph.packageNum = 0;
    //new frame
    FrameHead first_fh = createFrameHead(0,first_pack_addr_array,utctime);
    pack->ph._arg.frameAddr[0] = MSB_ADDR(FRAME_ENTRY_START_ADDR);
    pack->ph._arg.frameAddr[1] = MLSB_ADDR(FRAME_ENTRY_START_ADDR);
    pack->ph._arg.frameAddr[2] = LSB_ADDR(FRAME_ENTRY_START_ADDR);
    //update fh
    VOID osal_memcpy(fh0,&first_fh,FRAME_HEAD_LEN);
    //update ec->lastFrameAddr
    VOID osal_memcpy(ec->eh.lastFrameAddr,pack->ph._arg.frameAddr,3);
    ec->eh.height ++;
    
    return first_package_addr;
  }
  uint32 lastFrameAddr = FUSION32_ARRAY(ec->eh.lastFrameAddr);
  
  FrameHead fh = getFrameHead(lastFrameAddr);
  //last package addr
  uint32 lastPackageAddr = FUSION32_ARRAY(fh.lastPackageItemAddr);
  //new package addr
  PackageHead lastPack = getPackageHead(lastPackageAddr);
  uint32 newPackageAddr_32  = lastPackageAddr + (lastPack.height+1) * PACKAGE_HEAD_LEN;
  uint8 newPackageAddr[3] = {MSB_ADDR(newPackageAddr_32),MLSB_ADDR(newPackageAddr_32),LSB_ADDR(newPackageAddr_32)};
  uint32 frame_head_addr = lastFrameAddr;
  if(fh.day != utctime.day)
  {
    //if it's a new day, create a new frame
    fh = createFrameHead(0,newPackageAddr,utctime);
    frame_head_addr = lastFrameAddr + fh.height * FRAME_ITEM_LEN + FRAME_HEAD_LEN;
    uint8 frame_addr_tmp[3] = {MSB_ADDR(frame_head_addr),MLSB_ADDR(frame_head_addr),LSB_ADDR(frame_head_addr)};
    //update ec
    VOID osal_memcpy(ec->eh.lastFrameAddr,frame_addr_tmp,3);
    ec->eh.height ++;
    
    //update pack   a new frame
    pack->ph.packageNum = 0;                                                                     //packNum      1
    VOID osal_memcpy(pack->ph._arg.frameAddr,frame_addr_tmp,3);                                  //FrameAddr    1
    
  }else{ 
    //update pack old frame
    pack->ph.packageNum = fh.height;                                                            //packNum      2
    VOID osal_memcpy(pack->ph._arg.frameAddr,ec->eh.lastFrameAddr,3);                           //FrameAddr    2
    
  }
  //update pack common
  VOID osal_memcpy(pack->ph.prevPackageAddr, fh.lastPackageItemAddr,3);                         //prevPackage
  
  //update frame head common
  VOID osal_memcpy(fh.lastPackageItemAddr,newPackageAddr,3);
  fh.height +=1;
  //updatepakc
  
  VOID osal_memcpy(frame_head_addr_p,&frame_head_addr,FRAME_HEAD_LEN);
  VOID osal_memcpy(fh0,&fh,FRAME_HEAD_LEN);
}
//ec need preload
void write_back(EntryCache *ec,Package pack,UTCTimeStruct utctime)
{
  FrameHead fh;
  uint32 frame_head_addr;
  uint32 pack_addr = updatePackage(ec,&pack,utctime,&fh,&frame_head_addr);
  
  generateCacheByPackage(ec,&fh,pack,pack_addr,frame_head_addr,utctime );
  write_back_entry_cache(*ec,false);
  
  write_back_frame(frame_head_addr,&fh);
  write_back_package(&pack,pack_addr);
}

void write_back_package(Package *pack,uint32 _addr)
{
  PackageHead ph = pack->ph;
  writef(&ph,PACKAGE_HEAD_LEN,_addr);
  Datagrams *tmp = pack->dgs;
  while(tmp)
  {
    _addr += PACKAGE_HEAD_LEN;
    writef(tmp->dg.data,PACKAGE_HEAD_LEN,_addr);
    tmp = tmp->next_datagram;
  }
}

void write_back_entry_cache(EntryCache ec,bool force_all)
{
  writef(&ec.eh,ENTRY_HEAD_LEN,ENTRY_HEAD_ADDR);
  EntryItems *tmp_eis = ec.eis;
  uint32 _addr = ENTRY_ITEM_ADDR;
  while(tmp_eis->next_eis)
  {
      if(force_all)
      {
        EntryItem ei_t = tmp_eis->ei;
        writef(&ei_t,ENTRY_ITEM_LEN,_addr);
      }
      _addr += ENTRY_ITEM_LEN;
      tmp_eis = tmp_eis->next_eis;
  }
  EntryItem ei_t = (tmp_eis->ei);
  writef(&ei_t,ENTRY_ITEM_LEN,_addr);
}

void write_back_frame(uint32 head_addr,FrameHead *fh)
{
  writef(fh,FRAME_HEAD_LEN,head_addr);
}
 
bool generateCacheByPackage(EntryCache *ec,FrameHead *fh,Package pack,uint32 pack_addr, uint32 frame_head_addr,UTCTimeStruct utctime)
{
  //pack.ph._arg.frameAddr
  uint8 pack_addr_arr[3] = {MSB_ADDR(pack_addr),MLSB_ADDR(pack_addr),LSB_ADDR(pack_addr)};
  insertFrameItem(frame_head_addr,utctime.hour,utctime.minutes,pack.ph.height,pack_addr_arr);  //update Frame Item
  
  //frame addr pack.ph._arg.frameAddr
  EntryItem ei = createEntryItem(utctime,fh->height,pack.ph._arg.frameAddr);  //update Entry Item
  
  updateEntryCache(ec,ei);                      // update Entry cache (include Entry Head)
}

//get unit
EntryHead getEntryHead()
{     
     uint8 buf[16]={0};
     readf(ENTRY_HEAD_ADDR,ENTRY_HEAD_FLAG,buf);
     EntryHead eh;
     VOID osal_memcpy(&eh,buf,16);
     return eh;
}

FrameHead getFrameHead(uint32 _addr)
{
  uint8 buf[FRAME_HEAD_LEN] = {0};
  readf(_addr,FRAME_HEAD_FLAG,buf);
  FrameHead fh;
  VOID osal_memcpy(&fh,buf,FRAME_HEAD_LEN);
  return fh;
}

PackageHead getPackageHead(uint32 _addr)
{
  uint8 buf[PACKAGE_HEAD_LEN] = {0};
  readf(_addr,PACKAGE_HEAD_FALG,buf);
  PackageHead ph;
  VOID osal_memcpy(&ph,buf,PACKAGE_HEAD_LEN);
  return ph;
}
EntryItem getEntryItem(uint32 _addr)
{
  uint8 buf[8] = {0};
  readf(_addr,ENTRY_ITEM_FLAG,buf);
  EntryItem ei;
  VOID osal_memcpy(&ei,buf,8);
  return ei;
}

EntryCache getEntry()
{
  EntryCache ec;
  ec.eh = getEntryHead();
  uint32 _addr = ENTRY_ITEM_ADDR ;
  ec.eis = (EntryItems *)osal_mem_alloc(sizeof(EntryItems));
  ec.eis->next_eis = NULL;
  EntryItems *tmp_eis = ec.eis;
  tmp_eis->ei = getEntryItem(_addr);
  
  for(uint16 i=1;i<ec.eh.height;i++)
  {
    EntryItems *eis = (EntryItems *)osal_mem_alloc(sizeof(EntryItems));
    _addr += 0x08;
    eis->ei = getEntryItem(_addr);
    tmp_eis->next_eis = eis;
    tmp_eis = eis;
  }
  tmp_eis->next_eis = NULL;
  
  return ec;
}

bool updateEntryCache(EntryCache *ec,EntryItem ei)
{
    EntryItems *tmp_eis = ec->eis;
    while(tmp_eis->next_eis)
    {
      tmp_eis = tmp_eis->next_eis;
    }
    EntryItems *eis = (EntryItems*)osal_mem_alloc(sizeof(EntryItems));
    eis->ei = ei;
    eis->next_eis = NULL;
    tmp_eis->next_eis = eis;
   
    ec->eh.height += 1;
    ec->eh.lastFrameAddr[0]=ei.frameAddr[0];
    ec->eh.lastFrameAddr[1]=ei.frameAddr[1];
    ec->eh.lastFrameAddr[2]=ei.frameAddr[2];
    
    return true;
}

//Sync
bool sync()
{
  //readEntry
  //get Frame 
  
  //read Package
  //sensor data
  //send Flash Data
}

bool syncFromInterrupt()
{
  
}


//misc
bool isEqual(uint8 val1,uint8 val2)
{
  if(val1 ^ val2)
    return false;
  
  return true;
}
bool hasBit(uint8 val,uint8 bit)
{
  if(val & bit)
    return true;
  
  return false;
}

uint16 readf(uint32 addr,uint16 lenType,uint8 *pBuf)
{ //0x001F F0 00
  uint8 _addr[3] = {(addr & 0xff0000) >> 16, (addr & 0x00ff00) >> 8 , addr & 0x0000ff};
  uint16 _len;
  switch(lenType)
  {
  case COMMON_FLAG:
    _len = NORMAL_LEN;
    break;
  case ENTRY_HEAD_FLAG:
    _len = ENTRY_HEAD_LEN;
    break;
  case ENTRY_ITEM_FLAG:
    _len = ENTRY_ITEM_LEN;
    break;
  case ENTRY_ALL_FLAG:
    _len = pBuf[0] * ENTRY_ITEM_LEN;
    break;
  case FRAME_HEAD_FLAG:
    _len = FRAME_HEAD_LEN;
    break;
  case FRMAE_ITEM_FLAG:
    _len = FRAME_ITEM_LEN;
    break;
  case FRMAE_ALL_FLAG:
    _len = pBuf[0] * FRAME_ITEM_LEN;
    break;
  case PACKAGE_HEAD_FALG:
    _len = PACKAGE_HEAD_LEN;
    break;
  case DATAGRAM_FLAG:
    _len = pBuf[0] << 8 + pBuf[1];
    break;
  default:
    _len = lenType;
    break;
  }
  
  readMxFlash_Ex(_addr,_len,pBuf);
  return _len;
}
void readt(uint32 addr,uint8 lenType,uint8 *pBuf,void *stval)
{
   uint16 _len = readf(addr,lenType,pBuf);
   VOID osal_memcpy(stval,pBuf,_len);
}
void writef(void *val,uint16 len,uint32 addr)
{
  uint8 _addr[3] = {(addr & 0xff0000) >> 16, (addr & 0x00ff00) >> 8 , addr & 0x0000ff};
  uint8 *buf;
  buf = (uint8 *)osal_mem_alloc(len);
  VOID osal_memcpy(buf,val,len);
  writeMxFlash_Ex(_addr,len,buf);
  osal_mem_free(buf);
}

//void writeMxFlash_Ex(uint8 *addr,uint8 len,uint8 *pBuf);

//void readMxFlash_Ex(uint8 *addr,uint8 len,uint8 *pBuf);