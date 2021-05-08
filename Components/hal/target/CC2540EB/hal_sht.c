/**************************************************************************************************'
  Filename:       hal_sht.c
  Revised:        $Date: 2014-04-20 15:01:15 +0800 (Sun, 20 April 2014) $
  Revision:       $Revision: 1002 $

  Description:    Driver for the Sensirion SHT20 Humidity sensor

  @use example in the end comment
**************************************************************************************************/

/* ------------------------------------------------------------------------------------------------
*                                          Includes
* ------------------------------------------------------------------------------------------------
*/
#include "hal_sht.h"
#include "hal_sensor.h"
#include "hal_i2c.h"
#include "hal_drivers.h"
#include "hal_board.h"
/* ------------------------------------------------------------------------------------------------
*                                           Constants
* ------------------------------------------------------------------------------------------------
*/

// Sensor I2C address
#define HAL_SHT20_I2C_ADDRESS      0x40

#define S_REG_LEN                  2
#define DATA_LEN                   3

// Internal commands
#define SHT20_CMD_TEMP_T_H         0xE3 // command trig. temp meas. hold master
#define SHT20_CMD_HUMI_T_H         0xE5 // command trig. humidity meas. hold master
#define SHT20_CMD_TEMP_T_NH        0xF3 // command trig. temp meas. no hold master
#define SHT20_CMD_HUMI_T_NH        0xF5 // command trig. humidity meas. no hold master
#define SHT20_CMD_WRITE_U_R        0xE6 // command write user register
#define SHT20_CMD_READ_U_R         0xE7 // command read user register
#define SHT20_CMD_SOFT_RST         0xFE // command soft reset

#define HUMIDITY                        0x00
#define TEMPERATURE                     0x01

#define USR_REG_MASK               0x38  // Mask off reserved bits (3,4,5)
#define USR_REG_DEFAULT            0x02  // Disable OTP reload
#define USR_REG_RES_MASK           0x7E  // Only change bits 0 and 7 (meas. res.)
#define USR_REG_11BITRES           0x81  // 11-bit resolution

/* ------------------------------------------------------------------------------------------------
*                                           Type Definitions
* ------------------------------------------------------------------------------------------------
*/

/* ------------------------------------------------------------------------------------------------
*                                           Local Functions
* ------------------------------------------------------------------------------------------------
*/
static void SHT_Select(void);
static bool SHT_ReadData(uint8 *pBuf,uint8 nBytes);
static bool SHT_WriteCmd(uint8 cmd);

/* ------------------------------------------------------------------------------------------------
*                                           Local Variables
* ------------------------------------------------------------------------------------------------
*/
static uint8 usr;                         // Keeps user register value
static uint8 buf[6];                      // Data buffer
static bool  success;

/**************************************************************************************************
* @fn          SHT_Init
*
* @brief       Initialise the humidity sensor driver
*
* @return      none
**************************************************************************************************/
bool SHT_Init(void)
{
  SHT_Select();
  
  // Set 11 bit resolution
  HalSensorReadReg(SHT20_CMD_READ_U_R,&usr,1);
  uprintEx(&usr,1);
  
  usr &= USR_REG_RES_MASK;
  usr |= USR_REG_11BITRES;
  HalSensorWriteReg(SHT20_CMD_WRITE_U_R,&usr,1);
 
  success = FALSE;
  
  return true;
}

/*
*       DATA_LEN = 3
*       Data       
*       MSB                     LSB     Stat(0:Temp 1:Humi)     Resv            |Checksum
*       13(20)     7       6        2     1                       0             |7       0
*/

bool SHT_ReadTemp(uint8 *buf2)
{
  SHT_Select();
  SHT_WriteCmd(SHT20_CMD_TEMP_T_NH);
  WAIT_MS(100);
  success = SHT_ReadData(buf2, DATA_LEN);
  return 1;
}
bool SHT_ReadHumi(uint8 *buf2)
{
  SHT_Select();
  success = SHT_WriteCmd(SHT20_CMD_HUMI_T_NH);
  WAIT_MS(100);
  success = SHT_ReadData(buf2, DATA_LEN);
  return 1;
}


/**************************************************************************************************
* @fn          SHT_ExecMeasurementStep
*
* @brief       Execute measurement step
*
* @return      none
*/
bool SHT_ExecMeasurementStep(uint8 state)
{
  SHT_Select();

  switch (state)
  {
    case 0:
      // Turn on DC-DC control
      //HalDcDcControl(SS_HUMID,true);

      // Start temperature read
      success = SHT_WriteCmd(SHT20_CMD_TEMP_T_NH);
      break;

    case 1:
      // Read and store temperature value
      if (success)
      {
        success = SHT_ReadData(buf, DATA_LEN);

        // Start for humidity read
        if (success)
        {
          success = SHT_WriteCmd(SHT20_CMD_HUMI_T_NH);
        }
      }
      break;

    case 2:
      // Read and store humidity value
      if (success)
      {
        success = SHT_ReadData(buf+DATA_LEN, DATA_LEN);
      }

      // Turn of DC-DC control
      HalDcDcControl(SS_HUMID,false);
      break;
  }

  return success;
}


/**************************************************************************************************
* @fn          SHT_ReadMeasurement
*
* @brief       Get humidity sensor data
*
* @return      none
*/
bool SHT_ReadMeasurement(uint8 *pBuf)
{
  // Store temperature
  pBuf[0] = buf[1];
  pBuf[1] = buf[0];

  // Store humidity
  pBuf[2] = buf[4];
  pBuf[3] = buf[3];

  return success;
}



