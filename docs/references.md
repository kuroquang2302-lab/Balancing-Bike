# Phân Tích Kỹ Thuật Các Công Trình Tham Khảo

---

## 1. Động Lực Học Bánh Đà Và Giới Hạn Cơ Cấu Chấp Hành
- **Tác giả:** Tom Stanton
- **Tài liệu:** [Can Reaction Wheels control a Drone?](https://youtu.be/4kfBEaTncjI)
- **Điểm kỹ thuật cốt lõi:**
  - Mô-men phản lực từ bánh đà tỉ lệ với gia tốc góc $\tau = J \dot{\omega}$. Khi động cơ đạt tốc độ cực đại theo giới hạn điện áp, $\dot{\omega} \to 0$ làm triệt tiêu mô-men điều khiển.
  - Bộ điều tốc BLDC không cảm biến bị trượt pha và trễ khởi động khi đảo chiều qua $0\text{ RPM}$; hệ thống cân bằng bắt buộc sử dụng động cơ BLDC có cảm biến Hall tích hợp.

## 2. Điều Khiển Mô-Men FOC Và Giải Bão Hòa Điểm Đặt
- **Tác giả:** Every Flavor of Robot
- **Tài liệu:** [Reaction Cube Situation Is Crazy](https://youtu.be/OeoIK913tRk)
- **Điểm kỹ thuật cốt lõi:**
  - Khi điều khiển trực tiếp dòng điện sinh mô-men $\tau \propto i_q$, luật PD tạo ra hệ dao động bậc hai ổn định tự nhiên quanh vị trí cân bằng.
  - Để khử hiện tượng trôi vận tốc bánh đà do sai lệch điểm đặt tĩnh, vòng điều khiển ngoài dịch chuyển chậm góc tham chiếu $\theta_{\text{offset}}$ theo vận tốc bánh đà để trọng lực tự hãm động cơ về $0\text{ RPM}$.

## 3. Đặc Tính Đạo Hàm Vận Tốc Trên Bộ Truyền Động Velocity Mode
- **Tác giả:** James Bruton
- **Tài liệu:** [What is the Difference Between Reaction Wheels and Gyroscopes?](https://youtu.be/pJfMFUcquWM)
- **Điểm kỹ thuật cốt lõi:**
  - Phân biệt cơ chế sinh mô-men phản lực $\tau = J \dot{\omega}$ của bánh đà và mô-men tiến động của con quay CMG.
  - Trên các driver nhận lệnh vận tốc góc $\omega \approx K_v u$ như Nidec 24H, mô-men sinh ra tỉ lệ với đạo hàm lệnh điều khiển $\tau \propto \dot{u}$. Do đó thành phần tỉ lệ $-K_p e$ chỉ đóng vai trò giảm chấn theo vận tốc góc $-K_p \dot{e}$, trong khi thành phần tích phân $-K_i \int e\,dt$ khi lấy đạo hàm mới tạo ra mô-men phục hồi tĩnh $-K_i e$.

## 4. Tối Ưu Tỉ Số Mô-Men Quán Tính Và Khử Lệch Tĩnh
- **Tác giả:** Sydney Madueke
- **Tài liệu:** [Building a SELF-BALANCING reaction wheel](https://youtu.be/Cegxa2BuCUc)
- **Điểm kỹ thuật cốt lõi:**
  - Giảm khối lượng bánh đà và mô-men quán tính khung giúp động cơ Nidec đạt gia tốc góc tức thời cao hơn ở mức điện áp thấp.
  - Kết hợp khâu tích phân sai số góc có giới hạn Anti-windup để loại bỏ độ nghiêng tĩnh xác lập.

## 5. Thiết Kế Cơ Khí Gốc Và Bộ Điều Khiển Phản Hồi Trạng Thái
- **Tác giả:** RemRC
- **Tài liệu:** [Self-Balancing-Bike](https://github.com/remrc/Self-Balancing-Bike), [3-Reaction-Wheels-Self-Balancing](https://github.com/remrc/3-Reaction-Wheels-Self-Balancing) và [Thingiverse 5887157](https://www.thingiverse.com/thing:5887157)
- **Điểm kỹ thuật cốt lõi:**
  - Cung cấp thiết kế khung xe in 3D sử dụng động cơ Nidec 24H và Servo MG995.
  - Mã nguồn gốc trên Arduino Nano gộp $4$ biến trạng thái vào một biểu thức đơn vòng, dẫn đến hiện tượng thành phần hãm tốc độ động cơ làm suy giảm trực tiếp mô-men cứu góc nghiêng khi xe chao đảo lớn.

## 6. Mạch Bảo Vệ Phần Cứng Và Giao Tiếp Encoder Nidec 24H
- **Tác giả:** BD594 James Cochrane
- **Tài liệu:** [Build Your Own - 1D Wannabe Cubli Reaction Wheel](https://www.youtube.com/watch?v=vqV6nazapFQ) và [DIY 1D Wannabe Cubli](https://github.com/squirtifier/DIY---1D-Wannabe-Cubli-Reaction-Wheel)
- **Điểm kỹ thuật cốt lõi:**
  - Xác định sơ đồ chân Nidec 24H, mạch diode chống xung áp ngược trên đường nguồn động lực và điện trở kéo lên $10\text{ k}\Omega$ cho hai kênh tín hiệu Encoder cực máng hở.

## 7. Quy Trình Lắp Ráp Cơ Khí Chuẩn Hóa
- **Tác giả:** Dmitrii Tomin
- **Tài liệu:** [Printables 956136](https://www.printables.com/model/956136-3d-printed-self-balancing-bike) và [self-balancing_bike](https://github.com/Dmitrii-Tomin/self-balancing_bike)
- **Điểm kỹ thuật cốt lõi:**
  - Tài liệu hóa quy trình lắp ghép cơ khí $23$ bước cho cụm ổ bi 608ZZ, hệ truyền động đai răng GT2 bánh sau, cơ cấu lái thanh truyền Servo và cụm bánh đà phản lực.

## 8. Mô Hình Đối Sánh Dùng Động Cơ DC Và PID Một Vòng Lặp
- **Nguồn công bố:** *PID Control for Balancing Bike Model using Reaction Wheel*, *Journal of Fuzzy Systems and Control*, Vol. 2, No. 2, 2024, DOI: `10.59247/jfsc.v2i2.188`
- **Điểm kỹ thuật cốt lõi:**
  - Khảo sát mô hình xe đạp cân bằng bằng bánh đà sử dụng động cơ DC chổi than, mạch cầu H L298N và vi điều khiển Arduino Nano không có phản hồi tốc độ bánh đà.
  - Kết quả thực nghiệm trong bài báo cho thấy việc tăng hệ số tích phân $K_i$ trong cấu trúc một vòng lặp gây trễ pha lớn, kéo dài thời gian xác lập lên tới $55\text{ s}$ và xuất hiện dao động quanh điểm làm việc.
