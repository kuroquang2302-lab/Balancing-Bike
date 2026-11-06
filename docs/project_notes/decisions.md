# Hồ Sơ Quyết Định Thiết Kế Kiến Trúc

### ADR-001: Cấu trúc điều khiển hai vòng lặp phân tầng
- **Bối cảnh:** Bộ điều khiển một vòng lặp chỉ phản hồi góc nghiêng không thể ngăn bánh đà trôi tốc độ đến ngưỡng bão hòa điện áp khi tồn tại sai lệch điểm đặt tĩnh.
- **Quyết định:** Tách cấu trúc điều khiển thành hai vòng lặp: vòng trong ổn định góc nghiêng thân xe ở tần số $200\text{ Hz}$; vòng ngoài điều chỉnh chậm điểm đặt góc cân bằng $\theta_{\text{offset}}$ theo vận tốc bánh đà.
- **Hệ quả:** Duy trì khả năng đáp ứng tần số cao ở vòng trong đồng thời triệt tiêu vận tốc trôi của bánh đà ở trạng thái xác lập.

### ADR-002: Ngắt xung một chu kỳ khi đảo chiều động cơ Nidec 24H
- **Bối cảnh:** Đảo mức logic chân `DIR` trực tiếp khi đang cấp xung công suất lớn gây xung dòng cao trên mạch cầu H tích hợp của Nidec 24H.
- **Quyết định:** Ép `PWM_Duty = 100.0%` trong đúng $1$ chu kỳ lấy mẫu $5\text{ ms}$ tại thời điểm chuyển trạng thái chân `DIR`.
- **Hệ quả:** Loại bỏ hiện tượng sụt áp nguồn cục bộ mà không gây trễ pha đáng kể cho vòng điều khiển cân bằng.

### ADR-003: Phân chia độc lập 4 bộ định thời phần cứng trên STM32F411RE
- **Bối cảnh:** Khối thư viện mẫu sử dụng chung chân ngoại vi giữa bộ phát xung PWM và bộ đếm Encoder.
- **Quyết định:** Quy hoạch tách biệt: `TIM1` phát PWM $20\text{ kHz}$ cho hai động cơ Nidec, `TIM3` đọc Encoder bánh đà, `TIM2` đọc Encoder bánh sau và `TIM4` phát PWM $50\text{ Hz}$ cho Servo lái.
- **Hệ quả:** Toàn bộ quá trình tạo xung và đếm xung hoạt động bằng phần cứng độc lập với tải CPU bằng $0\%$.

### ADR-004: Máy trạng thái tự động hiệu chuẩn điểm cân bằng tĩnh
- **Bối cảnh:** Vị trí khối tâm thực tế thay đổi nhẹ theo cách gá lắp pin trong mỗi lần vận hành.
- **Quyết định:** Tự động lấy trung bình $100$ mẫu góc nghiêng khi xe được giữ đứng yên với $|\dot{\theta}| < 1.5^\circ/\text{s}$ trong vùng $\pm 4.5^\circ$ quanh mốc danh định $+1.30^\circ$ trước khi kích hoạt động cơ.
- **Hệ quả:** Xác lập chính xác điểm cân bằng tĩnh mà không cần nạp lại hằng số trong mã nguồn.

### ADR-005: Tách biệt kênh đọc chiết áp góc lái trên chân PA4
- **Bối cảnh:** Mã nguồn mẫu của khối Encoder cấu hình ngắt ngoài trên chân `PA4`, trùng với kênh `ADC1_IN4` của chiết áp góc lái.
- **Quyết định:** Loại bỏ khởi tạo ngắt `PA4` trong tệp thực thi C++ của khối `TIM3` và `TIM2`.
- **Hệ quả:** Bảo toàn kênh tương tự `PA4` để đo góc phản hồi cơ cấu lái.

### ADR-006: Ngắt an toàn tại ngưỡng góc nghiêng 8.0 độ
- **Bối cảnh:** Khi độ lệch góc vượt quá $8.0^\circ$, mô-men trọng lực vượt khả năng khôi phục của bánh đà ở mức nguồn $12\text{ V}$.
- **Quyết định:** Ngắt lệnh điều khiển và chốt cờ lỗi `fault_latched` ngay khi $|e| > 8.0^\circ$.
- **Hệ quả:** Ngăn ngừa động cơ tăng tốc cực đại khi khung xe đã nghiêng quá giới hạn làm việc.

