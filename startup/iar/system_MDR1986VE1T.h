/**
  * FILE system_MDR1986VE1T.h
  */

#ifndef __SYSTEM_MDR1986VE1T_H
#define __SYSTEM_MDR1986VE1T_H

#include <stdint.h>

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock)
                                           *   default value */

void SystemInit(void);
void SystemCoreClockUpdate(void);

#endif /* __SYSTEM_MDR1986VE1T_H */
