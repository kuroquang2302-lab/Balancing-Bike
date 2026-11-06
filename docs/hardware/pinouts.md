# Sơ Đồ Phân Bổ Chân Ngoại Vi STM32F411RE Nucleo-64

## 1. Bảng Kết Nối Tín Hiệu Phần Cứng

| Phân Hệ | Chân MCU | Ngoại Vi | Đặc Tính Tín Hiệu |
| :--- | :--- | :--- | :--- |
| **Động cơ bánh đà Nidec 24H** | `PA8` | `TIM1_CH1` | Xung PWM $20\text{ kHz}$ Active-Low, ngưỡng khởi động $94.5\%$ |
| | `PC7` | `GPIO Output` | Chân chiều quay `DIR`: `0` thuận, `1` nghịch |
| | `PC8` | `GPIO Output` | Chân phanh `BRAKE`: `1` nhả phanh, `0` đóng phanh |
| | `PA6` | `TIM3_CH1` | Kênh A Quadrature Encoder chế độ $4\text{x}$ phần cứng |
| | `PA7` | `TIM3_CH2` | Kênh B Quadrature Encoder chế độ $4\text{x}$ phần cứng |
| **Động cơ bánh sau Nidec 24H** | `PA11` | `TIM1_CH4` | Xung PWM $20\text{ kHz}$ Active-Low |
| | `PC6` | `GPIO Output` | Chân chiều quay `DIR` bánh sau |
| | `PC9` | `GPIO Output` | Chân phanh `BRAKE` bánh sau |
| | `PA15` | `TIM2_CH1` | Kênh A Quadrature Encoder bánh sau |
| | `PB3` | `TIM2_CH2` | Kênh B Quadrature Encoder bánh sau |
| **Cơ cấu lái Servo MG995** | `PB8` | `TIM4_CH3` | Xung PWM $50\text{ Hz}$, độ rộng xung $1.0\text{ ms} - 2.0\text{ ms}$ |
| | `PA4` | `ADC1_IN4` | Ngõ vào tương tự $0\text{ V} - 3.3\text{ V}$ đọc chiết áp góc lái |
| **Cảm biến IMU MPU-6050** | `PB6` | `I2C1_SCL` | Xung nhịp I2C Fast-Mode $400\text{ kHz}$ |
| | `PB7` | `I2C1_SDA` | Dữ liệu I2C Fast-Mode $400\text{ kHz}$, địa chỉ `0x68` |
| **Truyền thông UART / BLE** | `PA9` | `USART1_TX` | Truyền dữ liệu nối tiếp |
| | `PA10` | `USART1_RX` | Nhận dữ liệu nối tiếp |

---

## 2. Phân Phối Nguồn Điện

- **Tầng động lực $12\text{ V}$:** Pin Li-Po 3S $11.1\text{ V} - 12.6\text{ V}$ cấp trực tiếp cho đường nguồn công suất của hai động cơ BLDC Nidec 24H.
- **Tầng hạ áp $5\text{ V}$:** Mạch hạ áp DC-DC Buck $5\text{ V} - 3\text{ A}$ cấp nguồn cho Servo MG995 và chân nguồn của bo mạch Nucleo-64.
- **Tầng logic $3.3\text{ V}$:** Nguồn ổn áp tuyến tính $3.3\text{ V}$ trên Nucleo-64 cấp cho cảm biến MPU-6050 và điện trở kéo lên của các kênh Encoder. Toàn hệ thống nối chung mass `GND`.
