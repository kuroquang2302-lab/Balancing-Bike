#ifndef _SOC_STM_ENCODER_TIM2_H_
#define _SOC_STM_ENCODER_TIM2_H_

#ifdef __cplusplus
extern "C" {
#endif

void initEncoder_TIM2(void);
unsigned short int getEncoderCount_TIM2(void);
unsigned short int getIndexCount_TIM2(unsigned short int *indexCount);
void releaseEncoder_TIM2(void);

#ifdef __cplusplus
}
#endif

#endif // _SOC_STM_ENCODER_TIM2_H_