### ADR-007: Quy trình phanh điện từ hai giai đoạn
- **Bối cảnh:** Đóng phanh điện từ `BRAKE = 0` tức thời khi xe ngã truyền xung lực ngược vào khung xe.
- **Quyết định:** Khi `Enable_Run < 0.5`, ngắt xung PWM để bánh đà thả trôi trong $100$ chu kỳ đầu trước khi kéo `BRAKE = 0`.
- **Hệ quả:** Giảm chấn động cơ khí khi dừng hệ thống.

### ADR-008: Ràng buộc dải dịch chuyển điểm đặt quanh mốc hiệu chuẩn gốc
- **Bối cảnh:** Nếu điểm đặt dịch chuyển tự do khi có ngoại lực giữ xe, góc tham chiếu sẽ trôi ra ngoài vùng cân bằng.
- **Quyết định:** Khóa mốc hiệu chuẩn gốc `saved_offset` và kẹp biến `current_offset` trong khoảng $\pm 3.0^\circ$ quanh `saved_offset`.
- **Hệ quả:** Đảm bảo điểm đặt luôn nằm trong vùng làm việc hợp lệ.

### ADR-009: Tích hợp khóa chốt lỗi ngã và suy giảm tuyến tính ở tốc độ cao
- **Bối cảnh:** Ngưỡng cắt cứng tại $3000\text{ RPM}$ gây dao động đóng ngắt liên tục, và việc thiếu chốt lỗi làm động cơ tự kích hoạt khi nhấc xe lên.
- **Quyết định:** Áp dụng hàm suy giảm tuyến tính lệnh cùng chiều từ $2500\text{ RPM}$ đến $3000\text{ RPM}$; chỉ mở khóa `fault_latched` và cập nhật lại `saved_offset` khi xe được giữ thẳng đứng yên tĩnh trong $200$ chu kỳ liên tiếp.
- **Hệ quả:** Chuyển tiếp công suất mượt ở tốc độ cao và an toàn khi thao tác.

### ADR-010: Đồng bộ hóa cờ hiệu chuẩn giữa bộ lọc Kalman và bộ điều khiển
- **Bối cảnh:** Trong $200$ mẫu đầu khi đo độ trôi tĩnh Gyro, nếu bộ lọc xuất `0.0` thì bộ điều khiển sẽ chốt nhầm điểm cân bằng tại $0.0^\circ$.
- **Quyết định:** Bộ lọc xuất cờ báo bận `Gyro_X_Rate = 999.0` trong suốt $200$ mẫu đầu; bộ điều khiển khóa quá trình hiệu chuẩn cho đến khi `|Gyro_X_Rate| <= 500.0`, đồng thời chỉ cập nhật chậm Gyro bias khi gia tốc tổng hợp nằm trong ngưỡng $1.0g \pm 0.08g$.
- **Hệ quả:** Ngăn chặn triệt để lỗi chốt nhầm điểm đặt $0.0^\circ$ và hiện tượng nhiễm bẩn Gyro bias khi xe dao động.

### ADR-011: Bổ sung bù trước trọng lực, tích phân góc có Anti-windup và Slew-rate Limiter
- **Bối cảnh:** Trên động cơ điều khiển vận tốc Nidec 24H, mô-men phản lực tỉ lệ với đạo hàm lệnh điều khiển $\tau \propto \dot{u}$. Luật PD thuần túy mất khả năng sinh gia tốc góc tại điểm đổi chiều chuyển động khi $\dot{\theta} = 0$.
- **Quyết định:** Bổ sung thành phần bù trước trọng lực $-K_g \sin(e)$, thành phần tích phân sai số góc $-K_i \int e\,dt$ có kẹp biên độ $\pm 25.0\%$ kèm hệ số xả $0.995$ khi $|e| < 0.15^\circ$, và bộ giới hạn tốc độ biến thiên lệnh $\Delta u \le 8.0\% / 5\text{ ms}$.
- **Hệ quả:** Tái tạo mô-men phục hồi tĩnh tỉ lệ với góc nghiêng mà không gây bão hòa tích phân hay thay đổi giao diện cổng của khối Simulink.
