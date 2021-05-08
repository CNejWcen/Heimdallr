/**************************************************************************************************
  Filename:       heimdallr.c
  Revised:        $Date: 2013-08-15 15:28:40 -0700 (Thu, 15 Aug 2013) $
  Revision:       $Revision: 34986 $

  Description:    Watcher Heimdallr Application.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"

#include "OnBoard.h"
#include "hal_adc.h"


#if defined ( ACC_BMA250 )
#include "bma250.h"
#endif

#include "gatt.h"

#include "hci.h"

#include "gapgattserver.h"
#include "gattservapp.h"
#include "gatt_profile_uuid.h"

#if defined ( PLUS_BROADCASTER )
  #include "peripheralBroadcaster.h"
#else
  #include "peripheral.h"
#endif

#include "gapbondmgr.h"

#include "devinfoservice.h"
#include "proxreporter.h"
#include "battservice.h"
#include "accelerometer.h" 

#include "osal_snv.h"
#include "OSAL_Clock.h"

#include "heimdallr.h"
#include "hal_uart.h"
#include "AccI2C.h"
//#include "PWM.h"

   
#include "SensorProc.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Delay between power-up and starting advertising (in ms)
#define STARTDELAY                            500

// How often to check battery voltage (in ms)
#define BATTERY_CHECK_PERIOD                  10000

#define RESTART_SCAN_PERIOD                   20000
// How often (in ms) to read the accelerometer
#define ACCEL_READ_PERIOD                     50

// Minimum change in accelerometer before sending a notification
#define ACCEL_CHANGE_THRESHOLD                5

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

// keyfobProximityState values
#define KEYFOB_PROXSTATE_INITIALIZED          0   // Advertising after initialization or due to terminated link
#define KEYFOB_PROXSTATE_CONNECTED_IN_RANGE   1   // Connected and "within range" of the master, as defined by
                                                  // proximity profile
#define KEYFOB_PROXSTATE_PATH_LOSS            2   // Connected and "out of range" of the master, as defined by
                                                  // proximity profile
#define KEYFOB_PROXSTATE_LINK_LOSS            3   // Disconnected as a result of a supervision timeout

// Company Identifier: Texas Instruments Inc. (13)
#define TI_COMPANY_ID                         0x000D

#define INVALID_CONNHANDLE                    0xFFFF

#if defined ( PLUS_BROADCASTER )
  #define ADV_IN_CONN_WAIT                    500 // delay 500 ms
#endif

//#define DEBUG 

#define HAL_TEMP_TEST 
#define HAL_ADC_TEST
#define HAL_I2C_TEST
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

// Proximity State Variables
static uint8 keyfobProxLLAlertLevel = PP_ALERT_LEVEL_NO;     // Link Loss Alert
static uint8 keyfobProxIMAlertLevel = PP_ALERT_LEVEL_NO;     // Link Loss Alert
static int8  keyfobProxTxPwrLevel = 0;  // Tx Power Level (0dBm default)

// keyfobProximityState is the current state of the device
static uint8 keyfobProximityState;

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
  0x07,   // length of second data structure (7 bytes excluding length byte)
  GAP_ADTYPE_16BIT_MORE,   // list of 16-bit UUID's available, but not complete list
  LO_UINT16( LINK_LOSS_SERV_UUID ),        // Link Loss Service (Proximity Profile)
  HI_UINT16( LINK_LOSS_SERV_UUID ),
  LO_UINT16( IMMEDIATE_ALERT_SERV_UUID ),  // Immediate Alert Service (Proximity / Find Me Profile)
  HI_UINT16( IMMEDIATE_ALERT_SERV_UUID ),
  LO_UINT16( TX_PWR_LEVEL_SERV_UUID ),     // Tx Power Level Service (Proximity Profile)
  HI_UINT16( TX_PWR_LEVEL_SERV_UUID )
};

// GAP GATT Attributes
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "TI BLE Keyfob";
 

// Accelerometer Profile Parameters
static uint8 accelEnabler = FALSE;
static uint8 flag_sim800 = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void HeimDallr_ProcessOSALMsg( osal_event_hdr_t *pMsg );
static void peripheralStateNotificationCB( gaprole_States_t newState );
static void proximityAttrCB( uint8 attrParamID );
static void accelEnablerChangeCB( void );
static void accelRead( void );


static void WatcherHeimdallr_HandleSerial(mtOSALSerialData_t *cmdMsg);
static void ConvertUint32ToUint8(uint32 value,uint8 *result);
static void ConvertUint16ToUint8(uint32 value,uint8 *result);
//WatchDog
void Init_Watchdog(void);
void FeedDog(void);
//Test
static void performTest();
static void ValueToString(uint16 Data ,uint8 *Buff);
static void UartATCom(uint8 *command);
void hex2str( uint8 *pBuf,char *str,uint8 buflen);
void displayTime(void);
/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t keyFob_PeripheralCBs =
{
  peripheralStateNotificationCB,  // Profile State Change Callbacks
  NULL                // When a valid RSSI is read from controller
};

// GAP Bond Manager Callbacks
static gapBondCBs_t keyFob_BondMgrCBs =
{
  NULL,                     // Passcode callback (not used by application)
  NULL                      // Pairing / Bonding state Callback (not used by application)
};

// Proximity Peripheral Profile Callbacks
static proxReporterCBs_t keyFob_ProximityCBs =
{
  proximityAttrCB,              // Whenever the Link Loss Alert attribute changes
};

// Accelerometer Profile Callbacks
static accelCBs_t keyFob_AccelCBs =
{
  accelEnablerChangeCB,          // Called when Enabler attribute changes
};

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
  SensorPOST();                                                         //Power on self test
  HeimDallr_TaskID = task_id;
  
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
    uint8 pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
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
  ProxReporter_AddService( GATT_ALL_SERVICES );  // Proximity Reporter Profile
  Batt_AddService( );     // Battery Service
  Accel_AddService( GATT_ALL_SERVICES );      // Accelerometer Profile

  keyfobProximityState = KEYFOB_PROXSTATE_INITIALIZED;

  // Initialize Tx Power Level characteristic in Proximity Reporter
  {
    int8 initialTxPowerLevel = 0;
    ProxReporter_SetParameter( PP_TX_POWER_LEVEL, sizeof ( int8 ), &initialTxPowerLevel );
  }

//Init Uart use UART1 on port1 
  UartInit();
  RegisterForSerial(HeimDallr_TaskID);

#if defined( DEBUG )
  HalUARTWrite(0,"FirstBegining\r\n",osal_strlen("FirstBegining\r\n"));
#endif
  HalUARTWrite(0,"FirstBegining\r\n",osal_strlen("FirstBegining\r\n"));
  // makes sure LEDs are off
  //HalLedSet( (HAL_LED_1 | HAL_LED_2), HAL_LED_MODE_OFF );

  // For keyfob board set GPIO pins into a power-optimized state
  // Note that there is still some leakage current from the buzzer,
  // accelerometer, LEDs, and buttons on the PCB.
/*
  P0SEL = 0; // Configure Port 0 as GPIO
  P1SEL = 0x40; // Configure Port 1 as GPIO, except P1.6 for peripheral function for buzzer
  P2SEL = 0; // Configure Port 2 as GPIO

  P0DIR = 0xFC; // Port 0 pins P0.0 and P0.1 as input (buttons),
                // all others (P0.2-P0.7) as output
  P1DIR = 0xFF; // All port 1 pins (P1.0-P1.7) as output
  P2DIR = 0x1F; // All port 1 pins (P2.0-P2.4) as output

  P0 = 0x03; // All pins on port 0 to low except for P0.0 and P0.1 (buttons)
  P1 = 0;   // All pins on port 1 to low
  P2 = 0;   // All pins on port 2 to low
*/

  //P1SEL = 0x41;
  //P1DIR = 0xFF;
  // P1 = 0;
  
  //P0SEL = 0x80;
  // P0DIR = 0xFF;
  // P0 = 0;

 // pwmStop();
 // pwmStart(TIMER3_MARK,250);//100us     249.995Hz               P1_6//
