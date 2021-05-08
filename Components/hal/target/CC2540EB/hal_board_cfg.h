/*******************************************************************************
  Filename:       hal_board_cfg.h
  Revised:        $Date: 2013-09-12 11:31:38 -0700 (Thu, 12 Sep 2013) $
  Revision:       $Revision: 35293 $

  Description:

  Abstract board-specific registers, addresses, & initialization for H/W based on the
  Texas Instruments CC254x (8051 core).

*******************************************************************************/
#ifndef HAL_BOARD_CFG_H
#define HAL_BOARD_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * INCLUDES
 */

#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"

/*******************************************************************************
 * CONSTANTS
 */

/* Board Identifier */

#if !defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_BOARD_CC2530EB_REV13)
  #define HAL_BOARD_CC2530EB_REV17
#endif

/* Clock Speed */

#define HAL_CPU_CLOCK_MHZ             32

/* Sleep Clock */

#define EXTERNAL_CRYSTAL_OSC          0x00  // external 32kHz XOSC
#define INTERNAL_RC_OSC               0x80  // internal 32kHz RCOSC

// For non-USB, assume external, unless an internal crystal is explicitly indicated.
#if !defined (XOSC32K_INSTALLED) || (defined (XOSC32K_INSTALLED) && (XOSC32K_INSTALLED == TRUE))
#define OSC_32KHZ                     EXTERNAL_CRYSTAL_OSC
#else
#define OSC_32KHZ                     INTERNAL_RC_OSC
#endif

// Minimum Time for Stable External 32kHz Clock (in ms)
#define MIN_TIME_TO_STABLE_32KHZ_XOSC 400

  
  
/* DCDC control */
#define DCDC_BV           BV(2)
#define DCDC_SBIT         P1_2
#define DCDC_DDR          P1DIR
#define DCDC_SEL          P1SEL

  
/* Push Button Configuration */

#define ACTIVE_LOW                     !
#define ACTIVE_HIGH                    !!    /* double negation forces result to be '1' */

   
// Wait 1 [ms]
#define WAIT_1MS()      {for(unsigned short i=0;i<1000;i++) asm("NOP"); }

#define WAIT_10US()     {for(unsigned short i=0;i<10;i++) asm("NOP"); }
// Wait t [ms]
#define WAIT_MS(t)                      \
        for(unsigned short i = 0; i<t; i++)      \
            WAIT_1MS();
        
/*      LED          */   
#define BSP_LED_INIT() P1DIR |= 0x01;P1SEL &= ~0x01; \
                   P0DIR |= 0xC0;P0SEL &= ~0xC0;
#define LED_ON_STATE  0
#define LED_OFF_STATE 1
//Battery LED
#define BattLED_PIN    P1_0
#define BattLED_ON()   BattLED_PIN = LED_ON_STATE;
#define BattLED_OFF()  BattLED_PIN = LED_OFF_STATE;
#define BattLED_TOGGLE() BattLED_PIN = ~BattLED_PIN;
#define BattLED_FLASH()  BattLED_ON();                  \
                     WAIT_MS(500);              \
                     BattLED_OFF();                 \
                     WAIT_MS(500);
#define BattLED_FLASH_TWICE() BattLED_FLASH();BattLED_FLASH();
//Signal LED
#define SignalLED_PIN   P0_7
#define SignalLED_ON()  SignalLED_PIN = LED_ON_STATE;
#define SignalLED_OFF() SignalLED_PIN = LED_OFF_STATE;
#define SignalLED_TOGGLE()  SignalLED_PIN = ~SignalLED_PIN;
                      
#define SignalLED_FLASH()  SignalLED_ON();                  \
                     WAIT_MS(500);              \
                     SignalLED_OFF();                 \
                     WAIT_MS(500);
#define SignalLED_FLASH_TWICE()  SignalLED_FLASH();SignalLED_FLASH();
//Warning LED
#define WarnLED_PIN   P0_6
#define WarnLED_ON()  WarnLED_PIN = LED_ON_STATE;
#define WarnLED_OFF() WarnLED_PIN = LED_OFF_STATE;
#define WarnLED_TOGGLE()  WarnLED_PIN = ~WarnLED_PIN;
                      
#define WarnLED_FLASH()  WarnLED_ON();                  \
                     WAIT_MS(500);              \
                     WarnLED_OFF();                 \
                     WAIT_MS(500);
