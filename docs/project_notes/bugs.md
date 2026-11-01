# Bug Log - Dự án Xe Đạp Tự Cân Bằng UTH

### 2026-10-28 - Đảo ngược cực tính chân DIR Motor Nidec 24H
- **Issue**: Khi phát lệnh dương (`u >= 0`), bánh đà quay ngược chiều mong muốn so với chiều nghiêng của khung xe.
- **Root Cause**: Driver tích hợp bên trong Nidec 24H quy định chân 5 (DIR) mức LOW (0V) là quay theo chiều kim đồng hồ (CW) và mức HIGH (3.3V) là quay ngược chiều kim đồng hồ (CCW). Mã code ban đầu gán `1.0` cho lệnh dương và `0.0` cho lệnh âm.
- **Solution**: Đảo lại logic gán chân DIR trong `fcn_Nidec_Driver`: `if u >= 0 -> desired_dir = 0.0; else desired_dir = 1.0;`.
- **Prevention**: Luôn kiểm tra độc lập chiều quay bằng lệnh hằng số thủ công (`+20%` và `-20%`) trên bàn trước khi ghép vào vòng lặp điều khiển tự động.

### 2026-10-25 - Lỗi phanh cưỡng bức khi lệnh điều khiển đi qua điểm 0
- **Issue**: Bánh đà bị khựng giật cơ học liên tục mỗi khi lệnh điều khiển lướt qua vùng lân cận 0.
- **Root Cause**: Đoạn code ban đầu gộp chung `Enable_Run == 0` với `abs(Speed_Cmd) < 0.5%` và kéo chân `Brake_PC8 = 0.0` (kích hoạt phanh cứng điện từ của Nidec).
- **Solution**: Tách bạch 2 trạng thái: Chỉ khi tắt hẳn (`Enable_Run < 0.5`) hoặc xe ngã mới kéo `Brake_PC8 = 0.0`; còn khi lệnh gần 0 (`abs(u) < 0.5%`) thì giữ `Brake_PC8 = 1.0` (nhả phanh) và đưa `PWM_Duty = 100.0%` để bánh đà thả trôi tự do (coasting).
- **Prevention**: Không bao giờ kích hoạt chân phanh nhị phân (digital brake) trong vùng làm việc bình thường của bộ điều khiển cân bằng liên tục.

### 2026-10-23 - Xung đột chân ngoại vi giữa khối Encoder gốc và chân PWM/ADC
- **Issue**: Khối Encoder gốc của MathWorks (`soc_stm_encoder.cpp`) chiếm chân `PA8` (TIM1) và chân `PA4` (ngắt Index).
- **Root Cause**: Khối mẫu viết cho núm xoay Arduino cắm chân D7/D8 và ngắt chân PA4. Chân `PA8` đang dùng phát xung PWM bánh đà, chân `PA4` đang dùng đọc ADC chiết áp servo.
- **Solution**: Tạo khối độc lập `EncoderBlock_TIM3` cấu hình `TIM3` trên `PA6`/`PA7`, gỡ bỏ hoàn toàn ngắt `PA4`.
- **Prevention**: Khi dùng driver ngoại vi từ ví dụ mẫu, luôn kiểm tra sơ đồ thanh ghi và bảng chân pinout STM32 để tránh xung đột chân chức năng.

### 2026-10-28 - Lỗi đảo dấu khâu hồi tiếp vận tốc bánh đà (Kw, Kiw) gây kẹt tốc độ cao và nóng motor
- **Issue**: Khi đặt dấu trừ (`u = u_balance - u_desaturate`), xe không tự đưa tốc độ bánh đà về 0 RPM được mà bị kẹt nghiêng một bên, làm motor quay liên tục ở 2000–2500 RPM, ăn dòng 2.5A và nóng 50–60°C.
- **Root Cause**: 
  1. Con lắc ngược bánh đà là hệ pha không cực tiểu (Non-Minimum Phase) kết hợp với Nidec 24H là motor điều khiển tốc độ ($\dot{\omega}_m = (31.36 u - \omega_m)/\tau_m$).
  2. Kiểm chứng ma trận trạng thái 4 biến bằng công thức Ackermann cho thấy: Khi $K_p > 0, K_d > 0$, bắt buộc **$K_w > 0$ và $K_{iw} > 0$ phải mang dấu CỘNG (`+`)** (cùng dấu với $K_p \cdot \text{err}$), đúng như trong code gốc của Remrc (`+ K3*m_vel + K4*a_vel`) và Dimitri. Nếu mang dấu trừ, hệ kín xuất hiện cực thực dương (`+1.30`) gây mất ổn định.
  3. Ngoài ra, việc giữ xe nghiêng cố định bằng tay trên không ngăn cản khung xe trở về $0^\circ$, khiến bộ điều khiển bơm ga liên tục lên 100%.