//pwmStart(TIMER4_MARK,250);//2ms       5.66kHz                 P1_0        
//pwmStart(TIMER1_MARK,0);
  // initialize the ADC for battery reads
  HalAdcInit();
  /*
  uint32 testData = 0x43434343;
  
  osal_snv_write(HNV_ANY_TEST_DATA,sizeof(uint32),&testData);
  
  uint8 testRead;
  osal_snv_read(HNV_ANY_TEST_DATA,sizeof(uint32),&testRead);

#if defined( DEBUG )
  HalUARTWrite(0,&testRead,sizeof(uint8));
#endif
*/
#if defined(CLOCK_TEST)   
  //Init Time
  UTCTimeStruct utc;
  utc.year = 2014;
  utc.month = 3;
  utc.day =13;
  utc.hour = 21;
  utc.minutes = 55;
  utc.seconds = 40;
  osal_setClock(osal_ConvertUTCSecs(&utc));


  UTCTime tt = osal_getClock();
  UTCTimeStruct utctime;
  osal_ConvertUTCTime(&utctime,tt);
  uint8 result_tt[4];
  ConvertUint32ToUint8(tt,result_tt); 


  char str[10];
  hex2str(result_tt,str,4);
  HalUARTWrite(0,str,10);
  HalUARTWrite(0,"\r\n",2);
  /*
  typedef struct
{
  uint8 seconds;  // 0-59
  uint8 minutes;  // 0-59
  uint8 hour;     // 0-23
  uint8 day;      // 0-30
  uint8 month;    // 0-11
  uint16 year;    // 2000+
} UTCTimeStruct;

  utctime.year = 2014;
  utctime.month = 10;
  utctime.day =5;
  utctime.hour = 12;
  utctime.minutes = 33;
  utctime.seconds = 11;
  */
  displayTime();
