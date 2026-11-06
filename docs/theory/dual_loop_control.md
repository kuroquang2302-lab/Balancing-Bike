# Cơ Sở Lý Thuyết Động Lực Học Và Điều Khiển Hai Vòng Lặp

## 1. Phương Trình Động Lực Học Con Lắc Ngược Bánh Đà

Chuyển động góc nghiêng Roll của thân xe quanh trục tiếp đất được mô tả bởi phương trình vi phân:

$$I_{\text{bike}} \ddot{\theta} = m g h \sin(\theta - \theta_{\text{CM}}) - \tau_{\text{wheel}}$$

Mô-men phản lực từ bánh đà chỉ sinh ra khi có gia tốc góc:

$$\tau_{\text{wheel}} = J_{\text{wheel}} \dot{\omega}_{\text{wheel}}$$

## 2. Đặc Tính Truyền Động Velocity Mode Của Động Cơ Nidec 24H

- Mạch ASIC tích hợp bên trong động cơ BLDC Nidec 24H điều khiển vận tốc góc theo tần số và độ rộng xung PWM: $\omega_{\text{wheel}} \approx K_v u$.
- Do đó, mô-men phản lực tác dụng lên khung xe tỉ lệ với đạo hàm bậc nhất theo thời gian của lệnh điều khiển:

$$\tau_{\text{wheel}} = J_{\text{wheel}} K_v \frac{du}{dt}$$

- Nếu chỉ sử dụng bộ điều khiển PD thuần túy $u = -K_p e - K_d \dot{e}$ với $e = \theta - \theta_{\text{offset}}$, đạo hàm lệnh điều khiển trở thành:

$$\frac{du}{dt} = -K_p \dot{e} - K_d \ddot{e}$$

- Tại biên độ nghiêng cực đại khi thân xe bắt đầu đổi chiều chuyển động, vận tốc góc $\dot{e} = 0$ làm thành phần tỉ lệ $-K_p \dot{e}$ triệt tiêu, khiến bánh đà ngừng tăng tốc và mất mô-men phục hồi tĩnh.
- Khi bổ sung thành phần tích phân sai số góc $-K_i \int e\,dt$ kết hợp bù trước trọng lực $-K_g \sin(e)$, đạo hàm theo thời gian của lệnh điều khiển khôi phục đầy đủ thành phần đàn hồi tĩnh tỉ lệ với góc nghiêng $-K_i e$.

## 3. Cấu Trúc Bộ Điều Khiển Cân Bằng Phân Tầng

1. **Vòng điều khiển trong $f_s = 200\text{ Hz}$:**
   - Tổng hợp lệnh điều khiển từ khâu bù trước trọng lực, bộ điều khiển PID góc nghiêng và giảm chấn vận tốc góc bánh đà:

$$u = -K_g \sin(e) - K_p e - K_i \int e\,dt - K_d \dot{\theta} - K_w \omega_{\text{wheel}}$$

   - Trạng thái tích phân được kẹp chống bão hòa Anti-windup tại ngưỡng $\pm 25.0\%$ công suất và áp dụng hệ số suy giảm $0.995$ khi $|e| < 0.15^\circ$ để ngăn vọt lố khi qua vị trí cân bằng.
2. **Giới hạn độ dốc lệnh và bảo vệ quá tốc:**
   - Bộ giới hạn Slew-rate kẹp độ biến thiên lệnh tối đa $|\Delta u| \le 8.0\%$ trong mỗi chu kỳ $5\text{ ms}$.
   - Khi lệnh điều khiển cùng dấu với vận tốc bánh đà và $|\omega_{\text{wheel}}| > 2500\text{ RPM}$, biên độ lệnh suy giảm tuyến tính về $0\%$ tại ngưỡng $3000\text{ RPM}$.
3. **Vòng điều khiển ngoài giải bão hòa tốc độ bánh đà:**
   - Cập nhật điểm đặt góc cân bằng $\theta_{\text{offset}}$ theo phản hồi vận tốc thực tế từ Encoder TIM3:

$$\dot{\theta}_{\text{offset}} = -K_{\text{trim}} \frac{\omega_{\text{wheel}}}{1000}$$

   - Điểm đặt $\theta_{\text{offset}}$ được giới hạn trong dải $\pm 3.0^\circ$ quanh mốc hiệu chuẩn tĩnh ban đầu, hướng thân xe về đúng vị trí khối tâm vật lý để triệt tiêu vận tốc bánh đà về vùng lân cận $0\text{ RPM}$.
