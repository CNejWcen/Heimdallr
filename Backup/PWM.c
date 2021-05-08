/****
*
*       use for pwm
*
***/
#include "hal_mcu.h"
#include "PWM.h"

void pwmInit(uint8 timer)
{
  switch(timer)
  {
  case TIMER1_MARK:
    {
      PERCFG |= 0x40;             // Timer1 Alt2 Channel3
    
      P0DIR |= 0x80;              // P0_7 = output
      P0SEL |= 0x80;              // Peripheral function on P0_7
      
      T1CTL |= 0x03;              // Timer1 mode = 3 - Up/Down
      T1STAT &= ~0xFF;            //Disable all interrupts in Timer1
      
      T1CCTL3 &= ~0x80;           // Disable channel 3 RF interrupts
      T1CCTL3 &= ~0x40;           // Disable channel 3 interrupts
      T1CCTL3 |= 0x04;            // Ch3 mode = compare
      T1CCTL3 |= 0x10;            // Ch3 output compare mode = toggle on compare
    }
    break;
  case TIMER3_MARK:
    {
      // PWM output at P1_6
      // We will use Timer 3 Channel 0 at alternate location 2
      // Channel 0 will toggle on compare with 0 and counter will
      // count in up/down mode to T3CC0.
      PERCFG |= 0x20;             // Timer 3 Alternate location 2
      P1DIR |= 0x40;              // P1_6 = output
      P1SEL |= 0x40;              // Peripheral function on P1_6

      T3CTL &= ~0x10;             // Stop timer 3 (if it was running)
      T3CTL |= 0x04;              // Clear timer 3
      //T3CTL |= 0x08;              // Enable Timer 3 overflow interrupts
      T3CTL &= ~0x08;           // Disable Timer 3 overflow interrupts
      T3CTL |= 0x03;              // Timer 3 mode = 3 - Up/Down

      //T3CCTL0 |= 0x40;            // Enable channel 0 interrupts
      T3CCTL0 &= ~0x40;         // Disable channel 0 interrupts
      T3CCTL0 |= 0x04;            // Ch0 mode = compare
      T3CCTL0 |= 0x10;            // Ch0 output compare mode = toggle on compare
     
     // IEN1 |= 0x08; //T3中断使能 

      //IEN0 |= 0x80;//使能所有中断

    }
    break;
  case TIMER4_MARK:
    {   
      PERCFG &= ~0x10;            //Timer4 Alt1 Channel0        
      P1DIR  |= 0x01;             //P0_0              
      P1SEL  |= 0x01;
      
      T4CTL &= ~0x10;             // Stop timer 4(if it was running)
      T4CTL |= 0x04;              // Clear timer 4
      T4CTL &= ~0x08;             // Disable Timer 4 overflow interrupts
      T4CTL |= 0x03;              // Timer 4 mode = 4 - Up/Down

      T4CCTL0 &= ~0x40;           // Disable channel 0 interrupts
      T4CCTL0 |= 0x04;            // Ch0 mode = compare
      T4CCTL0 |= 0x10;            // Ch0 output compare mode = toggle on compare
    }
    break;
  default:
      
    break;
  }  
    
}
uint8 pwmStart(uint8 timer,uint8 frequency)
{
    pwmInit(timer);
  
    uint8 prescaler = 0;

    // Get current Timer tick divisor setting
    uint8 tickSpdDiv = (CLKCONSTA & 0x38)>>3;

    // Check if frequency too low
    if (frequency < (244 >> tickSpdDiv)){   // 244 Hz = 32MHz / 256 (8bit counter) / 4 (up/down counter and toggle on compare) / 128 (max timer prescaler)
        pwmStop();                       // A lower tick speed will lower this number accordingly.
        return 0;
    }

    // Calculate nr of ticks required to achieve target frequency
    uint32 ticks = (8000000/frequency) >> tickSpdDiv;      // 8000000 = 32M / 4;

    // Fit this into an 8bit counter using the timer prescaler
    while ((ticks & 0xFFFFFF00) != 0)
    {
        ticks >>= 1;
        prescaler += 32;
    }
    switch(timer)
    {
      case TIMER1_MARK:
        T1CTL &= ~0x0C;
        T1CTL |= 0x0C;                  //Prescaler = Tick Frequency / 8
        
        T1CC0L = 0x2F;
        T1CC0H = 0x8C;
        
        T1CCTL3 |= 0x38;                //Initialize output pin. CMP[2:0] is not changed
        break;
      case TIMER3_MARK:
         // Update registers
        T3CTL &= ~0xE0;
        T3CTL |= prescaler;
        T3CC0 = (uint8)ticks;

        // Start timer
        T3CTL |= 0x10; 
        break;
      case TIMER4_MARK:
        // Update registers
        T4CTL &= ~0xE0;
        T4CTL |= prescaler;
        T4CC0 = (uint8)ticks;

        // Start timer
        T4CTL |= 0x10; 
        break;
      default:
      
        break;
    }

    return 1;
}

void pwmStop(void)
{
    T3CTL &= ~0x10;             // Stop timer 3
    P1SEL &= ~0x40;
    P1_6 = 0;
    
    T4CTL &= ~0x10;             // Stop timer 4
    P1SEL &= ~0x01;
    P1_0 = 0;
}