#endif

#if defined ( DC_DC_P0_7 )
  // Enable stack to toggle bypass control on TPS62730 (DC/DC converter)
  HCI_EXT_MapPmIoPortCmd( HCI_EXT_PM_IO_PORT_P0, HCI_EXT_PM_IO_PORT_PIN7 );
#endif // defined ( DC_DC_P0_7 )

    
  // Setup a delayed profile startup
  osal_start_timerEx( HeimDallr_TaskID, MWH_START_DEVICE_EVT, STARTDELAY );
  osal_start_timerEx( HeimDallr_TaskID, MWH_RESTART_SCAN_DEVICE, RESTART_SCAN_PERIOD );
 
  P1SEL &= ~0x30;
  P1DIR |= 0x30;
  P1_4 = 1;
  P1_5 = 1;
  P2SEL &= ~0x01;
  P2DIR |= 0x01;
  P2_0 = 0;       //红灯      亮      
  osal_start_timerEx( HeimDallr_TaskID, MWH_START_UP_SIM800L0, 4000 );
 
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
  HalUARTWrite(0,".",1);
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
    VOID GAPRole_StartDevice( &keyFob_PeripheralCBs );

    // Start Bond Manager
    VOID GAPBondMgr_Register( &keyFob_BondMgrCBs );

    // Start the Proximity Profile
    VOID ProxReporter_RegisterAppCBs( &keyFob_ProximityCBs );

    // Set timer for first battery read event
    osal_start_timerEx( HeimDallr_TaskID, MWH_BATTERY_CHECK_EVT, BATTERY_CHECK_PERIOD );

    // Start the Accelerometer Profile
    VOID Accel_RegisterAppCBs( &keyFob_AccelCBs );

    //Set the proximity attribute values to default
    ProxReporter_SetParameter( PP_LINK_LOSS_ALERT_LEVEL,  sizeof ( uint8 ), &keyfobProxLLAlertLevel );
    ProxReporter_SetParameter( PP_IM_ALERT_LEVEL,  sizeof ( uint8 ), &keyfobProxIMAlertLevel );
    ProxReporter_SetParameter( PP_TX_POWER_LEVEL,  sizeof ( int8 ), &keyfobProxTxPwrLevel );

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
    
  if ( events & MWH_START_UP_SIM800L0)
  {
    HalUARTWrite(0,"21",2);
    P1_5 = 0;
    P2_0 = ~P2_0;
    osal_start_timerEx( HeimDallr_TaskID, MWH_START_UP_SIM800L0, 4000 );
    return ( events ^ MWH_START_UP_SIM800L0 );
  }
  
  if ( events & MWH_START_UP_SIM800L)
  {
    HalUARTWrite(0,"23",2);
    P1_5 = 1;
    return ( events ^ MWH_START_UP_SIM800L );
  }

  if ( events & MWH_ACCEL_READ_EVT )
  {
    
    bStatus_t status = Accel_GetParameter( ACCEL_ENABLER, &accelEnabler );

    if (status == SUCCESS)
    {
      if ( accelEnabler )
      {
        // Restart timer
        if ( ACCEL_READ_PERIOD )
        {
          osal_start_timerEx( HeimDallr_TaskID, MWH_ACCEL_READ_EVT, ACCEL_READ_PERIOD );
        }

        // Read accelerometer data
        accelRead();
      }
      else
      {
        // Stop the acceleromter
        osal_stop_timerEx( HeimDallr_TaskID, MWH_ACCEL_READ_EVT);
      }
    }
    else
    {
        //??
    }
    return (events ^ MWH_ACCEL_READ_EVT);
  }

  if ( events & MWH_BATTERY_CHECK_EVT )
  {
    // Restart timer
    if ( BATTERY_CHECK_PERIOD )
    {
      osal_start_timerEx( HeimDallr_TaskID, MWH_BATTERY_CHECK_EVT, BATTERY_CHECK_PERIOD );
    }

    // perform battery level check
    Batt_MeasLevel( );

    return (events ^ MWH_BATTERY_CHECK_EVT);
  } 
  
