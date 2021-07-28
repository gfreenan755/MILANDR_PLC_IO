/* Main Interrupt Service Routines. */
#include "MDR1986VE1T_IT.h"
#include "main.h"

extern unsigned char IN_State[NUMBER_PIN_IN];
extern unsigned char IN_Fault1[NUMBER_PIN_IN];
extern unsigned char IN_Fault2[NUMBER_PIN_IN];
extern unsigned char IN_Buf[NUMBER_PIN_IN];

// vt state
extern unsigned char OUT_State[NUMBER_PIN_OUT];
//fault vt
extern unsigned char OUT_Err[NUMBER_PIN_OUT];
extern unsigned char OUT_Buf[NUMBER_PIN_OUT];

extern unsigned char LED_State[NUMBER_PIN_IN];

extern unsigned int cnt_led;
extern unsigned short toggle_led;

extern unsigned char SWITCH_State[4];

extern unsigned char S;
extern unsigned char S1;

extern unsigned char S_buf;
extern unsigned char S1_buf;

extern unsigned short S_cnt;
extern unsigned short S1_cnt;

extern unsigned int Delay_cnt;

extern unsigned char ch;

extern unsigned int cnt;

// uc3843
extern unsigned char OUT_Enable;
extern unsigned char Search_Fault;
extern unsigned char OUT_Fault;

extern unsigned char Err_enable;

extern int cnt_impl;
extern unsigned char buf_state;
extern int cnt_run_led;

extern uint8_t selected_mode;

extern uint8_t init_flag;

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  // Должно быть оговорено, как реагировать.
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles Debug PendSV exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
}

/*******************************************************************************
* Function Name  : MIL_STD_1553B2_IRQHandler
* Description    : This function handles MIL_STD_1553B2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MIL_STD_1553B2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : MIL_STD_1553B1_IRQHandler
* Description    : This function handles MIL_STD_1553B1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MIL_STD_1553B1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_IRQHandler
* Description    : This function handles USB global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
void USB_IRQHandler(void)
{
}
*/