- **Solution**: 
  1. Khôi phục dấu CỘNG (`+`) chuẩn LQR: `u_calc = (Kp * err_angle) + (Kd * Gyro_X_Rate) + (Kw * Motor_RPM) + (Kiw * int_w)` (với hệ số điều chỉnh đồng bộ theo dấu của `Kp`).
  2. Giới hạn trần công suất `MAX_CMD = 70.0%` (tương ứng ngưỡng 0.2W không tải của Nidec) và tự động ngắt tích phân khi cầm tay giữ nghiêng lâu để bảo vệ nhiệt cho motor.
- **Prevention**: Luôn kiểm tra cực (eigenvalues) của ma trận hệ kín $A - BK$ trước khi đổi dấu các khâu hồi tiếp trạng thái.

### 2026-10-29 - Lỗi biến chưa định nghĩa err_raw trong khối State Machine
- **Issue**: Khối `fcn_Balance_Controller` báo lỗi biên dịch Coder: `Undefined function or variable 'err_raw'`.
- **Root Cause**: Dòng khai báo đặt tên là `err_check = Roll_Angle - Roll_Offset;` nhưng dòng điều kiện `if` bên dưới lại gõ nhầm thành `abs(err_raw) < 3.5`.
- **Solution**: Đổi đồng nhất thành `err_check` trong cả 2 dòng: `if (abs(Gyro_X_Rate) < 2.0) && (abs(err_check) < 4.0)`.
- **Prevention**: Thực hiện kiểm tra code tĩnh và test chạy script mô phỏng trước khi nạp vào Simulink Coder.

### 2026-10-29 - Lỗi tự ý thay đổi Active_Offset trong vòng lặp điều khiển khâu Kiw
- **Issue**: Khi hệ thống đang chạy ở chế độ cân bằng, giá trị trên khối Display `Active_Offset` bị trôi và chạy liên tục theo tốc độ động cơ, gây hiểu lầm là bộ Calib không lưu được giá trị hoặc bị lỗi.
- **Root Cause**: Khâu thích nghi $Kiw$ được lập trình nắn trực tiếp vào biến `saved_offset = saved_offset - shift_rate` thay vì tác động vào biến điều khiển $u$. Điều này vi phạm nguyên tắc thiết kế: Offset cân bằng vật lý đã calib thì phải cố định tĩnh 100%.
- **Solution**: 
  1. Khóa chết hoàn toàn biến `saved_offset` sau khi lấy mẫu 1.5s ở State 1, bảo đảm ngõ ra `Active_Offset` đứng yên tuyệt đối trên Display.
  2. Đưa khâu tích phân tốc độ $Kiw$ về đúng phương trình điều khiển vòng kín $u$: $u = K_p \cdot err + K_d \cdot Gyro_X - K_w \cdot Motor\_RPM - K_{iw} \cdot \int Motor\_RPM \cdot dt$.
- **Prevention**: Tuyệt đối không thay đổi điểm đặt (setpoint/offset) tĩnh trong vòng lặp điều khiển thời gian thực; mọi hiệu chỉnh bù mô-men tĩnh phải đi qua khâu tích phân của biến đầu ra $u$.

### 2026-10-29 - Tối ưu hóa điều khiển bão hòa bánh đà và Anti-windup (Review từ đồ thị thực nghiệm)
- **Issue**: Bánh đà bị dao động chậm cỡ $\pm 1800$ RPM và ngã ngay sau khi chạm trần tốc độ; xuất hiện bước nhảy giật khi vượt 1000 RPM và tích phân bị mất đột ngột khi góc chao đảo $> 3^\circ$.
- **Root Cause**:
  1. Cắt ga về 0 thô bạo ở 1850 RPM làm mất khả năng giảm tốc và có thể gây hãm động năng (phản điện áp) thay vì thả trôi.
  2. Hệ số $Kw \times 2.5$ bị gián đoạn nhảy bậc (step jump) khi chạm mốc 1000 RPM.
  3. Reset `int_rpm = 0.0` mỗi khi $|err| \ge 3^\circ$ làm mất lực giữ tĩnh đúng lúc xe cần nhất; thiếu code thực thi Anti-windup đóng băng tích phân khi bão hòa.
