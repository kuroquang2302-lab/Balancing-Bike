# Technical Specifications & Empirical Parameters

## 1. Mechanical Properties (RemRC 5887157 + Pontifex42 5995416)
- **Chassis:** `6 mm` bolted PETG truss frame (`M3`/`M2` fasteners with captive T-slot nuts) and hollow TPU front/rear tires.
- **Bearings & Transmission:** `4x 685ZZ` (`5 x 11 x 5 mm`) bearings (`2` front wheel, `2` rear wheel), `GT2 280 mm x 6 mm` closed-loop timing belt driving an `18T` motor pulley to an `80T` rear wheel gear (`4.44:1` ratio).
- **Reaction Flywheel:** `126.4 mm` outer diameter PETG hub with `M4` perimeter rim ballast bolts and nuts, coupled to the `9T` helical Nidec 24H shaft via a printed `DrilledAxis_2x` spline insert.

## 2. MPU-6050 & Control Loop Parameters (`BalancingBike.slx`)
- **I2C Address & Rate:** `0x68`, sampled every `T_s = 5 ms` (`200 Hz`).
- **Attitude Filter (`MPU6050_filter`):** Complementary filter (`0.99 * Gyro + 0.01 * Accel`), discrete gyro LPF `0.4 / (1 - 0.6 z^-1)`.
- **Nominal Balance Offset (`Const_Offset`):** `-1.005°` (real hardware settles near `-0.9° ± 0.3°` after trim).
- **Tuned Gains:** `Kp = 46.0`, `Kd = 2.0`, `Kw = 0.036`.
- **Outer Trim Loop:** `Slow_RPM_LPF` (`0.0025 / (1 - 0.9975 z^-1)`), `Trim_Deadzone` (`±30 RPM`), `Trim_Gain` (`1.5e-7`), `Trim_Sat` (`±1.5°`), `Not_Sat_Check` anti-windup (`< 85%`).
- **Safety Thresholds (`Arm_Hysteresis`):** Arms inside `±2.0°`, disarms past `±12.0°`.

## 3. Nidec 24H BLDC Characteristics @ 12V Supply
- **Control Signal:** `20 kHz` active-low PWM; motor starts rotating around `94.5%` duty cycle (`100%` = zero throttle, `0%` = maximum throttle).
- **Integrated Magnetic Encoder:** `100` pulses/revolution; `4x` quadrature decoding on `TIM3`/`TIM2` gives `400 PPR` (`1` count per `5 ms` equals `30 RPM`).
- **Command vs. No-Load RPM at 12V:**
  - `1% -> 111 RPM`
  - `10% -> 434 RPM`
  - `30% -> 989 RPM`
  - `50% -> 1560 RPM`
  - `70% -> 2180 RPM`
  - `100% -> 3136 RPM` (no load), approx. `2400..2500 RPM` loaded by reaction wheel.
- **Steady-State Velocity Gain:** `Kv ≈ 31.36 RPM/%` (`1 / Kv ≈ 0.0319 %/RPM`).
- **Current Draw:** `10 mA .. 80 mA` idle/steady; `0.3 A .. 0.5 A` peak during direction reversals at `30%` command.

## 4. Measured Hardware Performance
- **Continuous Stationary Balancing:** Over `5.5 min` (`335 s` logged in SDI, `Kp = 46.0`, `Kd = 2.0`, `Kw = 0.036`).
- **Steady-State Accuracy:** Roll angle stays within `±0.3°` around `-0.9°`, flywheel speed damped down to `+20 .. +115 RPM` (`20..25 RPM` ripple).
- **Disturbance Recovery:** Recovers from hand pushes in `~0.9 s`, and survived a `-17°` spike during extended logged runs without falling.
- **Untethered Driving:** Sustained `5 min` continuous forward, reverse, and steering cornering via the iPhone Bluefy Web BLE controller without tipping.
