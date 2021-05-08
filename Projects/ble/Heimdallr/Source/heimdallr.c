/**************************************************************************************************
  Filename:       heimdallr.c
  Revised:        $Date: 2013-08-15 15:28:40 -0700 (Thu, 15 Aug 2013) $
  Revision:       $Revision: 34986 $

  Description:    Watcher Heimdallr Application.
**************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "bcomdef.h"
#include "OSAL.h"
#include "hal_drivers.h"
#include "OSAL_PwrMgr.h"

#include "OnBoard.h"

#include "gatt.h"

#include "hci.h"

#include "gapgattserver.h"
#include "gattservapp.h"
#include "gatt_profile_uuid.h"

#include "peripheral.h"
#include "gapbondmgr.h"

#include "devinfoservice.h"

#include "battprofile.h"
 
#include "humiprofile.h"

//#include "protocolprofile.h"

#include "osal_snv.h"
#include "OSAL_Clock.h"
#include "hal_adc.h"

#include "heimdallr.h"
#include "hal_uart.h"
#include "hal_sht.h"
#include "hal_mpu.h"
#include "hal_dbflash.h"
 


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// How often to perform sensor reads (milliseconds)
#define IRTEMP_DEFAULT_PERIOD                   1000
#define HUMI_DEFAULT_PERIOD                     1000
#define HEART_DEFAULT_PERIOD                    5000
#define ACC_DEFAULT_PERIOD                      1000
#define FLASH_DEFAULT_PERIOD                    1000

// Constants for two-stage reading
#define IRTEMP_MEAS_DELAY                       275   // Conversion time 250 ms
#define FLASH_READ_PERIOD                       50
#define ACC_FSM_PERIOD                          20
#define HUMI_FSM_PERIOD                         20
#define HEART_STARTUP_TIME                      60    // Start-up time max. 50 ms

#define ASSEMBLE_PERIOD                         30000

   
// Delay between power-up and starting advertising (in ms)
#define STARTDELAY                            500

// How often to check battery voltage (in ms)
#define BATTERY_CHECK_PERIOD                  10000

#define RESTART_SCAN_PERIOD                   20000
// How often (in ms) to read the accelerometer
#define ACCEL_READ_PERIOD                     60
// Minimum change in accelerometer before sending a notification
#define ACCEL_CHANGE_THRESHOLD                5

#define HUMI_READ_PERIOD                      100 
//GAP Peripheral Role desired connection parameters

// Use limited discoverable mode to advertise for 30.72s, and then stop, or
// use general discoverable mode to advertise indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_LIMITED
//#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     80

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         TRUE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         5
/*
// keyfobProximityState values
#define HEIMDALLR_PROXSTATE_INITIALIZED          0   // Advertising after initialization or due to terminated link
#define HEIMDALLR_PROXSTATE_CONNECTED_IN_RANGE   1   // Connected and "within range" of the master, as defined by
                                                  // proximity profile
#define HEIMDALLR_PROXSTATE_PATH_LOSS            2   // Connected and "out of range" of the master, as defined by
                                                  // proximity profile
#define HEIMDALLR_PROXSTATE_LINK_LOSS            3   // Disconnected as a result of a supervision timeout
*/
// Company Identifier: Texas Instruments Inc. (13)
#define TI_COMPANY_ID                         0x000D

#define INVALID_CONNHANDLE                    0xFFFF

#if defined ( PLUS_BROADCASTER )
  #define ADV_IN_CONN_WAIT                    500 // delay 500 ms
#endif

//#define DEBUG 

//#define HAL_TEMP_TEST 
//#define HAL_ADC_TEST
//#define HAL_I2C_TEST
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 HeimDallr_TaskID;   // Task ID for internal task/event processing