#if defined ( PLUS_BROADCASTER )
  if ( events & MWH_ADV_IN_CONNECTION_EVT )
  {
    uint8 turnOnAdv = TRUE;
    // Turn on advertising while in a connection
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &turnOnAdv );
  }
#endif

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
  uint8 i,len,*str=NULL;  //len有用数据长度
  str=cmdMsg->msg;        //指向数据开头
  len=*str;               //msg里的第1个字节代表后面的数据长度
#if defined(DEBUG)
  performTest();
#endif
  /********打印出串口接收到的数据，用于提示*********/
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

        #if defined ( PLUS_BROADCASTER )
          osal_start_timerEx( HeimDallr_TaskID, MWH_ADV_IN_CONNECTION_EVT, ADV_IN_CONN_WAIT );
        #endif
          
        // Turn off LED that shows we're advertising
        //HalLedSet( HAL_LED_2, HAL_LED_MODE_OFF );
      }
      break;

    case GAPROLE_WAITING:
      {
        // then the link was terminated intentionally by the slave or master,
        // or advertising timed out
        keyfobProximityState = KEYFOB_PROXSTATE_INITIALIZED;

        // Turn off immediate alert
        ProxReporter_SetParameter(PP_IM_ALERT_LEVEL, sizeof(valFalse), &valFalse);
        keyfobProxIMAlertLevel = PP_ALERT_LEVEL_NO;
        
        // Change attribute value of Accelerometer Enable to FALSE
        Accel_SetParameter(ACCEL_ENABLER, sizeof(valFalse), &valFalse);
        // Stop the acceleromter
        accelEnablerChangeCB(); // SetParameter does not trigger the callback
        
      }
      break;

    case GAPROLE_WAITING_AFTER_TIMEOUT:
      {
        // the link was dropped due to supervision timeout
        keyfobProximityState = KEYFOB_PROXSTATE_LINK_LOSS;

        // Turn off immediate alert
        ProxReporter_SetParameter(PP_IM_ALERT_LEVEL, sizeof(valFalse), &valFalse);
        keyfobProxIMAlertLevel = PP_ALERT_LEVEL_NO;
        
        // Change attribute value of Accelerometer Enable to FALSE
        Accel_SetParameter(ACCEL_ENABLER, sizeof(valFalse), &valFalse);
        // Stop the acceleromter
        accelEnablerChangeCB(); // SetParameter does not trigger the callback
        
      }
      break;

    default:
      // do nothing
      break;
    }
  }

  gapProfileState = newState;
}

/*********************************************************************
 * @fn      proximityAttrCB
 *
 * @brief   Notification from the profile of an atrribute change by
 *          a connected device.
 *
 * @param   attrParamID - Profile's Attribute Parameter ID
 *            PP_LINK_LOSS_ALERT_LEVEL  - The link loss alert level value
 *            PP_IM_ALERT_LEVEL  - The immediate alert level value
 *
 * @return  none
 */
static void proximityAttrCB( uint8 attrParamID )
{
  switch( attrParamID )
  {

  case PP_LINK_LOSS_ALERT_LEVEL:
    ProxReporter_GetParameter( PP_LINK_LOSS_ALERT_LEVEL, &keyfobProxLLAlertLevel );
    break;

  case PP_IM_ALERT_LEVEL:
    {
      ProxReporter_GetParameter( PP_IM_ALERT_LEVEL, &keyfobProxIMAlertLevel );

      // if proximity monitor set the immediate alert level to low or high, then
      // the monitor calculated that the path loss to the keyfob (proximity observer)
      // has exceeded the threshold
    }
    break;

  default:
    // should not reach here!
    break;
  }

}

/*********************************************************************
 * @fn      accelEnablerChangeCB
 *
 * @brief   Called by the Accelerometer Profile when the Enabler Attribute
 *          is changed.
 *
 * @param   none
 *
 * @return  none
 */
static void accelEnablerChangeCB( void )
{
  bStatus_t status = Accel_GetParameter( ACCEL_ENABLER, &accelEnabler );

  if (status == SUCCESS){
    if (accelEnabler)
    {
      // Initialize accelerometer
      //accInit();
      // Setup timer for accelerometer task
      osal_start_timerEx( HeimDallr_TaskID, MWH_ACCEL_READ_EVT, ACCEL_READ_PERIOD );
    } else
    {
      // Stop the acceleromter
      //accStop();
      osal_stop_timerEx( HeimDallr_TaskID, MWH_ACCEL_READ_EVT);
    }
  } else
  {
    //??
  }
}