/**************************************************************************************************
* @fn          SHT_TEST
*
* @brief       Humidity sensor self test                only for power on self test
*     MSB                                         LSB 
      u8SerialNumber[7]                         u8SerialNumber[0] 
      SNA_1 SNA_0 SNB_3 SNB_2 SNB_1 SNB_0 SNC_1 SNC_0 

* @return      none
**************************************************************************************************/
bool SHT_TEST(void)
{ 
  uint8 val[8];
  uint8 crctmp;
  SHT_Select();
  SHT_WriteCmd(0xFA);
  SHT_WriteCmd(0x0F);
  
  SHT_Select();
  SHT_ReadData(&val[5],1);              //Read SNB_3 
  SHT_ReadData(&crctmp,1);              //CRC
  SHT_ReadData(&val[4],1);              //Read SNB_2 
  SHT_ReadData(&crctmp,1);              //CRC
  SHT_ReadData(&val[3],1);              //Read SNB_1
  SHT_ReadData(&crctmp,1);              //CRC
  SHT_ReadData(&val[2],1);              //Read SNB_0
  SHT_ReadData(&crctmp,1);              //CRC
  HalI2CDisable();
  
  SHT_Select();
  SHT_WriteCmd(0xFC);
  SHT_WriteCmd(0xC9);
 
  SHT_Select();
  SHT_ReadData(&val[0],1);              //Read SNC_1
  SHT_ReadData(&val[1],1);              //Read SNC_0 
  SHT_ReadData(&crctmp,1);              //CRC
  SHT_ReadData(&val[7],1);              //Read SNA_1
  SHT_ReadData(&val[6],1);              //Read SNA_0
  SHT_ReadData(&crctmp,1);              //CRC
  HalI2CDisable();
  
  uprintEx(val,8);
 //0x0080 7654 3210 0112
  uint32 valtmp1 = val[7] << 24 + val[6] << 16 + val[5] << 8 + val[4];
  uint32 valtmp2 = val[3] << 24 + val[2] << 16 + val[1] << 8 + val[0];
  if(valtmp1 == 0x00807654 && valtmp2 == 0x32100112)
  {
    uprint("hi");
    return true;
  }

 
  return false;
}



/* ------------------------------------------------------------------------------------------------
*                                           Private functions
* -------------------------------------------------------------------------------------------------
*/

/**************************************************************************************************
* @fn          SHT_Select
*
* @brief       Select the humidity sensor on the I2C-bus
*
* @return
*/
static void SHT_Select(void)
{
  //Set up I2C that is used to communicate with SHT20
  HalI2CInit(HAL_SHT20_I2C_ADDRESS,i2cClock_267KHZ);
}


/**************************************************************************************************
* @fn          SHT_WriteCmd
*
* @brief       Write a command to the humidity sensor
*
* @param       cmd - command to write
*
* @return      TRUE if the command has been transmitted successfully
**************************************************************************************************/
static bool SHT_WriteCmd(uint8 cmd)
{
  /* Send command */
  return HalI2CWrite(1,&cmd) == 1;
}


/**************************************************************************************************
* @fn          SHT_ReadData
*
* @brief       This function implements the I2C protocol to read from the SHT20.
*
* @param       pBuf - pointer to buffer to place data
*
* @param       nBytes - number of bytes to read
*
* @return      TRUE if the required number of bytes are received
**************************************************************************************************/
static bool SHT_ReadData(uint8 *pBuf, uint8 nBytes)
{
  /* Read data */
  return HalI2CRead(nBytes,pBuf ) == nBytes;
}

/*  Conversion algorithm, humidity
 *
double calcHumRel(uint16 rawH)
{
    double v;

    rawH &= ~0x0003; // clear bits [1..0] (status bits)
    //-- calculate relative humidity [%RH] --
    v = -6.0 + 125.0/65536 * (double)rawH; // RH= -6 + 125 * SRH/2^16

    return v;
}

 *  Conversion algorithm, temperature
 *
double calcHumTmp(uint16 rawT)
{
    double v;

    //-- calculate temperature [°C] --
    v = -46.85 + 175.72/65536 *(double)(qint16)rawT;

    return v;
}

*/

/*********************************************************************
*********************************************************************/


/******************************************************************************
//use example 

#define sensorHumPeriod 1000            //use for sleep
#define HUM_FSM_PERIOD 20               //Constants for two-stage reading
 if ( events & ST_HUMIDITY_SENSOR_EVT )
  {
    if (humiEnabled)
    {
      HalHumiExecMeasurementStep(humiState);
      if (humiState == 2)
      {
        readHumData();
        humiState = 0;
        osal_start_timerEx( sensorTag_TaskID, ST_HUMIDITY_SENSOR_EVT, sensorHumPeriod );
      }
      else
      {
        humiState++;
        osal_start_timerEx( sensorTag_TaskID, ST_HUMIDITY_SENSOR_EVT, HUM_FSM_PERIOD );
      }
    }
    else
    {
      resetCharacteristicValue( HUMIDITY_SERV_UUID, SENSOR_DATA, 0, HUMIDITY_DATA_LEN);
      resetCharacteristicValue( HUMIDITY_SERV_UUID, SENSOR_CONF, ST_CFG_SENSOR_DISABLE, sizeof ( uint8 ));
    }

    return (events ^ ST_HUMIDITY_SENSOR_EVT);
  }

******************************************************************************/