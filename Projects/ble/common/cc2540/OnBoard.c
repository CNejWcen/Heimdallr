/**************************************************************************************************
  Filename:       OnBoard.c
  Revised:        $Date: 2008-03-18 15:14:17 -0700 (Tue, 18 Mar 2008) $
  Revision:       $Revision: 16604 $

  Description:    This file contains the UI and control for the
                  peripherals on the EVAL development board
  Notes:          This file targets the Chipcon CC2430DB/CC2430EB

**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "OnBoard.h"
#include "hal_drivers.h"
#include "OSAL.h"
#include "OnBoard.h"

#include "hal_uart.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Task ID not initialized
#define NO_TASK_ID 0xFF


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
uint8 OnboardKeyIntEnable;


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern uint8 LL_PseudoRand( uint8 *randData, uint8 dataLen );

#if   defined FEATURE_ABL
#include "..\..\util\ABL\app\sbl_app.c"
#elif defined FEATURE_SBL
#include "..\..\util\SBL\app\sbl_app.c"
#elif defined FEATURE_EBL
#include "..\..\util\EBL\app\sbl_app.c"
#elif defined FEATURE_UBL_MSD
#include "..\..\util\UBL\soc_8051\usb_msd\app\ubl_app.c"
#else
void appForceBoot(void);
#endif

/*********************************************************************
 * LOCAL VARIABLES
 */
   
static uint8 registeredSerialTaskID = NO_TASK_ID;
/*********************************************************************
 * @fn      InitBoard()
 * @brief   Initialize the CC2540DB Board Peripherals
 * @param   level: COLD,WARM,READY
 * @return  None
 */
void InitBoard( uint8 level )
{
  if ( level == OB_COLD )
  {
    // Interrupts off
    osal_int_disable( INTS_ALL );
    // Turn all LEDs off
    //HalLedSet( HAL_LED_ALL, HAL_LED_MODE_OFF );
    // Check for Brown-Out reset
//    ChkReset();
  }
  else  // !OB_COLD
  {
    /* Initialize Key stuff */
    //OnboardKeyIntEnable = HAL_KEY_INTERRUPT_ENABLE;
    //HalKeyConfig( OnboardKeyIntEnable, OnBoard_KeyCallback);
  }
}

/*********************************************************************
 * @fn        Onboard_rand
 *
 * @brief    Random number generator
 *
 * @param   none
 *
 * @return  uint16 - new random number
 *
 *********************************************************************/
uint16 Onboard_rand( void )
{
  uint16 randNum;

  LL_PseudoRand( (uint8 *)&randNum, 2 );

  return ( randNum );
}

/*********************************************************************
 * @fn      _itoa
 *
 * @brief   convert a 16bit number to ASCII
 *
 * @param   num -
 *          buf -
 *          radix -
 *
 * @return  void
 *
 *********************************************************************/
void _itoa(uint16 num, uint8 *buf, uint8 radix)
{
  char c,i;
  uint8 *p, rst[5];

  p = rst;
  for ( i=0; i<5; i++,p++ )
  {
    c = num % radix;  // Isolate a digit
    *p = c + (( c < 10 ) ? '0' : '7');  // Convert to Ascii
    num /= radix;
    if ( !num )
      break;
  }

  for ( c=0 ; c<=i; c++ )
    *buf++ = *p--;  // Reverse character order

  *buf = '\0';
}


/*********************************************************************
 * @fn      Onboard_soft_reset
 *
 * @brief   Effect a soft reset.
 *
 * @param   none
 *
 * @return  none
 *
 *********************************************************************/
__near_func void Onboard_soft_reset( void )
{
  HAL_DISABLE_INTERRUPTS();
  asm("LJMP 0x0");
}

#if   defined FEATURE_ABL
#elif defined FEATURE_SBL
#elif defined FEATURE_EBL
#elif defined FEATURE_UBL_MSD
#else
/*********************************************************************
 * @fn      appForceBoot
 *
 * @brief   Common force-boot function for the HCI library to invoke.
 *
 * @param   none
 *
 * @return  void
 *********************************************************************/
void appForceBoot(void)
{
  // Dummy function for HCI library that cannot depend on the SBL build defines.
}
#endif



//UART
uint8 RegisterForSerial( uint8 task_id )
{
  // Allow only the first task
  if ( registeredSerialTaskID == NO_TASK_ID )
  {
    registeredSerialTaskID = task_id;
    return ( true );
  }
  else
    return ( false );
}

//串口回调函数
static void SerialCallback( uint8 port, uint8 event )
{   
  mtOSALSerialData_t  *pMsgSerial; 
  uint8 flag=0,i,j=0;   //flag是判断有没有收到数据，j记录数据长度 
  uint8 buf[128];       //串口buffer最大缓冲默认是128，我们这里用128. 
  (void)event;           // Intentionally unreferenced parameter    
  while (Hal_UART_RxBufLen(port)) //检测串口数据是否接收完成 
  {
   HalUARTRead (port,&buf[j], 1);   //把数据接收放到buf中 
   j++;                                      //记录字符数 
   flag=1;                         //已经从串口接收到信息 
  }
 
  if(flag==1)        //已经从串口接收到信息 
  {     /* Allocate memory for the data */ 
    //分配内存空间，为机构体内容+数据内容+1个记录长度的数据 
    pMsgSerial = (mtOSALSerialData_t *)osal_msg_allocate( sizeof    
            ( mtOSALSerialData_t )+j+1); 
     
    pMsgSerial->hdr.event = SERIAL_MSG;        // 事件号用SERIAL_MSG,需要添加
    pMsgSerial->msg = (uint8*)(pMsgSerial+1);  // 把数据定位到结构体数据部分 

   pMsgSerial->msg [0]= j;         //给上层的数据第一个是长度 
   for(i=0;i<j;i++)                //从第二个开始记录数据   
     pMsgSerial->msg [i+1]= buf[i];      
   osal_msg_send( registeredSerialTaskID, (uint8 *)pMsgSerial );  //登记任务，发往上层 
    /* deallocate the msg */ 
   osal_msg_deallocate ( (uint8 *)pMsgSerial );            //释放内存 
  } 
}

void UartInit(void)
{
  halUARTCfg_t uartConfig;
  uartConfig.configured                 =       TRUE;
  uartConfig.baudRate                   =       HAL_UART_BR_115200;
  uartConfig.flowControl                =       HAL_UART_FLOW_OFF;
  uartConfig.flowControlThreshold       =       MT_UART_THRESHOLD;
  uartConfig.rx.maxBufSize              =       MT_UART_RX_BUFF_MAX;
  uartConfig.tx.maxBufSize              =       MT_UART_TX_BUFF_MAX;
  uartConfig.idleTimeout                =       MT_UART_IDLE_TIMEOUT;
  uartConfig.intEnable                  =       TRUE;
  
  uartConfig.callBackFunc               =       SerialCallback;
 
  HalUARTOpen (HAL_UART_PORT_0, &uartConfig);
}
/*********************************************************************
*********************************************************************/
