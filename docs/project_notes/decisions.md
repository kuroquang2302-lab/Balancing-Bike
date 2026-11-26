# Architecture Decision Records (ADRs)

### ADR-001: Native Pure-Block Simulink Architecture (`BalancingBike.slx`)
- **Context:** Keeping control equations, Kalman math, and motor logic inside custom MATLAB Function scripts hid intermediate signals during `Monitor & Tune` and slowed down gain iteration on hardware.
- **Decision:** Replace all custom `.m` algorithm scripts with native Simulink math, filter, relay, deadzone, saturation, and logic blocks inside `BalancingBike.slx`, keeping only low-level C++ register System Objects in `src/hal`.
- **Consequences:** Full signal visibility in Simulation Data Inspector, direct workspace tuning, and clean Embedded Coder C generation at `200 Hz`.

### ADR-002: Cascaded Two-Loop Balance Control with Back-EMF Compensation
- **Context:** Pure PD control on a velocity-mode Nidec 24H motor stalls as soon as flywheel speed builds up because motor back-EMF opposes coasting spin, and any center-of-mass mismatch drives RPM to saturation (`~3000 RPM`).
- **Decision:** Pair an inner loop `u = Kw * Motor_RPM - Kp * e - Kd * dtheta` (`Kp = 46.0`, `Kd = 2.0`, `Kw = 0.036`) with a slow outer integral trim loop (`Slow_RPM_LPF`, `±30 RPM` deadzone, `Trim_Gain = 1.5e-7`, `±1.5°` clamp) and `< 85%` saturation anti-windup.
- **Consequences:** Eliminates monotonic flywheel runaway while automatically adapting the balance angle across slightly uneven floors and battery placements.

### ADR-003: Complementary Filter (`99% / 1%`) Over Heavy Kalman Tuning
- **Context:** The earlier adaptive Kalman script suffered from handshake lag and tuning sensitivity to motor vibration on the 3D-printed frame.
- **Decision:** Implement `MPU6050_filter` with `atan2(ay, az)`, an initial-step accelerometer seed (`Init_Seed_Switch`), a `99%` gyro / `1%` accel complementary blend, and a discrete gyro rate low-pass filter (`0.4 / (1 - 0.6 z^-1)`).
- **Consequences:** Zero startup deadlock, low phase lag, and reliable tilt tracking under flywheel vibration.

### ADR-004: One-Sample PWM Blanking & Active-Low Deadband Driver
- **Context:** Reversing the Nidec 24H `DIR` pin while PWM is active spikes supply current and stresses the internal driver H-bridge, while commands above `94.5%` duty sit inside the motor deadband.
- **Decision:** Scale non-zero commands onto `94.5 - 0.945 * |u|` at `20 kHz` (`100.0%` stop), and force `100.0%` PWM for one `5 ms` step (`Prev_Dir != Dir_Neg`) on every direction change.
- **Consequences:** Smooth zero-crossing without power brownouts or mechanical chatter.

### ADR-005: Independent Hardware Timers & Overflow-Safe Quadrature Reading
- **Context:** Third-party Simulink encoder blocks collided with `TIM1` PWM (`PA8`) and `PA4`, and raw 16-bit counters spike on `0 <-> 65535` rollovers.
- **Decision:** Assign dedicated timers (`TIM1` motor PWMs, `TIM3` flywheel encoder, `TIM2` rear encoder, `TIM4` steering servo) via custom C++ System Objects (`EncoderBlock_TIM3`, `EncoderBlock_TIM2`) and apply modulo `65536` signed unwrapping inside Simulink.
- **Consequences:** Deterministic encoder speed feedback with zero interrupt contention.

### ADR-006: Hysteresis Arming (`2° / 12°`) & Persistent Trim Across Brief Trips
- **Context:** Starting the controller while lying flat spins the wheel dangerously, and clearing `Trim_Memory` on every transient over-tilt erases the learned floor offset when picking the bike back up.
- **Decision:** Use `Arm_Hysteresis` (`arm <= 2.0°`, `disarm >= 12.0°`) and wire `Trim_Reset_Sw` to `User_Enable` so `Trim_Memory` resets only when the user explicitly turns balancing OFF.
- **Consequences:** Safe startup/fall cutoff plus instant re-balancing when stood back upright.

### ADR-007: RemRC `5887157 + 5995416` Bolted Chassis Migration
- **Context:** Earlier `3 mm` glued PETG frames deflected under flywheel torque, shifting the roll zero point dynamically between tests.
- **Decision:** Migrate to RemRC's `6 mm` bolted PETG truss frame paired with Pontifex42's dual-`685ZZ` bearing front wheel, `GT2-280mm` rear belt drive, and side-mounted `20 g` ballast balancing.
- **Consequences:** Eliminated structural neck flex and enabled repeatable >5-minute balancing runs.

### ADR-008: Open-Loop Rear Wheel Drive & Zero-Delay BLE Phase-1 Control
- **Context:** Closing a speed PI loop on `ENCODER_TIM2` before verifying rear-wheel polarity risked positive-feedback acceleration or abrupt longitudinal braking torques during roll balancing. Furthermore, feeding `Enable_Out` and `Active_Offset` back into `USART1_BLE` in the same step created a Simulink algebraic loop.
- **Decision:** Drive the rear Nidec 24H open-loop in `DRIVE and MOTION` (leaving `RPM_Back` terminated for future closed-loop work), insert 1-sample `Unit Delay` blocks (`Delay_BLE_EnOut`, `Delay_BLE_Ofs`) only on the telemetry feedback inputs into `USART1_BLE`, and offload BLE to an `ESP32-C3` + custom Web BLE UI (`docs/index.html`) for iPhone Bluefy.
- **Consequences:** Zero algebraic delay on control commands, smooth cornering and forward/reverse riding, and wireless iOS tuning without stalling the `200 Hz` STM32 loop.
