# Sơ đồ Chân Ngoại Vi (Pinout) STM32F411RE Nucleo-64

| Thiết bị Ngoại vi | Chân STM32 | Chức năng Phần cứng | Thông số Tín hiệu |
| :--- | :--- | :--- | :--- |
| **Motor 1 (Bánh đà)** | `PA8` | TIM1_CH1 | PWM 20 kHz Active-Low |
| | `PC7` | GPIO Output | DIR (0 = CW, 1 = CCW) |
| | `PC8` | GPIO Output | BRAKE (1 = Run, 0 = Brake) |
| | `PA6` | TIM3_CH1 | Encoder Kênh A (Dây Nâu) |
| | `PA7` | TIM3_CH2 | Encoder Kênh B (Dây Cam) |
| **Motor 2 (Bánh sau)** | `PA11` | TIM1_CH4 | PWM 20 kHz Active-Low |
| | `PC6` | GPIO Output | DIR |
| | `PC9` | GPIO Output | BRAKE |
| | `PA15` | TIM2_CH1 | Encoder Kênh A |
| | `PB3` | TIM2_CH2 | Encoder Kênh B |
| **Servo Lái (MG995)** | `PB8` | TIM4_CH3 | PWM 50 Hz (1.0ms - 2.0ms) |
| | `PA4` | ADC1_IN4 | Chiết áp phản hồi góc lái |
| **IMU MPU-6050** | `PB6` | I2C1_SCL | Tốc độ 400 kHz Fast-Mode |
| | `PB7` | I2C1_SDA | Tốc độ 400 kHz Fast-Mode |
