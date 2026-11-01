# Project Key Facts - Dự án Xe Đạp Tự Cân Bằng UTH

## Thông số Cơ khí & Vật lý
- **Tổng khối lượng xe hoàn thiện**: $700\text{g} - 800\text{g}$ (bao gồm khung in 3D PETG/PLA, 2 motor Nidec, Pin 3S LiPo, bo Nucleo-64).
- **Khối lượng bánh đà phản lực**: $100\text{g}$ (đã cân thực tế trên bàn).
- **Cơ cấu truyền động bánh sau**: Dây đai răng GT2 bản 6mm, chu vi 240mm.

## Dữ liệu Cảm biến IMU MPU-6050 Thực nghiệm (2026-10-28)
- **Độ lệch tĩnh góc Roll khi xe thẳng đứng (`Roll_Offset`)**: **`+1.30°`** (chính xác `1.2987°`).
- **Nhiễu trôi tĩnh vận tốc góc (`Gyro_X_Rate`)**: **`< 0.02°/s`** (đo thực tế `-0.0195°/s` ở trạng thái đứng yên sau 200 chu kỳ calib).
- **Đáp ứng động Gyro**: Đạt xung $\pm 25^\circ/\text{s} \rightarrow \pm 42^\circ/\text{s}$ khi lắc khung xe, tín hiệu sạch và không bị trôi.

## Thông số Động cơ BLDC Nidec 24H (Bãi máy in)
- **Điện áp danh định**: 12V DC (Nguồn cấp từ Pin Li-Po 3S $11.1 - 12.6\text{V}$).
- **Tần số PWM điều khiển**: $20\text{ kHz}$ (Chu kỳ $50\text{ µs}$), Logic Active-Low (100% duty = dừng, 0% duty = max tốc).
- **Ngưỡng vùng chết khởi động**: `ZERO_DUTY = 94.0%` (tại $94\%$ duty motor bắt đầu quay $111\text{ RPM}$).
- **Bảng đo tốc độ thực nghiệm (12V không tải)**:
  - $1\% \rightarrow 111\text{ RPM}$
  - $10\% \rightarrow 434\text{ RPM}$
  - $30\% \rightarrow 989\text{ RPM}$ (Có tải bánh đà $100\text{g}$: $980 - 1000\text{ RPM}$)
  - $50\% \rightarrow 1560\text{ RPM}$
  - $100\% \rightarrow 3136\text{ RPM}$
- **Dòng điện thực nghiệm**:
  - Không tải: $10 - 80\text{ mA}$ (Công suất $\approx 0.2\text{W}$ từ mức $70\%$ ga).
  - Có tải bánh đà $100\text{g}$ khi đảo chiều ở $30\%$ ga: vọt lên tối đa $0.2 - 0.3\text{A}$ (đỉnh max $0.5\text{A}$, an toàn dưới $2\text{A}$).
- **Thời gian đảo chiều có tải bánh đà 100g ở mức ga 30%**: $\approx 1\text{ giây}$ từ $+990\text{ RPM} \rightarrow 0 \rightarrow -990\text{ RPM}$ (đặc tính quán tính bình thường).
- **Độ phân giải Encoder**: Đĩa từ $100\text{ line/rev}$, giải mã $4\text{x}$ trên STM32 ra $400\text{ counts/rev}$. Cấp nguồn 3.3V nhận đủ xung ở $20.9\text{ kHz}$ ($3136\text{ RPM}$).

## Cấu hình Chân STM32F411RE Nucleo-64
- **Motor 1 (Bánh đà)**: PWM `PA8` (TIM1_CH1), DIR `PC7`, BRAKE `PC8`, ENC `PA6`/`PA7` (TIM3).
  - Quy ước DIR đã test thực tế: Lệnh dương ($u \ge 0$) $\rightarrow$ `DIR = 0.0`, Lệnh âm ($u < 0$) $\rightarrow$ `DIR = 1.0`.
- **Motor 2 (Bánh sau)**: PWM `PA11` (TIM1_CH4), DIR `PC6`, BRAKE `PC9`, ENC `PA15`/`PB3` (TIM2).
- **Servo MG995**: PWM `PB8` (TIM4_CH3, 50 Hz), Wiper ADC `PA4` (ADC1_IN4, max 3.306V, Center RAW = 0.5055).
- **MPU-6050**: I2C1 `PB6` (SCL) và `PB7` (SDA), tốc độ 400 kHz, địa chỉ `0x68`.

