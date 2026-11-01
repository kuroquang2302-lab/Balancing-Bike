# Work Log - Dự án Xe Đạp Tự Cân Bằng UTH

### 2026-10-28 - Đã chốt Roll_Offset = 1.30 độ, bước vào pha thử nghiệm cân bằng đóng vòng
- **Status**: In Progress (Chạy thử nghiệm cân bằng bánh đà trên tay & tại chỗ)
- **Tóm tắt công việc đã hoàn thành**:
  - Đo kiểm thực tế MPU-6050 trên xe hoàn thiện: `Roll_Offset = +1.30°` (`1.2987°`), `Gyro_X_Rate` tĩnh đạt `-0.0195°/s` (nhiễu cực thấp).
  - Tín hiệu động GyroX đáp ứng sắc nét $\pm 25^\circ/\text{s} \rightarrow \pm 42^\circ/\text{s}$ trên Scope.
  - Phần cứng Nidec 1 & 2 đã nghiệm thu độc lập hoàn tất.
- **Kế hoạch tiếp theo**:
  - Ghép nối `fcn_Balance_Controller` với `fcn_Nidec_Driver`.
  - Thực hiện bài test kiểm tra chiều phản lực trên tay (Hand-held Sign Convention Test) với $K_p = 15.0$ / $K_d = 0.6$.
  - Tiến hành thử nghiệm thăng bằng tại chỗ (Standstill Balancing) trên sàn.

### 2026-10-29 - Tích hợp và kiểm chứng Máy trạng thái Tự động Calib cho fcn_Balance_Controller
- **Status**: Completed (Code review & simulation verification pass 100%)
- **Nội dung hoàn thành**:
  - Đã soi và sửa lỗi gõ nhầm biến `err_raw` thành `err_check`.
  - Đồng bộ toàn bộ kiểu dữ liệu `double` cho persistent variables trong Coder.
  - Chạy mô phỏng Python kiểm chứng logic chuyển trạng thái: Khởi động nằm yên $\rightarrow$ Dựng xe 1s tự calib $\rightarrow$ Bật cân bằng $\rightarrow$ Ngã tự ngắt và reset State 0.

### 2026-10-29 - Chốt bản code fcn_Balance_Controller hoàn thiện (Active Offset + Kw + Bảo vệ)
- **Status**: Ready for Live Bench Test
- **Nội dung hoàn thành**:
  - Tích hợp tính năng tự động chốt điểm cân bằng tĩnh `Active_Offset` khi người dùng dựng xe giữ yên 1 giây.
  - Tích hợp khâu `Kw * Motor_RPM` để ghìm bánh đà không bị trôi tốc độ.
  - Cài đặt trần an toàn `MAX_GA = 40.0%` khi thử nghiệm trên tay.
  - Xác nhận cố định $K_p > 0$ và $K_d > 0$ theo dữ liệu thực tế của xe.

### 2026-10-29 - Tinh chỉnh ngưỡng ngắt quá tốc 1100 RPM và cửa sổ Calib 4.0 độ
- **Status**: Verified & Ready for bench test
- **Chi tiết**: Đã đồng bộ ngưỡng `overspeed_timer` về 1100 RPM tương ứng 85% trần ga 40%, thu hẹp cửa sổ calib về $|Roll| < 4.0^\circ$, và chốt dải $K_w = 0.002 - 0.005$.
