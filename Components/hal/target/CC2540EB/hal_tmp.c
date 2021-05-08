/**************************************************************************************************
  Filename:       hal_irtemp.c
  Revised:        $Date: 2014-4-20 16:58:46 +800 (Sun,20 April 2014) $
  Revision:       $Revision: 10007 $

  Description:    Driver for the TI TMP06 infrared thermophile sensor.


~After init
~select ¡ú TMP_Status ¡ú Read data
**************************************************************************************************/

/* ------------------------------------------------------------------------------------------------
*                                          Includes
* ------------------------------------------------------------------------------------------------
*/
#include "hal_tmp.h"
#include "hal_i2c.h"
#include "hal_sensor.h"

/* ------------------------------------------------------------------------------------------------
*                                           Constants
* ------------------------------------------------------------------------------------------------
*/

/* Slave address */
#define TMP006_I2C_ADDRESS              0x44

/* TMP006 register addresses */
#define TMP006_REG_ADDR_VOLTAGE         0x00
#define TMP006_REG_ADDR_TEMPERATURE     0x01
#define TMP006_REG_ADDR_CONFIG          0x02
#define TMP006_REG_MANF_ID              0xFE
#define TMP006_REG_PROD_ID              0xFE

/* TMP006 register values */
#define TMP006_VAL_CONFIG_RESET         0x7400  // Sensor reset state
#define TMP006_VAL_CONFIG_ON            0x7000  // Sensor on state
#define TMP006_VAL_CONFIG_OFF           0x0000  // Sensor off state
#define TMP006_VAL_MANF_ID              0x5449  // Manufacturer ID
#define TMP006_VAL_PROD_ID              0x0067  // Product ID

/* Bit values */
#define DATA_RDY_BIT                    0x8000 // Data ready

/* Register length */
#define TMP_REG_LEN                  2


/* ------------------------------------------------------------------------------------------------
*                                           Local Functions
* ------------------------------------------------------------------------------------------------
*/
static void TMP_Select(void);


/* ------------------------------------------------------------------------------------------------
*                                           Local Variables
* ------------------------------------------------------------------------------------------------
*/
static TMP_States_t irtSensorState = TMP006_OFF;

static uint8 configSensorReset[2] = {0x80, 0x00};  // Sensor reset
static uint8 configSensorOff[2] = {0x00, 0x80};    // Sensor standby
static uint8 configSensorOn[2] =  {0x70, 0x00};    // Conversion time 0.25 sec

/* ------------------------------------------------------------------------------------------------
*                                           Public functions
* -------------------------------------------------------------------------------------------------
*/


/**************************************************************************************************
 * @fn          TMP_Init
 *
 * @brief       Initialise the temperature sensor driver
 *
 * @return      none
 **************************************************************************************************/
bool TMP_Init(void)
{
  irtSensorState = TMP006_OFF;
  TMP_Shutdown();
  return true;
}

/**************************************************************************************************
 * @fn          TMP_Start
 *
 * @brief       Turn the sensor on
 *
 * @return      none
 **************************************************************************************************/
void TMP_Start(void)
{
  HalDcDcControl(SS_IRTEMP,true);
  TMP_Select();

  if (HalSensorWriteReg(TMP006_REG_ADDR_CONFIG, configSensorOn, TMP_REG_LEN))
  {
    irtSensorState = TMP006_IDLE;
  }
}

/**************************************************************************************************
 * @fn          TMP_Shutdown
 *
 * @brief       Turn the sensor off
 *
 * @return      none
 **************************************************************************************************/
void TMP_Shutdown(void)
{
  TMP_Select();

  if (HalSensorWriteReg(TMP006_REG_ADDR_CONFIG, configSensorOff, TMP_REG_LEN))
  {
    irtSensorState = TMP006_OFF;
  }
  HalDcDcControl(SS_IRTEMP,false);
}

/**************************************************************************************************
 * @fn          TMP_Read
 *
 * @brief       Read the sensor voltage and sensor temperature registers
 *
 * @param       Voltage and temperature in raw format (2 + 2 bytes)
 *
 * @return      TRUE if valid data
 **************************************************************************************************/
bool TMP_Read(uint8 *pBuf)
{
  uint16 v;
  uint16 t;
  bool success;

  if (irtSensorState != TMP006_DATA_READY)
  {
    return FALSE;
  }

  TMP_Select();

  // Read the sensor registers
  success = HalSensorReadReg(TMP006_REG_ADDR_VOLTAGE, (uint8 *)&v,TMP_REG_LEN );
  if (success)
  {
    success = HalSensorReadReg(TMP006_REG_ADDR_TEMPERATURE, (uint8 *)&t,TMP_REG_LEN );
  }

  if (success)
  {
    // Store values
    pBuf[0] = HI_UINT16( v );
    pBuf[1] = LO_UINT16( v );
    pBuf[2] = HI_UINT16( t );
    pBuf[3] = LO_UINT16( t );
  }

  // Turn off sensor
  if (HalSensorWriteReg(TMP006_REG_ADDR_CONFIG, configSensorOff, TMP_REG_LEN))
  {
    irtSensorState = TMP006_OFF;
  }
  HalDcDcControl(SS_IRTEMP,false);

  return success;
}


