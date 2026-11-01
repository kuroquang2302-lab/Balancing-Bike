# Cơ Sở Lý Thuyết Điều Khiển Cân Bằng Hai Vòng Lặp

## 1. Mô Hình Động Học Con Lắc Ngược Bánh Đà

Phương trình vi phân mô tả chuyển động góc nghiêng của thân xe quanh trục tiếp đất:

$$I_{\text{bike}} \ddot{\theta} = m g h \sin(\theta - \theta_{\text{CM}}) - \tau_{\text{motor}}$$

Mô-men phản lực từ bánh đà chỉ xuất hiện khi có sự biến thiên vận tốc góc của bánh đà:

$$\tau_{\text{motor}} = I_{\text{wheel}} \dot{\omega}_{\text{wheel}}$$

## 2. Hiện Tượng Bão Hòa Vận Tốc Và Hạn Chế Của Khâu Tích Phân Góc

- Khi điểm đặt góc nghiêng lệch một lượng $\theta_{\text{bias}} \neq 0$ so với vị trí khối tâm thực tế, trọng lực sinh ra mô-men gây lật không đổi $\tau_g = m g h \sin(\theta_{\text{bias}})$.
- Để duy trì trạng thái cân bằng tại góc lệch này, động cơ buộc phải gia tốc liên tục với $\dot{\omega}_{\text{wheel}}$ khác không, làm vận tốc bánh đà tăng tuyến tính cho đến khi chạm giới hạn tốc độ điện áp và mất khả năng sinh mô-men phục hồi.
- Việc sử dụng khâu tích phân góc truyền thống gây trễ pha lớn, kéo dài thời gian xác lập và tạo dao động chu kỳ biên độ lớn.

## 3. Cấu Trúc Điều Khiển Hai Vòng Lặp Phân Tầng

- **Vòng điều khiển trong:** Sử dụng luật tỉ lệ - vi sai kết hợp giảm chấn vận tốc bánh đà để ổn định góc nghiêng tức thời với tần số lấy mẫu 200 Hz:

$$u = -K_p (\theta - \theta_{\text{offset}}) - K_d \dot{\theta} - K_w \omega_{\text{wheel}}$$

- **Vòng điều khiển ngoài:** Điều chỉnh chậm điểm đặt góc cân bằng $\theta_{\text{offset}}$ dựa trên vận tốc thực tế của bánh đà đọc từ bộ đếm phần cứng TIM3:

$$\dot{\theta}_{\text{offset}} = -K_{\text{trim}} \frac{\omega_{\text{wheel}}}{1000}$$

Khi điểm đặt $\theta_{\text{offset}}$ hội tụ về đúng vị trí khối tâm vật lý của xe, mô-men trọng lực tự triệt tiêu và vận tốc bánh đà giảm dần về vùng lân cận 0 RPM.
