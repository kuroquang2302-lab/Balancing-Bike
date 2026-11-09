# Wireless BLE 5.0 Specification & ESP32-C3 Co-Processor Protocol

## 1. Dual-MCU Partitioning Rationale

To maintain deterministic, hard real-time execution for the $200\text{ Hz}$ ($5\text{ ms}$) balance control loop on the primary `STM32F411RE` microcontroller, all Bluetooth Low Energy (BLE 5.0) protocol stack processing and string parsing are delegated to an auxiliary `ESP32-C3 Super Mini` co-processor.

- **Client Controller (iOS):** Connects via `Adafruit Bluefruit LE Connect` or `LightBlue` using the Nordic UART Service (NUS).
- **Wireless Co-Processor (`ESP32-C3 Super Mini`):** Receives raw character streams, decodes button events and text commands, formats them into fixed 14-byte binary packets with an XOR checksum, and transmits them at $50\text{ Hz}$ ($20\text{ ms}$) to the STM32 via `USART1`.
- **Primary Control Core (`STM32F411RE Nucleo-64`):** Parses the 14-byte packets via UART DMA/interrupts and periodically replies with the active dynamic balance offset at $1\text{ Hz}$.

---

## 2. Hardware Interconnect (ESP32-C3 <-> STM32F411RE Nucleo-64)

| Wire # | ESP32-C3 Super Mini Pin | STM32F411RE Pin | Nucleo-64 Header | Signal Description |
| :---: | :--- | :--- | :---: | :--- |
| **1** | `GPIO21` (`U0TXD`) | `PA10` (`USART1_RX`) | `D2` (`CN9`) | Transmits 14-byte command frames (115200 bps, 8N1, 3.3V) |
| **2** | `GPIO20` (`U0RXD`) | `PA9` (`USART1_TX`) | `D8` (`CN5`) | Receives active offset telemetry from STM32 (115200 bps) |
| **3** | `GND` | `GND` | `GND` (`CN6`/`CN5`) | Common system ground reference plane |
| **4** | `5V` (`VBUS`) | `5V` | `5V` (`CN6` / Buck) | 5.0V supply input feeding onboard ME6211 3.3V LDO |

> [!WARNING]
> **Power Isolation Rule:** Never apply 5V power to the `VBUS` pin while a USB Type-C cable is simultaneously plugged into a computer. During USB programming or debugging on the ESP32-C3, disconnect the external 5V wire and keep only `GND`, `GPIO21`, and `GPIO20` connected to the STM32.

---

## 3. BLE Service Specification & iOS Command Protocol

