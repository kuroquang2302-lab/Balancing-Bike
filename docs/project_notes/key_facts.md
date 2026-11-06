# Thông Số Kỹ Thuật Và Đặc Tính Thực Nghiệm

## 1. Thông Số Cơ Khí
- **Tổng khối lượng hệ thống:** $750\text{ g} - 800\text{ g}$ bao gồm khung in 3D, hai động cơ BLDC Nidec 24H, Servo MG995, pin Li-Po 3S và bo mạch Nucleo-64.
- **Bánh đà phản lực:** Khối lượng $100.0\text{ g}$, đường kính $130\text{ mm}$, mô-men quán tính $J = 3.1 \times 10^{-4}\text{ kg}\cdot\text{m}^2$.
- **Chiều dài cơ sở:** $0.22\text{ m}$ giữa hai trục bánh xe.
- **Truyền động bánh sau:** Đai răng GT2 bản rộng $6\text{ mm}$, chu vi $240\text{ mm}$.

## 2. Cảm Biến Quán Tính MPU-6050
- **Vị trí lắp đặt:** Độ cao $60\text{ mm}$ so với mặt phẳng tiếp đất; trục X dọc thân xe, trục Y ngang thân xe.
- **Góc cân bằng tĩnh danh định:** $+1.30^\circ$.
- **Độ trôi tĩnh vận tốc góc sau hiệu chuẩn:** $< 0.02^\circ/\text{s}$.
- **Chu kỳ lấy mẫu và lọc trạng thái:** $T_s = 5\text{ ms}$, tần số $200\text{ Hz}$.

## 3. Đặc Tính Động Cơ BLDC Nidec 24H Ở Điện Áp 12V
- **Tín hiệu điều khiển:** PWM $20\text{ kHz}$ Active-Low; ngưỡng bắt đầu quay ứng với Duty Cycle $94.0\% - 94.5\%$.
- **Độ phân giải Encoder tích hợp:** Đĩa từ $100\text{ xung/vòng}$, giải mã Quadrature $4\text{x}$ trên TIM3/TIM2 đạt $400\text{ PPR}$.
- **Quan hệ tuyến tính giữa mức lệnh và tốc độ không tải tại $12\text{ V}$:**
  - $1\% \rightarrow 111\text{ RPM}$
  - $10\% \rightarrow 434\text{ RPM}$
  - $30\% \rightarrow 989\text{ RPM}$
  - $50\% \rightarrow 1560\text{ RPM}$
  - $70\% \rightarrow 2180\text{ RPM}$
  - $100\% \rightarrow 3136\text{ RPM}$ không tải, $2500\text{ RPM}$ có tải bánh đà
- **Hệ số khuếch đại vận tốc tĩnh:** $K_v \approx 31.36\text{ RPM}/\%$.
- **Dòng điện tiêu thụ:** $10\text{ mA} - 80\text{ mA}$ ở trạng thái xác lập; dòng đỉnh khi đảo chiều ở mức lệnh $30\%$ đạt $0.3\text{ A} - 0.5\text{ A}$.

## 4. Giới Hạn Sức Phản Điện Động Back-EMF Theo Điện Áp Nguồn
- Tại điện áp $24\text{ V}$, vận tốc cực đại của Nidec 24H đạt $\approx 6000\text{ RPM}$; ở $1500\text{ RPM}$ sức phản điện động chiếm $25\%$ điện áp nguồn.
- Tại điện áp $12\text{ V}$ của pin 3S, vận tốc cực đại giới hạn ở $3136\text{ RPM}$; khi vượt quá $1500\text{ RPM}$, sức phản điện động chiếm trên $50\%$ điện áp nguồn làm giảm biên độ gia tốc góc $\dot{\omega}$. Hệ thống do đó yêu cầu vòng điều khiển ngoài giữ tốc độ làm việc của bánh đà quanh vùng $0\text{ RPM}$.