static gaprole_States_t gapProfileState = GAPROLE_INIT;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 deviceName[] =
{
  // complete name  48 65 69 6D 64 61 6C 6C 72 
  0x0a,   // length of first data structure (11 bytes excluding length byte)
  0x09,   // AD Type = Complete local name
  0x48,   // 'H'
  0x65,   // 'e'
  0x69,   // 'i'
  0x6D,   // 'm'
  0x64,   // 'd'
  0x61,   // 'a'
  0x6C,   // 'l'
  0x6C,   // 'l'
  0x72,   // 'r'
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8 advertData[] =
{
  0x02,   // length of first data structure (2 bytes excluding length byte)
  GAP_ADTYPE_FLAGS,   // AD Type = Flags
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x01,   // length of second data structure (7 bytes excluding length byte)
  GAP_ADTYPE_16BIT_MORE   // list of 16-bit UUID's available, but not complete list
};

// GAP GATT Attributes
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "MT Baby Tag";
 

// Accelerometer Profile Parameters 
static uint8 humiEnabler = FALSE; 
static uint8 humiState = 0;


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void HeimDallr_ProcessOSALMsg( osal_event_hdr_t *pMsg );
static void peripheralStateNotificationCB( gaprole_States_t newState );
//static void proximityAttrCB( uint8 attrParamID ); 
static void humiChangeCB(uint8 id); 
static void readHumData(void); 

static void WatcherHeimdallr_HandleSerial(mtOSALSerialData_t *cmdMsg);
//static void ConvertUint32ToUint8(uint32 value,uint8 *result);
//static void ConvertUint16ToUint8(uint32 value,uint8 *result);
//WatchDog
void Init_Watchdog(void);
void FeedDog(void);
//Test
static void performTest();
static void ValueToString(uint16 Data ,uint8 *Buff);
static void resetCharacteristicValue(uint16 servUuid, uint8 paramID, uint8 value, uint8 paramLen);
//void hex2str( uint8 *pBuf,char *str,uint8 buflen);
//void displayTime(void);
/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t heimDallr_PeripheralCBs =
{
  peripheralStateNotificationCB,  // Profile State Change Callbacks
  NULL                // When a valid RSSI is read from controller
};

// GAP Bond Manager Callbacks
static gapBondCBs_t heimDallr_BondMgrCBs =
{
  NULL,                     // Passcode callback (not used by application)
  NULL                      // Pairing / Bonding state Callback (not used by application)
};

 
static sensorCBs_t heimDallr_HumiCBs =
{
  humiChangeCB,          // Called when Enabler attribute changes
};
 
//UPDATE_PROCESS(asm_process,ACCEL_XYZ_DONE);

static uint8 data_humi_all_buf[4]  = {0,0,0,0};
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      HeimDallr_Init
 *
 * @brief   Initialization function for the Heimdallr App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void HeimDallr_Init( uint8 task_id )
{
  HeimDallr_TaskID = task_id;
  //Init Uart use UART1 on port1 
  UartInit();
  RegisterForSerial(HeimDallr_TaskID);
  // UART init finished
  BSP_LED_INIT();
                                                        //Power on self test
  LedBlink();
  // halSleep(30000);
  //Init_Watchdog();
  // Setup the GAP
  VOID GAP_SetParamValue( TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL );
  
  // Setup the GAP Peripheral Role Profile
  {
    // For the CC2540DK-MINI keyfob, device doesn't start advertising until button is pressed
    uint8 initial_advertising_enable = TRUE;
    
    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16 gapRole_AdvertOffTime = 6000;
  
    uint8 enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16 desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16 desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16 desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;
   
    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    GAPRole_SetParameter( GAPROLE_ADVERT_OFF_TIME, sizeof( uint16 ), &gapRole_AdvertOffTime );
  
    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( deviceName ), deviceName );
    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );
  
    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
  }
  
  // Set the GAP Attributes
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );

  // Setup the GAP Bond Manager
  {
    // here need a if
    uint8 pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;         //GAPBOND_PAIRING_MODE_INITIATE
    uint8 mitm = TRUE;
    uint8 ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    uint8 bonding = TRUE;
    
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_BONDING_ENABLED, sizeof ( uint8 ), &bonding );
  }
  

  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );         // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES ); // GATT attributes
  DevInfo_AddService();   // Device Information Service
  
  Batt_AddService( );     // Battery Service
//  Accel_AddService( GATT_ALL_SERVICES );      // Accelerometer Profile
 
  Humi_AddService( GATT_ALL_SERVICES ); 
  
  // initialize the ADC for battery reads
  HalAdcInit();
  BattLED_ON();
  SignalLED_ON();
  
  SHT_Init();
  uint8 buf[3];
  SHT_ReadTemp(buf);
  dbg(" \nTemp Data:%02x %02x %02x\n",buf[0],buf[1],buf[2]);
  SHT_ReadHumi(buf);
  dbg("Humi Data:%02x %02x %02x\n",buf[0],buf[1],buf[2]);
  WAIT_MS(100);
  MPU_TEST();
  
  DbTest();
  /* 
   //SNV  only within 0x80 C 0xFE
  uint32 testData = 0x43434343;
  
  osal_snv_write(HNV_ANY_TEST_DATA,sizeof(uint32),&testData);
  
  uint8 testRead;
  osal_snv_read(HNV_ANY_TEST_DATA,sizeof(uint32),&testRead);
*/

 
  // Enable stack to toggle bypass control on TPS62730 (DC/DC converter)