- **Solution**:
  1. Thay thế ngưỡng cắt bằng bộ giới hạn một chiều liên tục: `Speed_Cmd = sign(Motor_RPM) * min(abs(Speed_Cmd), abs(Motor_RPM)/30.0)` khi $|RPM| > 1850$ và đang bơm thêm tốc độ.
  2. Thay $Kw \times 2.5$ bằng hàm cản vận tốc phi tuyến liên tục $C^0$: `extra = 1.5 * Kw * sign(Motor_RPM) * max(abs(Motor_RPM) - 1000.0, 0.0)`.
  3. Giữ nguyên tích lũy `int_rpm` và thực thi chuẩn Anti-windup đóng băng `int_rpm = int_prev` khi $|u| \ge MAX\_GA$ và tích phân cùng chiều làm tăng độ bão hòa.
- **Prevention**: Luôn đảm bảo tính liên tục ($C^0$ continuity) của luật điều khiển và kiểm chứng các hiệu ứng điện áp trung hòa trên phần cứng BLDC.

### 2026-10-29 - Phát hiện mâu thuẫn nguồn cấp 12V (3S) vs 24V (6S) của Dimitri
- **Issue**: Bánh đà quay nhanh nhưng không đủ lực vực xe dậy; khi tốc độ đạt 1200–1800 RPM thì mất hoàn toàn khả năng cứu xe.
- **Root Cause**: Thiết kế cơ khí bánh đà 13cm (100g) của Dimitri được tính toán cho hệ thống chạy **Pin 6S (24V)**, trong đó Nidec 24H quay tới 6000 RPM và công suất đỉnh gấp 4 lần. Khi chạy ở 12V (Pin 3S), trần tốc độ chỉ có 3136 RPM, khiến động cơ bị bão hòa điện áp (Back-EMF xấp xỉ 12V) ngay từ 1500 RPM, làm mô-men gia tốc $\tau = J \alpha$ tụt về 0.
- **Solution**:
  - Hướng 1 (Chuẩn thiết kế Dimitri): Nâng nguồn cấp motor lên 24V (ghép 2 pack 3S nối tiếp hoặc dùng nguồn tổ ong 24V trên bàn test).
  - Hướng 2 (Nếu bắt buộc giữ 12V 3S): Giảm bớt tải trọng viền bánh đà (tháo bớt 40 con ốc để giảm quán tính $J$, giúp motor 12V tăng tốc $\alpha$ nhanh hơn với mô-men hạn chế) và siết chặt dải cân bằng trong $\pm 1.0^\circ$.

### 2026-10-30 - Tối ưu hóa Torque Kick theo điều kiện RPM và mở trần RPM_LIMIT lên 3000
- **Issue**: Khi xe bị đẩy nghiêng, khâu Torque Kick tăng Kp đẩy motor chạm bão hòa ga MAX_GA quá sớm ở góc 1.2 độ; đoạn cắt Speed_Cmd = 0 ở 2700 RPM làm triệt tiêu mô-men cứu xe đúng lúc xe cần lực nhất.
- **Root Cause**: Torque Kick kích hoạt không điều kiện kể cả khi motor đã ở tốc độ cao; trần RPM_LIMIT = 2700 cắt lệnh về 0 khiến driver thả trôi (coasting) và mất hoàn toàn khả năng sinh mô-men hãm.
- **Solution**:
  1. Chỉ kích hoạt Torque Kick khi motor còn khoảng dự trữ tốc độ (`abs(Motor_RPM) < 1800.0` và `0.8° <= |err| <= 3.0°`).
  2. Tăng cường cản/xả tốc độ khi RPM cao (`speed_term = 0.004 * Motor_RPM` khi `|RPM| > 1800`).
  3. Mở trần RPM_LIMIT lên 3000.0 để không bóp chết lực cứu xe của motor Nidec ở nguồn 12V.
- **Prevention**: Luôn phân tách vùng làm việc của cơ cấu chấp hành theo tốc độ thực tế; không kích hoạt tăng ga khi cơ cấu chấp hành đã gần bão hòa.

