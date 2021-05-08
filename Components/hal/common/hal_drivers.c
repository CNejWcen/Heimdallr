/**************************************************************************************************
  Filename:       hal_drivers.c
  Revised:        $Date: 2007-07-06 10:42:24 -0700 (Fri, 06 Jul 2007) $
  Revision:       $Revision: 13579 $

  Description:    This file contains the interface to the Drivers Service.


**************************************************************************************************/

/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/

#include "hal_adc.h"
#if (defined HAL_AES) && (HAL_AES == TRUE)
#include "hal_aes.h"
#endif

#if (defined HAL_DMA) && (HAL_DMA == TRUE)
#include "hal_dma.h"
#endif

#include "hal_drivers.h" 
#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_types.h"
#include "hal_uart.h"
#ifdef CC2591_COMPRESSION_WORKAROUND
#include "mac_rx.h"
#endif
#include "OSAL.h"
#if defined POWER_SAVING
#include "OSAL_PwrMgr.h"
#endif
#if (defined HAL_HID) && (HAL_HID == TRUE)
#include "usb_hid.h"
#endif
#if (defined HAL_SPI) && (HAL_SPI == TRUE)
#include "hal_spi.h"
#endif

/**************************************************************************************************
 *                                      GLOBAL VARIABLES
 **************************************************************************************************/
uint8 Hal_TaskID;

//void HalLedUpdate( void ); /* Notes: This for internal only so it shouldn't be in hal_led.h */

/**************************************************************************************************
 * @fn      Hal_Init
 *
 * @brief   Hal Initialization function.
 *
 * @param   task_id - Hal TaskId
 *
 * @return  None
 **************************************************************************************************/
void Hal_Init( uint8 task_id )
{
  /* Register task ID */
  Hal_TaskID = task_id;

#ifdef CC2591_COMPRESSION_WORKAROUND
  osal_start_reload_timer( Hal_TaskID, PERIOD_RSSI_RESET_EVT, PERIOD_RSSI_RESET_TIMEOUT );
#endif
}

/**************************************************************************************************
 * @fn      Hal_DriverInit
 *
 * @brief   Initialize HW - These need to be initialized before anyone.
 *
 * @param   task_id - Hal TaskId
 *
 * @return  None
 **************************************************************************************************/
void HalDriverInit (void)
{
  /* TIMER */
#if (defined HAL_TIMER) && (HAL_TIMER == TRUE)
#endif

  /* ADC */
#if (defined HAL_ADC) && (HAL_ADC == TRUE)
  HalAdcInit();
#endif

  /* DMA */
#if (defined HAL_DMA) && (HAL_DMA == TRUE)
  // Must be called before the init call to any module that uses DMA.
  HalDmaInit();
#endif

  /* AES */
#if (defined HAL_AES) && (HAL_AES == TRUE)
  HalAesInit();
#endif

  /* UART */
#if (defined HAL_UART) && (HAL_UART == TRUE)
  HalUARTInit();
#endif
  
  /* SPI */
#if (defined HAL_SPI) && (HAL_SPI == TRUE)
  HalSpiInit();
#endif

  /* HID */
#if (defined HAL_HID) && (HAL_HID == TRUE)
  usbHidInit();
#endif
}

void dbg(char *fmt,...)
{
  va_list ap;
  char string[255];
  va_start(ap,fmt);
  vsprintf(string,fmt,ap);
  uprint((uint8 *)string);
  va_end(ap);
}

void uprintEx(uint8 *buf,uint16 len)
{
    HalUARTWrite(0,buf,len);
}

void uprint(uint8 *buf)
{
  uprintEx(buf,osal_strlen(buf));
}
void LedBlink()
{
  WarnLED_OFF();
  osal_start_timerEx(Hal_TaskID, HAL_LED_BLINK_EVENT,LED_CYCLE_PERIOD);
}
void LedStop()
{
  osal_stop_timerEx(Hal_TaskID, HAL_LED_BLINK_EVENT);
  WarnLED_OFF();
}
/**************************************************************************************************
 * @fn      Hal_ProcessEvent
 *
 * @brief   Hal Process Event
 *
 * @param   task_id - Hal TaskId
 *          events - events
 *
 * @return  None
 **************************************************************************************************/
uint16 Hal_ProcessEvent( uint8 task_id, uint16 events )
{
  
  uint8 *msgPtr;

  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    msgPtr = osal_msg_receive(Hal_TaskID);

    while (msgPtr)
    {
      /* Do something here - for now, just deallocate the msg and move on */

      /* De-allocate */
      osal_msg_deallocate( msgPtr );
      /* Next */
      msgPtr = osal_msg_receive( Hal_TaskID );
    }
    return events ^ SYS_EVENT_MSG;
  }


#ifdef CC2591_COMPRESSION_WORKAROUND
  if ( events & PERIOD_RSSI_RESET_EVT )
  {
    macRxResetRssi();
    return (events ^ PERIOD_RSSI_RESET_EVT);
  }
#endif

  // LED Blink events
  if ( events & HAL_LED_BLINK_EVENT )
  {
    WarnLED_TOGGLE();
    osal_start_timerEx(Hal_TaskID, HAL_LED_BLINK_EVENT,LED_CYCLE_PERIOD);
    return events ^ HAL_LED_BLINK_EVENT;
  }
  
  
#if defined POWER_SAVING
  if ( events & HAL_SLEEP_TIMER_EVENT )
  {
    halRestoreSleepLevel();
    return events ^ HAL_SLEEP_TIMER_EVENT;
  }
  
  if ( events & HAL_PWRMGR_HOLD_EVENT )
  {
    (void)osal_pwrmgr_task_state(Hal_TaskID, PWRMGR_HOLD);

    (void)osal_stop_timerEx(Hal_TaskID, HAL_PWRMGR_CONSERVE_EVENT);
    (void)osal_clear_event(Hal_TaskID, HAL_PWRMGR_CONSERVE_EVENT);

    return (events & ~(HAL_PWRMGR_HOLD_EVENT | HAL_PWRMGR_CONSERVE_EVENT));
  }

  if ( events & HAL_PWRMGR_CONSERVE_EVENT )
  {
    (void)osal_pwrmgr_task_state(Hal_TaskID, PWRMGR_CONSERVE);
    return events ^ HAL_PWRMGR_CONSERVE_EVENT;
  }
#endif

  return 0;
}

/**************************************************************************************************
 * @fn      Hal_ProcessPoll
 *
 * @brief   This routine will be called by OSAL to poll UART, TIMER...
 *
 * @param   task_id - Hal TaskId
 *
 * @return  None
 **************************************************************************************************/
void Hal_ProcessPoll ()
{
#if defined( POWER_SAVING )
  /* Allow sleep before the next OSAL event loop */
  ALLOW_SLEEP_MODE();
#endif
  
  /* UART Poll */
#if (defined HAL_UART) && (HAL_UART == TRUE)
  HalUARTPoll();
#endif
  
  /* SPI Poll */
#if (defined HAL_SPI) && (HAL_SPI == TRUE)
  HalSpiPoll();
#endif

  /* HID poll */
#if (defined HAL_HID) && (HAL_HID == TRUE)
  usbHidProcessEvents();
#endif
 
}

/**************************************************************************************************
**************************************************************************************************/

