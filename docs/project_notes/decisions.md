# Hồ Sơ Quyết Định Thiết Kế Kiến Trúc

### ADR-001: Cấu trúc điều khiển hai vòng lặp phân tầng
- **Bối cảnh:** Bộ điều khiển một vòng lặp chỉ phản hồi góc nghiêng không thể ngăn bánh đà trôi tốc độ đến ngưỡng bão hòa điện áp khi tồn tại sai lệch điểm đặt tĩnh.
- **Quyết định:** Tách cấu trúc điều khiển thành hai vòng lặp: vòng trong dùng bộ điều khiển tỉ lệ - vi sai kết hợp giảm chấn vận tốc góc để giữ ổn định thân xe tức thời; vòng ngoài điều chỉnh chậm điểm đặt góc cân bằng theo vận tốc bánh đà.
- **Kết quả:** Duy trì mô-men phục hồi góc tối đa ở tần số cao đồng thời triệt tiêu vận tốc trôi của bánh đà ở trạng thái xác lập.

### ADR-002: Ngắt xung một chu kỳ khi đảo chiều động cơ Nidec 24H
- **Bối cảnh:** Đảo chiều trực tiếp ở tốc độ cao gây sụt áp nguồn cục bộ và xung dòng lớn trên mạch cầu công suất tích hợp của động cơ.
- **Quyết định:** Chèn một chu kỳ lấy mẫu ngắt xung hoàn toàn trước khi áp dụng mức logic chiều quay mới.
- **Kết quả:** Loại bỏ hiện tượng sụt áp đường nguồn 5V cấp cho vi điều khiển khi bánh đà đảo chiều liên tục.

### ADR-003: Phân chia độc lập 4 bộ định thời phần cứng trên STM32F411RE
- **Bối cảnh:** Khối thư viện mẫu của MathWorks sử dụng chung chân ngoại vi giữa bộ tạo xung điều rộng xung và bộ đếm xung, gây xung đột phần cứng.
- **Quyết định:** Quy hoạch tách biệt hoàn toàn: TIM1 tạo xung 20 kHz cho hai động cơ Nidec, TIM3 đọc Encoder bánh đà, TIM2 đọc Encoder bánh sau và TIM4 tạo xung 50 Hz cho cơ cấu lái.
- **Kết quả:** Hoạt động đồng thời tất cả các kênh đo và điều khiển ở cấp phần cứng mà không tiêu tốn tài nguyên ngắt phần mềm.

### ADR-004: Cơ chế tự động hiệu chuẩn điểm cân bằng tĩnh
- **Bối cảnh:** Vị trí khối tâm thực tế thay đổi nhẹ theo cách bố trí pin và dây dẫn trong mỗi lần lắp đặt.
- **Quyết định:** Tự động tính giá trị trung bình góc nghiêng khi xe được giữ đứng yên trong vùng lân cận điểm cân bằng danh định trước khi kích hoạt động cơ.
- **Kết quả:** Loại bỏ sai số lắp đặt cơ khí mà không cần chỉnh sửa hằng số trong mã nguồn.

### ADR-005: Tách biệt kênh đọc chiết áp góc lái trên chân PA4
- **Bối cảnh:** Mã nguồn mẫu của khối Encoder sử dụng chân PA4 làm ngắt xung chỉ mục, trùng với kênh chuyển đổi tương tự sang số của chiết áp cơ cấu lái.
- **Quyết định:** Loại bỏ cấu hình ngắt ngoài trên chân PA4 trong tệp thực thi C++ của khối TIM3 và TIM2.
- **Kết quả:** Bảo toàn kênh `ADC1_IN4` trên chân PA4 để đo chính xác góc phản hồi cơ cấu lái.

### ADR-006: Giới hạn góc ngã an toàn tại ngưỡng 8.0 độ
- **Bối cảnh:** Khi góc nghiêng vượt quá 8.0 độ, mô-men trọng lực vượt quá khả năng phục hồi của bánh đà ở mức nguồn 12V.
- **Quyết định:** Ngắt ngay xung điều khiển động cơ khi độ lệch góc vượt quá 8.0 độ.
- **Kết quả:** Bảo vệ cơ khí và ngăn ngừa bánh đà tăng tốc cực đại khi xe đã nghiêng chạm mặt bàn.

