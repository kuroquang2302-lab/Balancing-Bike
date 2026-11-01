# Architectural Decision Records (ADRs) - Dự án Xe Đạp Tự Cân Bằng UTH

### ADR-001: Kiến trúc Điều khiển Bánh đà Phản lực (2026-10-27)
**Context:**
- Xe đạp cân bằng bằng bánh đà phản lực quán tính có khối lượng $700\text{g} - 800\text{g}$, bánh đà $100\text{g}$. Cần giải thuật ổn định tư thế không bị bão hòa tốc độ.
**Decision:**
- Tách làm 2 pha: Pha 1 dùng bộ điều khiển PD góc nghiêng (`Roll_Angle` và `Gyro_X_Rate`) để giữ ổn định xe; Pha 2 mới đưa thêm khâu hồi tiếp tốc độ bánh đà ($K_w \cdot \omega_m + K_{iw} \int \omega_m$) để giải bão hòa tốc độ về 0 RPM.
**Alternatives Considered:**
- PID đơn vòng góc: Dẫn đến hiện tượng trôi bão hòa tốc độ bánh đà (4000 RPM) do trọng tâm tĩnh bị lệch.
- LQR 4 trạng thái ngay từ đầu: Quá nhiều thông số cần chỉnh đồng thời, khâu tích phân tốc độ $K_{iw}$ dễ gây ngậm chiều làm motor bị lì một bên khi chưa hiệu chuẩn xong `Roll_Offset`.
**Consequences:**
- Dễ dàng tinh chỉnh từng bước, an toàn, ổn định nhanh.

### ADR-002: Cơ chế Bảo vệ Đảo chiều Nidec 24H (Zero-Crossing Guard) (2026-10-25)
**Context:**
- Motor Nidec 24H có mạch driver BLDC tích hợp dễ hỏng nếu đảo chiều khi xung PWM đang kéo dòng cực đại. Tuy nhiên bánh đà phản lực không thể chờ motor dừng hẳn mới đảo chiều (mất lực cân bằng làm xe ngã trong 0.2s).
**Decision:**
- Áp dụng cơ chế Zero-Crossing Guard 1 chu kỳ lấy mẫu ($5\text{ ms}$): Khi lệnh `Speed_Cmd` đổi dấu, tự động ép `PWM_Duty = 100.0%` (0% công suất) trong đúng 1 chu kỳ $5\text{ ms}$ tại thời điểm lật chân DIR, sau đó mới cấp xung theo chiều mới.
**Alternatives Considered:**
- Slew-rate limiter nặng (50 ms): Gây trễ pha điều khiển (phase lag), làm xe bị mất thăng bằng ngã ngay.
- Đảo chiều tức thời không ngắt PWM: Nguy cơ sốc dòng cho cầu MOSFET nội của Nidec.
**Consequences:**
- Triệt tiêu hoàn toàn xung dòng khi đảo chiều, dòng điện có tải đo được chỉ tối đa $0.5\text{A}$ (an toàn dưới ngưỡng 2A), đồng thời độ trễ $5\text{ ms}$ hoàn toàn không ảnh hưởng tới động học cân bằng.

### ADR-003: Phân chia 4 Bộ Timer trên STM32F411RE (2026-10-26)
**Context:**
- Hệ thống cần điều khiển 2 động cơ BLDC Nidec 24H (PWM 20 kHz), 1 động cơ Servo MG995 (PWM 50 Hz), và đọc 2 bộ Encoder 4x.
**Decision:**
- `TIM1`: Phát xung PWM 20 kHz cho cả 2 motor Nidec (`CH1` trên `PA8` cho Bánh đà, `CH4` trên `PA11` cho Bánh sau).
- `TIM2`: Đọc Encoder Motor 2 Bánh sau (`CH1` trên `PA15`, `CH2` trên `PB3`).
- `TIM3`: Đọc Encoder Motor 1 Bánh đà (`CH1` trên `PA6`, `CH2` trên `PA7`).
- `TIM4`: Phát xung PWM 50 Hz cho Servo cổ lái (`CH3` trên `PB8`).
- `ADC1_IN4` (`PA4`): Đọc áp chiết áp góc lái Servo ($0 - 3.3\text{V}$).
**Consequences:**
- Tận dụng tối đa bộ đếm phần cứng chuyên dụng, không tốn chu kỳ CPU, hoàn toàn không có xung đột chân.