#define WarnLED_FLASH_TWICE()  WarnLED_FLASH();WarnLED_FLASH();

/* OSAL NV implemented by internal flash pages. */

// Flash is partitioned into 8 banks of 32 KB or 16 pages.
#define HAL_FLASH_PAGE_PER_BANK        16

// Flash is constructed of 128 pages of 2 KB.
#define HAL_FLASH_PAGE_PHYS            2048

// SNV can use a larger logical page size to accomodate more or bigger items or extend lifetime.
#define HAL_FLASH_PAGE_SIZE            HAL_FLASH_PAGE_PHYS
#define HAL_FLASH_WORD_SIZE            4

// CODE banks get mapped into the XDATA range 8000-FFFF.
#define HAL_FLASH_PAGE_MAP             0x8000

// The last 16 bytes of the last available page are reserved for flash lock bits.
#define HAL_FLASH_LOCK_BITS            16

// NV page definitions must coincide with segment declaration in project *.xcl file.
#if defined NON_BANKED
#define HAL_NV_PAGE_END                30
#elif defined HAL_BOARD_CC2541S
#define HAL_NV_PAGE_END                125
#elif defined HAL_BOARD_F128
#define HAL_NV_PAGE_END                62
#else
#define HAL_NV_PAGE_END                126
#endif

// Re-defining Z_EXTADDR_LEN here so as not to include a Z-Stack .h file.
#define HAL_FLASH_IEEE_SIZE            8
#define HAL_FLASH_IEEE_PAGE            (HAL_NV_PAGE_END+1)
#define HAL_FLASH_IEEE_OSET            (HAL_FLASH_PAGE_SIZE - HAL_FLASH_LOCK_BITS - HAL_FLASH_IEEE_SIZE)
#define HAL_INFOP_IEEE_OSET            0xC

#define HAL_NV_PAGE_CNT                2
#define HAL_NV_PAGE_BEG                (HAL_NV_PAGE_END-HAL_NV_PAGE_CNT+1)

// Used by DMA macros to shift 1 to create a mask for DMA registers.
#define HAL_NV_DMA_CH                  0
#define HAL_DMA_CH_RX                  3
#define HAL_DMA_CH_TX                  4

#define HAL_NV_DMA_GET_DESC()  HAL_DMA_GET_DESC0()
#define HAL_NV_DMA_SET_ADDR(a) HAL_DMA_SET_ADDR_DESC0((a))

/* Critical Vdd Monitoring to prevent flash damage or radio lockup. */

// Vdd/3 / Internal Reference X ENOB --> (Vdd / 3) / 1.15 X 127
#define VDD_2_0  74   // 2.0 V required to safely read/write internal flash.
#define VDD_2_7  100  // 2.7 V required for the Numonyx device.

#define VDD_MIN_RUN   VDD_2_0
#define VDD_MIN_NV   (VDD_2_0+4)  // 5% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_XNV  (VDD_2_7+5)  // 5% margin over minimum to survive a page erase and compaction.

/*******************************************************************************
 * MACROS
 */

/* Cache Prefetch Control */

#define PREFETCH_ENABLE()  st( FCTL = 0x08; )
#define PREFETCH_DISABLE() st( FCTL = 0x04; )

/* Setting Clocks */

// switch to the 16MHz HSOSC and wait until it is stable
#define SET_OSC_TO_HSOSC()                                                     \
{                                                                              \
  CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_16MHZ;                            \
  while ( (CLKCONSTA & ~0x80) != CLKCONCMD_16MHZ );                            \
}

// switch to the 32MHz XOSC and wait until it is stable
#define SET_OSC_TO_XOSC()                                                      \
{                                                                              \
  CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_32MHZ;                            \
  while ( (CLKCONSTA & ~0x80) != CLKCONCMD_32MHZ );                            \
}

// set 32kHz OSC and wait until it is stable
#define SET_32KHZ_OSC()                                                        \
{                                                                              \
  CLKCONCMD = (CLKCONCMD & ~0x80) | OSC_32KHZ;                                 \
  while ( (CLKCONSTA & 0x80) != OSC_32KHZ );                                   \
}

