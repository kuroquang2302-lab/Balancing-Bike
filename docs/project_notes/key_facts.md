# Thông Số Kỹ Thuật Và Dữ Liệu Thực Nghiệm

## 1. Thông Số Cơ Khí Và Vật Lý
- **Tổng khối lượng xe hoàn thiện:** $780\text{ g} - 820\text{ g}$ bao gồm khung in 3D, hai động cơ Nidec 24H, cơ cấu lái Servo MG995, pin Li-Po 3S và bo mạch Nucleo-64; khối lượng khô khi chưa gắn pin đạt $653\text{ g} - 693\text{ g}$.
- **Khối lượng bánh đà phản lực nguyên bản:** $100.0\text{ g}$ với 21 cụm ốc viền, đường kính $130\text{ mm}$, mô-men quán tính $J = 3.1 \times 10^{-4}\text{ kg}\cdot\text{m}^2$.
- **Khối lượng bánh đà phản lực tinh gọn:** $73.3\text{ g}$ với 7 cụm bu lông M4x16 và 28 đai ốc M4 bố trí đối xứng tâm, giúp giảm xấp xỉ $35\%$ mô-men quán tính và tăng gia tốc góc đáp ứng ở mức nguồn 12V.
- **Độ cao khối tâm:** $0.08\text{ m} - 0.14\text{ m}$ tính từ mặt phẳng tiếp đất tùy cấu hình gá pin.
- **Chiều dài cơ sở:** $0.22\text{ m}$ giữa hai trục bánh xe.
- **Cơ cấu truyền động bánh sau:** Đai răng GT2 bản rộng $6\text{ mm}$, chu vi $240\text{ mm}$.

## 2. Dữ Liệu Cảm Biến Quán Tính MPU-6050
- **Vị trí lắp đặt:** Độ cao $60\text{ mm}$ tính từ mặt lốp, trục X hướng dọc thân xe từ bánh trước về bánh sau, trục Y hướng ngang từ phải sang trái.
- **Góc cân bằng tĩnh danh định:** $+1.30^\circ$.
- **Độ trôi tĩnh vận tốc góc sau hiệu chuẩn:** Nhỏ hơn $0.02^\circ/\text{s}$.
- **Tần số lấy mẫu vòng kín:** $200\text{ Hz}$, tương ứng chu kỳ $5\text{ ms}$.

## 3. Đặc Tính Động Cơ Không Chổi Than Nidec 24H
- **Điện áp vận hành:** $11.1\text{ V} - 12.6\text{ V}$ từ khối pin Li-Po 3S.
- **Tần số xung điều khiển:** $20\text{ kHz}$, logic tích cực mức thấp, ngưỡng bắt đầu quay ứng với hệ số lấp đầy $94.0\%$ đối với bánh đà $100.0\text{ g}$ và $94.5\%$ đối với bánh đà tinh gọn $73.3\text{ g}$.
- **Đặc tính tốc độ theo mức lệnh điều khiển tại điện áp 12V:**
  - Mức lệnh $1\%$: $111\text{ RPM}$
  - Mức lệnh $10\%$: $434\text{ RPM}$
  - Mức lệnh $30\%$: $989\text{ RPM}$ không tải, $980\text{ RPM}$ khi mang bánh đà $100\text{ g}$
  - Mức lệnh $50\%$: $1560\text{ RPM}$
  - Mức lệnh $70\%$: $2180\text{ RPM}$
  - Mức lệnh $100\%$: $3136\text{ RPM}$ không tải, $2500\text{ RPM}$ có tải
- **Dòng điện tiêu thụ:** $10\text{ mA} - 80\text{ mA}$ khi chạy ổn định; dòng đỉnh khi đảo chiều ở mức lệnh $30\%$ đạt $0.3\text{ A} - 0.5\text{ A}$.
- **Thời gian đảo chiều có tải từ $+990\text{ RPM}$ sang $-990\text{ RPM}$:** Xấp xỉ $1.0\text{ s}$ với bánh đà $100.0\text{ g}$ và rút ngắn xuống $0.65\text{ s}$ với bánh đà $73.3\text{ g}$.
- **Độ phân giải bộ mã hóa vòng quay:** Đĩa từ $100\text{ xung/vòng}$, giải mã nhân bốn trên phần cứng STM32 đạt $400\text{ xung/vòng}$.

