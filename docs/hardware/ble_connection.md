# Đặc Tả Giao Tiếp Không Dây BLE Và Cầu Nối ESP32-C3 Super Mini

## 1. Kiến Trúc Phối Hợp Hai Vi Điều Khiển

Để đảm bảo tính tất định thời gian thực cho vòng lặp điều khiển cân bằng ở tần số lấy mẫu $200\text{ Hz}$ ($5\text{ ms}$) trên vi điều khiển trung tâm `STM32F411RE`, toàn bộ ngăn xếp giao thức không dây Bluetooth Low Energy (BLE 5.0) được tách biệt sang vi điều khiển đồng xử lý `ESP32-C3 Super Mini`.

- **Thiết bị điều khiển (iOS):** Ứng dụng `Adafruit Bluefruit LE Connect` hoặc `LightBlue` kết nối với `ESP32-C3` thông qua dịch vụ chuẩn Nordic UART Service (NUS).
- **Khối đồng xử lý không dây (`ESP32-C3 Super Mini`):** Giải mã gói tin nút bấm điều khiển hướng lái và các lệnh tinh chỉnh thông số tại chỗ, sau đó đóng gói thành khung dữ liệu nhị phân cố định $14\text{ byte}$ có mã kiểm tra lỗi đồng vị `XOR` và phát định kỳ ở tần số $50\text{ Hz}$ ($20\text{ ms}$) sang `STM32F411RE`.
- **Khối điều khiển trung tâm (`STM32F411RE Nucleo-64`):** Thu nhận khung lệnh qua ngoại vi `USART1` và gửi phản hồi giá trị góc cân bằng động `Active_Offset` ở tần số thấp ($1\text{ Hz}$) về thiết bị di động.

---

## 2. Sơ Đồ Đấu Nối Phần Cứng (`ESP32-C3` ↔ `STM32F411RE Nucleo-64`)

| Dây Số | Chân `ESP32-C3 Super Mini` | Chân `STM32F411RE` | Ký Hiệu Header Nucleo-64 | Đặc Tính Tín Hiệu |
| :---: | :--- | :--- | :---: | :--- |
| **1** | `GPIO21` (`U0TXD`) | `PA10` (`USART1_RX`) | `D2` (`CN9`) | Truyền khung lệnh nhị phân $14\text{ byte}$, tốc độ $115200\text{ bps}$, mức logic $3.3\text{ V}$ |
| **2** | `GPIO20` (`U0RXD`) | `PA9` (`USART1_TX`) | `D8` (`CN5`) | Nhận phản hồi `Active_Offset` từ `STM32`, tốc độ $115200\text{ bps}$, mức logic $3.3\text{ V}$ |
| **3** | `GND` | `GND` | `GND` (`CN6` / `CN5`) | Điểm tham chiếu điện áp chung của toàn hệ thống |
| **4** | `5V` (`VBUS`) | `5V` | `5V` (`CN6` / Mạch hạ áp) | Nguồn nuôi $5.0\text{ V}$ qua bộ ổn áp nội `ME6211` tạo $3.3\text{ V}$ cho `ESP32-C3` |

> **Quy tắc cách ly nguồn cấp:** Không cấp đồng thời nguồn ngoài vào chân `5V` của `ESP32-C3 Super Mini` khi đang kết nối cáp `USB Type-C` với máy tính. Khi gỡ lỗi qua cổng USB của `ESP32-C3`, ngắt kết nối dây nguồn `5V` và chỉ giữ lại 3 dây `GND`, `GPIO21`, `GPIO20`.

---

## 3. Đặc Tả Dịch Vụ BLE Và Tập Lệnh Giao Tiếp Trên iOS