/**************************************************************************************************
 * @fn          TMP_Status
 *
 * @brief       Read the state of the sensor
 *
 * @return      none
 **************************************************************************************************/
TMP_States_t TMP_Status(void)
{
  if (irtSensorState != TMP006_OFF)
  {
    bool success;
    uint16 v;

    // Select this sensor on the I2C bus
    TMP_Select();

    // Read the data ready bit
    success = HalSensorReadReg(TMP006_REG_ADDR_CONFIG, (uint8 *)&v,TMP_REG_LEN );
    if ((v & DATA_RDY_BIT) && success)
    {
      irtSensorState = TMP006_DATA_READY;
    }
  }

  return irtSensorState;
}


/**************************************************************************************************
 * @fn          TMP_Test
 *
 * @brief       Run a sensor self-test          only for power on self test
 *
 * @return      TRUE if passed, FALSE if failed

 **************************************************************************************************/
bool TMP_TEST(void)
{
  uint16 val;

  // Select this sensor on the I2C bus
  TMP_Select();

  // Check manufacturer ID
  SS_ASSERT(HalSensorReadReg(TMP006_REG_MANF_ID, (uint8 *)&val, TMP_REG_LEN));
  val = (LO_UINT16(val) << 8) | HI_UINT16(val);
  SS_ASSERT(val == TMP006_VAL_MANF_ID);
  // Reset sensor
  SS_ASSERT(HalSensorWriteReg(TMP006_REG_ADDR_CONFIG, configSensorReset, TMP_REG_LEN));

  // Check config register (reset)
  SS_ASSERT(HalSensorReadReg(TMP006_REG_ADDR_CONFIG, (uint8 *)&val, TMP_REG_LEN));
  val = ((LO_UINT16(val) << 8) | HI_UINT16(val));
  SS_ASSERT(val == TMP006_VAL_CONFIG_RESET);

  // Turn sensor off
  SS_ASSERT(HalSensorWriteReg(TMP006_REG_ADDR_CONFIG, configSensorOff,TMP_REG_LEN));

  // Check config register (off)
  SS_ASSERT(HalSensorReadReg(TMP006_REG_ADDR_CONFIG, (uint8 *)&val, TMP_REG_LEN));
  val = ((LO_UINT16(val) << 8) | HI_UINT16(val));
  SS_ASSERT(val == TMP006_VAL_CONFIG_OFF);

  // Turn sensor on
  SS_ASSERT(HalSensorWriteReg(TMP006_REG_ADDR_CONFIG, configSensorOn, TMP_REG_LEN));

  // Check config register (on)
  SS_ASSERT(HalSensorReadReg(TMP006_REG_ADDR_CONFIG, (uint8 *)&val, TMP_REG_LEN));
  val = ((LO_UINT16(val) << 8) | HI_UINT16(val));
  SS_ASSERT(val == TMP006_VAL_CONFIG_ON);

  // Turn sensor off
  SS_ASSERT(HalSensorWriteReg(TMP006_REG_ADDR_CONFIG, configSensorOff, TMP_REG_LEN));

  return TRUE;
}



/* ------------------------------------------------------------------------------------------------
*                                           Private functions
* -------------------------------------------------------------------------------------------------
*/

/**************************************************************************************************
 * @fn          TMP_Select
 *
 * @brief       Select the TMP006 slave and set the I2C bus speed
 *
 * @return      none
 **************************************************************************************************/
static void TMP_Select(void)
{
  // Select slave and set clock rate
  HalI2CInit(TMP006_I2C_ADDRESS, i2cClock_533KHZ);
}

/*  Conversion algorithm for die temperature
 *  ================================================
 *
double calcTmpLocal(uint16 rawT)
{
    //-- calculate die temperature [°C] --
    m_tmpAmb = (double)((qint16)rawT)/128.0;

    return m_tmpAmb;
}

*
* Conversion algorithm for target temperature
*
double calcTmpTarget(uint16 rawT)
{
    //-- calculate target temperature [°C] -
    double Vobj2 = (double)(qint16)rawT;
    Vobj2 *= 0.00000015625;

    double Tdie2 = m_tmpAmb + 273.15;
    const double S0 = 6.4E-14;            // Calibration factor

    const double a1 = 1.75E-3;
    const double a2 = -1.678E-5;
    const double b0 = -2.94E-5;
    const double b1 = -5.7E-7;
    const double b2 = 4.63E-9;
    const double c2 = 13.4;
    const double Tref = 298.15;
    double S = S0*(1+a1*(Tdie2 - Tref)+a2*pow((Tdie2 - Tref),2));
    double Vos = b0 + b1*(Tdie2 - Tref) + b2*pow((Tdie2 - Tref),2);
    double fObj = (Vobj2 - Vos) + c2*pow((Vobj2 - Vos),2);
    double tObj = pow(pow(Tdie2,4) + (fObj/S),.25);
    tObj = (tObj - 273.15);

    return tObj;
}

*/

/*********************************************************************
*********************************************************************/

