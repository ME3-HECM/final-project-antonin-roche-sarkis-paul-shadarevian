#ifndef _timers_H
#define _timers_H

#include <xc.h>

#define _XTAL_FREQ 64000000
#define T_PERIOD 125 // enter your total period value here (i.e. timer ticks for 20 ms) (no = sign for #defines)

unsigned int on_period,off_period;

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

void Timer0_init(void);
void starttimer0(void);

#endif
