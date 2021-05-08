/**************************************************************************************************
  Filename:       hal_dma.c
  Revised:        $Date: 2012-07-18 12:44:06 -0700 (Wed, 18 Jul 2012) $
  Revision:       $Revision: 30952 $

  Description:    This file contains the interface to the DMA.


**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "hal_dma.h"
#include "hal_mcu.h"
#include "hal_types.h"
#include "hal_uart.h"

#if (defined HAL_IRGEN) && (HAL_IRGEN == TRUE)
#include "hal_irgen.h"
#endif

#if ((defined HAL_DMA) && (HAL_DMA == TRUE))

/*********************************************************************
 * GLOBAL VARIABLES
 */

halDMADesc_t dmaCh0;
halDMADesc_t dmaCh1234[4];

/******************************************************************************
 * @fn      HalDMAInit
 *
 * @brief   DMA Interrupt Service Routine
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
void HalDmaInit( void )
{
  HAL_DMA_SET_ADDR_DESC0( &dmaCh0 );
  HAL_DMA_SET_ADDR_DESC1234( dmaCh1234 );
#if (HAL_UART_DMA || \
   ((defined HAL_UART_SPI) && (HAL_UART_SPI != 0)) || \
   ((defined HAL_IRGEN) && (HAL_IRGEN == TRUE)))
  DMAIE = 1;
#endif
}

#if (HAL_UART_DMA || \
   ((defined HAL_UART_SPI) && (HAL_UART_SPI != 0)) || \
   ((defined HAL_IRGEN) && (HAL_IRGEN == TRUE)))
/******************************************************************************
 * @fn      HalDMAInit
 *
 * @brief   DMA Interrupt Service Routine
 *
 * @param   None
 *
 * @return  None
 *****************************************************************************/
HAL_ISR_FUNCTION( halDmaIsr, DMA_VECTOR )
{
  HAL_ENTER_ISR();

  DMAIF = 0;

#if (((defined HAL_UART_DMA) && (HAL_UART_DMA != 0)) || \
     ((defined HAL_UART_SPI) && (HAL_UART_SPI != 0)))
  if (HAL_DMA_CHECK_IRQ(HAL_DMA_CH_TX))
  {
    HAL_DMA_CLEAR_IRQ(HAL_DMA_CH_TX);
    extern void HalUARTIsrDMA(void);
    HalUARTIsrDMA();
  }
#endif

#if (defined HAL_IRGEN) && (HAL_IRGEN == TRUE)
  if ( HAL_IRGEN == TRUE && HAL_DMA_CHECK_IRQ( HAL_IRGEN_DMA_CH ) )
  {
    HAL_DMA_CLEAR_IRQ( HAL_IRGEN_DMA_CH );
    HalIrGenDmaIsr();
  }
#endif

  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();

  return;
}
#endif
#endif  // #if ((defined HAL_DMA) && (HAL_DMA == TRUE))

/******************************************************************************
******************************************************************************/
