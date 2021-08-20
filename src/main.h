#include <stdint.h>
#include <stdbool.h>

#include "system_MDR1986VE1T.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"

#include "EtherCAT_driver.h"

#define SA1_1_Pin PORT_Pin_1
#define SA1_1_GPIO_Port MDR_PORTC
#define SA2_1_Pin PORT_Pin_4
#define SA2_1_GPIO_Port MDR_PORTA
#define SA1_2_Pin PORT_Pin_5
#define SA1_2_GPIO_Port MDR_PORTA
#define SA2_2_Pin PORT_Pin_14
#define SA2_2_GPIO_Port MDR_PORTA

#define LED4_Pin PORT_Pin_12
#define LED4_GPIO_Port MDR_PORTB
#define LED3_Pin PORT_Pin_8
#define LED3_GPIO_Port MDR_PORTB
#define LED2_Pin PORT_Pin_2
#define LED2_GPIO_Port MDR_PORTB
#define LED1_Pin PORT_Pin_9
#define LED1_GPIO_Port MDR_PORTA

#define PLC_INPUT_INIT_Pin PORT_Pin_3
#define PLC_INPUT_INIT_Port MDR_PORTA

#define PLC_OUTPUT_INIT_Pin PORT_Pin_4
#define PLC_OUTPUT_INIT_Port MDR_PORTE

#define X_PIN9_Pin PORT_Pin_3
#define X_PIN9_GPIO_Port MDR_PORTB
#define X_PIN10_Pin PORT_Pin_4
#define X_PIN10_GPIO_Port MDR_PORTB
#define X_PIN7_Pin PORT_Pin_13
#define X_PIN7_GPIO_Port MDR_PORTA
#define X_PIN8_Pin PORT_Pin_1
#define X_PIN8_GPIO_Port MDR_PORTB
#define X_PIN5_Pin PORT_Pin_7
#define X_PIN5_GPIO_Port MDR_PORTB
#define X_PIN6_Pin PORT_Pin_12
#define X_PIN6_GPIO_Port MDR_PORTA
#define X_PIN3_Pin PORT_Pin_6
#define X_PIN3_GPIO_Port MDR_PORTB
#define X_PIN4_Pin PORT_Pin_8
#define X_PIN4_GPIO_Port MDR_PORTA

#define NUMBER_PIN_IN 4
#define NUMBER_PIN_OUT 4
#define NUMBER_GRUP 1
#define MAX_CNT_LED 5000
#define FILTER_CONST 50
#define DELAY_CONST 200
#define IMPULSE_CNT_CH 0

#define NONE 0
#define PLC_INPUT 1
#define PLC_OUTPUT 2

void main(void);

void GPIO_Init_GENERAL(void);
void GPIO_Init_INPUT(void);
void GPIO_Init_OUTPUT(void);
void Timer1_Init(void);


// Функции входов
void Read_Pin(unsigned char *Input);
void Test_Pin(unsigned char *Input);
void Test_Pin_Start1();
void Test_Pin_Stop1();
void Test_Pin_Start2();
void Test_Pin_Stop2();

// Функции выходов
void Write_VT(unsigned char *Input1, unsigned char *Input2);
void Reset_Buf(unsigned char *Buf, unsigned char Length);
void Set_Buf(unsigned char *Buf, unsigned char Length);

void Led_Out();
void Led_Action(unsigned char *Buf_state,unsigned char *Buf_err );
void Led_Number(unsigned char Number);
void Switch_update();
void Switch_indicate();
