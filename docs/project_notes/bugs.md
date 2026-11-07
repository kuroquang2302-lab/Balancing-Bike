# Nhật Ký Lỗi Kỹ Thuật Và Giải Pháp Xử Lý

### 1. Ngược cực tính chân chiều quay DIR trên động cơ Nidec 24H
- **Hiện tượng:** Khi xuất lệnh điều khiển dương, bánh đà quay ngược chiều phản lực cần thiết.
- **Nguyên nhân:** Mạch điều khiển tích hợp của Nidec 24H quy định mức logic `0` trên chân `PC7` là quay thuận và mức `1` là quay nghịch.
- **Giải pháp:** Chuẩn hóa trong `fcn_Nidec_Driver`: gán `Dir_PC7 = 0.0` khi `u >= 0.01` và `Dir_PC7 = 1.0` khi `u < 0.01`.

### 2. Khựng cơ khí khi tín hiệu điều khiển đi qua vùng chết quanh điểm 0
- **Hiện tượng:** Bánh đà bị giật cục bộ mỗi khi lệnh điều khiển đổi dấu hoặc có biên độ nhỏ.
- **Nguyên nhân:** Phiên bản cũ kéo chân phanh `Brake_PC8 = 0.0` ngay khi `|Speed_Cmd| < 0.5`.
- **Giải pháp:** Giữ `Brake_PC8 = 1.0` và đặt `PWM_Duty_PA8 = 100.0` để bánh đà thả trôi tự do trong vùng chết `|u| < 0.5`; chỉ kích hoạt phanh cứng khi `Enable_Run < 0.5` kéo dài quá $100$ chu kỳ.

### 3. Xung đột chân ngoại vi giữa khối Encoder mẫu và kênh ADC / PWM
- **Hiện tượng:** Khối Encoder mẫu của thư viện làm mất xung PWM trên `PA8` và gây sai lệch giá trị đọc ADC trên `PA4`.
- **Nguyên nhân:** Mã nguồn mẫu cấu hình `PA8` và ngắt ngoài trên chân `PA4`.
- **Giải pháp:** Xây dựng khối `EncoderBlock_TIM3` sử dụng `PA6`, `PA7` trên `TIM3` và xóa bỏ cấu hình ngắt `PA4`.

### 4. Trôi bão hòa vận tốc bánh đà khi điểm đặt lệch tĩnh
- **Hiện tượng:** Khi góc đặt lệch hơn $0.2^\circ$ so với vị trí khối tâm thực, bánh đà gia tốc một chiều lên ngưỡng bão hòa điện áp.
- **Nguyên nhân:** Mô-men trọng lực tĩnh khác không buộc bánh đà phải duy trì gia tốc góc liên tục.
- **Giải pháp:** Triển khai vòng điều khiển ngoài cập nhật `current_offset` theo vận tốc bánh đà với hệ số `-K_trim` để hội tụ điểm đặt về đúng vị trí khối tâm.

### 5. Hiệu chuẩn nhầm góc nghiêng khi xe nằm ngang lúc khởi động
- **Hiện tượng:** Khi xe đang đặt nghiêng trên mặt bàn, bộ điều khiển lấy góc nghiêng lớn làm mốc cân bằng.
- **Nguyên nhân:** Điều kiện hiệu chuẩn ban đầu chỉ kiểm tra vận tốc góc nhỏ mà không ràng buộc miền góc hợp lệ.
- **Giải pháp:** Chỉ tích lũy mẫu hiệu chuẩn khi $|\text{Roll\_Angle} - \text{NOMINAL\_OFFSET}| < 4.5^\circ$ và $|\text{Gyro\_X\_Rate}| < 1.5^\circ/\text{s}$.

### 6. Tự kích hoạt lại động cơ khi nhấc xe sau khi ngã
- **Hiện tượng:** Sau khi xe ngã vượt $8.0^\circ$, lúc người dùng dựng xe đi qua vùng $< 8.0^\circ$, động cơ tự chạy lại ở công suất lớn.
- **Nguyên nhân:** Thiếu biến nhớ chốt trạng thái lỗi sau khi vượt ngưỡng góc ngã.
- **Giải pháp:** Bổ sung biến `fault_latched`, giữ trạng thái ngắt động cơ cho đến khi tắt `Enable_Switch` hoặc giữ xe thẳng đứng yên tĩnh đủ $200$ chu kỳ để hiệu chuẩn lại `saved_offset`.

### 7. Đóng ngắt công suất liên tục tại ngưỡng 3000 RPM
- **Hiện tượng:** Khi tốc độ bánh đà chạm $3000\text{ RPM}$, lệnh điều khiển nhảy bậc giữa mức cực đại và $0\%$.
- **Nguyên nhân:** Sử dụng phép so sánh ngưỡng cứng dạng nhị phân.
- **Giải pháp:** Áp dụng hàm suy giảm tuyến tính hệ số `scale` trong dải từ $2500\text{ RPM}$ đến $3000\text{ RPM}$ đối với các lệnh cùng chiều quay.

### 8. Xung đột thời gian khởi tạo làm chốt nhầm điểm đặt 0.0 độ và nhiễm bẩn Gyro bias
- **Hiện tượng:** Điểm đặt bị chốt cố định tại $0.0^\circ$ khi `Enable_Switch` nối cứng mức `1`, đồng thời góc ước lượng bị trôi khi khung xe lắc mạnh.
- **Nguyên nhân:** Bộ lọc xuất `0.0` trong thời gian đo bias khiến bộ điều khiển chốt hiệu chuẩn sớm; gia tốc tiếp tuyến tại độ cao lắp đặt $60\text{ mm}$ làm lệch vector trọng trường biểu kiến.
- **Giải pháp:** Bộ lọc xuất `Gyro_X_Rate = 999.0` trong $200$ chu kỳ đầu để khóa bộ điều khiển; tăng hiệp phương sai nhiễu đo `current_R` lên gấp $3 - 10$ lần khi $|\text{total\_acc} - 1.0| > 0.08$ và chỉ cập nhật chậm `bias` khi hệ thống tĩnh.

### 9. Hụt mô-men phục hồi tại điểm đổi chiều trên động cơ điều khiển vận tốc
- **Hiện tượng:** Khi góc nghiêng vượt quá $2.0^\circ$ và vận tốc góc thân xe về $0^\circ/\text{s}$ tại biên độ cực đại, bánh đà ngừng tăng tốc làm xe không thể tự kéo về vị trí cân bằng.
- **Nguyên nhân:** Với cơ cấu chấp hành nhận lệnh vận tốc như Nidec 24H, đạo hàm của luật PD thuần túy bằng $0$ khi $\dot{\theta} = 0$ và $\ddot{\theta} = 0$.
- **Giải pháp:** Bổ sung khâu bù trước trọng lực `u_grav` và khâu tích phân sai số góc `int_err` có giới hạn Anti-windup để duy trì tốc độ biến thiên lệnh $\dot{u} \neq 0$ tại điểm đổi chiều.

