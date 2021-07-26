#include <stdint.h>
#include <stdbool.h>

#include "system_MDR1986VE1T.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_ssp.h"

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
#define MAX_CNT_LED 5000

void main(void);

void GPIO_Init(void);
void Timer1_Init(void);
void SPI2_Init(void);

void Read_Pin(unsigned char *Input);
void Test_Pin(unsigned char *Input);
void Test_Pin_Start1();
void Test_Pin_Stop1();
void Test_Pin_Start2();
void Test_Pin_Stop2();
void Led_Pin();

void SPI2_TransmitData(uint16_t data);
bool SPI2_TransmitReceiveData(uint16_t data);
