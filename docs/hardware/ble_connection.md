# Wireless BLE 5.0 Protocol & ESP32-C3 Co-Processor

## 1. Dual-MCU Partitioning Rationale

The primary `STM32F411RE` runs the deterministic `200 Hz` (`5 ms`) balance loop, hardware quadrature decoding (`TIM2`, `TIM3`), and `20 kHz` motor PWM. An auxiliary `ESP32-C3 Super Mini` handles the Bluetooth Low Energy (BLE 5.0) stack and text command parsing so radio events never delay the real-time balance controller.

- **Web BLE Client (`docs/index.html`):** Two-handed landscape controller at `https://kuroquang2302-lab.github.io/Balancing-Bike/` (tested on Bluefy for iOS and compatible with Chrome/Edge) or any Nordic UART Service (NUS) app such as Adafruit Bluefruit LE Connect or LightBlue.
- **BLE Bridge (`ESP32-C3 Super Mini`):** Parses Web BLE / NUS commands, suppresses redundant notify replies during continuous joystick drags (`SPD:`, `STR:`, Pad `!B5..B8`), dispatches BLE notifications from the main `loop()` task (`MTU = 185`), and streams fixed `14-byte` XOR-checksummed frames to the STM32 at `50 Hz` (`20 ms`).
- **Real-Time Core (`STM32F411RE Nucleo-64`):** Reads the `14-byte` UART frames continuously via `DMA2_Stream2` circular buffer with a `500 ms` (`100`-step) communications watchdog, and sends live telemetry back to the ESP32-C3 every `500 ms` (`2 Hz`) plus immediately upon arming (`A:<roll> R:<rpm> O:<ofs> E:<0|1>`).

---

## 2. Hardware Interconnect (`ESP32-C3` to `STM32F411RE`)

| Wire # | ESP32-C3 Pin | STM32F411RE Pin | Nucleo-64 Header | Signal Description |
| :---: | :--- | :--- | :---: | :--- |
| **1** | `GPIO21` (`U0TXD`) | `PA10` (`USART1_RX`) | `D2` (`CN9`) | Sends 14-byte command frames (`115200 bps`, 8N1, 3.3V) |
| **2** | `GPIO20` (`U0RXD`) | `PA9` (`USART1_TX`) | `D8` (`CN5`) | Receives live STM32 telemetry lines (`115200 bps`, 8N1, 3.3V) |
| **3** | `GND` | `GND` | `GND` (`CN6`/`CN5`) | Common system ground |
| **4** | `5V` (`VBUS`) | `5V` | `5V` (`CN6` / Buck) | 5.0V rail feeding the onboard 3.3V regulator |

> [!WARNING]
> Never feed `5V` into `VBUS` while a USB cable is plugged into the ESP32-C3 for flashing or serial monitoring. Disconnect the external `5V` wire during USB flashing and keep `GND`, `GPIO21`, and `GPIO20` connected.

---

## 3. BLE Service & Command Protocol

### 3.1. Nordic UART Service (NUS) Identifiers
- **Device Name:** `BalancingBike-C3`
- **Service UUID:** `6e400001-b5a3-f393-e0a9-e50e24dcca9e`
- **RX Characteristic (Phone to ESP32):** `6e400002-b5a3-f393-e0a9-e50e24dcca9e` (`WRITE` / `WRITE_NR`)
- **TX Characteristic (ESP32 to Phone):** `6e400003-b5a3-f393-e0a9-e50e24dcca9e` (`NOTIFY` / `READ`)

### 3.2. Control Pad Mapping (`!B<id><state>`)

| Control Action | Raw Packet (Press / Release) | Behavior |
| :--- | :--- | :--- |
| **Enable (`1`)** | `!B11` / `!B10` | Arms balance control (`Enable_Switch = 1`) |
| **Disarm / E-Stop (`2`)** | `!B21` / `!B20` | Disarms controller (`Enable_Switch = 0`, `Drive_Speed = 0`, `Steer_Angle = 0`) |
| **Gear Down (`3`)** | `!B31` / `!B30` | Decrements drive speed gear by `10%` (min `10%`) |
| **Gear Up (`4`)** | `!B41` / `!B40` | Increments drive speed gear by `10%` (max `100%`, default `30%`) |
| **Forward (`UP`)** | `!B51` / `!B50` | Drives forward at `+speed_gear`; resets to `0%` on release |
| **Reverse (`DOWN`)** | `!B61` / `!B60` | Drives reverse at `-speed_gear`; resets to `0%` on release |
| **Steer Left (`LEFT`)** | `!B71` / `!B70` | Steers front servo left (`-25°`); centers to `0°` on release |
| **Steer Right (`RIGHT`)** | `!B81` / `!B80` | Steers front servo right (`+25°`); centers to `0°` on release |

### 3.3. Text Commands & Telemetry Format

