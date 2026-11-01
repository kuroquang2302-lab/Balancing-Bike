# Thông Số Kỹ Thuật Và Dữ Liệu Thực Nghiệm

## 1. Thông Số Cơ Khí Và Vật Lý
- **Tổng khối lượng xe hoàn thiện:** $750\text{ g} - 800\text{ g}$ bao gồm khung in 3D, hai động cơ Nidec 24H, pin Li-Po 3S và bo mạch Nucleo-64.
- **Khối lượng bánh đà phản lực:** $100\text{ g}$, đường kính $130\text{ mm}$, mô-men quán tính $J = 3.1 \times 10^{-4}\text{ kg}\cdot\text{m}^2$.
- **Độ cao khối tâm:** $0.14\text{ m} - 0.15\text{ m}$ tính từ mặt phẳng tiếp đất.
- **Chiều dài cơ sở:** $0.22\text{ m}$ giữa hai trục bánh xe.
- **Cơ cấu truyền động bánh sau:** Đai răng GT2 bản rộng $6\text{ mm}$, chu vi $240\text{ mm}$.

## 2. Dữ Liệu Cảm Biến Quán Tính MPU-6050
- **Vị trí lắp đặt:** Độ cao $60\text{ mm}$ tính từ mặt lốp, trục X hướng dọc thân xe từ bánh trước về bánh sau, trục Y hướng ngang từ phải sang trái.
- **Góc cân bằng tĩnh danh định:** $+1.30^\circ$.
- **Độ trôi tĩnh vận tốc góc sau hiệu chuẩn:** Nhỏ hơn $0.02^\circ/\text{s}$.
- **Tần số lấy mẫu vòng kín:** $200\text{ Hz}$, tương ứng chu kỳ $5\text{ ms}$.

## 3. Đặc Tính Động Cơ Không Chổi Than Nidec 24H
- **Điện áp vận hành:** $11.1\text{ V} - 12.6\text{ V}$ từ khối pin Li-Po 3S.
- **Tần số xung điều khiển:** $20\text{ kHz}$, logic tích cực mức thấp, ngưỡng bắt đầu quay ứng với hệ số lấp đầy $94.0\%$.
- **Đặc tính tốc độ theo mức lệnh điều khiển tại điện áp 12V:**
  - Mức lệnh $1\%$: $111\text{ RPM}$
  - Mức lệnh $10\%$: $434\text{ RPM}$
  - Mức lệnh $30\%$: $989\text{ RPM}$ không tải, $980\text{ RPM}$ khi mang bánh đà $100\text{ g}$
  - Mức lệnh $50\%$: $1560\text{ RPM}$
  - Mức lệnh $70\%$: $2180\text{ RPM}$
  - Mức lệnh $100\%$: $3136\text{ RPM}$ không tải, $2500\text{ RPM}$ có tải
- **Dòng điện tiêu thụ:** $10\text{ mA} - 80\text{ mA}$ khi chạy ổn định; dòng đỉnh khi đảo chiều ở mức lệnh $30\%$ đạt $0.3\text{ A} - 0.5\text{ A}$.
- **Thời gian đảo chiều có tải từ $+990\text{ RPM}$ sang $-990\text{ RPM}$:** Xấp xỉ $1.0\text{ s}$.
- **Độ phân giải bộ mã hóa vòng quay:** Đĩa từ $100\text{ xung/vòng}$, giải mã nhân bốn trên phần cứng STM32 đạt $400\text{ xung/vòng}$.

## 4. Phân Tích Ảnh Hưởng Của Điện Áp Nguồn
- Ở mức điện áp 24V, động cơ đạt vận tốc tối đa $6000\text{ RPM}$, sức phản điện động tại $1500\text{ RPM}$ chỉ chiếm $25\%$ điện áp nguồn nên duy trì được mô-men gia tốc lớn.
- Ở mức điện áp 12V, vận tốc tối đa đạt $3136\text{ RPM}$; khi vượt $1500\text{ RPM}$, sức phản điện động chiếm trên $50\%$ điện áp nguồn làm giảm khả năng sinh gia tốc góc. Do đó hệ thống bắt buộc phải duy trì vòng điều khiển ngoài để đưa tốc độ bánh đà về vùng lân cận 0 RPM.

## 5. Số Liệu Đối Sánh Học Thuật
- **Mô hình đối sánh trong nước:** Nghiên cứu trên tạp chí *Journal of Fuzzy Systems and Control*, Tập 2, Số 2, Năm 2024 sử dụng khung xe khối lượng $434\text{ g}$, bánh đà $43\text{ g}$, vi điều khiển Arduino Nano và động cơ điện một chiều có chổi than qua mạch cầu H L298N.
- **Hạn chế của cấu trúc một vòng lặp:** Không trang bị bộ mã hóa đo vận tốc bánh đà và dùng khâu tích phân góc truyền thống khiến thời gian xác lập kéo dài tới $55\text{ s}$ kèm dao động liên tục quanh điểm làm việc.
- **Cải tiến của đề tài:** Kết hợp động cơ không chổi than Nidec 24H, bộ đếm phần cứng TIM3 400 xung/vòng và thuật toán điều khiển hai vòng lặp giúp rút ngắn thời gian xác lập xuống dưới $0.8\text{ s}$ và duy trì cân bằng tĩnh liên tục trên $75\text{ s}$.
