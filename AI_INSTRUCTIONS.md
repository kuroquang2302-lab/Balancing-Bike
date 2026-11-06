# Quy Chuẩn Quản Lý Tri Thức Và Mã Nguồn Dự Án

## 1. Hệ Thống Hồ Sơ Tri Thức Dự Án

Dự án lưu trữ toàn bộ dữ liệu thực nghiệm, lịch sử xử lý lỗi và quyết định thiết kế kiến trúc trong thư mục `docs/project_notes/` nhằm đảm bảo tính nhất quán xuyên suốt quá trình phát triển:

- **bugs.md:** Nhật ký lỗi kỹ thuật thực tế, nguyên nhân gốc rễ và giải pháp khắc phục triệt để.
- **decisions.md:** Hồ sơ các quyết định thiết kế kiến trúc từ `ADR-001` đến `ADR-013`.
- **key_facts.md:** Thông số cơ khí, bảng bóc tách khối lượng, đặc tính động cơ Nidec 24H, cấu hình chân vi điều khiển và dữ liệu đối sánh học thuật.
- **issues.md:** Nhật ký tiến độ thực hiện qua từng giai đoạn kiểm chuẩn và tối ưu hóa.

## 2. Quy Trình Tra Cứu Và Cập Nhật

1. Trước khi thay đổi thuật toán điều khiển hoặc cấu hình phần cứng, bắt buộc đối chiếu với `docs/project_notes/decisions.md` và `docs/project_notes/key_facts.md`.
2. Khi phát sinh lỗi phần cứng hoặc sai lệch đáp ứng điều khiển, tra cứu `docs/project_notes/bugs.md` để áp dụng biện pháp xử lý đã được kiểm chứng và bổ sung nhật ký mới sau khi khắc phục.
3. Sau mỗi giai đoạn nâng cấp, cập nhật trạng thái hoàn thành vào `docs/project_notes/issues.md`.

## 3. Quy Chuẩn Ngôn Ngữ Và Chú Thích Học Thuật

- **Trong các tệp mã nguồn MATLAB và C/C++:** Toàn bộ chú thích phải sử dụng tiếng Việt không dấu chuẩn mã ASCII để tương thích tuyệt đối với trình biên dịch Simulink Coder trên mọi hệ điều hành. Tuyệt đối không sử dụng dấu ngoặc đơn, dấu ngoặc kép hoặc dấu chấm than trong dòng chú thích, đồng thời không pha trộn từ tiếng Anh vào câu chú thích tiếng Việt.
- **Trong các tệp tài liệu Markdown:** Sử dụng ngôn phong tiếng Việt học thuật, trang trọng, cô đọng và chuẩn xác về mặt thuật ngữ kỹ thuật. Không sử dụng dấu ngoặc đơn để giải thích chêm xen thuật ngữ tiếng Anh trong câu văn xuôi, không dùng từ ngữ cảm tính hoặc biểu tượng cảm xúc.
- **Quy chuẩn đặt tên tín hiệu:** Giữ đồng nhất tuyệt đối tên các cổng giao tiếp trên mô hình Simulink gồm `Roll_Angle`, `Gyro_X_Rate`, `Motor_RPM`, `Enable_Switch`, `Speed_Cmd`, `Enable_Out` và `Active_Offset`.