| Command Syntax | Valid Range | Default Value | Description |
| :--- | :---: | :---: | :--- |
| `EN:<0\|1>` | `0` or `1` | `0` | Enable (`1`) or disable (`0`) balance control |
| `SPD:<val>` | `-100 .. +100` | `0` | Direct rear wheel throttle percentage (`45 ms` paced from joystick) |
| `STR:<val>` | `-45 .. +45` | `0` | Direct steering angle in degrees (Web UI sends `-25 .. +25`) |
| `KP:<val>` | `0.0 .. 200.0` | `46.0` | Inner-loop proportional gain `Kp` |
| `KD:<val>` | `0.0 .. 50.0` | `2.00` | Inner-loop derivative gain `Kd` |
| `TRIM:<val>` | `0.0 .. 10.0` | `0.036` | Parsed by ESP32; terminated in `BalancingBike.slx` (`Const_Kw = 0.036` fixed) |
| `OFS:<val>` | `-10.0 .. +10.0` | `-1.01` | Parsed by ESP32; terminated in `BalancingBike.slx` (`Const_Offset = -1.005` fixed) |
| `STATUS` / `PING` | `n/a` | `n/a` | Requests immediate ESP32 status line (`OK \| EN:... KP:... KD:...`) |
| `STOP` / `ESTOP` | `n/a` | `n/a` | Clears enable, throttle, and steering immediately |
| `RESET` / `REBOOT` | `n/a` | `n/a` | Sends zero frame, flushes UART, and restarts ESP32-C3 |

In `BalancingBike.slx`, `USART1_BLE` drives `Enable_Switch`, `Drive_Speed`, `Steer_Angle`, `Kp`, and `Kd` live. Its `K_trim` and `Nominal_Offset` outputs are terminated so `Const_Kw` (`0.036`) and `Const_Offset` (`-1.005°`) remain fixed in flash. STM32 live telemetry is forwarded to the phone as `[STM32] A:<roll> R:<rpm> O:<active_offset> E:<0|1>`, and the Web UI updates the `Ofs` readout exclusively from this live STM32 stream.

---

## 4. Binary Inter-MCU Packet (`14 Bytes`)

Frames transmit from `ESP32-C3` (`GPIO21`) to `STM32F411RE` (`PA10`) in little-endian byte order at `50 Hz` (`20 ms`) plus immediately on incoming BLE writes:

| Byte Index | Field Name | Type | Scaling Factor | Description |
| :---: | :--- | :---: | :---: | :--- |
| `0` | `HEADER_1` | `uint8` | `n/a` | Sync byte 1: `0xAA` |
| `1` | `HEADER_2` | `uint8` | `n/a` | Sync byte 2: `0x55` |
| `2` | `Enable_Switch` | `uint8` | `x 1` | Controller enable flag (`0` or `1`) |
| `3` | `Drive_Speed` | `int8` | `x 1` | Rear motor throttle (`-100 .. +100%`) |
| `4` | `Steer_Angle` | `int8` | `x 1` | Steering angle (`-45° .. +45°`) |
| `5..6` | `Kp_Scaled` | `uint16` | `x 10` | `Kp * 10` (e.g. `46.0 -> 460`) |
| `7..8` | `Kd_Scaled` | `uint16` | `x 100` | `Kd * 100` (e.g. `2.00 -> 200`) |
| `9..10` | `Ktrim_Scaled` | `uint16` | `x 100` | `K_trim * 100` (e.g. `0.036 -> 4`) |
| `11..12` | `Offset_Scaled` | `int16` | `x 100` | Nominal offset * 100 (e.g. `-1.01° -> -101`) |
| `13` | `Checksum` | `uint8` | `n/a` | Bitwise XOR checksum over bytes `2` through `12` |

$$\text{Checksum} = \bigoplus_{k=2}^{12} \text{Frame}[k]$$

---

## 5. Buzzer Feedback & Power States (`ESP32-C3`)

### 5.1. Non-Blocking Buzzer Patterns (`PIN_BUZZER = GPIO3`, `2.7 kHz`)
- **Single Short Beep (`80 ms`):** BLE client connect, `STATUS`, `KP:`, `KD:`, `TRIM:`, or `EN:1`.
- **Double Beep (`70 ms ON / 70 ms OFF / 70 ms ON`):** First STM32 telemetry packet received after arming or `OFS:` command.
- **Long Single Beep (`350 ms`):** BLE disconnect, `EN:0`, `STOP`, or hibernation entry.

### 5.2. Radio Power Management
1. **Active Connected Mode:** CPU at `80 MHz`, BLE TX power `0 dBm` (`ESP_PWR_LVL_N0`), status LED solid ON (`LOW`), `50 Hz` UART frames active.
2. **Eco Standby (`> 60 s` disconnected):** Advertising interval relaxed to `800 ms .. 1000 ms`, TX power `-6 dBm`, short LED blink every `2 s`, periodic UART frames paused.
3. **Duty-Cycled Hibernation (`> 180 s` disconnected):** Cycles radio OFF for `4.0 s` and advertising ON for `1.0 s` at `-3 dBm` until a client reconnects.
