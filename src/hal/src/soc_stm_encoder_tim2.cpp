/* Connections cho Motor 2 (Nidec 24H Banh sau) tren Nucleo-F411RE:
 * - PA_15 = TIM2_CH1 (Encoder A - Morpho CN7 Pin 17)
 * - PB_3  = TIM2_CH2 (Encoder B - Arduino D3 / CN9 Pin 4)
 * - Chay song song doc lap voi TIM1 (PWM) va TIM3 (Encoder Banh da)
 */
#include "soc_stm_encoder_tim2.h"
#include "mw_mbed_interface.h"
#ifdef TARGET_NUCLEO_F767ZI
#include "stm32f767xx.h"
#elif defined(TARGET_NUCLEO_F401RE)
#include "stm32f401xe.h"
#elif defined(TARGET_NUCLEO_F411RE)
#include "stm32f411xe.h"
#else
#error "Encoder supported with STM32 Nucleo-F767ZI, STM32 Nucleo-F401RE or STM32 Nucleo-F411RE boards."
#endif

void initEncoder_TIM2(void)
{
    // 1. Bat Clock cho GPIOA (Bit 0) va GPIOB (Bit 1) tren AHB1ENR
    RCC->AHB1ENR |= 0x00000003;

    // 2. Cau hinh PA15 (TIM2_CH1 - AF1)
    GPIOA->MODER   &= ~(GPIO_MODER_MODER15);
    GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPDR15);
    GPIOA->AFR[1]  &= ~(0xF0000000);
    GPIOA->MODER   |= (GPIO_MODER_MODER15_1);          // Alternate Function
    GPIOA->OTYPER  |= (GPIO_OTYPER_OT_15);
    GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR15);
    GPIOA->PUPDR   |= (GPIO_PUPDR_PUPDR15_0);          // Pull-up 3.3V
    GPIOA->AFR[1]  |= 0x10000000;                      // AF1 = TIM2_CH1 tren PA15

    // 3. Cau hinh PB3 (TIM2_CH2 - AF1)
    GPIOB->MODER   &= ~(GPIO_MODER_MODER3);
    GPIOB->PUPDR   &= ~(GPIO_PUPDR_PUPDR3);
    GPIOB->AFR[0]  &= ~(0x0000F000);
    GPIOB->MODER   |= (GPIO_MODER_MODER3_1);           // Alternate Function
    GPIOB->OTYPER  |= (GPIO_OTYPER_OT_3);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3);
    GPIOB->PUPDR   |= (GPIO_PUPDR_PUPDR3_0);           // Pull-up 3.3V
    GPIOB->AFR[0]  |= 0x00001000;                      // AF1 = TIM2_CH2 tren PB3

    // 4. Bat Clock cho TIM2 (Bit 0 cua APB1ENR)
    RCC->APB1ENR   |= 0x00000001;

    // 5. Cau hinh TIM2 chay Encoder Mode 3 (Dem 4x, gioi han 16-bit 0->65535)
    TIM2->CR1   = 0x0000;
    TIM2->SMCR  = 0x0003;     // SMS = 011 (Encoder mode 3)
    TIM2->CCMR1 = 0x0101;     // CC1S = 01, CC2S = 01
    TIM2->CCMR2 = 0x0000;
    TIM2->CCER  = 0x0011;     // CC1P, CC2P enable
    TIM2->PSC   = 0x0000;     // Prescaler = 0
    TIM2->ARR   = 0xFFFF;     // Ep ve 16-bit (65535) de dung chung bo unwrap voi TIM3
    TIM2->CNT   = 0x0000;     // Reset counter
    TIM2->CR1   = 0x0001;     // Enable counter (CEN = 1)
}

unsigned short int getEncoderCount_TIM2(void)
{
    return (unsigned short int)(TIM2->CNT & 0xFFFF);
}

unsigned short int getIndexCount_TIM2(unsigned short int *indexCount)
{
    *indexCount = 0;
    return 0;
}

void releaseEncoder_TIM2(void)
{
    TIM2->CR1 = 0x0000;
}
