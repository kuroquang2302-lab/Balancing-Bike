# Self-Balancing Bicycle using Reaction Wheel & Steering Control (UTH)

[![MATLAB Simulink](https://img.shields.io/badge/MATLAB-Simulink%20MBD-orange.svg)](https://www.mathworks.com/)
[![Target MCU](https://img.shields.io/badge/MCU-STM32F411RE%20Nucleo--64-blue.svg)](https://www.st.com/)
[![Actuator](https://img.shields.io/badge/BLDC-Nidec%2024H%2012V-green.svg)]()
[![Status](https://img.shields.io/badge/Balance%20Run-75s%20Verified-brightgreen.svg)]()

Hệ thống xe đạp tự cân bằng hai bánh (Hybrid Self-Balancing Bicycle) kết hợp giữa **Bánh đà phản lực (Reaction Wheel)** cho trạng thái đứng yên và **Bẻ lái trợ lực (Steering Control Servo MG995)** cho trạng thái di chuyển tiến. Dự án được phát triển theo phương pháp luận Thiết kế dựa trên Mô hình (Model-Based Design - MBD) 100% bằng Simulink trên bo mạch nhúng STM32F411RE Nucleo-64.

---

## 🎯 Điểm nổi bật & Cải tiến Kỹ thuật
- **Kiến trúc Điều khiển 2 Vòng lặp (Dual-Loop Architecture):** Vòng trong PD góc cực nhanh kết hợp Vòng ngoài Điều chế điểm đặt động (Adaptive Setpoint Modulation) giúp bánh đà tự giải bão hòa tốc độ về quanh 0 RPM.
- **Tận dụng 4 Bộ Timer Phần cứng STM32:** TIM1 (2 kênh PWM 20 kHz cho BLDC Nidec), TIM2 & TIM3 (Encoder chế độ 4x đọc xung phần cứng), TIM4 (PWM 50 Hz cho Servo).
- **Bộ Driver Nidec 24H Thông minh:** Tích hợp cơ chế bảo vệ Dead-time 1 chu kỳ khi đảo chiều và Bộ tuần tự phanh 2 nhịp (0.5s thả trôi chống sốc, sau đó mới đóng phanh cưỡng bức).
- **Đối sánh Học thuật Quốc tế & Trong nước:** Hoàn thiện và khắc phục các giới hạn của nghiên cứu HCMUTE 2024 (tránh trễ pha 55s của khâu $K_i$ góc) và bài báo quốc tế Justin Yoon et al. (2026).

---

## 📁 Cấu trúc Thư mục Dự án

```text
Balancing-Bike/
├── README.md                      # Tài liệu tổng quan & hướng dẫn kỹ thuật
├── AI_INSTRUCTIONS.md             # Bộ quy tắc bộ nhớ dự án cho AI & IDE Tools
├── .gitignore                     # Cấu hình bỏ qua file tạm Simulink & Coder
├── docs/
│   ├── project_notes/             # Bộ nhớ dự án (Project Memory)
│   │   ├── bugs.md                # Nhật ký lỗi thực tế & giải pháp khắc phục
│   │   ├── decisions.md           # Hồ sơ quyết định kiến trúc (ADR-001 -> ADR-009)
│   │   ├── key_facts.md           # Dữ liệu đo đạc thực nghiệm, thông số Nidec & Pinout
│   │   └── issues.md              # Bảng theo dõi tiến độ công việc
│   ├── hardware/
│   │   └── pinouts.md             # Bảng phân bổ chân STM32F411RE chi tiết
│   └── theory/
│       └── dual_loop_control.md   # Động học con lắc ngược & luật điều khiển 2 vòng
├── src/
│   ├── controllers/
│   │   └── fcn_Balance_Controller.m # Khối MATLAB Function điều khiển cân bằng
│   ├── drivers/
│   │   └── fcn_Nidec_Driver.m       # Khối phát xung PWM & điều khiển phanh Nidec 24H
│   ├── filters/
│   │   └── fcn_MPU6050_Filter.m     # Bộ lọc bù góc Roll & trừ trôi tĩnh Gyro
│   └── hal/
│       ├── EncoderBlock_TIM3.m      # MATLAB System Object đọc Encoder TIM3 (PA6/PA7)
│       ├── EncoderBlock_TIM2.m      # MATLAB System Object đọc Encoder TIM2 (PA15/PB3)
│       ├── include/soc_stm_encoder_tim3.h
│       └── src/soc_stm_encoder_tim3.cpp
└── scripts/
    └── push_to_github.sh          # Kịch bản 1 lệnh đẩy toàn bộ dự án lên GitHub
```

---

## ⚡ Hướng dẫn Đẩy lên GitHub trong 1 Dòng lệnh

Nếu bạn đã clone repo về máy local:
```bash
git add .
git commit -m "feat: complete balancing bike project architecture, dual-loop controller, and memory"
git push origin main
```

Sau khi đẩy lên, Graphify MCP Server trên workspace `quang-s-workspace` sẽ tự động index toàn bộ đồ thị mã nguồn (Code Graph) với đầy đủ cấu trúc AST và liên kết phụ thuộc!