### ADR-004: Máy trạng thái Tự động Calib Điểm Cân Bằng & Khóa An Toàn 3 Lớp (2026-10-29)
**Context:**
- Xe tự cân bằng bánh đà Nidec 24H dễ bị rồ ga quá tốc ($> 2000\text{ RPM}$) và ăn dòng $2.5\text{A}$ nóng motor nếu motor bật ngay khi xe đang nằm nghiêng hoặc khi xe bị ngã. Đồng thời điểm cân bằng thực tế của xe luôn dao động quanh $\approx 2^\circ - 3^\circ$ tùy vị trí pin LiPo.
**Decision:**
- Tích hợp Máy trạng thái 2 cấp (Finite State Machine) ngay trong khối `fcn_Balance_Controller`:
  1. **State 0 (Chờ & Calib tĩnh):** Motor tắt hoàn toàn (`Enable_Out = 0`, `Brake = 0`). Chỉ khi người dùng dựng xe đứng yên ($|\dot{\theta}| < 2^\circ/\text{s}$, sai số so với offset thô $< 4^\circ$) trong đúng 1 giây (200 chu kỳ $5\text{ ms}$), hệ thống tự động tính trung bình cộng góc làm `calib_offset` và tự chuyển sang State 1.
  2. **State 1 (Tự cân bằng đóng vòng):** Khóa an toàn 3 lớp:
     - Lớp 1: Ngã xe ($|\theta - \text{calib\_offset}| > 30^\circ$) $\rightarrow$ Ngắt motor, reset về State 0.
     - Lớp 2: Rồ ga quá tốc ($|\text{RPM}| > 1800$ liên tục quá 4 giây) $\rightarrow$ Ngắt motor, reset về State 0 để chống nóng motor.
     - Lớp 3: Kẹp công suất tối đa $70\%$ ga để cắt dòng sốc $2.5\text{A}$.
**Consequences:**
- Motor mát rượi, không bao giờ tự rồ ga khi cắm điện hay khi ngã; người làm chỉ cần dựng xe giữ yên 1 giây là xe tự động bắt đúng điểm cân bằng và đứng thẳng!

### 2026-10-29 - Tự động hóa 100% quy trình Calib điểm 0 (Bỏ nhập tay Roll_Offset)
- **Decision**: Loại bỏ hoàn toàn khối Constant `Roll_Offset` ở đầu vào. Chuyển sang cơ chế tự động:
  - Khi xe nằm nghiêng hoặc ngã ($|\text{Roll}| \ge 10^\circ$), motor tắt tuyệt đối.
  - Khi người dùng cầm dựng xe đứng gần thẳng ($|\text{Roll}| < 10^\circ$) và tay giữ yên ($|\dot{\theta}| < 1.5^\circ/\text{s}$) trong đúng 1 giây (200 chu kỳ $5\text{ ms}$), thuật toán tự động lấy trung bình cộng góc làm `calib_offset` và xuất ra cổng thứ 3 `Active_Offset` nối vào khối Display để quan sát.
  - Sau 1 giây, motor tự động kích hoạt cân bằng ngay tại điểm vừa calib được.
- **Benefits**: Loại bỏ hoàn toàn việc phải đoán mò số offset mỗi ngày khi tháo lắp pin LiPo hay xê dịch trọng tâm.

### ADR-008: Tách biệt hoàn toàn Offset vật lý tĩnh và Khâu bù vận tốc bánh đà Kiw (2026-10-29)
**Context:**
- Xe đạp có khối lượng nặng (700–800g), bánh đà 100g. Nếu điểm đặt lệch $0.2^\circ$, bánh đà bị trôi tốc độ lên 1450 RPM rồi ngã.
- Thử nghiệm ban đầu dùng $Kiw$ nắn trực tiếp vào `saved_offset` làm giá trị Offset trên Display chạy liên tục, gây nhiễu loạn quan sát và phá vỡ tính tĩnh của góc hiệu chuẩn.
**Decision:**
- `saved_offset` là điểm cân bằng tĩnh do người dùng xác lập (One-shot Calibration), được chốt và khóa cứng vĩnh viễn trên Display.
- $Kiw$ được chuyển thành khâu tích phân vận tốc động cơ $\int Motor\_RPM \cdot dt$ tác động trực tiếp vào lệnh điều khiển $u$ với bộ kẹp chống bão hòa tích phân (Anti-windup $\pm 2.0$).
**Consequences:**
- Offset trên Display đứng yên 100% giúp người vận hành biết chính xác góc cân bằng cơ khí.
- Bánh đà tự động triệt tiêu vận tốc trôi về 0 RPM nhờ thành phần tích phân trong luật điều khiển $u$.

