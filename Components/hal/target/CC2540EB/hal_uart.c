/**************************************************************************************************
  Filename:     hal_uart.c
  Revised:      $Date: 2013-02-06 09:21:21 -0800 (Wed, 06 Feb 2013) $
  Revision:     $Revision: 33001 $

  Description:  This file contains the interface to the H/W UART driver.

**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "hal_board_cfg.h"
#include "hal_defs.h"
#include "hal_drivers.h"
#include "hal_types.h"
#include "hal_uart.h"
#if defined POWER_SAVING
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#endif

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

void HalUARTIsrDMA(void);

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#if HAL_UART_DMA
#include "_hal_uart_dma.c"
#endif
#if HAL_UART_ISR
#include "_hal_uart_isr.c"
#endif
#if HAL_UART_SPI
#include "_hal_uart_spi.c"
#endif
#if HAL_UART_USB
#include "_hal_uart_usb.c"
#endif

/******************************************************************************
 * @fn      HalUARTInit
 *
 * @brief   Initialize the UART
 *
 * @param   none
 *
 * @return  none
 *****************************************************************************/
void HalUARTInit(void)
{
#if (HAL_UART_DMA && HAL_UART_SPI)  // When both are defined, port is run-time choice.
  if (HAL_UART_PORT)
  {
    HalUARTInitSPI();
  }
  else
  {
    HalUARTInitDMA();
  }
#else
#if HAL_UART_DMA
  HalUARTInitDMA();
#endif
#if HAL_UART_ISR
  HalUARTInitISR();
#endif
#if HAL_UART_SPI
  HalUARTInitSPI();
#endif
#if HAL_UART_USB
  HalUARTInitUSB();
#endif
#endif
}

/******************************************************************************
 * @fn      HalUARTOpen
 *
 * @brief   Open a port according tp the configuration specified by parameter.
 *
 * @param   port   - UART port
 *          config - contains configuration information
 *
 * @return  Status of the function call
 *****************************************************************************/
uint8 HalUARTOpen(uint8 port, halUARTCfg_t *config)
{
#if (HAL_UART_DMA == 1)
  if (port == HAL_UART_PORT_0)  HalUARTOpenDMA(config);
#endif
#if (HAL_UART_DMA == 2)
  if (port == HAL_UART_PORT_1)  HalUARTOpenDMA(config);
#endif
#if (HAL_UART_ISR == 1)
  if (port == HAL_UART_PORT_0)  HalUARTOpenISR(config);
#endif
#if (HAL_UART_ISR == 2)
  if (port == HAL_UART_PORT_1)  HalUARTOpenISR(config);
#endif
#if (HAL_UART_SPI == 1)
  if (port == HAL_UART_PORT_0)  HalUARTOpenSPI(config);
#endif
#if (HAL_UART_SPI == 2)
  if (port == HAL_UART_PORT_1)  HalUARTOpenSPI(config);
#endif
#if (HAL_UART_USB)
  HalUARTOpenUSB(config);
#endif
#if (HAL_UART_DMA == 0) && (HAL_UART_ISR == 0) && (HAL_UART_SPI == 0)
  // UART is not enabled. Do nothing.
  (void) port;   // unused argument
  (void) config; // unused argument
#endif

  return HAL_UART_SUCCESS;
}

/*****************************************************************************
 * @fn      HalUARTRead
 *
 * @brief   Read a buffer from the UART
 *
 * @param   port - USART module designation
 *          buf  - valid data buffer at least 'len' bytes in size
 *          len  - max length number of bytes to copy to 'buf'
 *
 * @return  length of buffer that was read
 *****************************************************************************/
uint16 HalUARTRead(uint8 port, uint8 *buf, uint16 len)
{
#if (HAL_UART_DMA == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTReadDMA(buf, len);
#endif
#if (HAL_UART_DMA == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTReadDMA(buf, len);
#endif
#if (HAL_UART_ISR == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTReadISR(buf, len);
#endif
#if (HAL_UART_ISR == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTReadISR(buf, len);
#endif
#if (HAL_UART_SPI == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTReadSPI(buf, len);
#endif
#if (HAL_UART_SPI == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTReadSPI(buf, len);
#endif

#if HAL_UART_USB
  return HalUARTRx(buf, len);
#else
  #if (HAL_UART_DMA == 0) && (HAL_UART_ISR == 0) && (HAL_UART_SPI == 0)
    // UART is not enabled. Do nothing.
    (void) port;   // unused argument
    (void) buf;   // unused argument
    (void) len;   // unused argument
  #endif
  return 0;
#endif
}

