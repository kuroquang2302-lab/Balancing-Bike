# STM32F411RE Nucleo-64 Pin Allocation & Power Distribution

## 1. Hardware Signal Mapping

| Subsystem | MCU Pin | Peripheral | Signal Characteristics |
| :--- | :---: | :--- | :--- |
| **Flywheel Motor (Nidec 24H)** | `PA8` | `TIM1_CH1` | 20 kHz PWM Active-Low logic, 94.5% deadband starting threshold |
| | `PC7` | `GPIO Output` | Direction control (`DIR`): `0` = CW, `1` = CCW |
| | `PC8` | `GPIO Output` | Braking control (`BRAKE`): `1` = Release, `0` = Engage |
| | `PA6` | `TIM3_CH1` | Hardware 4x Quadrature Encoder Channel A |
| | `PA7` | `TIM3_CH2` | Hardware 4x Quadrature Encoder Channel B |
| **Rear Drive Motor (Nidec 24H)** | `PA11` | `TIM1_CH4` | 20 kHz PWM Active-Low logic |
| | `PC6` | `GPIO Output` | Drive direction control (`DIR`) |
| | `PC9` | `GPIO Output` | Drive braking control (`BRAKE`) |
| | `PA15` | `TIM2_CH1` | Hardware 4x Quadrature Encoder Channel A |
| | `PB3` | `TIM2_CH2` | Hardware 4x Quadrature Encoder Channel B |
| **Steering Servo (MG995)** | `PB8` | `TIM4_CH3` | 50 Hz PWM, 1.0 ms to 2.0 ms pulse width ($0^\circ \dots 180^\circ$) |
| | `PA4` | `ADC1_IN4` | 0 V to 3.3 V analog input for steering potentiometer feedback |
| **Inertial Sensor (MPU-6050)** | `PB6` | `I2C1_SCL` | I2C Fast-Mode clock @ 400 kHz |
| | `PB7` | `I2C1_SDA` | I2C Fast-Mode data line @ 400 kHz (I2C address: `0x68`) |
| **BLE Co-Processor (ESP32-C3)** | `PA9` | `USART1_TX` | 115200 bps, 8N1, 3.3V logic (transmits active offset telemetry) |
| | `PA10` | `USART1_RX` | 115200 bps, 8N1, 3.3V logic (receives 14-byte command packets) |

---

## 2. Power Distribution Architecture

The electrical system utilizes a three-tier power architecture to ensure noise immunity and voltage stability:

- **12V High-Current Drive Rail:**
  - Powered by a 3S LiPo battery (nominal 11.1 V, fully charged 12.6 V).
  - Supplies the high-side power stages of both Nidec 24H BLDC motors directly.
  - High-frequency motor switching noise is decoupled via low-ESR electrolytic bypass capacitors.
- **5V Logic & Actuator Rail:**
  - Stepped down from 12V via an onboard 5V / 3A DC-DC switching buck converter.
  - Powers the TowerPro MG995 steering servo and supplies the `5V` input pin of the Nucleo-64 and ESP32-C3 Super Mini.
- **3.3V Sensitive Logic Rail:**
  - Regulated via the onboard low-dropout (LDO) linear regulator on the Nucleo-64 board.
  - Supplies the MPU-6050 IMU sensor and the $10\text{ k}\Omega$ pull-up resistors for the open-drain Nidec encoder outputs.
  - A common ground reference plane (`GND`) is bonded across all boards and power rails.
