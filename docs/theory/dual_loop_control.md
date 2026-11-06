# Cơ Sở Lý Thuyết Điều Khiển Cân Bằng Hai Vòng Lặp Và Kiến Trúc Lai Hợp Bốn Tầng

## 1. Mô Hình Động Học Con Lắc Ngược Bánh Đà

Phương trình vi phân mô tả chuyển động góc nghiêng của thân xe quanh trục tiếp đất:

$$I_{\text{bike}} \ddot{\theta} = m g h \sin(\theta - \theta_{\text{CM}}) - \tau_{\text{motor}}$$

Mô-men phản lực từ bánh đà chỉ xuất hiện khi có sự biến thiên vận tốc góc của bánh đà:

$$\tau_{\text{motor}} = I_{\text{wheel}} \dot{\omega}_{\text{wheel}}$$

## 2. Hiện Tượng Bão Hòa Vận Tốc Và Sự Khác Biệt Giữa Điều Khiển Mô-Men Với Điều Khiển Vận Tốc

- Khi điểm đặt góc nghiêng lệch một lượng $\theta_{\text{bias}} \neq 0$ so với vị trí khối tâm thực tế, trọng lực sinh ra mô-men gây lật không đổi $\tau_g = m g h \sin(\theta_{\text{bias}})$.
- Để duy trì trạng thái cân bằng tại góc lệch này, động cơ buộc phải gia tốc liên tục với $\dot{\omega}_{\text{wheel}}$ khác không, làm vận tốc bánh đà tăng tuyến tính cho đến khi chạm giới hạn tốc độ điện áp và mất khả năng sinh mô-men phục hồi.
- Đối với động cơ không chổi than Nidec 24H có mạch điều khiển vận tốc tích hợp, tín hiệu điều rộng xung quyết định vận tốc góc xác lập $\omega_{\text{wheel}} \approx K_v u$. Do đó, mô-men phản lực sinh ra tỉ lệ với đạo hàm theo thời gian của lệnh điều khiển:

$$\tau_{\text{motor}} \propto \frac{du}{dt}$$

- Nếu chỉ sử dụng luật tỉ lệ - vi sai thuần túy $u = -K_p (\theta - \theta_{\text{offset}}) - K_d \dot{\theta}$, đạo hàm của lệnh điều khiển sẽ trở thành $\dot{u} = -K_p \dot{\theta} - K_d \ddot{\theta}$. Tại thời điểm thân xe nghiêng đến biên độ cực đại và bắt đầu đổi chiều chuyển động, vận tốc góc thân xe $\dot{\theta}$ bằng 0 làm thành phần tỉ lệ mất khả năng sinh gia tốc góc cho bánh đà.
- Khi bổ sung thành phần tích phân sai số góc có giới hạn chống bão hòa $-K_i \int (\theta - \theta_{\text{offset}}) dt$ kết hợp bù trước trọng lực, đạo hàm của khâu tích phân sẽ tái tạo lại chính xác thành phần mô-men phục hồi tĩnh $-K_i (\theta - \theta_{\text{offset}})$, giúp động cơ tiếp tục tăng tốc mạnh mẽ để kéo thân xe trở về vị trí thẳng đứng.

## 3. Cấu Trúc Điều Khiển Lai Hợp Bốn Tầng Hai Vòng Lặp

Hệ thống điều khiển được tổ chức thành bốn tầng chức năng phối hợp chặt chẽ ở tần số lấy mẫu 200 Hz:

- **Tầng thứ nhất - Bù trước mô-men trọng lực:** Tính toán trực tiếp thành phần bù phi tuyến theo góc nghiêng thực tế nhằm gánh phần lớn mô-men gây lật của trọng trường:

$$u_{\text{grav}} = -K_g \sin(\theta - \theta_{\text{offset}})$$

- **Tầng thứ hai - Điều khiển tỉ lệ - tích phân - vi sai vòng trong:** Ổn định góc nghiêng tức thời và sinh mô-men phục hồi tĩnh với cơ chế kẹp biên độ tích phân trong ngưỡng an toàn và xả nhẹ tích phân khi sai số góc nhỏ hơn $0.15^\circ$:

$$u_{\text{PID}} = -K_p (\theta - \theta_{\text{offset}}) - K_i \int (\theta - \theta_{\text{offset}}) dt - K_d \dot{\theta} - K_w \omega_{\text{wheel}}$$

- **Tầng thứ ba - Giới hạn tốc độ biến thiên lệnh và thích nghi phần cứng:** Giới hạn độ biến thiên lệnh điều khiển tối đa $8.0\%$ trong mỗi chu kỳ $5\text{ ms}$ để loại bỏ hiện tượng giật cục bộ, kết hợp bù vùng chết khởi động tại ngưỡng hệ số lấp đầy $94.5\%$ tương ứng với bánh đà tinh gọn $73.3\text{ g}$ và suy giảm tuyến tính lệnh cùng chiều khi vận tốc vượt quá $2500\text{ RPM}$.

- **Tầng thứ tư - Điều chỉnh điểm đặt cân bằng động vòng ngoài:** Điều chỉnh chậm điểm đặt góc cân bằng $\theta_{\text{offset}}$ dựa trên vận tốc thực tế của bánh đà đọc từ bộ đếm phần cứng TIM3:

$$\dot{\theta}_{\text{offset}} = -K_{\text{trim}} \frac{\omega_{\text{wheel}}}{1000}$$

Khi điểm đặt $\theta_{\text{offset}}$ hội tụ về đúng vị trí khối tâm vật lý của xe, mô-men trọng lực tự triệt tiêu và vận tốc bánh đà giảm dần về vùng lân cận 0 RPM.
