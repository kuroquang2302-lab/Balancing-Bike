# Lý thuyết Điều khiển Cân bằng Bánh đà Phản lực 2 Vòng

## 1. Động học Bánh đà Con lắc ngược (Inverted Pendulum with Reaction Wheel)

Phương trình vi phân chuyển động quanh trục Roll:
$$I_{bike} \ddot{\theta} = m g h \sin(\theta - \theta_{CM}) - \tau_{motor}$$

Trong đó phản lực cân bằng chỉ xuất hiện khi bánh đà gia tốc:
$$\tau_{motor} = I_{wheel} \dot{\omega}_{wheel}$$

## 2. Nghịch lý Bão hòa Tốc độ và Sai lầm của Khâu Ki truyền thống
- Nếu góc cân bằng lệch tĩnh $\theta_{bias} \neq 0$, trọng lực tạo mô-men kéo liên tục $\tau_g = mgh \sin(\theta_{bias}) = \text{const} \neq 0$.
- Để triệt tiêu mô-men này, bánh đà phải tăng tốc liên tục $\dot{\omega} = \text{const}$, dẫn đến vận tốc trôi tuyến tính đến trần bão hòa (3000 RPM) và ngã xe.
- Thử nghiệm của ĐH Sư phạm Kỹ thuật TP.HCM (HCMUTE 2024) chứng minh việc đưa $K_i$ góc truyền thống làm tăng thời gian xác lập lên **55 giây** và gây mất ổn định nghiêm trọng.

## 3. Kiến trúc 2 Vòng Lặp Phân tầng (Dual-Loop Architecture)
- **Vòng trong (Inner PD Loop):** Dập tắt dao động tức thời:
  $$u = - (K_p \cdot e + K_d \cdot \dot{\theta})$$
- **Vòng ngoài (Outer Adaptive Setpoint Loop):** Tự động dịch nhẹ `Active_Offset` về đúng tâm trọng lực dựa trên vận tốc bánh đà:
  $$\dot{s} = - K_{trim} \cdot \frac{\text{Motor\_RPM}}{1000}$$
  Khi thân xe hơi nghiêng đúng vào góc cân bằng trọng lực thực tế, **trọng lực tự đỡ lấy xe**, mô-men motor yêu cầu giảm về $0\text{ N}\cdot\text{m}$, bánh đà tự động hãm về **0 RPM**.
