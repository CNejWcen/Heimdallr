#ifndef __PWM_H_
#define __PWM_H_

#include "hal_types.h"

#define TIMER1_MARK 0x01
#define TIMER3_MARK 0x03
#define TIMER4_MARK 0x04

extern void pwmInit(uint8 timer);
extern uint8 pwmStart(uint8 timer,uint8 frequency);
extern void pwmStop(void);



#endif