/***********************************************
  * Before doing VDD measurement. You should:
  * 1)      disable PM IO port by:
  * HCI_EXT_MapPmInOutPortCmd(HCI_EXT_PM_IO_PORT_NONE, HCI_EXT_PM_IO_PORT_PIN2)
  * 2)      Set P1.2 to low by P1DIR |= 0x04;P1 |= 0x04
  * 3)      Check battery with battMeasure()
  * 4)      Set P1.2 to high
  * 5)      Enable PM IO port by:
  * HCI_EXT_MapPmInOutPortCmd(HCI_EXT_PM_IO_PORT_P1, HCI_EXT_PM_IO_PORT_PIN2)
***********************************************/
  //HCI_EXT_MapPmIoPortCmd( HCI_EXT_PM_IO_PORT_P1, HCI_EXT_PM_IO_PORT_PIN2 );
  //DCDC_SBIT = 0;
  //Need to be concerned
    
  // Setup a delayed profile startup
  osal_start_timerEx( HeimDallr_TaskID, MWH_START_DEVICE_EVT, STARTDELAY );
  osal_start_timerEx( HeimDallr_TaskID, MWH_RESTART_SCAN_DEVICE, RESTART_SCAN_PERIOD );
  
  
  
 /*
  P1SEL &= ~0x30;
  P1DIR |= 0x30;
  P1_4 = 1;
  P1_5 = 1;
  */
  //osal_start_timerEx( HeimDallr_TaskID, MWH_START_UP_SIM800L0, 4000 );
 
}

