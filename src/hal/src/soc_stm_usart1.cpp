/* =========================================================================
 * GIAO TIEP BLE QUA NGOAI VI USART1 VA DMA2 TREN STM32F411RE
 * Chan tin hieu : PA9 - USART1_TX, PA10 - USART1_RX (115200 bps, 8N1)
 * ========================================================================= */

#include "soc_stm_usart1.h"
#include "mw_mbed_interface.h"
#include <stdint.h>
#include <math.h>

#if defined(TARGET_NUCLEO_F411RE) || defined(TARGET_NUCLEO_F401RE)
#include "stm32f411xe.h"
#endif

#define RX_DMA_BUF_SIZE        64U
#define TX_RING_BUF_SIZE       64U
#define WATCHDOG_TIMEOUT_STEPS 100U

static volatile uint8_t s_rx_dma_buf[RX_DMA_BUF_SIZE];
static uint32_t         s_rx_tail          = 0U;
static uint8_t          s_win[14];
static uint8_t          s_win_count        = 0U;
static uint32_t         s_watchdog_cnt     = 0U;

static uint8_t          s_tx_buf[TX_RING_BUF_SIZE];
static uint8_t          s_tx_head          = 0U;
static uint8_t          s_tx_tail          = 0U;
static uint8_t          s_prev_enable_out  = 0U;
static double           s_last_sent_ofs    = -1.005;
static uint32_t         s_telem_cnt        = 0U;

static double           s_enable_sw        = 0.0;
static double           s_drive_spd        = 0.0;
static double           s_steer_ang        = 0.0;
static double           s_kp               = 46.0;
static double           s_kd               = 2.00;
static double           s_ktrim            = 0.036;
static double           s_nom_ofs          = -1.005;

static void pushTxChar(uint8_t c)
{
    uint8_t next_head = (uint8_t)((s_tx_head + 1U) & (TX_RING_BUF_SIZE - 1U));
    if (next_head != s_tx_tail)
    {
        s_tx_buf[s_tx_head] = c;
        s_tx_head           = next_head;
    }
}

static void pushSignedInt(int32_t val)
{
    if (val < 0)
    {
        pushTxChar((uint8_t)'-');
        val = -val;
    }
    char digits[12];
    int n = 0;
    do
    {
        digits[n++] = (char)('0' + (val % 10));
        val /= 10;
    } while (val > 0 && n < 12);
    while (n > 0)
    {
        pushTxChar((uint8_t)digits[--n]);
    }
}

static void pushScaled2(double val)
{
    int32_t scaled = (int32_t)((val >= 0.0) ? (val * 100.0 + 0.5) : (val * 100.0 - 0.5));
    if (scaled < 0)
    {
        pushTxChar((uint8_t)'-');
        scaled = -scaled;
    }
    pushSignedInt(scaled / 100);
    pushTxChar((uint8_t)'.');
    int32_t frac = scaled % 100;
    pushTxChar((uint8_t)('0' + (frac / 10)));
    pushTxChar((uint8_t)('0' + (frac % 10)));
}

static void queueLiveTelemetry(double roll_angle, double motor_rpm, double ofs_val, uint8_t cur_en)
{
    int32_t rpm_int = (int32_t)((motor_rpm >= 0.0) ? (motor_rpm + 0.5) : (motor_rpm - 0.5));
    pushTxChar((uint8_t)'A');
    pushTxChar((uint8_t)':');
    pushScaled2(roll_angle);
    pushTxChar((uint8_t)' ');
    pushTxChar((uint8_t)'R');
    pushTxChar((uint8_t)':');
    pushSignedInt(rpm_int);
    pushTxChar((uint8_t)' ');
    pushTxChar((uint8_t)'O');
    pushTxChar((uint8_t)':');
    pushScaled2(ofs_val);
    pushTxChar((uint8_t)' ');
    pushTxChar((uint8_t)'E');
    pushTxChar((uint8_t)':');
    pushTxChar((uint8_t)(cur_en ? '1' : '0'));
    pushTxChar((uint8_t)'\n');
}

void initUSART1_BLE(void)
{
#if defined(TARGET_NUCLEO_F411RE) || defined(TARGET_NUCLEO_F401RE)
    RCC->AHB1ENR |= 0x00000001U | (1U << 22);
    RCC->APB2ENR |= (1U << 4);

    GPIOA->MODER   &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
    GPIOA->MODER   |=  (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1);
    GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT_9 | GPIO_OTYPER_OT_10);
    GPIOA->OSPEEDR |=  (GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10);
    GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPDR9 | GPIO_PUPDR_PUPDR10);
    GPIOA->PUPDR   |=  (GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0);
    GPIOA->AFR[1]  &= ~0x00000FF0U;
    GPIOA->AFR[1]  |=  0x00000770U;

    DMA2_Stream2->CR &= ~1U;
    uint32_t timeout = 10000U;
    while ((DMA2_Stream2->CR & 1U) != 0U && timeout > 0U)
    {
        timeout--;
    }
    DMA2->LIFCR        = 0x003D0000U;
    DMA2_Stream2->PAR  = (uint32_t)&(USART1->DR);
    DMA2_Stream2->M0AR = (uint32_t)s_rx_dma_buf;
    DMA2_Stream2->NDTR = RX_DMA_BUF_SIZE;
    DMA2_Stream2->FCR  = 0x00000000U;
    DMA2_Stream2->CR   = (4U << 25) | (2U << 16) | (1U << 10) | (1U << 8);
    DMA2_Stream2->CR  |= 1U;

    USART1->CR1 = 0x0000U;
    USART1->CR2 = 0x0000U;
    USART1->CR3 = (1U << 6);

    uint32_t ppre2 = (RCC->CFGR >> 13) & 0x07U;
    uint32_t pclk2 = SystemCoreClock;
    if (ppre2 >= 4U)
    {
        pclk2 >>= (ppre2 - 3U);
    }
    USART1->BRR = (pclk2 + 57600U) / 115200U;
    USART1->CR1 = (1U << 13) | (1U << 3) | (1U << 2);
