/* Connections for Nidec 24H on Nucleo-F411RE:
 * - PA_6 = TIM3_CH1 (Encoder A - Brown wire)
 * - PA_7 = TIM3_CH2 (Encoder B - Orange wire)
 */
#include "soc_stm_encoder_tim3.h"
#include "mw_mbed_interface.h"
#if defined(TARGET_NUCLEO_F411RE) || defined(TARGET_NUCLEO_F401RE)
#include "stm32f411xe.h"
#endif

void initEncoder_TIM3(void)
{
    RCC->AHB1ENR |= 0x00000001; // GPIOA Clock
    GPIOA->MODER   &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7);
    GPIOA->AFR[0]  &= ~(0xFF000000);

    GPIOA->MODER   |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); // AF mode
    GPIOA->OTYPER  |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);
    GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7);
    GPIOA->PUPDR   |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0); // Pull-up

    GPIOA->AFR[0]  |= 0x22000000; // AF2 (TIM3)

    RCC->APB1ENR   |= 0x00000002; // TIM3 Clock
    TIM3->CR1   = 0x0000;
    TIM3->SMCR  = 0x0003;     // Encoder mode 3 (4x)
    TIM3->CCMR1 = 0x0101;     // CC1S = 01, CC2S = 01
    TIM3->CCMR2 = 0x0000;
    TIM3->CCER  = 0x0011;     // CC1P, CC2P enable
    TIM3->PSC   = 0x0000;     // Prescaler = 0
    TIM3->ARR   = 0xFFFF;     // 16-bit
    TIM3->CNT   = 0x0000;     // Reset
    TIM3->CR1   = 0x0001;     // CEN = 1
}

unsigned short int getEncoderCount_TIM3(void)
{
    return ((TIM_TypeDef *)TIM3)->CNT;
}

unsigned short int getIndexCount_TIM3(unsigned short int *indexCount)
{
    *indexCount = 0;
    return 0;
}

void releaseEncoder_TIM3(void)
{
    TIM3->CR1 = 0x0000;
}