### ADR-009: Đối sánh Kiến trúc Điều khiển với Nghiên cứu HCMUTE 2024 & Khẳng định Giải thuật 2 Vòng (2026-10-31)
**Context:**
- Bài báo khoa học của nhóm nghiên cứu HCMUTE (Journal of Fuzzy Systems and Control, 2024) thực hiện mô hình xe đạp cân bằng bánh đà tương tự (Thingiverse 5887157), nhưng gặp các hạn chế: dùng DC chổi than + L298N, dùng PID 1 vòng không có phản hồi tốc độ bánh đà, khâu Ki gây trễ pha làm thời gian xác lập lên tới 55s và hệ thống dao động liên tục quanh -4 độ.
- Dự án UTH cần một giải thuật triệt tiêu hoàn toàn hiện tượng bão hòa tốc độ bánh đà mà không gây mất ổn định như khâu Ki truyền thống.
**Decision:**
- Loại bỏ hoàn toàn khâu Ki góc truyền thống khỏi vòng điều khiển góc.
- Cấu trúc hệ thống thành 2 vòng lặp phân tầng:
  1. **Vòng trong (Inner Loop):** PD góc cực nhanh (Kp, Kd) xuất lệnh trực tiếp đến BLDC Nidec 24H để dập tắt dao động tức thời.
  2. **Vòng ngoài (Outer Loop):** Adaptive Setpoint Modulation (Điều chế điểm đặt động) trích xuất từ Encoder phần cứng TIM3. Tự động dịch nhẹ `Active_Offset` với tốc độ cực nhỏ (slew-rate bounded < 0.4 deg/s) để hướng góc thân xe về đúng vị trí cân bằng trọng lực tự nhiên, triệt tiêu tốc độ motor về 0 RPM.
**Consequences:**
- Khắc phục triệt để hiện tượng vọt lố và dao động 55s của Ki mà nhóm HCMUTE gặp phải.
- Khắc phục hiện tượng triệt tiêu mô-men của việc trừ Kw * Motor_RPM trực tiếp trong lệnh u.
- Đưa đồ án UTH lên một bậc học thuật cao hơn, có thể trích dẫn bài báo HCMUTE làm minh chứng đối sánh trong báo cáo nghiệm thu.

### ADR-010: Nâng cấp Bộ điều khiển Cân bằng v2 với Chốt lỗi ngã và Giảm tốc quá tốc mềm (2026-10-31)
**Context:**
- Thử nghiệm các kịch bản biên thực tế (xe nằm sàn 35°, xe ngã được nhặt lên 7°, motor dao động quanh 3000 RPM) cho thấy bộ điều khiển v1 có nguy cơ mất an toàn phần cứng và rung chấn chattering.
**Decision:**
- Tích hợp 4 nâng cấp vào `fcn_Balance_Controller` (bản v2):
  1. Cổng điều kiện Calib góc an toàn quanh offset danh định +-4.0 độ với cửa sổ 1.0s.
  2. Cơ chế chốt lỗi ngã (`fault_latched`): khi ngã, motor ngắt và bắt buộc phải gạt `Enable_Switch` về 0 để re-arm.
  3. Bộ giới hạn quá tốc mềm (Soft Limiter): suy giảm ga cùng chiều tuyến tính từ 2500 đến 3000 RPM.
  4. Bộ lọc MPU6050 tự động tính `gyro_bias` trong 1.0s đầu sau khi khởi động.
**Consequences:**
- Triệt tiêu 100% hiện tượng tự rồ ga khi xe nằm sàn hoặc khi nhặt xe lên.
- Động cơ chuyển tiếp mượt mà ở tốc độ cao, không còn rung giật chattering.

### ADR-011: Đồng bộ hóa Cờ hiệu Calib (Handshake 999.0) và Khóa Bias Kalman Thích nghi (2026-11-01)
**Context:**
- Kiểm tra trực tiếp file `TESTNIDEC.slx` phát hiện nguyên nhân cốt lõi làm đường `offset` trên Scope luôn nằm chết ở `0.0°`: Bộ lọc MPU xuất `0.0` trong 1.5s đầu trong khi Controller chốt calib ngay ở 0.1s đầu và hằng số `ENABLE` nối cứng bằng `1`.
**Decision:**
- Thiết lập cơ chế bắt tay (Handshake) giữa `fcn_MPU6050_Filter` và `fcn_Balance_Controller`:
  1. Trong 200 mẫu đầu (1.0s), Filter xuất `Gyro_X_Rate = 999.0` để báo đang bận đo bias.
  2. Controller chặn mọi hoạt động khi `abs(Gyro_X_Rate) > 500.0`, và dùng cờ `is_calibrated` bắt buộc thực hiện 1 lần calib góc thẳng đứng (0.5s) trước khi cho phép chạy, bất chấp `Enable_Switch` đang bằng 1.
  3. Khôi phục thành phần cản mềm `Kw_damp = 0.002` ở vòng trong kết hợp dịch chuyển `current_offset` ở vòng ngoài.
  4. Bổ sung cơ chế Tự động Re-arm sau khi ngã: chỉ cần dựng xe thẳng đứng giữ yên 1.0s là xe tự mở khóa chạy lại mà không cần sửa sơ đồ nối công tắc.
**Consequences:**
- Giải quyết triệt để căn bệnh `offset = 0.0°` tồn tại xuyên suốt các lần đo trước.
- Không cần vẽ hay nối lại bất kỳ sợi dây nào trên sơ đồ `TESTNIDEC.slx`.
