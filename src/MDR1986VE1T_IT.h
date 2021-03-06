/* This file contains all the functions prototypes for the interrupt. */

#ifndef __MDR1986VE1T_IT_H
#define __MDR1986VE1T_IT_H

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void MIL_STD_1553B2_IRQHandler(void);
void MIL_STD_1553B1_IRQHandler(void);
/*void USB_IRQHandler(void);*/
void CAN1_IRQHandler(void);
void CAN2_IRQHandler(void);
void DMA_IRQHandler(void);
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);
void SSP1_IRQHandler(void);
void BUSY_IRQHandler(void);
void ARINC429R_IRQHandler(void);
void POWER_IRQHandler(void);
void WWDG_IRQHandler(void);
void Timer4_IRQHandler(void);
void Timer1_IRQHandler(void);
void Timer2_IRQHandler(void);
void Timer3_IRQHandler(void);
void ADC_IRQHandler(void);
void ETHERNET_IRQHandler(void);
void SSP3_IRQHandler(void);
void SSP2_IRQHandler(void);
void ARINC429T1_IRQHandler(void);
void ARINC429T2_IRQHandler(void);
void ARINC429T3_IRQHandler(void);
void ARINC429T4_IRQHandler(void);
void BKP_IRQHandler(void);
void EXT_INT1_IRQHandler(void);
void EXT_INT2_IRQHandler(void);
void EXT_INT3_IRQHandler(void);
void EXT_INT4_IRQHandler(void);

#endif /* __MDR1986VE1T_IT_H */
