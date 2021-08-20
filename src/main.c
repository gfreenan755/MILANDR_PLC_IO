#include "main.h"

unsigned char IN_State[NUMBER_PIN_IN];
unsigned char IN_Fault1[NUMBER_PIN_IN];
unsigned char IN_Fault2[NUMBER_PIN_IN];
unsigned char IN_Buf[NUMBER_PIN_IN];

// vt state
unsigned char OUT_State[NUMBER_PIN_OUT];
//fault vt
unsigned char OUT_Err[NUMBER_PIN_OUT];
unsigned char OUT_Buf[NUMBER_PIN_OUT];

unsigned char LED_State[NUMBER_PIN_IN];

unsigned int cnt_led;
unsigned short toggle_led;


unsigned char SWITCH_State[4];

unsigned char S;
unsigned char S1;

unsigned char S_buf;
unsigned char S1_buf;

unsigned short S_cnt;
unsigned short S1_cnt;

unsigned int Delay_cnt;

unsigned char ch;

unsigned int cnt;

// uc3843
unsigned char OUT_Enable;
unsigned char Search_Fault;
unsigned char OUT_Fault;

unsigned char Err_enable;

int cnt_impl;
unsigned char buf_state;
int cnt_run_led;

uint8_t selected_mode;

uint8_t init_flag;

uint16_t spi_data;

extern ETHERCAT EtherCATDevice;

void main(void)
{
    SystemCoreClockUpdate();
    // Если частота ядра МК настроена неправильно
    if (SystemCoreClock != 144*1000*1000) {
        while(1) {}
    }
    
    //Инициализируем EtherCAT 
    if(EtherCAT_Init()){
      EtherCATDevice.IsInit = true;
    } else {
     EtherCATDevice.IsInit = false;
    }
    
    //Инициализируем периферию
    GPIO_Init_GENERAL();
    Switch_update();
    
    if (SWITCH_State[0] == 1){                                                  // SA1 вверх
    selected_mode = PLC_INPUT;
    GPIO_Init_INPUT();
    PORT_SetBits(PLC_INPUT_INIT_Port,PLC_INPUT_INIT_Pin);
    
    
    } else if (SWITCH_State[1] == 1){                                           // SA1 вниз
    selected_mode = PLC_OUTPUT;
    GPIO_Init_OUTPUT();
    PORT_SetBits(PLC_OUTPUT_INIT_Port,PLC_OUTPUT_INIT_Pin);
    
    
    } else {
    selected_mode = NONE;
    PORT_ResetBits(PLC_INPUT_INIT_Port,PLC_INPUT_INIT_Pin);
    PORT_ResetBits(PLC_OUTPUT_INIT_Port,PLC_OUTPUT_INIT_Pin);
    }
    Timer1_Init();
    
    // Основной цикл программы
    while(1) {
        
      Switch_indicate();  
    
    }
}

void GPIO_Init_GENERAL(void){
  
// Соответствие между ножками МК и светодиодами, "A9" - порт A, пин 9
// A9  A8  A7  A6  A5  A4  A3  E4
// B2  B1  B0  A15 A14 A13 D4  D5
// B8  B7  B6  B5  B4  B3  D2  D3
// B12 B13 B14 B15 C2  C1  D0  D1
  
    // Объявление структуры настроек порта
    PORT_InitTypeDef port_settings;

    PORT_StructInit(&port_settings);
    port_settings.PORT_MODE    = PORT_MODE_DIGITAL;
    port_settings.PORT_SPEED   = PORT_SPEED_SLOW;
    
    // Порт А. Включаем тактирование
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTA), ENABLE);
    // Настройка пинов 3 на вывод (Светодиод общ назн)
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_3;
    PORT_Init(MDR_PORTA, &port_settings);
    // Настройка пинов 4,5,14 на ввод (Переключатели 1_2, 2_1, 2_2)
    port_settings.PORT_OE      = PORT_OE_IN;
    port_settings.PORT_Pin = PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_14;
    PORT_Init(MDR_PORTA, &port_settings);
    
    // Порт C. Включаем тактирование (Переключатель 1_1)
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTC), ENABLE);
    // Настройка пина 1 на ввод
    port_settings.PORT_OE      = PORT_OE_IN;
    port_settings.PORT_Pin = PORT_Pin_1;
    PORT_Init(MDR_PORTC, &port_settings);
    
    // Порт D. Включаем тактирование (Светодиоды общ назначения)
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTD), ENABLE);
    // Настройка пинов 0,1,2,3,5 на вывод
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_5;
    PORT_Init(MDR_PORTD, &port_settings);
    
    // Порт E. Включаем тактирование (Светодиоды общ назначения)
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTE), ENABLE);
    // Настройка пинов 4 на вывод
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_4;
    PORT_Init(MDR_PORTE, &port_settings);

}

