# Nhật Ký Lỗi Kỹ Thuật Và Biện Pháp Khắc Phục

### 1. Ngược cực tính chân điều khiển chiều quay động cơ Nidec 24H
- **Hiện tượng:** Khi xuất lệnh điều khiển dương, bánh đà quay ngược chiều phản lực cần thiết để nâng thân xe.
- **Nguyên nhân:** Mạch điều khiển tích hợp bên trong động cơ Nidec 24H quy định mức logic thấp trên chân `PC7` là quay thuận chiều kim đồng hồ và mức logic cao là quay ngược chiều kim đồng hồ.
- **Khắc phục:** Chuẩn hóa quy ước trong `fcn_Nidec_Driver`: gán `Dir_PC7 = 0.0` khi lệnh điều khiển lớn hơn hoặc bằng 0 và `Dir_PC7 = 1.0` khi lệnh điều khiển âm.

### 2. Rung giật cơ khí khi lệnh điều khiển đi qua vùng lân cận điểm không
- **Hiện tượng:** Bánh đà bị khựng cơ học mỗi khi tín hiệu điều khiển đổi dấu hoặc đi qua vùng biên độ nhỏ.
- **Nguyên nhân:** Phiên bản đầu tiên kích hoạt chân phanh điện từ `Brake_PC8 = 0.0` ngay trong vùng chết `|Speed_Cmd| < 0.5`.
- **Khắc phục:** Tách biệt trạng thái dừng và vùng chết: giữ `Brake_PC8 = 1.0` và đưa độ rộng xung về `100.0%` để bánh đà thả trôi tự do khi lệnh nằm trong vùng `|Speed_Cmd| < 0.5`; chỉ đóng phanh khi hệ thống đã ngắt hoàn toàn sau 0.5 giây.

### 3. Xung đột tài nguyên ngoại vi giữa khối Encoder và kênh đọc chiết áp
- **Hiện tượng:** Khi biên dịch mô hình có đồng thời khối đọc Encoder và khối đọc điện áp góc lái, tín hiệu điều rộng xung ở chân `PA8` và kênh đọc tương tự ở chân `PA4` hoạt động sai lệch.
- **Nguyên nhân:** Mã nguồn mẫu của thư viện dùng chân `PA8` và ngắt ngoài trên chân `PA4`.
- **Khắc phục:** Xây dựng khối `EncoderBlock_TIM3` sử dụng hai chân `PA6` và `PA7` trên bộ định thời TIM3, loại bỏ hoàn toàn cấu hình ngắt trên chân `PA4`.

### 4. Bão hòa tốc độ bánh đà khi giữ nguyên điểm đặt lệch tĩnh
- **Hiện tượng:** Khi điểm đặt góc nghiêng lệch trên 0.2 độ so với vị trí khối tâm thực, bánh đà tăng tốc một chiều liên tục lên trên 2000 RPM rồi mất khả năng giữ thăng bằng.
- **Nguyên nhân:** Mô-men trọng lực tĩnh khác không buộc bánh đà phải duy trì gia tốc góc liên tục cho đến khi chạm giới hạn tốc độ của nguồn 12V.
- **Khắc phục:** Kết hợp thành phần giảm chấn vận tốc bánh đà ở vòng trong và khâu dịch chuyển điểm đặt chậm ở vòng ngoài để đưa thân xe về đúng góc cân bằng trọng lực tự nhiên.

### 5. Lỗi hiệu chuẩn nhầm góc nghiêng khi xe nằm trên mặt phẳng ngang
- **Hiện tượng:** Khi để xe nằm nghiêng trên bàn với công tắc ở trạng thái chờ, bộ điều khiển lấy góc nằm ngang làm mốc cân bằng mới và kích hoạt động cơ ngay khi bật công tắc.
- **Nguyên nhân:** Điều kiện hiệu chuẩn ban đầu chỉ kiểm tra vận tốc góc nhỏ mà không giới hạn khoảng góc nghiêng hợp lệ.
- **Khắc phục:** Bổ sung điều kiện chỉ cho phép tích lũy mẫu hiệu chuẩn khi góc nghiêng nằm trong phạm vi $\pm 4.5^\circ$ quanh mốc danh định $+1.30^\circ$.

### 6. Tự động kích hoạt lại động cơ với công suất lớn khi nhấc xe sau khi ngã
- **Hiện tượng:** Sau khi xe ngã vượt quá 8.0 độ và ngắt động cơ, lúc người vận hành nhấc xe lên đi qua vùng dưới 8.0 độ, động cơ tự động chạy lại ở mức công suất cực đại.
- **Nguyên nhân:** Nhánh kiểm tra ngã xe chưa có biến nhớ trạng thái lỗi giữa các chu kỳ lấy mẫu.
- **Khắc phục:** Bổ sung biến chốt trạng thái `fault_latched`, giữ động cơ ở trạng thái ngắt cho đến khi công tắc được tắt hoặc xe được dựng thẳng đứng và giữ yên liên tục trong 1.0 giây để tính lại trung bình góc cân bằng mới.