/*********************************************************************
 * @fn      HeimDallr_ProcessEvent
 *
 * @brief   Heimdallr Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 HeimDallr_ProcessEvent( uint8 task_id, uint16 events )
{

  //displayTime();
  dbg("a");
  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;
    if ( (pMsg = osal_msg_receive( HeimDallr_TaskID )) != NULL )
    {
      HeimDallr_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      // Release the OSAL message
      VOID osal_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & MWH_START_DEVICE_EVT )
  { 
    // Start the Device
    VOID GAPRole_StartDevice( &heimDallr_PeripheralCBs );

    // Start Bond Manager
    VOID GAPBondMgr_Register( &heimDallr_BondMgrCBs );
    
    // Set timer for first battery read event
    osal_start_timerEx( HeimDallr_TaskID, MWH_BATTERY_CHECK_EVT, BATTERY_CHECK_PERIOD );

    // Start the Accelerometer Profile
     
    VOID Humi_RegisterAppCBs( &heimDallr_HumiCBs );

    
    osal_start_timerEx( HeimDallr_TaskID, MWH_ASSEMBLE_PACKAGE_EVT, ASSEMBLE_PERIOD);
    //Set the proximity attribute values to default
  //  ProxReporter_SetParameter( PP_LINK_LOSS_ALERT_LEVEL,  sizeof ( uint8 ), &keyfobProxLLAlertLevel );
  //  ProxReporter_SetParameter( PP_IM_ALERT_LEVEL,  sizeof ( uint8 ), &keyfobProxIMAlertLevel );
  //  ProxReporter_SetParameter( PP_TX_POWER_LEVEL,  sizeof ( int8 ), &keyfobProxTxPwrLevel );

    // Set LED1 on to give feedback that the power is on, and a timer to turn off
    //HalLedSet( HAL_LED_1, HAL_LED_MODE_ON );
    //osal_pwrmgr_device( PWRMGR_ALWAYS_ON ); // To keep the LED on continuously.
    //osal_start_timerEx( HeimDallr_TaskID, MWH_POWERON_LED_TIMEOUT_EVT, 1000 );

    return ( events ^ MWH_START_DEVICE_EVT );
  }
   
  //Check update enable scan
  if ( events & MWH_RESTART_SCAN_DEVICE)
  {
    uint8 initial_advertising_enable = TRUE;
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    
    if(RESTART_SCAN_PERIOD)
    {
      osal_start_timerEx( HeimDallr_TaskID, MWH_RESTART_SCAN_DEVICE, RESTART_SCAN_PERIOD );
    }
    return ( events ^ MWH_RESTART_SCAN_DEVICE );
  }
 
  if ( events & MWH_HUMI_READ_EVT )
  {
    uprint("humi");
  //  if(isConnected(gapProfileState) && flash_asm_flag[0] == PACKAGE_ASM_START)
 //   {
        
 //       return (events ^ MWH_HUMI_READ_EVT);
 //   }
    
    bStatus_t status = Humi_GetParameter( SENSOR_CONF, &humiEnabler );
    
    if (status == SUCCESS)
    {
      if (humiEnabler)
      {
        SHT_ExecMeasurementStep(humiState);
        if (humiState == 2)
        {
          readHumData();
          humiState = 0;
          osal_start_timerEx( HeimDallr_TaskID, MWH_HUMI_READ_EVT, HUMI_READ_PERIOD );
        }
        else
        {
          humiState++;
          osal_start_timerEx( HeimDallr_TaskID, MWH_HUMI_READ_EVT, HUMI_FSM_PERIOD );
        }
      }
      else
      {
        // resetCharacteristicValue( HUMIDITY_SERV_UUID, SENSOR_DATA, 0, HUMIDITY_DATA_LEN);
        //  resetCharacteristicValue( HUMIDITY_SERV_UUID, SENSOR_CONF, ST_CFG_SENSOR_DISABLE, sizeof ( uint8 ));
      }
    }
      return (events ^ MWH_HUMI_READ_EVT);
  }
  
  
  if ( events & MWH_BATTERY_CHECK_EVT )
  {
    // Restart timer
    if ( BATTERY_CHECK_PERIOD )
    {
      osal_start_timerEx( HeimDallr_TaskID, MWH_BATTERY_CHECK_EVT, BATTERY_CHECK_PERIOD );
    }

    // perform battery level check
    Batt_MeasLevel();

    return (events ^ MWH_BATTERY_CHECK_EVT);
  } 

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      HeimDallr_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void HeimDallr_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{ 
  switch ( pMsg->event )
  {
     case SERIAL_MSG:
      WatcherHeimdallr_HandleSerial( (mtOSALSerialData_t *)pMsg );
      break;
  }
}

static void WatcherHeimdallr_HandleSerial(mtOSALSerialData_t *cmdMsg)
{
  uint8 i,len,*str=NULL;  //len鏈夌敤鏁版嵁闀垮害
  str=cmdMsg->msg;        //鎸囧悜鏁版嵁寮�ご
  len=*str;               //msg閲岀殑绗�涓瓧鑺備唬琛ㄥ悗闈㈢殑鏁版嵁闀垮害
#if defined(DEBUG)
  performTest();
#endif
  /********鎵撳嵃鍑轰覆鍙ｆ帴鏀跺埌鐨勬暟鎹紝鐢ㄤ簬鎻愮ず*********/
  for(i=1;i<=len;i++)
    HalUARTWrite(0,str+i,1 ); 
  HalUARTWrite(0,"\r\n",2); 
}

