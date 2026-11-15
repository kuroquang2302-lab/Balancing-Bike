/* Connections cho Nidec 24H tren Nucleo-F411RE:
 * - PA_6 = TIM3_CH1 (Encoder A - Day Nau)
 * - PA_7 = TIM3_CH2 (Encoder B - Day Cam)
 * - Da loai bo ngat PA_4 de bao ve kenh ADC Chiet ap Servo
 */
#include "soc_stm_encoder_tim3.h"
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

void initEncoder_TIM3(void)
{
    // 1. Bat Clock cho GPIOA (Bit 0 cua AHB1ENR)
    RCC->AHB1ENR |= 0x00000001;

    // 2. Xoa sach bit cau hinh cu cua PA6 va PA7
    GPIOA->MODER   &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7);
    GPIOA->AFR[0]  &= ~(0xFF000000);

    // 3. Cau hinh PA6, PA7 sang Alternate Function (MODER = 10), Pull-up 3.3V
    GPIOA->MODER   |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
    GPIOA->OTYPER  |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);
    GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7);
    GPIOA->PUPDR   |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0);

    // 4. Gan Alternate Function AF2 (TIM3_CH1 & TIM3_CH2) cho PA6 va PA7
    GPIOA->AFR[0]  |= 0x22000000;

    // 5. Bat Clock cho TIM3 (Bit 1 cua APB1ENR)
    RCC->APB1ENR   |= 0x00000002;

    // 6. Cau hinh TIM3 chay Encoder Mode 3 (Dem 4x ca 2 kenh A va B)
    TIM3->CR1   = 0x0000;
    TIM3->SMCR  = 0x0003;     // SMS = 011 (Encoder mode 3)
    TIM3->CCMR1 = 0x0101;     // CC1S = 01, CC2S = 01
    TIM3->CCMR2 = 0x0000;
    TIM3->CCER  = 0x0011;     // CC1P, CC2P enable
    TIM3->PSC   = 0x0000;     // Prescaler = 0
    TIM3->ARR   = 0xFFFF;     // 16-bit full range (0 -> 65535)
    TIM3->CNT   = 0x0000;     // Reset counter
    TIM3->CR1   = 0x0001;     // Enable counter (CEN = 1)
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
