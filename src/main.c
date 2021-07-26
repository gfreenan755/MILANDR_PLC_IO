#include "main.h"

//unsigned int tmp = 0;

unsigned char IN_State[NUMBER_PIN_IN];
unsigned char IN_Fault1[NUMBER_PIN_IN];
unsigned char IN_Fault2[NUMBER_PIN_IN];
unsigned char IN_Buf[NUMBER_PIN_IN];
unsigned char LED_State[NUMBER_PIN_IN];
unsigned char SWITCH_State[4];

unsigned int cnt_led;
unsigned short togle_led;
unsigned short S_cnt;

unsigned int cnt;

unsigned char S_LED1;
unsigned char S_LED2;
unsigned char S_LED3;
unsigned char S_LED4;

int cnt_impl=0;
unsigned char buf_state=0;

uint16_t spi_data = 0;

void main(void)
{
    SystemCoreClockUpdate();
    // Если частота ядра МК настроена неправильно
    if (SystemCoreClock != 144*1000*1000) {
        while(1) {}
    }
    
    //Инициализируем периферию
    GPIO_Init();
    Timer1_Init();
    SPI2_Init();

    // Основной цикл программы
    while(1) {

      //Переключение светодиодов по состоянию переключателей SW1, SW2
        if (!PORT_ReadInputDataBit(SA1_1_GPIO_Port, SA1_1_Pin)){
          PORT_SetBits(MDR_PORTD, PORT_Pin_2);
            SWITCH_State[0] = 1;
            spi_data = 1;
        } else if (!PORT_ReadInputDataBit(SA1_2_GPIO_Port, SA1_2_Pin)){
            PORT_SetBits(MDR_PORTD, PORT_Pin_3);
            SWITCH_State[1] = 1;
            spi_data = 2;
        } else if (!PORT_ReadInputDataBit(SA2_1_GPIO_Port, SA2_1_Pin)){
            PORT_SetBits(MDR_PORTD, PORT_Pin_0);
            SWITCH_State[2] = 1;
            spi_data = 3;
        } else if (!PORT_ReadInputDataBit(SA2_2_GPIO_Port, SA2_2_Pin)){
            PORT_SetBits(MDR_PORTD, PORT_Pin_1);
            SWITCH_State[3] = 1;
            spi_data = 4;
        } else {
          PORT_ResetBits(MDR_PORTD, PORT_Pin_0);
          PORT_ResetBits(MDR_PORTD, PORT_Pin_1);
          PORT_ResetBits(MDR_PORTD, PORT_Pin_2);
          PORT_ResetBits(MDR_PORTD, PORT_Pin_3);
            SWITCH_State[0] = 0;
            spi_data = 0;
        }
        
        SPI2_TransmitData(spi_data);
    
    }
}

void GPIO_Init(void){
  
    // Объявление структуры настроек порта
    PORT_InitTypeDef port_settings;

    PORT_StructInit(&port_settings);
    port_settings.PORT_MODE    = PORT_MODE_DIGITAL;
    port_settings.PORT_SPEED   = PORT_SPEED_SLOW;
    
    // Порт А. Включаем тактирование
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTA), ENABLE);
    // Настройка пинов 9,12 на ввод
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_12;
    PORT_Init(MDR_PORTA, &port_settings);
    // Настройка пинов 4,5,13,14 на вывод
    port_settings.PORT_OE      = PORT_OE_IN;
    port_settings.PORT_Pin = PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_13 | PORT_Pin_14;
    PORT_Init(MDR_PORTA, &port_settings);
    
    // Порт B. Включаем тактирование
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTB), ENABLE);
    // Настройка пинов 1,2,4,8,12 на ввод
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_4 | PORT_Pin_8 | PORT_Pin_12;
    PORT_Init(MDR_PORTB, &port_settings);
    // Настройка пинов 3,6,7 на вывод
    port_settings.PORT_OE      = PORT_OE_IN;
    port_settings.PORT_Pin = PORT_Pin_3 | PORT_Pin_6 | PORT_Pin_7;
    PORT_Init(MDR_PORTB, &port_settings);
    
    // Порт C. Включаем тактирование
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTC), ENABLE);
    // Настройка пина 1 на вывод
    port_settings.PORT_OE      = PORT_OE_IN;
    port_settings.PORT_Pin = PORT_Pin_1;
    PORT_Init(MDR_PORTC, &port_settings);
    
    // Порт D. Включаем тактирование
    RST_CLK_PCLKcmd(PCLK_BIT(MDR_PORTD), ENABLE);
    // Настройка пинов 0,1,2,3,5 на вывод
    port_settings.PORT_OE      = PORT_OE_OUT;
    port_settings.PORT_Pin = PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_5;
    PORT_Init(MDR_PORTD, &port_settings);

}