void GPIO_Init_INPUT(void){
  
    // Объявление структуры настроек порта
    PORT_InitTypeDef port_settings;

    PORT_StructInit(&port_settings);
    port_settings.PORT_MODE    = PORT_MODE_DIGITAL;
    port_settings.PORT_SPEED   = PORT_SPEED_SLOW;
    
    // Порт А. 
    // Настройка пинов 8,9,12 на вывод (9 - Светодиод индикации LED1)
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_12;
    PORT_Init(MDR_PORTA, &port_settings);
    // Настройка пинов 13 на ввод
    port_settings.PORT_OE      = PORT_OE_IN;
    port_settings.PORT_Pin = PORT_Pin_13;
    PORT_Init(MDR_PORTA, &port_settings);
    
    // Порт B. Включаем тактирование
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTB), ENABLE);
    // Настройка пинов 2,8,12 на вывод (Светодиоды индикации LED2-4)
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_2 | PORT_Pin_8 | PORT_Pin_12;
    PORT_Init(MDR_PORTB, &port_settings);
    // Настройка пинов 3,6,7 на ввод
    port_settings.PORT_OE      = PORT_OE_IN;
    port_settings.PORT_Pin = PORT_Pin_3 | PORT_Pin_6 | PORT_Pin_7;
    PORT_Init(MDR_PORTB, &port_settings);

}

void GPIO_Init_OUTPUT(void){

    // Объявление структуры настроек порта
    PORT_InitTypeDef port_settings;

    PORT_StructInit(&port_settings);
    port_settings.PORT_MODE    = PORT_MODE_DIGITAL;
    port_settings.PORT_SPEED   = PORT_SPEED_SLOW;
    
    // Порт А. 
    // Настройка пинов 8,9,13 на вывод (9 - Светодиод индикации LED1)
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_13;
    PORT_Init(MDR_PORTA, &port_settings);
    // Настройка пинов 12 на ввод
    port_settings.PORT_OE      = PORT_OE_IN;
    port_settings.PORT_Pin = PORT_Pin_12;
    PORT_Init(MDR_PORTA, &port_settings);
    
    // Порт B. Включаем тактирование
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTB), ENABLE); 
    // Настройка пинов 2,3,6,7,8,12 на вывод( 2,8,12 Светодиоды индикации LED2-4)
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_12;
    PORT_Init(MDR_PORTB, &port_settings);

}