/*********************************************************************
 * @fn      accelRead
 *
 * @brief   Called by the application to read accelerometer data
 *          and put data in accelerometer profile
 *
 * @param   none
 *
 * @return  none
 */
static void accelRead( void )
{

  static int8 x, y, z;
  int8 new_x, new_y, new_z;

  // Read data for each axis of the accelerometer
  //accReadAcc(&new_x, &new_y, &new_z);

  // Check if x-axis value has changed by more than the threshold value and
  // set profile parameter if it has (this will send a notification if enabled)
  if( (x < (new_x-ACCEL_CHANGE_THRESHOLD)) || (x > (new_x+ACCEL_CHANGE_THRESHOLD)) )
  {
    x = new_x;
    Accel_SetParameter(ACCEL_X_ATTR, sizeof ( int8 ), &x);
  }

  // Check if y-axis value has changed by more than the threshold value and
  // set profile parameter if it has (this will send a notification if enabled)
  if( (y < (new_y-ACCEL_CHANGE_THRESHOLD)) || (y > (new_y+ACCEL_CHANGE_THRESHOLD)) )
  {
    y = new_y;
    Accel_SetParameter(ACCEL_Y_ATTR, sizeof ( int8 ), &y);
  }

  // Check if z-axis value has changed by more than the threshold value and
  // set profile parameter if it has (this will send a notification if enabled)
  if( (z < (new_z-ACCEL_CHANGE_THRESHOLD)) || (z > (new_z+ACCEL_CHANGE_THRESHOLD)) )
  {
    z = new_z;
    Accel_SetParameter(ACCEL_Z_ATTR, sizeof ( int8 ), &z);
  }

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
  WDCTL  = 0x00;         //这是必须的，打开IDLE才能设置看门狗
  WDCTL |= 0x08;         //时间间隔一秒，看门狗模式  
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

static void ConvertUint32ToUint8(uint32 value,uint8 *result)
{ 
  result[0] = (value & 0x000000ff);
  result[1] = (value & 0x0000ff00) >> 8;
  result[2] = (value & 0x00ff0000) >> 16;
  result[3] = (value & 0xff000000) >> 24;
}

static void ConvertUint16ToUint8(uint32 value,uint8 *result)
{ 
  result[0]     =       value & 0xff;
  result[1]     =       (value >> 8);
}

/*
 *       将HEX值转化成相对应的HEX字符
 */
void hex2str( uint8 *pBuf, char *str, uint8 buflen)
{
  uint8       i;
  char        hex[] = "0123456789ABCDEF";
  char        *pStr = str;

  *pStr++ = '0';
  *pStr++ = 'x';

  // Start from end of addr
  pBuf += buflen;

  for ( i = buflen; i > 0; i-- )
  {
    *pStr++ = hex[*--pBuf >> 4];
    *pStr++ = hex[*pBuf & 0x0F];
  }

  *pStr = 0;
}
 

static void UartATCom(uint8 *command)
{
  HalUARTWrite(0,command,osal_strlen(command));
}


void displayTime(void)
{
  UTCTime tt = osal_getClock();
  UTCTimeStruct utctime;
  osal_ConvertUTCTime(&utctime,tt);
  
  uint8 yearstr[5];
  _itoa(utctime.year,yearstr,10);
  HalUARTWrite(0,yearstr,osal_strlen(yearstr));
  HalUARTWrite(0,"-",1);
  
  uint8 mounthstr[5];
  _itoa((uint16)utctime.month,mounthstr,10);
  HalUARTWrite(0,mounthstr,osal_strlen(mounthstr));
  HalUARTWrite(0,"-",1);
  
  uint8 daystr[5];
  _itoa((uint16)utctime.day,daystr,10);
  HalUARTWrite(0,daystr,osal_strlen(daystr));  
  HalUARTWrite(0," ",1);
  
  uint8 hourstr[5];
  _itoa((uint16)utctime.hour,hourstr,10);
  HalUARTWrite(0,hourstr,osal_strlen(hourstr));
  HalUARTWrite(0,":",1);
  
  uint8 minutestr[5];
  _itoa((uint16)utctime.minutes,minutestr,10);
  HalUARTWrite(0,minutestr,osal_strlen(minutestr));
  HalUARTWrite(0,":",1);
  
  uint8 secondstr[5];
  _itoa((uint16)utctime.seconds,secondstr,10);
  HalUARTWrite(0,secondstr,osal_strlen(secondstr));
  HalUARTWrite(0,"\r\n",2);
}
/*********************************************************************
*********************************************************************/
