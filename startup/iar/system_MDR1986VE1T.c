/**
  * FILE system_1986VE1T.c
  */

#include "MDR1986VE1T.h"
#include "MDR32F9Qx_config.h"

/*******************************************************************************
*  Clock Definitions
*******************************************************************************/
uint32_t SystemCoreClock = (uint32_t)8000000;       /*!< System Clock Frequency (Core Clock)
                                                     *   default value */

/**
  * @brief  Update SystemCoreClock according to Clock Register Values
  * @note   None
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
    uint32_t cpu_c1_freq, cpu_c2_freq, cpu_c3_freq;
    uint32_t pll_mul;
    uint32_t cpu_c3_sel;

    /* Compute CPU_CLK frequency */

    /* Determine CPU_C1 frequency */
    if (MDR_RST_CLK->CPU_CLOCK & (1u << 1)) {
        cpu_c1_freq = HSE_Value;
    } else {
        cpu_c1_freq = HSI_Value;
    }

    if (MDR_RST_CLK->CPU_CLOCK & (1u << 0)) {
        cpu_c1_freq /= 2;
    }

    /* Determine CPU_C2 frequency */
    cpu_c2_freq = cpu_c1_freq;

    if (MDR_RST_CLK->CPU_CLOCK & (1u << 2)) {
        /* Determine CPU PLL output frequency */
        pll_mul = ((MDR_RST_CLK->PLL_CONTROL >> 8) & (uint32_t)0xF) + 1;
        cpu_c2_freq *= pll_mul;
    }

    /*Select CPU_CLK from HSI, CPU_C3, LSE, LSI cases */
    switch ((MDR_RST_CLK->CPU_CLOCK >> 8) & (uint32_t)0x03) {
    case 0 :
        /* HSI */
        SystemCoreClock = HSI_Value;
        break;
    case 1 :
        /* CPU_C3 */
        /* Determine CPU_C3 frequency */
        cpu_c3_sel = (MDR_RST_CLK->CPU_CLOCK >> 4 & (uint32_t)0xF);
        if (cpu_c3_sel > 7) {
            cpu_c3_freq = cpu_c2_freq >> (cpu_c3_sel - 7);
        } else {
            cpu_c3_freq = cpu_c2_freq;
        }
        SystemCoreClock = cpu_c3_freq;
        break;
    case 2 :
        /* LSE */
        SystemCoreClock = LSE_Value;
        break;
    default : /* case 3 */
        /* LSI */
        SystemCoreClock = LSI_Value;
        break;
    }
}

/**
  * @brief  Setup the microcontroller system clock configuration.
  * @note   This function should be used only after reset.
  * @param  None
  * @retval None
  */
void SystemInit (void)
{
    // Включение тактирования RST_CLK и BKP_CLK
    MDR_RST_CLK->PER_CLOCK   = (uint32_t)0x8000010;

    // Устанавливается минимальная допустимая задержка для считывания команд
    //  из flash-памяти при работе на частоте 144 МГц
    MDR_EEPROM->CMD = 5 << EEPROM_CMD_DELAY_Pos;

    // Запуск внешнего кварца
    MDR_RST_CLK->HS_CONTROL = RST_CLK_HS_CONTROL_HSE_ON;
    while (!(MDR_RST_CLK->CLOCK_STATUS & RST_CLK_CLOCK_STATUS_HSE_RDY)) {}

    // Переключение CPU PLL на тактирование от внешнего кварца.
    // Частота внешнего кварца 16 МГц
    MDR_RST_CLK->CPU_CLOCK = 2 << RST_CLK_CPU_CLOCK_CPU_C1_SEL_Pos;

    // Инициализация CPU PLL:
    // 16 МГц * (8 + 1) = 144 МГц - максимальная гарантированная частота
    MDR_RST_CLK->PLL_CONTROL = 8 << RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos;
    // Включение CPU PLL
    MDR_RST_CLK->PLL_CONTROL |= RST_CLK_PLL_CONTROL_PLL_CPU_ON;
    while (!(MDR_RST_CLK->CLOCK_STATUS & RST_CLK_CLOCK_STATUS_PLL_CPU_RDY)) {}

    // Переключение CPU на тактирование от CPU PLL
    MDR_RST_CLK->CPU_CLOCK |= (1 << RST_CLK_CPU_CLOCK_HCLK_SEL_Pos)
        | (1 << RST_CLK_CPU_CLOCK_CPU_C2_SEL_Pos);
}