/*********************************************************************
 * @fn      peripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateNotificationCB( gaprole_States_t newState )
{
  uint16 connHandle = INVALID_CONNHANDLE;
  uint8 valFalse = FALSE;

  if ( gapProfileState != newState )
  {
    switch( newState )
    {
    case GAPROLE_STARTED:
      {
        // Set the system ID from the bd addr
        uint8 systemId[DEVINFO_SYSTEM_ID_LEN];
        GAPRole_GetParameter(GAPROLE_BD_ADDR, systemId);

        // shift three bytes up
        systemId[7] = systemId[5];
        systemId[6] = systemId[4];
        systemId[5] = systemId[3];

        // set middle bytes to zero
        systemId[4] = 0;
        systemId[3] = 0;

        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
      }
      break;

    //if the state changed to connected, initially assume that keyfob is in range
    case GAPROLE_ADVERTISING:
      {
        // Visual feedback that we are advertising.
        //HalLedSet( HAL_LED_2, HAL_LED_MODE_ON );
      }
      break;
      
    //if the state changed to connected, initially assume that keyfob is in range      
    case GAPROLE_CONNECTED:
      {
        // set the proximity state to either path loss alert or in range depending
        // on the value of keyfobProxIMAlertLevel (which was set by proximity monitor)
        GAPRole_GetParameter( GAPROLE_CONNHANDLE, &connHandle );
   
        // Turn off LED that shows we're advertising
        //HalLedSet( HAL_LED_2, HAL_LED_MODE_OFF );
      }
      break;

    case GAPROLE_WAITING:
      {
        // then the link was terminated intentionally by the slave or master,
        // or advertising timed out
        
        // Change attribute value of Accelerometer Enable to FALSE
 //       Accel_SetParameter(ACCEL_ENABLER, sizeof(valFalse), &valFalse);
        // Stop the acceleromter
  //      accelEnablerChangeCB(); // SetParameter does not trigger the callback
        
      }
      break;
 
    case GAPROLE_WAITING_AFTER_TIMEOUT:
      {
        // the link was dropped due to supervision timeout

        // Change attribute value of Accelerometer Enable to FALSE
 //       Accel_SetParameter(ACCEL_ENABLER, sizeof(valFalse), &valFalse);
        // Stop the acceleromter
   //     accelEnablerChangeCB(); // SetParameter does not trigger the callback
        
      }
      break;

    default:
      // do nothing
      break;
    }
  } 
  gapProfileState = newState;
}

static void humiChangeCB(uint8 id)
{
  bStatus_t status = Humi_GetParameter( SENSOR_CONF, &humiEnabler );
   
  uprint("humiChangeCB.");
  if (status == SUCCESS){ 
    if (humiEnabler != 0x00)
    { 
      // Initialize accelerometer
      SHT_Init();
      // Setup timer for accelerometer task
      osal_start_timerEx( HeimDallr_TaskID, MWH_HUMI_READ_EVT, HUMI_READ_PERIOD );
    } 
    else {
      // Stop the acceleromter
      osal_stop_timerEx( HeimDallr_TaskID, MWH_HUMI_READ_EVT);
    }
  } else
  {
    //??
  }
}

/*********************************************************************
 * @fn      readHumData
 *
 * @brief   Read humidity data
 *
 * @param   none
 *
 * @return  none
 */
static void readHumData(void)
{
  uint8 hData[HUMIDITY_DATA_LEN];

  if (SHT_ReadMeasurement(hData))
  {
    //paramCallback cb = {Humi_SetParameter,Humi_AsmParameter,SENSOR_DATA,HUMIDITY_DATA_LEN,data_humi_all_buf ,hData};
    //transferData(gapProfileState,&cb,flash_asm_flag,&ec);
  }
}

 

/*********************************************************************
 * @fn      resetCharacteristicValue
 *
 * @brief   Initialize a characteristic value to zero
 *
 * @param   servID - service ID (UUID)
 *
 * @param   paramID - parameter ID of the value is to be cleared
 *
 * @param   vakue - value to initialise with
 *
 * @param   paramLen - length of the parameter
 *
 * @return  none
 */
static void resetCharacteristicValue(uint16 servUuid, uint8 paramID, uint8 value, uint8 paramLen)
{
  uint8* pData = osal_mem_alloc(paramLen);

  if (pData == NULL)
  {
    return;
  }

  osal_memset(pData,value,paramLen);

  switch(servUuid)
  {
  
    case HUMI_SERV_UUID:
      Humi_SetParameter( paramID, paramLen, pData);
      break;

  
    default:
      // Should not get here
      break;
  }

  osal_mem_free(pData);
}


/*********************************************************************
 * @fn      resetCharacteristicValues
 *
 * @brief   Initialize all the characteristic values
 *
 * @return  none
 */
static void resetCharacteristicValues( void )
{
  

  resetCharacteristicValue( HUMI_SERV_UUID, SENSOR_DATA, 0, HUMIDITY_DATA_LEN);
  resetCharacteristicValue( HUMI_SERV_UUID, SENSOR_CONF, ST_CFG_SENSOR_DISABLE, sizeof ( uint8 ));
  resetCharacteristicValue( HUMI_SERV_UUID, SENSOR_PERI, HUMI_DEFAULT_PERIOD , sizeof ( uint8 ));\
    
  
}