void Timer1_Init(void){
  
  // Объявление структуры настроек таймера
  TIMER_CntInitTypeDef timer_settings;
  
  // Вводим начальные настройки таймера
  timer_settings.TIMER_IniCounter = 0;                                          
  timer_settings.TIMER_Prescaler = 3;                                           /*!< Specifies the prescaler value used to divide the TIMER clock.
	 	 	 	 	 	 	 	   	 	  This parameter can be a number between 0x0000 and 0xFFFF.
	 	 	 	 	 	 	 	   	 	  CLK = TIMER_CLK/(TIMER_Prescaler + 1) */
  timer_settings.TIMER_Period = 899;                                           /*!< Specifies the period value to be loaded into the
										 Auto-Reload Register (ARR) at the next update event.
										 This parameter must be a number between 0x0000 and 0xFFFFFFFF.  */
  // 144 МГц / (3+1) = 36 МГц    36 МГц / 900 = 40 кГц
  
  timer_settings.TIMER_CounterMode = TIMER_CntMode_ClkFixedDir;
  timer_settings.TIMER_CounterDirection = TIMER_CntDir_Up;
  timer_settings.TIMER_EventSource      = TIMER_EvSrc_None;
  timer_settings.TIMER_FilterSampling   = TIMER_FDTS_TIMER_CLK_div_1;
  timer_settings.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_Immediately;         // CNT обнуляется при достижении ARR
  timer_settings.TIMER_ETR_FilterConf   = TIMER_Filter_1FF_at_TIMER_CLK;        // Без фильтра
  timer_settings.TIMER_ETR_Prescaler    = TIMER_ETR_Prescaler_None;
  timer_settings.TIMER_ETR_Polarity     = TIMER_ETRPolarity_NonInverted;
  timer_settings.TIMER_BRK_Polarity     = TIMER_BRKPolarity_NonInverted;
    
  // Включаем тактирование 
  RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);
  // Без внешнего предделителя (144 МГц)
  TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv1);
  
  // Инициализируем таймер
  TIMER_CntInit(MDR_TIMER1, &timer_settings);
  
  // Включаем прерывание по сравнению
  TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);
  // Разрешаем прерывание
  NVIC_EnableIRQ(TIMER1_IRQn);
  
  // Включаем таймер
  TIMER_Cmd(MDR_TIMER1, ENABLE);

}

void Read_Pin(unsigned char *Input) { // Считывание данных с портов

        Input[0] = PORT_ReadInputDataBit(X_PIN3_GPIO_Port, X_PIN3_Pin);
	Input[1] = PORT_ReadInputDataBit(X_PIN5_GPIO_Port, X_PIN5_Pin);
	Input[2] = PORT_ReadInputDataBit(X_PIN7_GPIO_Port, X_PIN7_Pin);
	Input[3] = PORT_ReadInputDataBit(X_PIN9_GPIO_Port, X_PIN9_Pin);
}

void Test_Pin(unsigned char *Input) { // Определение ошибки 1
	// Считали данные
	Read_Pin (Input);

	// Определили где неисправность
	for (int i = 0; i < NUMBER_PIN_IN; i++) {
		Input[i] = !Input[i];
	}

}

void Test_Pin_Start1() { //
        PORT_SetBits(X_PIN4_GPIO_Port, X_PIN4_Pin);
}

void Test_Pin_Stop1() { //
        PORT_ResetBits(X_PIN4_GPIO_Port, X_PIN4_Pin);
}

void Test_Pin_Start2() { //
        PORT_SetBits(X_PIN6_GPIO_Port, X_PIN6_Pin);
}

void Test_Pin_Stop2() { //
        PORT_ResetBits(X_PIN6_GPIO_Port, X_PIN6_Pin);
}

void Write_VT(unsigned char *Input1, unsigned char *Input2) {

	if (Input1[0] == 1 && Input2[0] == 0)
		PORT_SetBits(X_PIN3_GPIO_Port, X_PIN3_Pin);
	else
		PORT_ResetBits(X_PIN3_GPIO_Port, X_PIN3_Pin);

	if (Input1[1] == 1 && Input2[1] == 0)
		PORT_SetBits(X_PIN5_GPIO_Port, X_PIN5_Pin);
	else
		PORT_ResetBits(X_PIN5_GPIO_Port, X_PIN5_Pin);

	if (Input1[2] == 1 && Input2[2] == 0)
		PORT_SetBits(X_PIN7_GPIO_Port, X_PIN7_Pin);
	else
		PORT_ResetBits(X_PIN7_GPIO_Port, X_PIN7_Pin);

	if (Input1[3] == 1 && Input2[3] == 0)
		PORT_SetBits(X_PIN9_GPIO_Port, X_PIN9_Pin);
	else
		PORT_ResetBits(X_PIN9_GPIO_Port, X_PIN9_Pin);
}