## 4. Phân Tích Ảnh Hưởng Của Điện Áp Nguồn
- Ở mức điện áp 24V, động cơ đạt vận tốc tối đa $6000\text{ RPM}$, sức phản điện động tại $1500\text{ RPM}$ chỉ chiếm $25\%$ điện áp nguồn nên duy trì được mô-men gia tốc lớn.
- Ở mức điện áp 12V, vận tốc tối đa đạt $3136\text{ RPM}$; khi vượt $1500\text{ RPM}$, sức phản điện động chiếm trên $50\%$ điện áp nguồn làm giảm khả năng sinh gia tốc góc. Do đó hệ thống bắt buộc phải kết hợp giảm mô-men quán tính bánh đà xuống $73.3\text{ g}$, bổ sung khâu tích phân sai số góc có giới hạn chống bão hòa và duy trì vòng điều khiển ngoài để đưa tốc độ bánh đà về vùng lân cận 0 RPM.

## 5. Số Liệu Đối Sánh Học Thuật
- **Mô hình đối sánh trong nước:** Nghiên cứu *PID Control for Balancing Bike Model using Reaction Wheel* trên tạp chí *Journal of Fuzzy Systems and Control*, Tập 2, Số 2, Năm 2024, mã định danh DOI `10.59247/jfsc.v2i2.188` sử dụng khung xe khối lượng $434\text{ g}$, bánh đà $43\text{ g}$, vi điều khiển Arduino Nano và động cơ điện một chiều có chổi than qua mạch cầu H L298N.
- **Hạn chế của cấu trúc một vòng lặp:** Không trang bị bộ mã hóa đo vận tốc bánh đà và dùng khâu tích phân góc truyền thống không có vòng khử bão hòa khiến thời gian xác lập kéo dài tới $55\text{ s}$ kèm dao động liên tục quanh điểm làm việc lệch tĩnh từ $-1.0^\circ$ đến $-4.0^\circ$.
- **Cải tiến của đề tài:** Kết hợp động cơ không chổi than Nidec 24H, bộ đếm phần cứng TIM3 400 xung/vòng và thuật toán điều khiển lai hợp bốn tầng hai vòng lặp giúp rút ngắn thời gian xác lập xuống dưới $0.8\text{ s}$ và duy trì cân bằng tĩnh liên tục trên $75\text{ s}$.

## 6. Bảng Bóc Tách Khối Lượng Thực Tế Toàn Xe
- **Tổng khối lượng nhựa in 3D từ phần mềm Bambu Studio:** $192.33\text{ g}$ phần chi tiết chính, tổng lượng nhựa tiêu thụ tính cả phần cấu trúc đỡ là $204.64\text{ g}$; trong đó riêng phần nhựa bánh đà nặng $34.89\text{ g}$ và toàn bộ phần khung, gá động cơ, càng lái, hai bánh xe nặng $157.44\text{ g}$.
- **Khối lượng cụm kim loại viền bánh đà tinh gọn theo tiêu chuẩn cơ khí DIN 912 và DIN 934:**
  - 7 bu lông lục giác chìm đầu trụ M4x16 chuẩn DIN 912: $7 \times 2.25\text{ g} = 15.75\text{ g}$.
  - 28 đai ốc lục giác M4 chuẩn DIN 934: $28 \times 0.81\text{ g} = 22.68\text{ g}$.
  - Tổng khối lượng bánh đà tinh gọn hoàn thiện: $34.89\text{ g} + 15.75\text{ g} + 22.68\text{ g} = 73.32\text{ g}$.
- **Khối lượng các cụm cơ điện tử trên xe theo tài liệu lắp ghép 23 bước:**
  - Hai động cơ không chổi than Nidec 24H: $2 \times 155\text{ g} = 310.0\text{ g}$.
  - Cơ cấu lái Servo MG995 cùng phụ kiện trục lái: $55.0\text{ g}$.
  - Năm ổ bi cầu 608ZZ và các bu lông, đai ốc M3, M4, thanh ren M4 lắp khung: $115.0\text{ g} - 125.0\text{ g}$.
  - Bo mạch vi điều khiển STM32F411RE Nucleo-64, cảm biến MPU-6050, mạch hạ áp và dây dẫn: $85.0\text{ g} - 95.0\text{ g}$.
  - Khối pin Li-Po 3S 11.1V dung lượng 1300 mAh đến 1500 mAh: $115.0\text{ g} - 135.0\text{ g}$.