### 7. Đóng ngắt công suất liên tục tại ngưỡng giới hạn tốc độ 3000 RPM
- **Hiện tượng:** Khi vận tốc bánh đà dao động quanh 3000 RPM, lệnh điều khiển nhảy đột ngột giữa mức công suất cao và 0%, gây rung cơ khí.
- **Nguyên nhân:** Bộ giới hạn tốc độ sử dụng phép so sánh ngưỡng cứng không có dải chuyển tiếp.
- **Khắc phục:** Áp dụng hàm suy giảm tuyến tính lệnh điều khiển cùng chiều quay trong dải từ 2500 RPM đến 3000 RPM.

### 8. Xung đột thời gian khởi tạo làm điểm đặt bị chốt tại 0.0 độ và trễ pha vận tốc góc
- **Hiện tượng:** Trên đồ thị thực nghiệm, đường điểm đặt luôn nằm cố định tại 0.0 độ thay vì +1.30 độ, đồng thời thân xe rung ở tần số 6 Hz - 10 Hz khi giữ thăng bằng.
- **Nguyên nhân:**
  1. Trong 1.5 giây đầu tiên khi đo độ lệch con quay hồi chuyển, bộ lọc cũ xuất `Roll_Angle = 0.0` và `Gyro_X_Rate = 0.0`, khiến bộ điều khiển hiểu nhầm xe đang đứng yên tại 0.0 độ và chốt `saved_offset = 0.0`.
  2. Bộ lọc thông thấp vận tốc góc có hệ số 0.25 gây trễ pha 15 ms, kết hợp với bộ thông số khuếch đại cao và gia tốc tiếp tuyến tại độ cao lắp đặt 60 mm làm phát sinh dao động tự kích.
- **Khắc phục:**
  1. Thiết lập tín hiệu báo bận `Gyro_X_Rate = 999.0` trong 1.0 giây đầu từ bộ lọc và bổ sung cờ `is_calibrated` trong bộ điều khiển để bắt buộc hiệu chuẩn góc thẳng đứng sau khi bộ lọc đã sẵn sàng.
  2. Khóa cập nhật độ lệch tĩnh của con quay hồi chuyển khi có gia tốc động, nâng hệ số lọc thông thấp vận tốc góc lên 0.60 để giảm trễ pha xuống 3 ms và đưa thông số điều khiển về mức ổn định.

### 9. Suy giảm mô-men phục hồi tại điểm đổi chiều khi chỉ dùng luật tỉ lệ - vi sai trên động cơ điều khiển vận tốc
- **Hiện tượng:** Khi thân xe bị đẩy nghiêng vượt quá 2.0 độ và dừng lại ở biên độ nghiêng cực đại trước khi đổ, bánh đà đạt tốc độ cao nhưng không tiếp tục tăng tốc để kéo xe về vị trí thẳng đứng.
- **Nguyên nhân:** Động cơ Nidec 24H hoạt động ở chế độ điều khiển vận tốc nên mô-men phản lực tỉ lệ với đạo hàm bậc nhất của lệnh điều khiển. Với luật tỉ lệ - vi sai thuần túy, khi vận tốc góc thân xe bằng không tại điểm đổi chiều, tốc độ biến thiên lệnh điều khiển giảm về không làm mất mô-men phản lực đúng lúc mô-men trọng lực đang lớn nhất.
- **Khắc phục:** Bổ sung tầng bù trước mô-men trọng lực và thành phần tích phân sai số góc có giới hạn chống bão hòa ở mức 25% công suất kèm bộ giới hạn tốc độ biến thiên lệnh 8.0% mỗi chu kỳ, đảm bảo động cơ luôn duy trì gia tốc góc dương để thắng mô-men trọng lực.

### 10. Quá tải quán tính khi khởi động bánh đà 100.0 g ở mức nguồn 12V
- **Hiện tượng:** Với bánh đà gắn đủ 21 cụm ốc nặng 100.0 g và ngưỡng vùng chết 94.0%, động cơ Nidec 24H ở điện áp 12V bị khựng nhẹ khi khởi động từ trạng thái đứng yên và mất tới 1.0 giây để đảo chiều hoàn toàn.
- **Nguyên nhân:** Mô-men quán tính của 21 cụm ốc vượt quá dải gia tốc tối ưu của động cơ khi vận hành ở mức áp 12V thay vì 24V.
- **Khắc phục:** Giảm số lượng ốc viền xuống 7 cụm bu lông M4x16 và 28 đai ốc M4 bố trí đối xứng tâm để đưa khối lượng bánh đà về mức 73.3 g, đồng thời hiệu chuẩn lại ngưỡng vùng chết khởi động lên 94.5% giúp bánh đà khởi động mượt và rút ngắn thời gian đảo chiều xuống 0.65 giây.
