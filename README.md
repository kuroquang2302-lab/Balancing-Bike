# Hệ Thống Xe Đạp Tự Cân Bằng Sử Dụng Bánh Đà Phản Lực Và Bẻ Lái Trợ Lực

[![MATLAB Simulink](https://img.shields.io/badge/MATLAB-Simulink-orange.svg)](https://www.mathworks.com/)
[![Target MCU](https://img.shields.io/badge/STM32-F411RE%20Nucleo--64-blue.svg)](https://www.st.com/)
[![Actuator](https://img.shields.io/badge/BLDC-Nidec%2024H%2012V-green.svg)]()
[![Status](https://img.shields.io/badge/Trang%20Thai-Da%20Kiem%20Chung%2075s-brightgreen.svg)]()
[![University](https://img.shields.io/badge/Truong-UTH%20TP.HCM-red.svg)](https://ut.edu.vn/)

Dự án nghiên cứu và phát triển hệ thống xe đạp hai bánh tự cân bằng kết hợp giữa cơ cấu bánh đà phản lực cho trạng thái đứng yên tại chỗ và cơ cấu bẻ lái trợ lực cho trạng thái di chuyển tiến. Toàn bộ thuật toán xử lý tín hiệu và điều khiển vòng kín được thiết kế theo phương pháp dựa trên mô hình trên nền tảng MATLAB/Simulink và triển khai trực tiếp trên vi điều khiển STM32F411RE Nucleo-64.

---

## Điểm Nổi Bật Về Kỹ Thuật

- **Kiến trúc điều khiển lai hợp bốn tầng hai vòng lặp:** Tầng thứ nhất tính toán bù trước mô-men trọng lực theo góc nghiêng thực tế; tầng thứ hai sử dụng bộ điều khiển tỉ lệ - tích phân - vi sai tốc độ cao kết hợp giảm chấn vận tốc bánh đà để sinh mô-men hồi phục góc tĩnh và ổn định thân xe tức thời; tầng thứ ba giới hạn tốc độ biến thiên lệnh và bù vùng chết khởi động tại ngưỡng 94.5%; tầng thứ tư ở vòng ngoài tự động điều chỉnh điểm đặt cân bằng nhằm khử bão hòa vận tốc bánh đà về vùng lân cận điểm không.
- **Phân bổ độc lập các bộ định thời và ngoại vi phần cứng:** Cấu hình tách biệt TIM1 cho phát xung điều rộng xung tần số 20 kHz, TIM2 và TIM3 cho giải mã xung bộ mã hóa vòng quay chế độ nhân bốn phần cứng không tiêu tốn chu kỳ vi xử lý, TIM4 cho điều khiển góc lái Servo MG995, ADC1 cho đọc chiết áp phản hồi góc lái và USART1 cho truyền thông không dây.
- **Thuật toán điều khiển động cơ an toàn:** Tích hợp cơ chế ngắt xung một chu kỳ khi đảo chiều quay, giới hạn mềm công suất khi tiệm cận tốc độ tối đa và quy trình phanh điện từ hai giai đoạn bảo vệ cơ khí.
- **Bộ lọc Kalman thích nghi một chiều:** Tự động ước lượng độ lệch tĩnh của con quay hồi chuyển khi khởi động và điều chỉnh hiệp phương sai nhiễu đo theo biên độ gia tốc tổng hợp để loại bỏ nhiễu rung cơ khí.
- **Tối ưu hóa mô-men quán tính và khối lượng cơ khí:** Tinh giản bánh đà phản lực từ cấu hình 21 cụm ốc 100.0 g xuống cấu hình 7 cụm bu lông M4x16 và 28 đai ốc M4 bố trí đối xứng tâm đạt khối lượng 73.3 g, giúp động cơ Nidec 24H ở mức nguồn 12V tăng tốc góc nhanh hơn 35% trong khi tổng khối lượng toàn xe hoàn thiện đạt 780 g đến 820 g.

---

## Tài Liệu Và Công Trình Thực Nghiệm Tham Khảo

Đề tài kế thừa, kiểm chứng và phát triển dựa trên các công trình nghiên cứu học thuật và thực nghiệm mở quốc tế:

| Tác Giả Và Nguồn Công Bố | Liên Kết Tài Liệu Và Mã Nguồn | Giá Trị Khoa Học Và Kỹ Thuật Kế Thừa |
| :--- | :--- | :--- |
| **Tom Stanton** | [Can Reaction Wheels control a Drone?](https://youtu.be/4kfBEaTncjI) | Phân tích vật lý cơ cấu bánh đà phản lực, hiện tượng bão hòa vận tốc góc và yêu cầu đáp ứng gia tốc nhanh khi điều khiển tư thế. |
| **Every Flavor of Robot** | [Reaction Cube Situation Is Crazy](https://youtu.be/OeoIK913tRk) | Chứng minh sự khác biệt giữa điều khiển mô-men xoắn trực tiếp và điều khiển vận tốc; đề xuất cơ chế dịch chuyển điểm đặt vòng ngoài để khử bão hòa tốc độ động cơ. |
| **James Bruton** | [What is the Difference Between Reaction Wheels and Gyroscopes?](https://youtu.be/pJfMFUcquWM) | Phân biệt nguyên lý bánh đà phản lực và con quay hồi chuyển điều khiển mô-men; chứng minh trên bộ truyền động điều khiển vận tốc, thành phần tích phân sai số góc đóng vai trò sinh mô-men phục hồi tĩnh. |
| **Sydney Madueke** | [Building a SELF-BALANCING reaction wheel](https://youtu.be/Cegxa2BuCUc) | Thực nghiệm tối ưu hóa tỉ số mô-men quán tính bằng cách thu gọn khối lượng bánh đà và khung cơ khí, kết hợp thành phần tích phân để triệt tiêu độ lệch góc xác lập. |
| **RemRC** | [Self-Balancing-Bike](https://github.com/remrc/Self-Balancing-Bike), [3-Reaction-Wheels](https://github.com/remrc/3-Reaction-Wheels-Self-Balancing) và [Thingiverse 5887157](https://www.thingiverse.com/thing:5887157) | Thiết kế mẫu cơ khí xe đạp in 3D nguyên bản sử dụng động cơ Nidec 24H, làm cơ sở để đề tài kế thừa kết cấu khung và nâng cấp lên kiến trúc điều khiển hai vòng lặp trên STM32F411RE. |
| **BD594 James Cochrane** | [Build Your Own - 1D Wannabe Cubli Reaction Wheel](https://www.youtube.com/watch?v=vqV6nazapFQ) và [Kho mã nguồn](https://github.com/squirtifier/DIY---1D-Wannabe-Cubli-Reaction-Wheel) | Thiết kế mạch bảo vệ xung điện áp ngược và cấu hình điện trở kéo lên 10 kΩ cho ngõ ra cực máng hở của bộ mã hóa vòng quay trên động cơ Nidec 24H. |
| **Dmitrii Tomin** | [Printables 956136](https://www.printables.com/model/956136-3d-printed-self-balancing-bike) và [Kho mã nguồn](https://github.com/Dmitrii-Tomin/self-balancing_bike) | Quy trình lắp ghép cơ khí chuẩn hóa 23 bước và thông số in 3D thực nghiệm cho toàn bộ chi tiết khung xe. |
| **Tạp chí Journal of Fuzzy Systems and Control Năm 2024** | Bài báo *PID Control for Balancing Bike Model using Reaction Wheel*, Tập 2, Số 2, DOI: `10.59247/jfsc.v2i2.188` | Công trình đối sánh trong nước sử dụng động cơ điện một chiều có chổi than và bộ điều khiển một vòng lặp với thời gian xác lập 55 giây, làm cơ sở đối chứng hiệu quả vượt trội của kiến trúc hai vòng lặp trong đề tài. |

---

## Cấu Trúc Thư Mục

```text
Balancing-Bike/
├── README.md                              # Tài liệu tổng quan dự án
├── AI_INSTRUCTIONS.md                     # Quy chuẩn quản lý tri thức và mã nguồn dự án
├── .gitignore                             # Cấu hình loại trừ tệp biên dịch tạm
├── docs/
│   ├── references.md                      # Phân tích chi tiết các công trình tham khảo
│   ├── project_notes/
│   │   ├── bugs.md                        # Nhật ký lỗi kỹ thuật và biện pháp khắc phục
│   │   ├── decisions.md                   # Hồ sơ quyết định thiết kế kiến trúc
│   │   ├── key_facts.md                   # Thông số vật lý và dữ liệu thực nghiệm
│   │   └── issues.md                      # Nhật ký tiến độ triển khai
│   ├── hardware/
│   │   └── pinouts.md                     # Sơ đồ phân bổ chân vi điều khiển STM32F411RE
│   └── theory/
│       └── dual_loop_control.md           # Cơ sở lý thuyết động học và điều khiển lai hợp
├── src/
│   ├── controllers/
│   │   └── fcn_Balance_Controller.m       # Khối điều khiển cân bằng lai hợp bốn tầng
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
└── scripts/
    └── push_to_github.sh                  # Kịch bản đồng bộ mã nguồn lên GitHub
```

---

## Đồng Bộ Mã Nguồn Lên Kho Lưu Trữ

Chạy các lệnh sau tại thư mục gốc của dự án để cập nhật mã nguồn lên nhánh chính:

```bash
git add .
git commit -m "feat: update four-layer hybrid controller, verified references, and mass budget"
git push origin main
```