#endif
    s_rx_tail         = 0U;
    s_win_count       = 0U;
    s_watchdog_cnt    = 0U;
    s_tx_head         = 0U;
    s_tx_tail         = 0U;
    s_prev_enable_out = 0U;
}

void stepUSART1_BLE(double enable_out, double active_offset,
                    double roll_angle, double motor_rpm,
                    double *enable_sw, double *drive_spd, double *steer_ang,
                    double *kp, double *kd, double *k_trim, double *nom_ofs)
{
#if defined(TARGET_NUCLEO_F411RE) || defined(TARGET_NUCLEO_F401RE)
    uint32_t sr = USART1->SR;
    if ((sr & 0x0FU) != 0U)
    {
        volatile uint32_t dummy = USART1->DR;
        (void)dummy;
    }

    uint32_t dma_head = (RX_DMA_BUF_SIZE - (DMA2_Stream2->NDTR & (RX_DMA_BUF_SIZE - 1U))) & (RX_DMA_BUF_SIZE - 1U);
    bool frame_received = false;

    while (s_rx_tail != dma_head)
    {
        uint8_t b = s_rx_dma_buf[s_rx_tail];
        s_rx_tail = (s_rx_tail + 1U) & (RX_DMA_BUF_SIZE - 1U);

        for (int i = 0; i < 13; ++i)
        {
            s_win[i] = s_win[i + 1];
        }
        s_win[13] = b;
        if (s_win_count < 14U)
        {
            s_win_count++;
        }

        if (s_win_count >= 14U && s_win[0] == 0xAAU && s_win[1] == 0x55U)
        {
            uint8_t xor_sum = 0U;
            for (int i = 2; i <= 12; ++i)
            {
                xor_sum ^= s_win[i];
            }
            if (xor_sum == s_win[13])
            {
                uint8_t  en_u8     = s_win[2];
                int8_t   spd_i8    = (int8_t)s_win[3];
                int8_t   str_i8    = (int8_t)s_win[4];
                uint16_t kp_u16    = (uint16_t)s_win[5] | ((uint16_t)s_win[6] << 8);
                uint16_t kd_u16    = (uint16_t)s_win[7] | ((uint16_t)s_win[8] << 8);
                uint16_t ktrim_u16 = (uint16_t)s_win[9] | ((uint16_t)s_win[10] << 8);
                int16_t  ofs_i16   = (int16_t)((uint16_t)s_win[11] | ((uint16_t)s_win[12] << 8));

                s_enable_sw = (en_u8 > 0U) ? 1.0 : 0.0;
                s_drive_spd = (double)spd_i8;
                s_steer_ang = (double)str_i8;
                s_kp        = (double)kp_u16 * 0.1;
                s_kd        = (double)kd_u16 * 0.01;
                s_ktrim     = (double)ktrim_u16 * 0.01;
                s_nom_ofs   = (double)ofs_i16 * 0.01;

                s_win_count    = 0U;
                frame_received = true;
            }
        }
    }

    if (frame_received)
    {
        s_watchdog_cnt = 0U;
    }
    else
    {
        if (s_watchdog_cnt < WATCHDOG_TIMEOUT_STEPS)
        {
            s_watchdog_cnt++;
        }
        else
        {
            s_enable_sw = 0.0;
            s_drive_spd = 0.0;
            s_steer_ang = 0.0;
        }
    }

    uint8_t cur_en = (enable_out >= 0.5) ? 1U : 0U;
    s_telem_cnt++;
    if ((cur_en == 1U && s_prev_enable_out == 0U) ||
        (s_telem_cnt >= 100U && s_tx_tail == s_tx_head))
    {
        queueLiveTelemetry(roll_angle, motor_rpm, active_offset, cur_en);
        s_last_sent_ofs = active_offset;
        s_telem_cnt     = 0U;
    }
    s_prev_enable_out = cur_en;

    if (s_tx_tail != s_tx_head && ((USART1->SR & (1U << 7)) != 0U))
    {
        USART1->DR = s_tx_buf[s_tx_tail];
        s_tx_tail  = (uint8_t)((s_tx_tail + 1U) & (TX_RING_BUF_SIZE - 1U));
    }
#else
    (void)enable_out;
    (void)active_offset;
    (void)roll_angle;
    (void)motor_rpm;
#endif

    *enable_sw = s_enable_sw;
    *drive_spd = s_drive_spd;
    *steer_ang = s_steer_ang;
    *kp        = s_kp;
    *kd        = s_kd;
    *k_trim    = s_ktrim;
    *nom_ofs   = s_nom_ofs;
}

void releaseUSART1_BLE(void)
{
#if defined(TARGET_NUCLEO_F411RE) || defined(TARGET_NUCLEO_F401RE)
    DMA2_Stream2->CR &= ~1U;
    USART1->CR1       = 0x0000U;
#endif
}
