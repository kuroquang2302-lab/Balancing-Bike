# Hệ Thống Xe Đạp Tự Cân Bằng Sử Dụng Bánh Đà Phản Lực Và Bẻ Lái Trợ Lực

[![MATLAB Simulink](https://img.shields.io/badge/MATLAB-Simulink-orange.svg)](https://www.mathworks.com/)
[![Target MCU](https://img.shields.io/badge/STM32-F411RE%20Nucleo--64-blue.svg)](https://www.st.com/)
[![Actuator](https://img.shields.io/badge/BLDC-Nidec%2024H%2012V-green.svg)]()

Dự án nghiên cứu và phát triển hệ thống xe đạp hai bánh tự cân bằng kết hợp giữa cơ cấu bánh đà phản lực cho trạng thái đứng yên tại chỗ và cơ cấu bẻ lái trợ lực cho trạng thái di chuyển tiến. Toàn bộ thuật toán xử lý tín hiệu và điều khiển vòng kín được thiết kế theo phương pháp dựa trên mô hình trên nền tảng MATLAB/Simulink và triển khai trực tiếp trên vi điều khiển STM32F411RE Nucleo-64.

---

## Điểm Nổi Bật Về Kỹ Thuật

- **Kiến trúc điều khiển hai vòng lặp phân tầng:** Vòng trong sử dụng bộ điều khiển tỉ lệ - vi sai tốc độ cao kết hợp giảm chấn vận tốc bánh đà để ổn định góc nghiêng tức thời; vòng ngoài tự động điều chỉnh điểm đặt cân bằng nhằm khử bão hòa vận tốc bánh đà về vùng lân cận điểm không.
- **Phân bổ độc lập 4 bộ định thời phần cứng:** Cấu hình tách biệt TIM1 cho phát xung điều rộng xung tần số 20 kHz, TIM2 và TIM3 cho giải mã xung Encoder chế độ nhân bốn phần cứng, và TIM4 cho điều khiển góc lái.
- **Thuật toán điều khiển động cơ an toàn:** Tích hợp cơ chế ngắt xung một chu kỳ khi đảo chiều quay, giới hạn mềm công suất khi tiệm cận tốc độ tối đa và quy trình phanh hai giai đoạn bảo vệ cơ khí.
- **Bộ lọc Kalman thích nghi một chiều:** Tự động ước lượng độ lệch tĩnh của con quay hồi chuyển khi khởi động và điều chỉnh hiệp phương sai nhiễu đo theo biên độ gia tốc tổng hợp để loại bỏ nhiễu rung cơ khí.

---

## Cấu Trúc Thư Mục

```text
Balancing-Bike/
├── README.md                              # Tài liệu tổng quan dự án
├── docs/
│   ├── project_notes/
│   │   ├── bugs.md                        # Nhật ký lỗi kỹ thuật và biện pháp khắc phục
│   │   ├── decisions.md                   # Hồ sơ quyết định thiết kế kiến trúc
│   │   ├── key_facts.md                   # Thông số vật lý và dữ liệu thực nghiệm
│   │   └── issues.md                      # Nhật ký tiến độ triển khai
│   ├── hardware/
│   │   └── pinouts.md                     # Sơ đồ phân bổ chân vi điều khiển STM32F411RE
│   └── theory/
│       └── dual_loop_control.md           # Cơ sở lý thuyết động học và điều khiển hai vòng lặp
├── src/
│   ├── controllers/
│   │   └── fcn_Balance_Controller.m       # Khối điều khiển cân bằng hai vòng lặp
│   ├── drivers/
│   │   └── fcn_Nidec_Driver.m             # Khối điều khiển công suất và phanh động cơ Nidec 24H
│   ├── filters/
│   │   └── fcn_MPU6050_Filter.m           # Khối lọc Kalman thích nghi cho cảm biến MPU-6050
│   └── hal/
│       ├── EncoderBlock_TIM3.m            # Khối giao tiếp phần cứng đọc Encoder TIM3
│       ├── Setup_Encoder_TIM3.m           # Kịch bản khởi tạo mã nguồn ngoại vi TIM3
│       ├── Setup_Encoder_TIM2.m           # Kịch bản khởi tạo mã nguồn ngoại vi TIM2
│       ├── include/soc_stm_encoder_tim3.h # Tệp tiêu đề giao tiếp thanh ghi TIM3
│       └── src/soc_stm_encoder_tim3.cpp   # Tệp thực thi cấu hình thanh ghi TIM3
```

---