void Timer1_Init(void){
  
  // Объявление структуры настроек таймера
  TIMER_CntInitTypeDef timer_settings;
  
  // Вводим начальные настройки таймера
  timer_settings.TIMER_IniCounter = 0;                                          
  timer_settings.TIMER_Prescaler = 3;                                           /*!< Specifies the prescaler value used to divide the TIMER clock.
	 	 	 	 	 	 	 	   	 	  This parameter can be a number between 0x0000 and 0xFFFF.
	 	 	 	 	 	 	 	   	 	  CLK = TIMER_CLK/(TIMER_Prescaler + 1) */
  timer_settings.TIMER_Period = 3599;                                           /*!< Specifies the period value to be loaded into the
										 Auto-Reload Register (ARR) at the next update event.
										 This parameter must be a number between 0x0000 and 0xFFFFFFFF.  */
  // 144 МГц / (3+1) = 36 МГц    36 МГц / 3600 = 10 кГц
  
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

void SPI2_Init(void){

    // Инициализируем пины SPI2
    /* Enable peripheral clocks */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | PCLK_BIT(MDR_PORTC)
                    | PCLK_BIT(MDR_PORTC), ENABLE);

    /* Configure SPI pins: CLK, RXD, TXD */
    PORT_InitTypeDef PORT_InitStructure;
    PORT_StructInit(&PORT_InitStructure);
    
    PORT_InitStructure.PORT_Pin   = PORT_Pin_10;
    PORT_InitStructure.PORT_OE    = PORT_OE_IN;
    PORT_InitStructure.PORT_FUNC  = PORT_FUNC_MAIN;
    PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);
    
    PORT_InitStructure.PORT_Pin   = PORT_Pin_9 | PORT_Pin_11;
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);
    
    PORT_InitStructure.PORT_Pin   = PORT_Pin_12;
    PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);
    
    /* Enable peripheral clocks */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | PCLK_BIT(MDR_SSP2), ENABLE);

    /* Reset all SSP settings */
    SSP_DeInit(MDR_SSP2);
    SSP_BRGInit(MDR_SSP2, SSP_HCLKdiv1);

    /* SSP MASTER configuration */
    SSP_InitTypeDef sSSP;
    SSP_StructInit(&sSSP);

    /* Скорость передачи: 144 / (17 + 1) / 2 = 4 МГц */
    sSSP.SSP_SCR = 17;
    sSSP.SSP_CPSDVSR = 2;
    sSSP.SSP_Mode = SSP_ModeMaster;
    sSSP.SSP_WordLength = SSP_WordLength16b;
    sSSP.SSP_SPH = SSP_SPH_1Edge;
    sSSP.SSP_SPO = SSP_SPO_Low;
    sSSP.SSP_FRF = SSP_FRF_SPI_Motorola;
    sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
    SSP_Init(MDR_SSP2, &sSSP);

    /* Enable SSP */
    SSP_Cmd(MDR_SSP2, ENABLE);

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

void Led_Pin() {
	if (cnt >= MAX_CNT_LED) {
		cnt = 0;
		togle_led = !togle_led;
		for (int i = 0; i < NUMBER_PIN_IN; i++) {
		if ((IN_Fault1[i] == 1)) // ((IN_Fault1[i] == 1) || (IN_Fault2[i] == 1))
			LED_State[i] = togle_led;
		else
			LED_State[i] = IN_State[i];
		}
	} else {
		cnt++;
	}

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

void SPI2_TransmitData(uint16_t data){

    PORT_ResetBits(MDR_PORTC, PORT_Pin_12);
    
    SSP_SendData(MDR_SSP2, data);
    
    while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_BSY) == SET) {}
    PORT_SetBits(MDR_PORTC,PORT_Pin_12);

}

bool SPI2_TransmitReceiveData(uint16_t data){

    PORT_ResetBits(MDR_PORTC, PORT_Pin_12);
    
    SSP_SendData(MDR_SSP2, data);
    
    while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_BSY) == SET) {}
    PORT_SetBits(MDR_PORTC,PORT_Pin_12);

    bool ret = false;
    while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_RNE) == SET) {
        if (SSP_ReceiveData(MDR_SSP2) == data) {
            ret = true;
        }
    }
    return ret;

}

