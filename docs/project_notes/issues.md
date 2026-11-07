# Tiến Độ Phát Triển Hệ Thống

### Giai đoạn 1: Giao tiếp ngoại vi và cảm biến quán tính
- Cấu hình giao tiếp I2C Fast-Mode $400\text{ kHz}$ đọc dữ liệu thô $14\text{ byte}$ từ cảm biến MPU-6050 và xây dựng bộ lọc Kalman 1D.
- Khởi tạo các bộ định thời phần cứng `TIM1`, `TIM2`, `TIM3`, `TIM4` và kênh đọc tương tự `ADC1_IN4` trên vi điều khiển STM32F411RE.

### Giai đoạn 2: Xây dựng trình điều khiển động cơ và máy trạng thái hiệu chuẩn
- Thiết kế khối `fcn_Nidec_Driver` điều khiển xung PWM $20\text{ kHz}$ Active-Low, bảo vệ ngắt xung khi đảo chiều quay và phanh hai giai đoạn.
- Xây dựng máy trạng thái tự động hiệu chuẩn góc cân bằng tĩnh, cơ chế khóa lỗi khi ngã `fault_latched` và tự động mở khóa khi dựng xe thẳng đứng.

### Giai đoạn 3: Tối ưu hóa bộ lọc Kalman thích nghi và khử trễ pha
- Thiết lập cơ chế báo bận `Gyro_X_Rate = 999.0` trong $200$ chu kỳ đầu để đồng bộ thời gian khởi tạo giữa bộ lọc và bộ điều khiển.
- Điều chỉnh động hiệp phương sai nhiễu đo theo biên độ gia tốc tổng hợp nhằm loại bỏ nhiễu gia tốc tiếp tuyến và nâng hệ số lọc thông thấp vận tốc góc lên $0.60$.

### Giai đoạn 4: Hoàn thiện kiến trúc điều khiển cân bằng hai vòng lặp
- Tích hợp khâu bù trước mô-men trọng lực, bộ điều khiển PID vòng trong có kẹp chống bão hòa tích phân Anti-windup và bộ giới hạn độ dốc lệnh Slew-rate.
- Kết hợp vòng điều khiển ngoài điều chỉnh điểm đặt động theo phản hồi vận tốc bánh đà từ `TIM3` và hàm suy giảm tuyến tính công suất ở tốc độ cao.

### Giai đoạn 5: Tích hợp cầu nối điều khiển không dây BLE trên ESP32-C3 Super Mini
- Xây dựng firmware máy chủ BLE 5.0 theo chuẩn Nordic UART Service (NUS) trên vi điều khiển ESP32-C3 Super Mini, tương thích với ứng dụng điều khiển trên hệ điều hành iOS.
- Thiết kế giao thức đóng gói khung nhị phân 14 byte có mã kiểm tra XOR truyền qua ngoại vi `USART1` (`PA9`/`PA10`) ở tần số $50\text{ Hz}$ để điều khiển hướng lái, vận tốc và tinh chỉnh thông số bộ điều khiển tại chỗ.