### 2026-10-31 - 4 Lỗ hổng Động học & An toàn phát hiện bởi Review thực nghiệm (v2)
- **Bug 1 (Calib không kiểm tra góc)**: Xe nằm sàn 35° vẫn tự ghi đè `saved_offset = 35.0°` vì cửa sổ cũ chỉ 0.1s và không có cổng kiểm tra góc danh định. Khắc phục: Bổ sung cổng `abs(Roll_Angle - NOMINAL_OFFSET) < 4.0°` và kéo dài cửa sổ lên 1.0s (200 mẫu).
- **Bug 2 (Bảo vệ ngã không chốt)**: Khi xe ngã (>8°) rồi dựng lại (<8°), motor tự động bật lại với ga cực đại -95%, gây nguy hiểm. Khắc phục: Bổ sung chốt lỗi `fault_latched = true`, chỉ cho phép reset (re-arm) khi gạt `Enable_Switch < 0.5`.
- **Bug 3 (Bảo vệ quá tốc dạng On/Off gây rung chấn)**: Quanh ngưỡng 3000 RPM, lệnh ga nhảy 45% -> 0% -> 45% gây giật nảy motor. Khắc phục: Áp dụng suy giảm tuyến tính (Soft Limiter) từ 2500 đến 3000 RPM.
- **Bug 4 (MPU6050 Gyro bias chưa được trừ)**: Biến `gyro_bias` trong bộ lọc bị bỏ quên ở 0.0 khiến calib bị tê liệt nếu gyro trôi > 1.5 deg/s. Khắc phục: Tự động lấy trung bình 200 mẫu đầu tiên (1.0s) sau khi bật nguồn để tính chính xác `gyro_bias`.

### 2026-11-01 - Giải phẫu file TESTNIDEC.slx: Phát hiện "Bẫy xung đột Calib 0.0°" và Nhiễm độc Bias Kalman
- **Bug 5 (Xung đột thời gian Calib giữ chết offset ở 0.0°)**: Trong `TESTNIDEC.slx`, khối `fcn_MPU6050_Filter` gán cứng `Roll_Angle = 0.0; Gyro_X_Rate = 0.0;` trong suốt 1.5s đầu (300 mẫu) để đo Gyro bias. Trong khi đó, khối `fcn_Balance_Controller` chỉ cần thấy `|Gyro_X_Rate| < 2.0` trong 0.1s (20 mẫu) là lập tức chốt `saved_offset = mean(Roll_Angle) = 0.0°`! Đồng thời khối `ENABLE` trên sơ đồ bị nối cứng bằng `1`, khiến xe không bao giờ calib lại được và mãi mãi chạy với `offset = 0.0°` (lệch 1.30° so với trọng tâm thật).
  - *Giải pháp*: Khi bộ lọc MPU đang calib (1.0s đầu), xuất cờ hiệu `Gyro_X_Rate = 999.0` để khóa bộ điều khiển không được calib sớm. Bổ sung biến `is_calibrated` bắt buộc chờ xe dựng thẳng và giữ yên 0.5s mới chốt `saved_offset` kể cả khi `ENABLE = 1`.
- **Bug 6 (Nhiễm độc Bias của Kalman do gia tốc tiếp tuyến ở độ cao 6cm)**: MPU đặt cao 6cm so với mặt bàn chịu gia tốc tiếp tuyến $a_y = r \ddot{\theta}$ khi xe lắc. Code Kalman cũ liên tục cập nhật `bias` ngay cả khi xe đang chịu gia tốc ngang, làm trôi `bias` của Gyro.
  - *Giải pháp*: Khóa cứng `bias` sau khi calib; chỉ tinh chỉnh cực chậm khi `|total_acc - 1g| <= 0.08g` và `|rate| < 1.0 deg/s`. Tăng trọng số `R_meas` gấp 10 lần khi `acc_dev > 0.20g`.
- **Bug 7 (Trễ pha 15ms từ bộ lọc IIR 0.25 và chưa bật DLPF 0x1A)**: Hệ số `gyro_lpf = 0.25` gây trễ pha 3 chu kỳ (15ms), cộng hưởng với `Kp = 56, Kd = 5` tạo dao động tự kích 6-10 Hz làm xe rung bần bật.
  - *Giải pháp*: Nâng hệ số lọc lên `0.60` (giảm trễ xuống 3ms), hạ `Kp = 45, Kd = 3.8`, tăng I2C lên 400 kHz và ghi `0x03` (42 Hz) vào thanh ghi `0x1A` của MPU-6050.
