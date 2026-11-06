# Tổng Hợp Và Phân Tích Các Công Trình Nghiên Cứu Tham Khảo

Tài liệu này hệ thống hóa các công trình thực nghiệm, mã nguồn mở và bài báo khoa học quốc tế cũng như trong nước được nhóm nghiên cứu đối chiếu, kế thừa và cải tiến trong quá trình phát triển hệ thống xe đạp tự cân bằng.

---

## 1. Phân Tích Vật Lý Bánh Đà Và Lựa Chọn Động Cơ Truyền Động

- **Tác giả:** Tom Stanton
- **Công trình:** [Can Reaction Wheels control a Drone?](https://youtu.be/4kfBEaTncjI)
- **Nội dung cốt lõi:**
  - Phân tích định luật bảo toàn mô-men động lượng khi sử dụng bánh đà phản lực để ổn định tư thế quanh trục nghiêng.
  - Chỉ ra rằng mô-men phản lực chỉ xuất hiện khi bánh đà có gia tốc góc khác không. Khi bánh đà đã chạm ngưỡng vận tốc cực đại của nguồn điện, gia tốc góc triệt tiêu làm hệ thống mất hoàn toàn khả năng sinh mô-men cân bằng.
  - Chứng minh các bộ điều tốc điện tử thông thường dành cho máy bay mô hình không có cảm biến vị trí rotor sẽ gặp hiện tượng trượt bước và trễ lớn khi đảo chiều qua điểm 0 RPM, do đó bắt buộc phải sử dụng động cơ không chổi than có cảm biến Hall tích hợp và điều khiển vòng kín.

## 2. Cơ Chế Điều Khiển Mô-Men Xoắn Và Khử Bão Hòa Điểm Đặt

- **Tác giả:** Every Flavor of Robot
- **Công trình:** [Reaction Cube Situation Is Crazy](https://youtu.be/OeoIK913tRk)
- **Nội dung cốt lõi:**
  - Khi điều khiển trực tiếp dòng điện sinh mô-men xoắn theo phương pháp điều khiển định hướng từ trường, luật điều khiển tỉ lệ - vi sai trực tiếp biến phương trình động lực học của con lắc ngược thành hệ dao động điều hòa bậc hai có cản và ổn định tự nhiên quanh vị trí cân bằng.
  - Đề xuất thuật toán vòng ngoài điều chỉnh chậm điểm đặt góc nghiêng theo vận tốc góc thực tế của bánh đà, tận dụng mô-men trọng lực tự nhiên của khung cơ khí để hãm bánh đà giảm tốc về vùng lân cận 0 RPM.

## 3. Sự Khác Biệt Giữa Điều Khiển Mô-Men Và Điều Khiển Vận Tốc

- **Tác giả:** James Bruton
- **Công trình:** [What is the Difference Between Reaction Wheels and Gyroscopes?](https://youtu.be/pJfMFUcquWM)
- **Nội dung cốt lõi:**
  - Phân biệt rõ cơ chế sinh mô-men do gia tốc góc của bánh đà phản lực và cơ chế sinh mô-men tiến động của con quay hồi chuyển điều khiển mô-men.
  - Chỉ ra đặc tính quan trọng trên các bộ truyền động nhận lệnh điều khiển theo vận tốc góc như Nidec 24H: do mô-men phản lực tỉ lệ với đạo hàm bậc nhất theo thời gian của vận tốc góc, thành phần tỉ lệ theo góc nghiêng bị đạo hàm thành vận tốc góc thân xe, còn thành phần tích phân sai số góc khi lấy đạo hàm mới tạo ra mô-men phục hồi tỉ lệ thuận với góc nghiêng tĩnh.

## 4. Tối Ưu Hóa Tỉ Số Mô-Men Quán Tính Và Triệt Tiêu Độ Lệch Tĩnh

- **Tác giả:** Sydney Madueke
- **Công trình:** [Building a SELF-BALANCING reaction wheel](https://youtu.be/Cegxa2BuCUc)
- **Nội dung cốt lõi:**
  - Giải quyết hiện tượng động cơ Nidec quay ở tốc độ rất cao nhưng không đủ mô-men nhấc khung cơ khí dậy bằng cách thu gọn khối lượng bánh đà và giảm mô-men quán tính của phần khung.
  - Khẳng định vai trò của thành phần tích phân có giới hạn chống bão hòa trong việc xóa bỏ hoàn toàn độ lệch góc nghiêng xác lập khi giữ thăng bằng.

## 5. Thiết Kế Cơ Khí Xe Đạp Nguyên Bản Dùng Động Cơ Nidec 24H

- **Tác giả:** RemRC
- **Công trình:** [Self-Balancing-Bike](https://github.com/remrc/Self-Balancing-Bike), [3-Reaction-Wheels-Self-Balancing](https://github.com/remrc/3-Reaction-Wheels-Self-Balancing) và [Mẫu thiết kế 3D Thingiverse 5887157](https://www.thingiverse.com/thing:5887157)
- **Nội dung cốt lõi:**
  - Cung cấp bộ bản vẽ chi tiết in 3D cho xe đạp tự cân bằng sử dụng động cơ không chổi than Nidec 24H và cơ cấu lái Servo MG995.
  - Mã nguồn gốc sử dụng vi điều khiển Arduino Nano 8-bit với luật điều khiển gộp bốn biến trạng thái vào chung một biểu thức tính toán. Đề tài đã kế thừa phần kết cấu cơ khí này và nâng cấp toàn diện lên vi điều khiển 32-bit STM32F411RE cùng kiến trúc điều khiển lai hợp bốn tầng tách biệt.

## 6. Mạch Bảo Vệ Phần Cứng Và Giao Tiếp Bộ Mã Hóa Vòng Quay

- **Tác giả:** BD594 James Cochrane
- **Công trình:** [Build Your Own - 1D Wannabe Cubli Reaction Wheel](https://www.youtube.com/watch?v=vqV6nazapFQ) và [Kho mã nguồn DIY 1D Wannabe Cubli](https://github.com/squirtifier/DIY---1D-Wannabe-Cubli-Reaction-Wheel)
- **Nội dung cốt lõi:**
  - Phân tích sơ đồ chân thực tế của họ động cơ không chổi than Nidec 24H tích hợp mạch điều khiển.
  - Hướng dẫn chi tiết việc bố trí diode chống xung điện áp ngược trên đường nguồn động lực và điện trở kéo lên cho hai kênh tín hiệu cực máng hở của bộ mã hóa vòng quay trước khi đưa vào vi điều khiển.

## 7. Quy Trình Lắp Ghép Cơ Khí Chuẩn Hóa 23 Bước

- **Tác giả:** Dmitrii Tomin
- **Công trình:** [3D Printed Self-Balancing Bike trên Printables 956136](https://www.printables.com/model/956136-3d-printed-self-balancing-bike) và [Kho mã nguồn hướng dẫn lắp ghép](https://github.com/Dmitrii-Tomin/self-balancing_bike)
- **Nội dung cốt lõi:**
  - Chuẩn hóa tài liệu hướng dẫn lắp ráp cơ khí gồm 23 bước chi tiết từ khâu đóng ổ bi 608ZZ, lắp cụm cổ lái, căng đai răng GT2 bánh sau đến lắp ráp bánh đà phản lực.
  - Cung cấp bảng định mức vật tư cơ khí tiêu chuẩn dùng làm cơ sở để nhóm nghiên cứu tính toán chính xác ngân sách khối lượng của từng cụm chi tiết trên xe.

## 8. Nghiên Cứu Đối Sánh Học Thuật Trong Nước Năm 2024

- **Nguồn công bố:** Bài báo *PID Control for Balancing Bike Model using Reaction Wheel*, Tạp chí *Journal of Fuzzy Systems and Control*, Tập 2, Số 2, Năm 2024, Mã định danh DOI: `10.59247/jfsc.v2i2.188`
- **Nội dung cốt lõi:**
  - Xây dựng mô hình toán học và thực nghiệm xe đạp tự cân bằng bằng bánh đà trên khung xe khối lượng 434 g, bánh đà 43 g, sử dụng vi điều khiển Arduino Nano, cảm biến MPU-6050 và động cơ điện một chiều có chổi than điều khiển qua mạch cầu H L298N.
  - Chỉ ra rằng cấu trúc điều khiển một vòng lặp không có phản hồi vận tốc bánh đà khi tăng hệ số tích phân sẽ gây trễ pha lớn, làm thời gian xác lập kéo dài tới 55 giây kèm dao động quanh vị trí cân bằng.
  - Kết quả của bài báo là minh chứng thực nghiệm quan trọng khẳng định tính ưu việt của giải pháp sử dụng động cơ không chổi than Nidec 24H kết hợp bộ đếm phần cứng TIM3 và kiến trúc điều khiển hai vòng lặp trong đề tài hiện tại, giúp giảm thời gian xác lập xuống dưới 0.8 giây và duy trì trạng thái cân bằng ổn định liên tục trên 75 giây.