/*******************************************************************************
* Function Name  : CAN1_IRQHandler
* Description    : This function handles CAN1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_IRQHandler(void)
{
}

/*******************************************************************************

* Function Name  : CAN2_IRQHandler
* Description    : This function handles CAN2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA_IRQHandler
* Description    : This function handles DMA global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_IRQHandler ( void )
{
}

/*******************************************************************************
* Function Name  : UART1_IRQHandler
* Description    : This function handles UART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : UART2_IRQHandler
* Description    : This function handles UART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SSP1_IRQHandler
* Description    : This function handles SSP1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSP1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : BUSY_IRQHandler
* Description    : This function handles BUSY global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BUSY_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429R_IRQHandler
* Description    : This function handles ARINC429R global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429R_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : POWER_IRQHandler
* Description    : This function handles POWER global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void POWER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIMER4_IRQHandler
* Description    : This function handles TIMER4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : Timer1_IRQHandler
* Description    : This function handles Timer1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer1_IRQHandler(void)
{
  // Сбрасываем флаг прерывания
  TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
  
  // Моргаем светодиодом PD0
  if (cnt_run_led < 9999){
    cnt_run_led++;
  } else {
    cnt_run_led = 0;
    if (selected_mode != NONE)
      PORT_ToggleBit(MDR_PORTD, PORT_Pin_5);
    
  }
  if (selected_mode == NONE){
    if(cnt_run_led == 1 || cnt_run_led == 2500)
      PORT_ToggleBit(PLC_INPUT_INIT_Port, PLC_INPUT_INIT_Pin);
    if(cnt_run_led == 5000 || cnt_run_led == 7500)
      PORT_ToggleBit(PLC_OUTPUT_INIT_Port, PLC_OUTPUT_INIT_Pin);
  }
  
  Switch_update();
  
  if (selected_mode == PLC_INPUT){
    
    switch (Delay_cnt) {
    case 0:
      Read_Pin(IN_State);
      if(IN_State[IMPULSE_CNT_CH]==1 && buf_state ==0)
      {
        cnt_impl++;
      }
      buf_state =IN_State[IMPULSE_CNT_CH];
      break;
    case 1:
      Test_Pin_Start1();
      break;
    case 2:
      Test_Pin(IN_Fault1);
      break;
    case 3:
      Test_Pin_Stop1();
      Test_Pin_Start2();
      break;
    case 4:
      Test_Pin(IN_Fault2);
      break;
    case 5:
      Test_Pin_Stop2();
      break;
    case 6:
      for (int i = 0; i < NUMBER_PIN_IN; i++) {
        IN_Buf[i] = IN_State[i];
      }
      break;
    default:
      break;
    }
    
    if (SWITCH_State[2] == 1){                                                  // SA2 вверх
      Led_Number(cnt_impl);                                                     // вывод на светодиоды числа
    } else if (SWITCH_State[3] == 1){                                           // SA2 вниз
      cnt_impl = 0;                                                             // сброс счетчика импульсов
    } else {                                                                    // SA2 середина
      Led_Action(IN_State, IN_Fault1);                                          // вывод состояния входов
    }
    
    if(Delay_cnt >= FILTER_CONST)
      Delay_cnt=0;
    else
      Delay_cnt++;
    
    
    
  } else if (selected_mode == PLC_OUTPUT){
    
    if (Delay_cnt > 0) {
      Delay_cnt--;
    } else {
      Delay_cnt = 0;
    }
    
    if (Delay_cnt == 0) {
      switch (S) {
      case 0:
        PORT_SetBits(X_PIN4_GPIO_Port, X_PIN4_Pin);
        S = 1;
        S_buf = 0;
        Delay_cnt = DELAY_CONST;
        break;
      case 1:
        PORT_ResetBits(X_PIN4_GPIO_Port, X_PIN4_Pin);
        Delay_cnt = DELAY_CONST;
        S = 2;
        S_buf = 1;
        break;
        
      case 2:
        OUT_Fault = PORT_ReadInputDataBit(X_PIN6_GPIO_Port, X_PIN6_Pin);
        
        if (OUT_Fault == 0 && S_buf == 1) {
          S = 0;
          S_cnt++;
          if (S_cnt >= 5) {
            S_cnt = 0;
            Err_enable = 1;
            S = 5;
          }
          Delay_cnt = DELAY_CONST;
        } else if (OUT_Fault == 0 && S_buf == 3) {
          Search_Fault = 1;
          S = 4;
          S1 = 0;
          ch = 0;
          S1_cnt = 0;
          Reset_Buf(OUT_Buf, NUMBER_PIN_OUT);
          Write_VT(OUT_Buf, OUT_Err);
          Delay_cnt = DELAY_CONST;
        } else
          S = 3;
        
        break;
        
      case 3:
        Write_VT(OUT_State, OUT_Err);
        S = 2;
        S_buf = 3;
        break;
        
      case 4:
        
        switch (S1) {
        case 0:
          PORT_SetBits(X_PIN4_GPIO_Port, X_PIN4_Pin);
          S1 = 1;
          S1_buf = 0;
          Delay_cnt = DELAY_CONST;
          break;
        case 1:
          PORT_ResetBits(X_PIN4_GPIO_Port, X_PIN4_Pin);
          Delay_cnt = DELAY_CONST;
          S1 = 2;
          S1_buf = 1;
          break;
          
        case 2:
          OUT_Fault = PORT_ReadInputDataBit(X_PIN6_GPIO_Port, X_PIN6_Pin);
          
          if (OUT_Fault == 0 && S1_buf == 1) {
            S1 = 0;
            S1_cnt++;
            Delay_cnt = DELAY_CONST;
            
            if (S1_cnt >= 5) {
              S1_cnt = 0;
              Err_enable = 1;
              S = 5;
            }
            
          } else if (OUT_Fault == 0 && S1_buf == 3) {
            OUT_Err[ch] = 1;
            OUT_State[ch] = 0;
            OUT_Buf[ch] = 0;
            Reset_Buf(OUT_Buf, NUMBER_PIN_OUT);
            Write_VT(OUT_Buf, OUT_Err);
            Delay_cnt = DELAY_CONST;
            S1 = 0;
          } else
            S1 = 3;
          
          if (S1_buf == 3) {
            if (ch == NUMBER_PIN_OUT - 1) {
              S1 = 0;
              S_cnt = 0;
              S = 0;
              Delay_cnt = DELAY_CONST;
            } else
              ch++;
            
          }
          
          S1_buf = 2;
          break;
          
        case 3:
          Reset_Buf(OUT_Buf, NUMBER_PIN_OUT);
          OUT_Buf[ch] = OUT_State[ch];
          Write_VT(OUT_Buf, OUT_Err);
          S1 = 2;
          S1_buf = 3;
          Delay_cnt = DELAY_CONST*10;
          break;
        }
        
        break;
        
      case 5:
        Reset_Buf(OUT_Buf, NUMBER_PIN_OUT);
        Write_VT(OUT_Buf, OUT_Err);
        Set_Buf(OUT_Err, NUMBER_PIN_OUT);
        if(Err_enable == 0 ){
          S=0;
          S1=0;
          S_cnt = 0;
        }
        break;
        
      }
    }
    
    if (SWITCH_State[2] == 1){                                                  // SA2 вверх
      Set_Buf(OUT_State, NUMBER_PIN_OUT);
    } else if (SWITCH_State[3] == 1){                                           // SA2 вниз
      Reset_Buf(OUT_Err, NUMBER_PIN_OUT);
      Err_enable = 0;
    } else {                                                                    // SA2 середина
      Reset_Buf(OUT_State, NUMBER_PIN_OUT);
    }
    
    Led_Action(OUT_State, OUT_Err);
    
  }
  
}

/*******************************************************************************
* Function Name  : Timer2_IRQHandler
* Description    : This function handles Timer2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : Timer3_IRQHandler
* Description    : This function handles Timer3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ETHERNET_IRQHandler
* Description    : This function handles ETHERNET global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ETHERNET_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SSP3_IRQHandler
* Description    : This function handles SSP3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSP3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SSP2_IRQHandler
* Description    : This function handles SSP2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSP2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429T1_IRQHandler
* Description    : This function handles ARINC429T1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429T1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429T2_IRQHandler
* Description    : This function handles ARINC429T2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429T2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429T3_IRQHandler
* Description    : This function handles ARINC429T3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429T3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429T3_IRQHandler
* Description    : This function handles ARINC429T3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429T4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : BKP_IRQHandler
* Description    : This function handles BKP global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BKP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXT_INT1_IRQHandler
* Description    : This function handles EXT_INT1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXT_INT1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXT_INT2_IRQHandler
* Description    : This function handles EXT_INT2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXT_INT2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXT_INT3_IRQHandler
* Description    : This function handles EXT_INT3 global interrupt request.
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXT_INT3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXT_INT4_IRQHandler
* Description    : This function handles EXT_INT4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXT_INT4_IRQHandler(void)
{
}