### ADR-007: Quy trình phanh điện từ hai giai đoạn
- **Bối cảnh:** Kích hoạt phanh điện từ ngay lập tức khi xe vừa ngã làm toàn bộ động lượng của bánh đà truyền ngược vào khung xe gây nảy cơ khí.
- **Quyết định:** Khi ngắt trạng thái cân bằng, cho phép bánh đà thả trôi tự do trong 0.5 giây đầu tiên trước khi đóng phanh điện từ để dừng hẳn.
- **Kết quả:** Khung xe tiếp đất êm và bánh đà dừng hoàn toàn sau 0.7 giây.

### ADR-008: Ràng buộc phạm vi dịch chuyển điểm đặt quanh mốc hiệu chuẩn gốc
- **Bối cảnh:** Nếu điểm đặt dịch chuyển không giới hạn khi người dùng giữ xe nghiêng bằng tay, giá trị góc đặt sẽ trôi ra ngoài vùng làm việc hợp lệ.
- **Quyết định:** Lưu trữ cố định mốc hiệu chuẩn gốc `saved_offset` và giới hạn biến điểm đặt động `current_offset` trong khoảng $\pm 3.0^\circ$ quanh mốc gốc.
- **Kết quả:** Đảm bảo điểm đặt luôn nằm trong nón cân bằng an toàn và dễ dàng phục hồi về mốc gốc khi khởi động lại.

### ADR-009: Loại bỏ khâu tích phân góc truyền thống
- **Bối cảnh:** Khâu tích phân sai số góc trực tiếp gây trễ pha và làm xuất hiện dao động chu kỳ chậm.
- **Quyết định:** Sử dụng bộ điều khiển tỉ lệ - vi sai thuần túy ở vòng trong kết hợp điều chỉnh điểm đặt theo vận tốc bánh đà ở vòng ngoài.
- **Kết quả:** Thời gian xác lập đạt dưới 0.8 giây, triệt tiêu hoàn toàn dao động chu kỳ dài.

### ADR-010: Tích hợp khóa chốt lỗi ngã và giới hạn mềm vận tốc tối đa
- **Bối cảnh:** Bộ so sánh ngưỡng tốc độ dạng đóng ngắt nhị phân tại 3000 RPM gây hiện tượng đóng ngắt liên tục, và việc không chốt trạng thái ngã khiến động cơ tự chạy lại khi nhấc xe lên.
- **Quyết định:** Áp dụng hàm suy giảm tuyến tính lệnh điều khiển cùng chiều từ 2500 RPM đến 3000 RPM, đồng thời bổ sung biến chốt trạng thái ngã chỉ mở khóa khi xe được dựng thẳng đứng và giữ yên trong 1.0 giây.
- **Kết quả:** Vận hành êm ở dải tốc độ cao và đảm bảo an toàn khi thao tác bằng tay.

### ADR-011: Đồng bộ hóa cờ hiệu chuẩn giữa bộ lọc Kalman và bộ điều khiển
- **Bối cảnh:** Trong 1.0 giây đầu khi bộ lọc cảm biến đang lấy mẫu độ lệch tĩnh của con quay hồi chuyển, nếu bộ lọc xuất giá trị 0.0 thì bộ điều khiển phía sau sẽ chốt nhầm điểm cân bằng gốc tại 0.0 độ.
- **Quyết định:** Bộ lọc xuất giá trị báo hiệu `Gyro_X_Rate = 999.0` trong suốt 200 chu kỳ hiệu chuẩn đầu tiên; bộ điều khiển kiểm tra điều kiện này để chờ bộ lọc hoàn tất trước khi lấy mẫu góc cân bằng tĩnh, đồng thời khóa cập nhật độ lệch con quay hồi chuyển khi phát hiện gia tốc động lớn.
- **Kết quả:** Điểm cân bằng tĩnh luôn được xác lập chính xác quanh mốc thực tế +1.30 độ và không bị trôi bởi gia tốc tiếp tuyến của khung xe.
