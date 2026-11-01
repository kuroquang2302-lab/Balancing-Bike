#ifndef SOC_STM_ENCODER_TIM3_H
#define SOC_STM_ENCODER_TIM3_H

#ifdef __cplusplus
extern "C" {
#endif

void initEncoder_TIM3(void);
unsigned short int getEncoderCount_TIM3(void);
unsigned short int getIndexCount_TIM3(unsigned short int *indexCount);
void releaseEncoder_TIM3(void);

#ifdef __cplusplus
}
#endif

#endif
