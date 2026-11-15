#ifndef SOC_STM_USART1_H
#define SOC_STM_USART1_H

#ifdef __cplusplus
extern "C" {
#endif

void initUSART1_BLE(void);
void stepUSART1_BLE(double enable_out, double active_offset,
                    double roll_angle, double motor_rpm,
                    double *enable_sw, double *drive_spd, double *steer_ang,
                    double *kp, double *kd, double *k_trim, double *nom_ofs);
void releaseUSART1_BLE(void);

#ifdef __cplusplus
}
#endif

#endif