### 3.1. Nordic UART Service (NUS) Identifiers
- **Advertised Device Name:** `BalancingBike-C3`
- **Service UUID:** `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
- **RX Characteristic UUID (iOS -> ESP32):** `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` (`WRITE` / `WRITE_WITHOUT_RESPONSE`)
- **TX Characteristic UUID (ESP32 -> iOS):** `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` (`NOTIFY` / `READ`)

### 3.2. Bluefruit Control Pad Mapping (`!B<id><state><crc>`)

| iOS Control Pad Button | Raw Packet (Press / Release) | Control Function |
| :--- | :--- | :--- |
| **Button `1`** | `!B11` / `!B10` | Arm / Enable balance controller (`Enable_Switch = 1`) |
| **Button `2`** | `!B21` / `!B20` | Emergency Disarm: stop flywheel and rear motor (`Enable_Switch = 0`, `Drive_Speed = 0`) |
| **Button `3`** | `!B31` / `!B30` | Decrement rear drive speed gear by $10\%$ (Minimum: $10\%$) |
| **Button `4`** | `!B41` / `!B40` | Increment rear drive speed gear by $10\%$ (Maximum: $100\%$) |
| **Up Arrow (`UP`)** | `!B51` / `!B50` | Drive forward while held; returns to $0\%$ upon release |
| **Down Arrow (`DOWN`)** | `!B61` / `!B60` | Drive reverse while held; returns to $0\%$ upon release |
| **Left Arrow (`LEFT`)** | `!B71` / `!B70` | Steer servo left ($-25^\circ$) while held; returns to $0^\circ$ upon release |
| **Right Arrow (`RIGHT`)** | `!B81` / `!B80` | Steer servo right ($+25^\circ$) while held; returns to $0^\circ$ upon release |

### 3.3. Live Terminal Tuning Commands (UART Terminal Mode)

| Command Syntax | Valid Range | Default Value | Description |
| :--- | :---: | :---: | :--- |
| `EN:<0\|1>` | $0$ or $1$ | $0$ | Enable ($1$) or disable ($0$) balance control |
| `SPD:<val>` | $-100 \dots +100$ | $0$ | Direct rear motor drive throttle percentage |
| `STR:<val>` | $-45 \dots +45$ | $0$ | Direct steering servo angle in degrees |
| `KP:<val>` | $0.0 \dots 200.0$ | $45.0$ | Proportional gain $K_p$ for inner balance loop |
| `KD:<val>` | $0.0 \dots 50.0$ | $3.80$ | Derivative gain $K_d$ for inner balance loop |
| `TRIM:<val>` | $0.0 \dots 10.0$ | $0.50$ | Trim gain $K_{\text{trim}}$ for outer momentum dumping loop |
| `OFS:<val>` | $-10.0 \dots +10.0$ | $+1.30$ | Nominal static balance offset angle in degrees |
| `STATUS` / `PING` | -- | -- | Print active runtime state and firmware version to terminal |
| `STOP` | -- | -- | Instantaneous disarm and actuator shutdown |
| `RESET` / `REBOOT` | -- | -- | Send zero frame, flush UART, emit beep, and software reboot ESP32 |

---

## 4. Binary Inter-MCU Packet Structure ($14\text{ Bytes}$)

Frames are transmitted from `ESP32-C3` (`GPIO21`) to `STM32F411RE` (`PA10`) in little-endian format at $50\text{ Hz}$ ($20\text{ ms}$ interval):

| Byte Index | Field Name | Data Type | Scaling Factor | Description |
| :---: | :--- | :---: | :---: | :--- |
| `0` | `HEADER_1` | `uint8` | — | Sync byte 1: `0xAA` |
| `1` | `HEADER_2` | `uint8` | — | Sync byte 2: `0x55` |
| `2` | `Enable_Switch` | `uint8` | $\times 1$ | Controller arm state (`0` or `1`) |
| `3` | `Drive_Speed` | `int8` | $\times 1$ | Rear motor throttle ($-100 \dots +100\%$) |
| `4` | `Steer_Angle` | `int8` | $\times 1$ | Steering angle ($-45^\circ \dots +45^\circ$) |
| `5..6` | `Kp_Scaled` | `uint16` | $\times 10$ | $K_p \times 10$ (e.g. $45.0 \to 450$) |
| `7..8` | `Kd_Scaled` | `uint16` | $\times 100$ | $K_d \times 100$ (e.g. $3.80 \to 380$) |
| `9..10` | `Ktrim_Scaled` | `uint16` | $\times 100$ | $K_{\text{trim}} \times 100$ (e.g. $0.50 \to 50$) |
| `11..12` | `Offset_Scaled` | `int16` | $\times 100$ | Nominal offset angle $\times 100$ (e.g. $+1.30^\circ \to 130$) |
| `13` | `Checksum` | `uint8` | — | Longitudinal XOR checksum over bytes 2 through 12 |

Checksum verification formula at the STM32 receiver:

$$\text{Checksum} = \bigoplus_{k=2}^{12} \text{Frame}[k]$$

---

## 5. Acoustic Buzzer Feedback & Thermal Management

### 5.1. Non-Blocking Buzzer Patterns (2.7 kHz Resonance)
- **Single Short Beep (80 ms):** Triggered on BLE client connection, parameter tuning command acknowledge, or enable command.
- **Double Beep (70 ms ON / 70 ms OFF / 70 ms ON):** Triggered upon receiving the first dynamic offset confirmation from STM32 or nominal offset adjustment.
- **Triple Beep (70 ms cadence):** Triggered on BLE disconnect, emergency stop, or hibernation entry.

### 5.2. 3-Tier Power & Thermal Management
1. **Active Connected Mode:** CPU clock at 80 MHz, BLE TX power at 0 dBm (`ESP_PWR_LVL_N0`), continuous 50 Hz frame transmission.
2. **Eco Standby Mode (Triggered after 60s idle disconnect):** Advertising interval relaxed to 800 ms – 1000 ms, RF power reduced to -6 dBm, single 60 ms LED flash every 2.0 s, periodic frames halted.
3. **Duty-Cycled Hibernation Mode (Triggered after 180s idle disconnect):** Deep radio duty-cycling with 4.0s radio OFF followed by 1.0s radio ON at -3 dBm. Reduces thermal dissipation and power draw significantly while allowing spontaneous reconnection without physical power cycling.