/******************************************************************************
 * @fn      HalUARTWrite
 *
 * @brief   Write a buffer to the UART.
 *
 * @param   port - UART port
 *          buf  - pointer to the buffer that will be written, not freed
 *          len  - length of
 *
 * @return  length of the buffer that was sent
 *****************************************************************************/
uint16 HalUARTWrite(uint8 port, uint8 *buf, uint16 len)
{
#if (HAL_UART_DMA == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTWriteDMA(buf, len);
#endif
#if (HAL_UART_DMA == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTWriteDMA(buf, len);
#endif
#if (HAL_UART_ISR == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTWriteISR(buf, len);
#endif
#if (HAL_UART_ISR == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTWriteISR(buf, len);
#endif
#if (HAL_UART_SPI == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTWriteSPI(buf, len);
#endif
#if (HAL_UART_SPI == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTWriteSPI(buf, len);
#endif

#if HAL_UART_USB
  HalUARTTx(buf, len);
  return len;
#else
  #if (HAL_UART_DMA == 0) && (HAL_UART_ISR == 0) && (HAL_UART_SPI == 0)
    // UART is not enabled. Do nothing.
    (void) port;   // unused argument
    (void) buf;   // unused argument
    (void) len;   // unused argument
  #endif
  return 0;
#endif
}

/******************************************************************************
 * @fn      HalUARTSuspend
 *
 * @brief   Suspend UART hardware before entering PM mode 1, 2 or 3.
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
void HalUARTSuspend( void )
{
#if HAL_UART_ISR
  HalUARTSuspendISR();
#endif
}

/******************************************************************************
 * @fn      HalUARTResume
 *
 * @brief   Resume UART hardware after exiting PM mode 1, 2 or 3.
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
void HalUARTResume( void )
{
#if HAL_UART_ISR
  HalUARTResumeISR();
#endif
}

/***************************************************************************************************
 * @fn      HalUARTPoll
 *
 * @brief   Poll the UART.
 *
 * @param   none
 *
 * @return  none
 *****************************************************************************/
void HalUARTPoll(void)
{
#if (HAL_UART_DMA && HAL_UART_SPI)  // When both are defined, port is run-time choice.
  if (HAL_UART_PORT)
  {
    HalUARTPollSPI();
  }
  else
  {
    HalUARTPollDMA();
  }
#else
#if HAL_UART_DMA
  HalUARTPollDMA();
#endif
#if HAL_UART_ISR
  HalUARTPollISR();
#endif
#if HAL_UART_SPI
  HalUARTPollSPI();
#endif
#if HAL_UART_USB
  HalUARTPollUSB();
#endif
#endif
}

/**************************************************************************************************
 * @fn      Hal_UART_RxBufLen()
 *
 * @brief   Calculate Rx Buffer length - the number of bytes in the buffer.
 *
 * @param   port - UART port
 *
 * @return  length of current Rx Buffer
 **************************************************************************************************/
uint16 Hal_UART_RxBufLen( uint8 port )
{
#if (HAL_UART_DMA == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTRxAvailDMA();
#endif
#if (HAL_UART_DMA == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTRxAvailDMA();
#endif
#if (HAL_UART_ISR == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTRxAvailISR();
#endif
#if (HAL_UART_ISR == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTRxAvailISR();
#endif
#if (HAL_UART_SPI == 1)
  if (port == HAL_UART_PORT_0)  return HalUARTRxAvailSPI();
#endif
#if (HAL_UART_SPI == 2)
  if (port == HAL_UART_PORT_1)  return HalUARTRxAvailSPI();
#endif

#if HAL_UART_USB
  return HalUARTRxAvailUSB();
#else
  #if (HAL_UART_DMA == 0) && (HAL_UART_ISR == 0) && (HAL_UART_SPI == 0)
    // UART is not enabled. Do nothing.
    (void) port;   // unused argument
  #endif
  return 0;
#endif
}

void HalUARTIsrDMA(void)
{
#if (HAL_UART_DMA && HAL_UART_SPI)  // When both are defined, port is run-time choice.
  if (HAL_UART_PORT)
  {
    HalUART_DMAIsrSPI();
  }
  else
  {
    HalUART_DMAIsrDMA();
  }
#elif HAL_UART_DMA
  HalUART_DMAIsrDMA();
#elif HAL_UART_SPI
  HalUART_DMAIsrSPI();
#endif
}

/******************************************************************************
******************************************************************************/
