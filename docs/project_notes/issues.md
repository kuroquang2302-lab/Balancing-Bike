# Nhật Ký Tiến Độ Thực Hiện Dự Án

### Giai đoạn 1: Kiểm chuẩn phần cứng và cảm biến quán tính - 2026-10-28
- **Trạng thái:** Hoàn thành
- **Nội dung thực hiện:**
  - Đo kiểm thực tế cảm biến MPU-6050 trên khung xe hoàn chỉnh, xác định góc cân bằng tĩnh danh định tại $+1.30^\circ$ và độ trôi vận tốc góc tĩnh dưới $0.02^\circ/\text{s}$.
  - Nghiệm thu độc lập hai động cơ không chổi than Nidec 24H và bộ đếm xung phần cứng TIM2, TIM3 trên vi điều khiển STM32F411RE.

### Giai đoạn 2: Xây dựng khối điều khiển và cơ chế hiệu chuẩn tự động - 2026-10-29
- **Trạng thái:** Hoàn thành
- **Nội dung thực hiện:**
  - Thiết kế máy trạng thái hiệu chuẩn góc cân bằng tĩnh tự động và tích hợp thành phần giảm chấn vận tốc bánh đà.
  - Chuẩn hóa kiểu dữ liệu thực độ chính xác kép cho toàn bộ biến trạng thái nhằm đảm bảo tương thích trình biên dịch Simulink Coder.
  - Thiết lập quy trình phanh điện từ hai giai đoạn và bảo vệ ngắt xung khi đảo chiều quay.

### Giai đoạn 3: Thử nghiệm cân bằng thực tế và tối ưu hóa hai vòng lặp - 2026-10-30
- **Trạng thái:** Hoàn thành
- **Nội dung thực hiện:**
  - Triển khai kiến trúc điều khiển hai vòng lặp kết hợp điều chỉnh điểm đặt động.
  - Thử nghiệm thực tế đạt mốc tự cân bằng tĩnh liên tục trong 75 giây tại chỗ với biên độ dao động góc nghiêng nằm trong dải $\pm 0.8^\circ$.
  - Thực hiện thí nghiệm đối sánh đáp ứng ở các mức điện áp nguồn 12V, 15V và 20V để xác định mối quan hệ giữa độ lợi vòng hở và độ trễ pha cảm biến.

### Giai đoạn 4: Hoàn thiện an toàn vận hành và khử rung - 2026-11-01
- **Trạng thái:** Hoàn thành
- **Nội dung thực hiện:**
  - Kiểm tra toàn diện mô hình `TESTNIDEC.slx`, xử lý triệt để lỗi xung đột thời gian hiệu chuẩn ban đầu giữa khối lọc cảm biến và khối điều khiển.
  - Tối ưu hóa bộ lọc Kalman thích nghi nhằm loại bỏ ảnh hưởng của gia tốc tiếp tuyến tại vị trí lắp đặt cảm biến cao 60 mm và giảm độ trễ pha đường vận tốc góc từ 15 ms xuống 3 ms.
  - Tích hợp khóa chốt lỗi ngã xe, cơ chế tự mở khóa khi dựng xe thẳng đứng và bộ giới hạn mềm công suất ở dải tốc độ cao.

### Giai đoạn 5: Tối ưu hóa quán tính bánh đà 73.3 g và nâng cấp bộ điều khiển lai hợp bốn tầng - 2026-11-06
- **Trạng thái:** Hoàn thành
- **Nội dung thực hiện:**
  - Bóc tách chính xác ngân sách khối lượng toàn xe từ dữ liệu phần mềm cắt lớp in 3D Bambu Studio và tiêu chuẩn cơ khí quốc tế DIN 912, DIN 934; tinh giản bánh đà xuống cấu hình 7 cụm bu lông M4x16 và 28 đai ốc M4 đạt khối lượng $73.3\text{ g}$.
  - Nâng cấp khối `fcn_Balance_Controller` lên kiến trúc lai hợp bốn tầng gồm bù trước trọng lực, điều khiển tỉ lệ - tích phân - vi sai có kẹp chống bão hòa, giới hạn tốc độ biến thiên lệnh $8.0\%$ mỗi chu kỳ và điều chỉnh điểm đặt động vòng ngoài.
  - Hiệu chuẩn ngưỡng vùng chết khởi động `ZERO_DUTY = 94.5%` trong `fcn_Nidec_Driver` và hoàn thiện hệ thống tài liệu tham khảo học thuật đã kiểm chứng.
