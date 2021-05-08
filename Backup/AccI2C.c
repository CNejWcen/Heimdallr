/*********************************************************************
 * 产品代号 Watcher Heimdallr
 * 所有前缀一律使用MWH 意为 Maitian Watcher Heimdallr
 * Version 20140304
 ********************************************************************/
#include "AccI2C.h"
#include "hal_i2c.h"


void AccInit()
{
    i2cClock_t clockRate = i2cClock_123KHZ;
    HalI2CInit(DEVICE_ADDR,clockRate);
    AccSetting();
}
void AccStop()
{
  HalI2CDisable();
}

void AccSetting()
{
  //set mode:sniff/disable auto-wake/enable auto-sniff/sleep timer divided 1
  uint8 mode            =       0x0A;
  uint8 sleep           =       0x20;
  uint8 inten           =       0xFF;
  uint8 sample_rate     =       0xE0;
  uint8 tapen           =       0xE0;
  uint8 tapp            =       0x07;
  uint8 drop            =       0x00;
  uint8 shdb            =       0x20;
  AccWrite(MODE_REG,mode);
  AccWrite(SC_REG,sleep);
  AccWrite(INT_REG,inten);
  AccWrite(SAMPR_REG,sample_rate);
  AccWrite(TAPEN_REG,tapen);
  AccWrite(TAPP_REG,tapp);
  AccWrite(DROP_REG,drop);
  AccWrite(SHDB_REG,shdb);
}

uint8 AccRead(uint8 reg_addr,uint8 *rbuf)
{
  HalI2CWrite(1,&reg_addr);
  HalI2CRead(1,rbuf);
  return 1;
}
void AccWrite(uint8 reg_addr,uint8 data)
{
  uint8 buf[]={reg_addr,data};
  HalI2CWrite(2,buf);
}

uint8 AccGetOPSTAT()
{
  uint8 op[1]={0};
  AccRead(OP_STAT,op);
  return op[0];
}

uint8 AccGetXAxis()
{
  uint8 x[1]={0};
  AccRead(XOUT_REG,x);
  return x[0];
}
uint8 AccGetYAxis()
{
  uint8 y[1]={0};
  AccRead(YOUT_REG,y);
  return y[0];
}
uint8 AccGetZAxis()
{
  uint8 z[1]={0};
  AccRead(ZOUT_REG,z);
  return z[0];
}
uint8 AccGetTilt()
{
  uint8 tilt[1]={0};
  AccRead(TILT_REG,tilt);
  return tilt[0];
}