static void performTest()
{
    //  HAL_ADC_RESOLUTION_14   8191
    //  HAL_ADC_RESOLUTION_12   2047
#if defined( HAL_ADC_TEST )
    HalAdcSetReference(HAL_ADC_REF_AVDD);
    uint16 adc_vol=HalAdcRead(HAL_ADC_CHN_AIN0,HAL_ADC_RESOLUTION_14);
    uint16 vol = (uint16)(2.8 * adc_vol / 8191 * 100);
    uint8 vols[8];
    ValueToString(vol,vols);
    #if defined( DEBUG )
        HalUARTWrite(0,vols,8);
    #endif
#endif
#if defined( HAL_TEMP_TEST )
    //According to CC2541 Page 10 Analog Temperature Sensor    
    TR0 = 0x01;
    ATEST = 0x01;
    HalAdcSetReference(HAL_ADC_REF_125V);
    uint16 adc_temp=HalAdcRead(HAL_ADC_CHN_TEMP,HAL_ADC_RESOLUTION_12);
    // actual temperature = (ADC_TEMP - (1480 - 4.5*25))/4.5 = (ADC_TEMP - 1367.5)/4.5
    uint16 tmp = (uint16)(100*(adc_temp - 1367.5)/4.5);                         //(uint16)(1.24 * adc_temp / 8191 * 100);
    uint8 temp[8];
    ValueToString(tmp,temp);

    #if defined( DEBUG )
        HalUARTWrite(0,temp,8);
    #endif

#endif
    
#if defined( HAL_I2C_TEST )
    uint8 charValue5[5] = { 0,0,0,0,0 };
   // AccInit();
    charValue5[0]=AccGetXAxis();
    charValue5[1]=AccGetYAxis();
    charValue5[2]=AccGetZAxis();
    charValue5[3]=AccGetTilt();
    charValue5[4]=AccGetOPSTAT();
    //AccStop();
#if !defined( DEBUG )    
    HalUARTWrite(0,charValue5,5);
#endif
#endif
    
    
}

static void ValueToString(uint16 Data ,uint8 *Buff)
{
  Buff[0] = (Data/10000)+0x30; 
  Buff[1] = ((Data/1000)%10)+0x30; 
  Buff[2] = ((Data/100)%10)+0x30; 
  Buff[3] = '.'; 
  Buff[4] = ((Data/10)%10)+0x30; 
  Buff[5] = (Data%10)+0x30; 
  Buff[6] = '\n'; 
  Buff[7] = '\0';
}

void Init_Watchdog(void) 
{ 
  WDCTL  = 0x00;         //杩欐槸蹇呴』鐨勶紝鎵撳紑IDLE鎵嶈兘璁剧疆鐪嬮棬鐙�
  WDCTL |= 0x08;         //鏃堕棿闂撮殧涓�锛岀湅闂ㄧ嫍妯″紡
}

void FeedDog(void) 
{ 
  WDCTL = 0xa0;
  WDCTL = 0x50; 
}


//T3 Interrupt Done
HAL_ISR_FUNCTION(TIMER3_OVF_ISR,T3_VECTOR)
{
  HAL_ENTER_ISR();
  T3IF = 0;
  P1DIR |= 0x80;
  P1SEL &= ~0x80;
  P1_7 = 0;
  if(0x01 & T3OVFIF)
  {
    HalUARTWrite(0,"T3_OVF_ISR\r\n",osal_strlen("T3_OVF_ISR\r\n"));
  }
  if(0x01 & T3CH0IF)
  {
    HalUARTWrite(0,"T3_ISR\r\n",osal_strlen("T3_ISR\r\n"));
  }
  
  HAL_EXIT_ISR();
  return;
}

//LIS3DH INT1/INT2
HAL_ISR_FUNCTION(LIS3DH_PORT1_ISR,P1INT_VECTOR)
{
  HAL_ENTER_ISR();
  P1IF = 0x00;
  if(P1IFG & 0x02)      //Port1.1       LIS3DH INT1
  {
    // we stop the Port1.1 INT 
    //uint8 dir6d=0x00;
    //LIS3DH_ReadReg(LIS3DH_INT1_SRC, &dir6d);
      //LIS3DH_Get6DPosition(&dir6d);
    //uprint("=");
    //uprintEx(&dir6d,1);
    //uprint("*");
  }
  
  if(P1IFG & 0x04)      //Port1.2       LIS3DH INT2
  {
    P1IEN &= ~0x04; 
    P1IF = 0x00;
    static uint8 click;
  //  LIS3DH_GetClickResponse(&click);
//    paramCallback cb = {Accel_SetParameter,Accel_AsmParameter,ACCEL_CLICK_ATTR, 1, &data_accel_6d_buf,&click};
//    transferData(gapProfileState,&cb,flash_asm_flag,&ec);
    uprint("=C=");
    uprintEx(&click,1);
    P1IEN |= 0x04; 
  }
  HAL_EXIT_ISR();
  return;
}