void Reset_Buf(unsigned char *Buf, unsigned char Length) {
	for (int i = 0; i < Length; i++) {
		Buf[i] = 0;
	}

}

void Set_Buf(unsigned char *Buf, unsigned char Length) {
	for (int i = 0; i < Length; i++) {
		Buf[i] = 1;
	}

}

void Led_Out(){

	if (LED_State[0])
                PORT_SetBits(LED1_GPIO_Port, LED1_Pin);
	else
                PORT_ResetBits(LED1_GPIO_Port, LED1_Pin);

	if (LED_State[1])
                PORT_SetBits(LED2_GPIO_Port, LED2_Pin);
	else
                PORT_ResetBits(LED2_GPIO_Port, LED2_Pin);

	if (LED_State[2])
                PORT_SetBits(LED3_GPIO_Port, LED3_Pin);
	else
                PORT_ResetBits(LED3_GPIO_Port, LED3_Pin);

	if (LED_State[3])
                PORT_SetBits(LED4_GPIO_Port, LED4_Pin);
	else
                PORT_ResetBits(LED4_GPIO_Port, LED4_Pin);


}

void Led_Action(unsigned char *Buf_state,unsigned char *Buf_err ) {
  
  
    if (cnt >= MAX_CNT_LED) {
		cnt = 0;
		toggle_led = !toggle_led;
		
	} else {
		cnt++;
	}
          for (int i = 0; i < NUMBER_PIN_IN; i++) {
		if ((Buf_err[i] == 1)) 
			LED_State[i] = toggle_led;
		else
			LED_State[i] = Buf_state[i];
		}

  
  Led_Out();
          
}

void Led_Number(unsigned char Number) {
  
LED_State[0] = Number&1;
LED_State[1] = (Number>>1) & 1;
LED_State[2] = (Number>>2) & 1;
LED_State[3] = (Number>>3) & 1;

Led_Out();

}

void Switch_update(){

  if (!PORT_ReadInputDataBit(SA1_1_GPIO_Port, SA1_1_Pin)){
  SWITCH_State[0] = 1;
  } else {
  SWITCH_State[0] = 0;
  }
  
  if (!PORT_ReadInputDataBit(SA1_2_GPIO_Port, SA1_2_Pin)){
  SWITCH_State[1] = 1;
  } else {
  SWITCH_State[1] = 0;
  }
  
  if (!PORT_ReadInputDataBit(SA2_1_GPIO_Port, SA2_1_Pin)){
  SWITCH_State[2] = 1;
  } else {
  SWITCH_State[2] = 0;
  }
  
  if (!PORT_ReadInputDataBit(SA2_2_GPIO_Port, SA2_2_Pin)){
  SWITCH_State[3] = 1;
  } else {
    SWITCH_State[3] = 0;
  }

}

void Switch_indicate(){

  if (SWITCH_State[0] == 1){
  PORT_SetBits(MDR_PORTD, PORT_Pin_2);
  } else {
  PORT_ResetBits(MDR_PORTD, PORT_Pin_2);
  }
  
  if (SWITCH_State[1] == 1){
  PORT_SetBits(MDR_PORTD, PORT_Pin_3);
  } else {
  PORT_ResetBits(MDR_PORTD, PORT_Pin_3);
  }
  
  if (SWITCH_State[2] == 1){
  PORT_SetBits(MDR_PORTD, PORT_Pin_0);
  } else {
  PORT_ResetBits(MDR_PORTD, PORT_Pin_0);
  }
  
  if (SWITCH_State[3] == 1){
  PORT_SetBits(MDR_PORTD, PORT_Pin_1);
  } else {
  PORT_ResetBits(MDR_PORTD, PORT_Pin_1);
  }
 
}