### 3.1. Định danh dịch vụ Nordic UART Service (NUS)
- **Tên thiết bị quảng bá:** `BalancingBike-C3`
- **Service UUID:** `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
- **RX Characteristic UUID (Ghi lệnh từ iOS xuống ESP32):** `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` (`WRITE` / `WRITE_WITHOUT_RESPONSE`)
- **TX Characteristic UUID (Thông báo từ ESP32 lên iOS):** `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` (`NOTIFY` / `READ`)

### 3.2. Bảng ánh xạ nút bấm chế độ Control Pad (`!B<id><state><crc>`)

| Phím Trên Ứng Dụng iOS | Mã Gói Tin Nhấn / Nhả | Tác Vụ Điều Khiển Tương Ứng |
| :--- | :--- | :--- |
| **Phím số `1`** | `!B11` / `!B10` | Kích hoạt chế độ tự cân bằng (`Enable_Switch = 1`) |
| **Phím số `2`** | `!B21` / `!B20` | Ngắt khẩn cấp động cơ bánh đà và dừng bánh sau (`Enable_Switch = 0`, `Drive_Speed = 0`) |
| **Phím số `3`** | `!B31` / `!B30` | Giảm mức tốc độ đặt trước của bánh sau đi $10\%$ (Tối thiểu $10\%$) |
| **Phím số `4`** | `!B41` / `!B40` | Tăng mức tốc độ đặt trước của bánh sau thêm $10\%$ (Tối đa $100\%$) |
| **Mũi tên Lên (`▲`)** | `!B51` / `!B50` | Cấp lệnh chạy tiến bánh sau khi nhấn giữ; tự động đưa về $0\%$ khi nhả phím |
| **Mũi tên Xuống (`▼`)** | `!B61` / `!B60` | Cấp lệnh chạy lùi bánh sau khi nhấn giữ; tự động đưa về $0\%$ khi nhả phím |
| **Mũi tên Trái (`◄`)** | `!B71` / `!B70` | Bẻ lái cơ cấu Servo sang trái ($-25^\circ$) khi nhấn giữ; tự trả về $0^\circ$ khi nhả phím |
| **Mũi tên Phải (`►`)** | `!B81` / `!B80` | Bẻ lái cơ cấu Servo sang phải ($+25^\circ$) khi nhấn giữ; tự trả về $0^\circ$ khi nhả phím |

### 3.3. Tập lệnh văn bản tinh chỉnh thông số tại chỗ (Chế độ UART Terminal)

| Cú Pháp Lệnh | Phạm Vi Hợp Lệ | Giá Trị Mặc Định | Mô Tả Chức Năng |
| :--- | :---: | :---: | :--- |
| `EN:<0\|1>` | $0$ hoặc $1$ | $0$ | Bật hoặc tắt trạng thái hoạt động của bộ điều khiển cân bằng |
| `SPD:<val>` | $-100 \div +100$ | $0$ | Đặt trực tiếp phần trăm công suất động cơ truyền động bánh sau |
| `STR:<val>` | $-45 \div +45$ | $0$ | Đặt trực tiếp góc quay cơ cấu lái Servo MG995 (đơn vị: độ) |
| `KP:<val>` | $0.0 \div 200.0$ | $45.0$ | Tinh chỉnh hệ số khuếch đại tỉ lệ $K_p$ của vòng điều khiển trong |
| `KD:<val>` | $0.0 \div 50.0$ | $3.80$ | Tinh chỉnh hệ số khuếch đại vi sai $K_d$ của vòng điều khiển trong |
| `TRIM:<val>` | $0.0 \div 10.0$ | $0.50$ | Tinh chỉnh hệ số dịch chuyển điểm đặt $K_{\text{trim}}$ của vòng điều khiển ngoài |
| `OFS:<val>` | $-10.0 \div +10.0$ | $+1.30$ | Tinh chỉnh góc cân bằng tĩnh danh định $\theta_{\text{nominal}}$ (đơn vị: độ) |
| `STATUS` | — | — | Yêu cầu xuất toàn bộ thông số hiện hành lên màn hình thiết bị iOS |
| `STOP` | — | — | Ngắt ngay lập tức công suất các cơ cấu chấp hành |

---

## 4. Cấu Trúc Khung Truyền Nhị Phân `USART1` ($14\text{ Bytes}$)

Khung truyền từ `ESP32-C3` (`GPIO21`) sang `STM32F411RE` (`PA10`) được đóng gói theo chuẩn thứ tự byte thấp trước (`Little-Endian`):

| Vị Trí Byte | Tên Trường Dữ Liệu | Kiểu Dữ Liệu | Hệ Số Quy Đổi | Mô Tả Chi Tiết |
| :---: | :--- | :---: | :---: | :--- |
| `0` | `HEADER_1` | `uint8` | — | Byte đồng bộ đầu khung thứ nhất (`0xAA`) |
| `1` | `HEADER_2` | `uint8` | — | Byte đồng bộ đầu khung thứ hai (`0x55`) |
| `2` | `Enable_Switch` | `uint8` | $\times 1$ | Trạng thái kích hoạt cân bằng (`0` hoặc `1`) |
| `3` | `Drive_Speed` | `int8` | $\times 1$ | Lệnh tốc độ động cơ bánh sau ($-100\%$ đến $+100\%$) |
| `4` | `Steer_Angle` | `int8` | $\times 1$ | Lệnh góc lái cơ cấu Servo ($-45^\circ$ đến $+45^\circ$) |
| `5..6` | `Kp_Scaled` | `uint16` | $\times 10$ | Hệ số $K_p$ nhân $10$ (Ví dụ: $45.0 \rightarrow 450$) |
| `7..8` | `Kd_Scaled` | `uint16` | $\times 100$ | Hệ số $K_d$ nhân $100$ (Ví dụ: $3.80 \rightarrow 380$) |
| `9..10` | `Ktrim_Scaled` | `uint16` | $\times 100$ | Hệ số $K_{\text{trim}}$ nhân $100$ (Ví dụ: $0.50 \rightarrow 50$) |
| `11..12` | `Offset_Scaled` | `int16` | $\times 100$ | Góc cân bằng danh định nhân $100$ (Ví dụ: $+1.30^\circ \rightarrow 130$) |
| `13` | `Checksum` | `uint8` | — | Phép toán `XOR` liên tiếp từ `Byte 2` đến `Byte 12` |

Phương trình kiểm tra tính toàn vẹn của khung truyền tại bộ thu `STM32F411RE`:

$$\text{Checksum} = \bigoplus_{k=2}^{12} \text{Frame}[k]$$