### 2026-10-29 - Xác nhận chiều quay Kp dương & Tích hợp hoàn thiện Kw cùng Active Offset
- **Xác nhận chiều quay Kp**: Đã kiểm chứng thực tế $K_p > 0$ là chiều phản lực đúng, không cần đảo dấu âm.
- **Tích hợp khâu Kw**: Đưa khâu hồi tiếp tốc độ `Motor_RPM` trở lại luật điều khiển (`- Kw * Motor_RPM`) để ghìm tốc độ bánh đà không bị trôi lên trần 1800 RPM. Bắt đầu từ $K_w = 0.0$ và tăng dần.
- **Giới hạn công suất test tay**: Kẹp `MAX_GA = 40.0%` khi thử nghiệm trên tay để chống giật sốc và bảo vệ an toàn cho motor.

### 2026-10-29 - Tối ưu 3 điểm an toàn & toán học từ phản biện thực tế
1. **Khóa bảo vệ quá tốc theo MAX_GA**: Hạ ngưỡng ngắt quá tốc từ 1800 RPM xuống **1100 RPM** để khớp với trần công suất `MAX_GA = 40.0%` (ở 40% ga Nidec đạt max 1310 RPM không tải).
2. **Thu hẹp cửa sổ Calib góc**: Thu hẹp điều kiện kích hoạt calib tự động từ $< 8.0^\circ$ xuống **$|\text{Roll\_Angle}| < 4.0^\circ$** để tránh chốt nhầm điểm cân bằng khi xe bị giữ nghiêng quá lớn.
3. **Định lượng độ suy giảm lệnh của Kw**: Với $K_{\text{mot}} \approx 31.36\text{ RPM/\%}$, lệnh bị co theo hệ số $\frac{1}{1 + 31.36 K_w}$. Xác định dải khởi đầu an toàn $K_w = 0.002 - 0.005$ (giảm $6\% - 14\%$ lệnh bù) để không làm yếu lực cân bằng.

## Sự thật kỹ thuật tối thượng: Nguồn 24V (6S LiPo) trong thiết kế gốc của Dimitri (2026-10-29)
- **Nguồn cấp thiết kế gốc của Dimitri (Carbon Aeronautics)**: **6S Li-Po battery (22.2V - 25.2V)** qua DC-DC Buck converter 24V -> 5V.
- **Động cơ Nidec 24H ở 24V**:
  - Tốc độ cực đại: **~6000 RPM**.
  - Dải gia tốc dự trữ tại 1500 RPM: Vẫn còn **18V điện áp khả dụng** (Back-EMF chỉ chiếm 25%), mô-men gia tốc cực đại vẫn duy trì 75%.
  - Công suất tức thời gấp **4 LẦN** so với chạy ở 12V.
- **Sự khác biệt khi chạy 12V (3S LiPo hiện tại)**:
  - Tốc độ cực đại bị bóp nghẽn ở **3136 RPM**.
  - Tại 1500–1800 RPM, Back-EMF đã chiếm 50%–60% điện áp cấp (12V) $\rightarrow$ mô-men gia tốc tụt dốc thảm hại, không còn lực kéo xe dậy.

## Tài liệu Đối sánh Thực nghiệm: Báo cáo HCMUTE 2024 (BBRW)
- **Nguồn**: *Journal of Fuzzy Systems and Control*, Vol. 2, No. 2, 2024. Nhóm SV ĐH Sư phạm Kỹ thuật TP.HCM (HCMUTE) dưới sự hướng dẫn của TS. Nguyễn Văn Đông Hải.
- **Mô hình**: Khung xe Thingiverse (thing:5887157), khối lượng xe  = 434	ext{g}$, bánh đà  = 43	ext{g}$ ( = 1.6 	imes 10^{-4}	ext{ kg}\cdot	ext{m}^2$).
- **Phần cứng**: Arduino Nano, MPU-6050 ( = 3	ext{ ms}$), Động cơ DC chổi than ( = 6.83\ \Omega$) điều khiển qua mạch cầu H L298N.
- **Kết quả thực nghiệm**:
  - Điểm cân bằng thực tế lệch tĩnh: Setpoint buộc phải đặt **hBc1.0^\circ* đến **hBc4.0^\circ* (không thể đặt /usr/bin/bash^\circ$).
  - Khâu tích phân $: Khi tăng $ từ 5 lên 30, thời gian xác lập nổ tung lên **55 giây** và gây mất ổn định (hệ thống dao động mạnh).
  - Điểm yếu cốt lõi: Không có Encoder đo tốc độ bánh đà, không có cơ chế giải bão hòa tốc độ (Desaturation), xe phải dựa vào bánh xe in 3D to bè để phụ trợ thăng bằng.
- **Giá trị học thuật cho đồ án UTH**: Dùng làm tài liệu tham khảo đối sánh trực tiếp trong báo cáo môn học, chứng minh giải pháp BLDC Nidec 24H + Encoder TIM3 + Điều chế điểm đặt động (Adaptive Setpoint) của UTH vượt trội hoàn toàn so với mô hình DC + L298N kinh điển.
