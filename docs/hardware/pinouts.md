# Bảng Phân Bổ Chân Ngoại Vi STM32F411RE Nucleo-64

## 1. Sơ Đồ Kết Nối Tín Hiệu Điều Khiển Và Đo Lường

| Khối Chức Năng | Chân Vi Điều Khiển | Tài Nguyên Phần Cứng | Đặc Tính Tín Hiệu |
| :--- | :--- | :--- | :--- |
| **Động cơ bánh đà** | `PA8` | `TIM1_CH1` | Điều rộng xung 20 kHz, tích cực mức thấp |
| | `PC7` | `GPIO Output` | Chiều quay: `0` là chiều thuận, `1` là chiều nghịch |
| | `PC8` | `GPIO Output` | Phanh điện từ: `1` là nhả phanh, `0` là đóng phanh |
| | `PA6` | `TIM3_CH1` | Kênh A bộ mã hóa vòng quay chế độ nhân bốn phần cứng |
| | `PA7` | `TIM3_CH2` | Kênh B bộ mã hóa vòng quay chế độ nhân bốn phần cứng |
| **Động cơ bánh sau** | `PA11` | `TIM1_CH4` | Điều rộng xung 20 kHz, tích cực mức thấp |
| | `PC6` | `GPIO Output` | Điều khiển chiều quay bánh sau |
| | `PC9` | `GPIO Output` | Điều khiển phanh bánh sau |
| | `PA15` | `TIM2_CH1` | Kênh A bộ mã hóa vòng quay bánh sau |
| | `PB3` | `TIM2_CH2` | Kênh B bộ mã hóa vòng quay bánh sau |
| **Cơ cấu lái Servo MG995** | `PB8` | `TIM4_CH3` | Điều rộng xung 50 Hz, độ rộng từ 1.0 ms đến 2.0 ms |
| | `PA4` | `ADC1_IN4` | Đọc điện áp chiết áp phản hồi góc lái từ 0 V đến 3.3 V |
| **Cảm biến quán tính MPU-6050** | `PB6` | `I2C1_SCL` | Xung nhịp giao tiếp I2C tốc độ 400 kHz |
| | `PB7` | `I2C1_SDA` | Đường dữ liệu giao tiếp I2C tốc độ 400 kHz |
| **Khối truyền thông không dây** | `PA9` | `USART1_TX` | Đường truyền dữ liệu nối tiếp lên thiết bị giám sát |
| | `PA10` | `USART1_RX` | Đường nhận lệnh điều khiển từ thiết bị giám sát |

---

## 2. Quy Tắc Phân Phối Nguồn Điện

1. **Nguồn động lực 12V:** Cấp trực tiếp từ khối pin Li-Po 3S có dải điện áp từ 11.1 V đến 12.6 V tới hai động cơ không chổi than Nidec 24H.
2. **Nguồn hạ áp 5V:** Sử dụng mạch ổn áp xung hạ áp từ 12V xuống 5V dòng tối đa 3 A để cấp nguồn riêng cho cơ cấu lái Servo MG995 và đường nguồn đầu vào của bo mạch Nucleo-64, ngăn ngừa hiện tượng sụt áp khi cơ cấu lái chịu tải.
3. **Nguồn logic 3.3V:** Lấy từ ngõ ra ổn áp tuyến tính 3.3V trên bo mạch STM32F411RE Nucleo-64 để cấp cho cảm biến quán tính MPU-6050, mạch kéo lên của bộ mã hóa vòng quay và khối truyền thông không dây. Toàn bộ các khối chức năng được nối chung điểm chuẩn điện áp không.
