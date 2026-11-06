# Hệ Thống Xe Đạp Tự Cân Bằng Sử Dụng Bánh Đà Phản Lực Và Cơ Cấu Lái Servo

[![MATLAB Simulink](https://img.shields.io/badge/MATLAB-Simulink-orange.svg)](https://www.mathworks.com/)
[![Target MCU](https://img.shields.io/badge/STM32-F411RE%20Nucleo--64-blue.svg)](https://www.st.com/)
[![Actuator](https://img.shields.io/badge/BLDC-Nidec%2024H%2012V-green.svg)]()

Hệ thống điều khiển nhúng cho mô hình xe đạp hai bánh tự cân bằng sử dụng bánh đà phản lực kết hợp cơ cấu lái Servo MG995. Toàn bộ thuật toán lọc tín hiệu, máy trạng thái hiệu chuẩn và điều khiển vòng kín được thiết kế trên MATLAB/Simulink ở tần số lấy mẫu $200\text{ Hz}$ và biên dịch trực tiếp xuống vi điều khiển ARM Cortex-M4 STM32F411RE.

---

## Kiến Trúc Kỹ Thuật

- **Điều khiển cân bằng phân tầng hai vòng lặp:**
  - **Vòng trong $200\text{ Hz}$:** Kết hợp bù trước mô-men trọng lực phi tuyến $u_{\text{grav}} = -K_g \sin(e)$, luật điều khiển PID góc nghiêng có kẹp chống bão hòa tích phân Anti-windup và giảm chấn vận tốc góc bánh đà. Tích hợp giới hạn độ dốc lệnh Slew-rate $\Delta u \le 8.0\% / 5\text{ ms}$ và suy giảm tuyến tính công suất trong dải $2500\text{ RPM} - 3000\text{ RPM}$.
  - **Vòng ngoài:** Điều chỉnh điểm đặt góc cân bằng $\theta_{\text{offset}}$ theo phản hồi vận tốc bánh đà từ bộ đếm TIM3 nhằm khử độ lệch khối tâm tĩnh và đưa vận tốc bánh đà về vùng lân cận $0\text{ RPM}$.
- **Giao tiếp công suất Nidec 24H:** Điều khiển xung PWM $20\text{ kHz}$ logic Active-Low, bù vùng chết khởi động tại ngưỡng Duty Cycle $94.5\%$, ngắt xung bảo vệ $1$ chu kỳ mẫu khi đảo mức logic chân chiều quay `DIR` và đóng phanh điện từ `BRAKE` sau $0.5\text{ s}$ thả trôi.
- **Lọc trạng thái cảm biến MPU-6050:** Bộ lọc Kalman 1D ước lượng góc nghiêng Roll và vận tốc góc trục X qua giao tiếp I2C $400\text{ kHz}$, tự động xác định độ trôi tĩnh Gyro trong $200$ chu kỳ đầu và điều chỉnh ma trận hiệp phương sai nhiễu đo $R$ theo biên độ gia tốc tổng hợp.
- **Định thời ngoại vi phần cứng độc lập:**
  - `TIM1`: Phát $2$ kênh xung PWM $20\text{ kHz}$ điều khiển động cơ bánh đà và động cơ dẫn động bánh sau.
  - `TIM3` và `TIM2`: Giải mã phần cứng Quadrature Encoder chế độ $4\text{x}$ độ phân giải $400\text{ PPR}$.
  - `TIM4` và `ADC1_IN4`: Phát xung PWM $50\text{ Hz}$ điều khiển Servo MG995 và đọc điện áp chiết áp phản hồi góc lái.

---

## Công Trình Và Tài Liệu Tham Khảo

| Tác Giả / Nguồn | Tài Liệu / Mã Nguồn | Nội Dung Kỹ Thuật Đối Chiếu |
| :--- | :--- | :--- |
| **Tom Stanton** | [Can Reaction Wheels control a Drone?](https://youtu.be/4kfBEaTncjI) | Động lực học bánh đà phản lực, giới hạn bão hòa vận tốc góc và yêu cầu điều khiển vòng kín qua vùng $0\text{ RPM}$. |
| **Every Flavor of Robot** | [Reaction Cube Situation Is Crazy](https://youtu.be/OeoIK913tRk) | Phân tích điều khiển mô-men FOC và thuật toán dịch chuyển điểm đặt vòng ngoài để giải bão hòa tốc độ bánh đà. |
| **James Bruton** | [What is the Difference Between Reaction Wheels and Gyroscopes?](https://youtu.be/pJfMFUcquWM) | Đặc tính đạo hàm vận tốc trên bộ truyền động Velocity Mode; vai trò của thành phần tích phân góc $K_i$ trong việc sinh mô-men phục hồi tĩnh. |
| **Sydney Madueke** | [Building a SELF-BALANCING reaction wheel](https://youtu.be/Cegxa2BuCUc) | Tối ưu tỉ số mô-men quán tính giữa bánh đà với khung cơ khí và khử sai số góc xác lập bằng khâu tích phân. |
| **RemRC** | [Self-Balancing-Bike](https://github.com/remrc/Self-Balancing-Bike), [3-Reaction-Wheels](https://github.com/remrc/3-Reaction-Wheels-Self-Balancing) và [Thingiverse 5887157](https://www.thingiverse.com/thing:5887157) | Thiết kế cơ khí gốc sử dụng động cơ Nidec 24H và luật điều khiển phản hồi trạng thái trên Arduino Nano. |
| **BD594 James Cochrane** | [Build Your Own - 1D Wannabe Cubli Reaction Wheel](https://www.youtube.com/watch?v=vqV6nazapFQ) và [DIY 1D Wannabe Cubli](https://github.com/squirtifier/DIY---1D-Wannabe-Cubli-Reaction-Wheel) | Mạch bảo vệ xung áp ngược và cấu hình điện trở kéo lên $10\text{ k}\Omega$ cho ngõ ra Encoder cực máng hở của Nidec 24H. |
| **Dmitrii Tomin** | [Printables 956136](https://www.printables.com/model/956136-3d-printed-self-balancing-bike) và [self-balancing_bike](https://github.com/Dmitrii-Tomin/self-balancing_bike) | Quy trình lắp ráp cơ khí $23$ bước và cấu hình in 3D cho khung xe. |
| **Journal of Fuzzy Systems and Control 2024** | *PID Control for Balancing Bike Model using Reaction Wheel*, Vol. 2, No. 2, DOI: `10.59247/jfsc.v2i2.188` | Mô hình đối sánh dùng động cơ DC + L298N và bộ điều khiển PID một vòng lặp không có phản hồi tốc độ bánh đà. |

---

## Cấu Trúc Thư Mục

```text
Balancing-Bike/
├── README.md                              # Tổng quan kiến trúc hệ thống
├── .gitignore                             # Loại trừ tệp sinh tự động của Simulink Coder
├── docs/
│   ├── references.md                      # Phân tích kỹ thuật các công trình tham khảo
│   ├── hardware/
│   │   └── pinouts.md                     # Sơ đồ chân ngoại vi STM32F411RE và phân phối nguồn
│   ├── theory/
│   │   └── dual_loop_control.md           # Mô hình toán học và cấu trúc điều khiển hai vòng lặp
│   └── project_notes/
│       ├── decisions.md                   # Hồ sơ quyết định thiết kế kiến trúc
│       ├── key_facts.md                   # Thông số vật lý và đặc tính thực nghiệm phần cứng
│       ├── bugs.md                        # Nhật ký lỗi kỹ thuật và giải pháp xử lý
│       └── issues.md                      # Tiến độ phát triển các phân hệ
└── src/
    ├── controllers/
    │   └── fcn_Balance_Controller.m       # Khối điều khiển cân bằng hai vòng lặp
    ├── drivers/
    │   └── fcn_Nidec_Driver.m             # Khối giao tiếp PWM 20 kHz, chiều quay và phanh Nidec 24H
    ├── filters/
    │   └── fcn_MPU6050_Filter.m           # Khối lọc Kalman 1D thích nghi cho MPU-6050
    └── hal/
        ├── EncoderBlock_TIM3.m            # System Object đọc bộ đếm phần cứng TIM3
        ├── Setup_Encoder_TIM3.m           # Kịch bản khởi tạo mã nguồn C/C++ cho TIM3
        ├── Setup_Encoder_TIM2.m           # Kịch bản khởi tạo mã nguồn C/C++ cho TIM2
        ├── include/soc_stm_encoder_tim3.h # Khai báo nguyên mẫu hàm thanh ghi TIM3
        └── src/soc_stm_encoder_tim3.cpp   # Thực thi cấu hình thanh ghi GPIOA và TIM3
```