#define START_HSOSC_XOSC()                                                     \
{                                                                              \
  SLEEPCMD &= ~OSC_PD;            /* start 16MHz RCOSC & 32MHz XOSC */         \
  while (!(SLEEPSTA & XOSC_STB)); /* wait for stable 32MHz XOSC */             \
}

#define STOP_HSOSC()                                                           \
{                                                                              \
  SLEEPCMD |= OSC_PD;             /* stop 16MHz RCOSC */                       \
}

/* Board Initialization */
#define HAL_BOARD_INIT()                                                       \
{                                                                              \
  /* Set to 16Mhz to set 32kHz OSC, then back to 32MHz */                      \
  START_HSOSC_XOSC();                                                          \
  SET_OSC_TO_HSOSC();                                                          \
  SET_32KHZ_OSC();                                                             \
  SET_OSC_TO_XOSC();                                                           \
  STOP_HSOSC();                                                                \
                                                                               \
  /* Enable cache prefetch mode. */                                            \
  PREFETCH_ENABLE();                                                           \
}


/* Driver Configuration */

/* Set to TRUE enable H/W TIMER usage, FALSE disable it */
#ifndef HAL_TIMER
#define HAL_TIMER FALSE
#endif

/* Set to TRUE enable ADC usage, FALSE disable it */
#ifndef HAL_ADC
#define HAL_ADC TRUE
#endif

/* Set to TRUE enable DMA usage, FALSE disable it */
#ifndef HAL_DMA
#define HAL_DMA TRUE
#endif

/* Set to TRUE enable Flash access, FALSE disable it */
#ifndef HAL_FLASH
#define HAL_FLASH TRUE
#endif

/* Set to TRUE enable AES usage, FALSE disable it */
#ifndef HAL_AES
#define HAL_AES TRUE
#endif

#ifndef HAL_AES_DMA
#define HAL_AES_DMA TRUE
#endif

/* Set to TRUE enable LCD usage, FALSE disable it */
#ifndef HAL_LCD
#define HAL_LCD FALSE
#endif

/* Set to TRUE enable LED usage, FALSE disable it */
#ifndef HAL_LED
#define HAL_LED TRUE
#endif
#if (!defined BLINK_LEDS) && (HAL_LED == TRUE)
#define BLINK_LEDS
#endif

/* Set to TRUE enable KEY usage, FALSE disable it */
#ifndef HAL_KEY
#define HAL_KEY FALSE
#endif

/* Set to TRUE enable UART usage, FALSE disable it */
#ifndef HAL_UART
#if (defined ZAPP_P1) || (defined ZAPP_P2) || (defined ZTOOL_P1) || (defined ZTOOL_P2)
#define HAL_UART TRUE
#else
#define HAL_UART FALSE
#endif
#endif

#if HAL_UART
// Always prefer to use DMA over ISR.
#if HAL_DMA
  #ifndef HAL_UART_DMA
    #if (defined ZAPP_P1) || (defined ZTOOL_P1)
      #define HAL_UART_DMA  1
    #elif (defined ZAPP_P2) || (defined ZTOOL_P2)
      #define HAL_UART_DMA  2
    #else
      #define HAL_UART_DMA  1
    #endif
  #endif
  #ifndef HAL_UART_ISR
    #define HAL_UART_ISR  0
  #endif
#else
  #ifndef HAL_UART_ISR
    #if (defined ZAPP_P1) || (defined ZTOOL_P1)
      #define HAL_UART_ISR  1
    #elif (defined ZAPP_P2) || (defined ZTOOL_P2)
      #define HAL_UART_ISR  2
    #else
      #define HAL_UART_ISR  1
    #endif
  #endif
  #ifndef HAL_UART_DMA
    #define HAL_UART_DMA  0
  #endif
#endif

// Used to set P2 priority - USART0 over USART1 if both are defined.
#if ((HAL_UART_DMA == 1) || (HAL_UART_ISR == 1))
#define HAL_UART_PRIPO             0x00
#else
#define HAL_UART_PRIPO             0x40
#endif

#else
#define HAL_UART_DMA  0
#define HAL_UART_ISR  0
#endif

#if !defined HAL_UART_SPI
#define HAL_UART_SPI  0
#endif

#define HAL_UART_PORT 0
#define HAL_SPI_MASTER 1

#ifdef __cplusplus
}
#endif

#endif /* HAL_BOARD_CFG_H */

/*******************************************************************************
*/
