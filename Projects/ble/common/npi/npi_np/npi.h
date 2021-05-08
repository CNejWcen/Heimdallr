/*******************************************************************************
  Filename:       npi.h
  Revised:        $Date: 2007-10-28 09:35:41 -0700 (Sun, 28 Oct 2007) $
  Revision:       $Revision: 15796 $

  Description:    This file contains the Network Processor Interface (NPI),
                  which abstracts the physical link between the Application
                  Processor (AP) and the Network Processor (NP). The NPI
                  serves as the HAL's client for the SPI and UART drivers, and
                  provides API and callback services for its client.
 
*******************************************************************************/

#ifndef NPI_H
#define NPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include "hal_board.h"
#include "hal_uart.h"

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/* UART port */
#if !defined NPI_UART_PORT

#if ((defined HAL_UART_SPI) && (HAL_UART_SPI != 0)) // FOR SPI
#if (HAL_UART_SPI == 2)  
#define NPI_UART_PORT                  HAL_UART_PORT_1
#else
#define NPI_UART_PORT                  HAL_UART_PORT_0
#endif
#else // FOR UART
#if ((defined HAL_UART_DMA) && (HAL_UART_DMA  == 1))
#define NPI_UART_PORT                  HAL_UART_PORT_0
#elif ((defined HAL_UART_DMA) && (HAL_UART_DMA  == 2))
#define NPI_UART_PORT                  HAL_UART_PORT_1
#else
#define NPI_UART_PORT                  HAL_UART_PORT_0
#endif
#endif // Endif for HAL_UART_SPI/DMA 
#endif //Endif for NPI_UART_PORT

#if !defined( NPI_UART_FC )
#define NPI_UART_FC                    TRUE
#endif // !NPI_UART_FC

#define NPI_UART_FC_THRESHOLD          48
#define NPI_UART_RX_BUF_SIZE           128
#define NPI_UART_TX_BUF_SIZE           128
#define NPI_UART_IDLE_TIMEOUT          6
#define NPI_UART_INT_ENABLE            TRUE

#if !defined( NPI_UART_BR )
#define NPI_UART_BR                    HAL_UART_BR_115200
#endif // !NPI_UART_BR

/*******************************************************************************
 * TYPEDEFS
 */

typedef void (*npiCBack_t) ( uint8 port, uint8 event );

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * FUNCTIONS
 */

//
// Network Processor Interface APIs
//

extern void   NPI_InitTransport( npiCBack_t npiCBack );
extern uint16 NPI_ReadTransport( uint8 *buf, uint16 len );
extern uint16 NPI_WriteTransport( uint8 *, uint16 );
extern uint16 NPI_RxBufLen( void );
extern uint16 NPI_GetMaxRxBufSize( void );
extern uint16 NPI_GetMaxTxBufSize( void );

/*******************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif /* NPI_H */
