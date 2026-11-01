# Bảng Phân Bổ Chân Ngoại Vi STM32F411RE Nucleo-64

| Khối Chức Năng | Chân Vi Điều Khiển | Tài Nguyên Phần Cứng | Đặc Tính Tín Hiệu |
| :--- | :--- | :--- | :--- |
| **Động cơ bánh đà** | `PA8` | `TIM1_CH1` | Điều rộng xung 20 kHz, tích cực mức thấp |
| | `PC7` | `GPIO Output` | Chiều quay: `0` là chiều thuận, `1` là chiều nghịch |
| | `PC8` | `GPIO Output` | Phanh điện từ: `1` là nhả phanh, `0` là đóng phanh |
| | `PA6` | `TIM3_CH1` | Kênh A bộ mã hóa vòng quay |
| | `PA7` | `TIM3_CH2` | Kênh B bộ mã hóa vòng quay |
| **Động cơ bánh sau** | `PA11` | `TIM1_CH4` | Điều rộng xung 20 kHz, tích cực mức thấp |
| | `PC6` | `GPIO Output` | Điều khiển chiều quay bánh sau |
| | `PC9` | `GPIO Output` | Điều khiển phanh bánh sau |
| | `PA15` | `TIM2_CH1` | Kênh A bộ mã hóa vòng quay |
| | `PB3` | `TIM2_CH2` | Kênh B bộ mã hóa vòng quay |
| **Cơ cấu lái Servo MG995** | `PB8` | `TIM4_CH3` | Điều rộng xung 50 Hz, độ rộng từ 1.0 ms đến 2.0 ms |
| | `PA4` | `ADC1_IN4` | Đọc điện áp chiết áp phản hồi góc lái |
| **Cảm biến quán tính MPU-6050** | `PB6` | `I2C1_SCL` | Xung nhịp giao tiếp I2C tốc độ 400 kHz |
| | `PB7` | `I2C1_SDA` | Đường dữ liệu giao tiếp I2C tốc độ 400 kHz |
