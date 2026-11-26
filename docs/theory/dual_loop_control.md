# Theoretical Modeling & Cascaded Two-Loop Control Architecture

## 1. Reaction Wheel Inverted Pendulum Dynamics

The lateral roll motion of the bicycle about its tire contact line follows the nonlinear inverted pendulum model:

$$I_{\text{bike}} \ddot{\theta} = m g h \sin(\theta - \theta_{\text{CM}}) - \tau_{\text{wheel}}$$

where $\theta$ is the roll angle relative to vertical, $\theta_{\text{CM}}$ is the static center-of-mass offset angle, $m$ is total mass, $h$ is center-of-mass height, $I_{\text{bike}}$ is frame roll inertia, and $\tau_{\text{wheel}}$ is reaction torque from the flywheel. Reaction torque is proportional to flywheel angular acceleration:

$$\tau_{\text{wheel}} = J_{\text{wheel}} \dot{\omega}_{\text{wheel}}$$

---

## 2. Velocity-Mode Actuator Dynamics (Nidec 24H)

The internal driver ASIC on the Nidec 24H regulates motor speed proportionally to the applied PWM command rather than motor current:

$$\omega_{\text{wheel}} \approx K_v u$$

where $K_v \approx 31.36\text{ RPM}/\%$ at `12 V` ($1/K_v \approx 0.0319\text{ \%/RPM}$). Because back-EMF naturally resists spin and drags an uncommanded rotor back toward zero, a pure PD controller loses restoring torque as flywheel speed builds up. Adding feedforward speed compensation $+K_w \omega_{\text{wheel}}$ cancels internal back-EMF damping so the PD error term produces true angular acceleration ($\dot{\omega}_{\text{wheel}}$). Setting $K_w = 0.036 > 1/K_v$ cancels back-EMF and leaves a slight restoring drag (`+0.0041 * RPM`) that pulls wheel speed smoothly toward zero.

---

## 3. Signal Conditioning (`200 Hz`, `T_s = 5 ms`)

- **Attitude Estimation (`MPU6050_filter`):** Reads `14` raw bytes over `I2C` (`0x68`), reconstructs signed 16-bit accelerometer and gyroscope values, computes tilt via `atan2(ay, az)`, and runs a complementary filter (`99%` integrated gyro + `1%` accelerometer). Raw angular rate is smoothed by a discrete low-pass filter (`0.4 / (1 - 0.6 z^-1)`). An `Init_Seed_Switch` initializes the integrator from the accelerometer during the first step, and `Valid_Angle_Switch` outputs `999.0` if sensor magnitude collapses (`Fault_Angle`).
- **Flywheel Speed (`ENCODER_TIM3`):** Reads the 16-bit hardware quadrature counter on `PA6`/`PA7`, applies modulo wrap-around correction (`+32768 -> mod 65536 -> -32768`) to prevent overflow spikes across `0` / `65535`, scales by `30` (`60 / (400 * 0.005 s)`) to yield RPM, and filters through `0.25 / (1 - 0.75 z^-1)`.

---

## 4. Cascaded Two-Loop Controller (`BalancingBike.slx`)

### 4.1. Inner Balance Loop (`PD + Kw`)
At each `5 ms` step, the inner controller evaluates:

$$e = \theta - \theta_{\text{offset}}$$

$$u = K_w \omega_{\text{wheel}} - K_p e - K_d \dot{\theta}$$

- **Tuned Gains:** $K_p = 46.0$, $K_d = 2.0$, $K_w = 0.036$.
- **Output Saturation:** `Sat_PWM` clamps `u` within `[-95.0, +95.0]` before entering `Driver`.

### 4.2. Outer Integral Trim Loop (`CoG Compensation`)
Any lateral center-of-mass offset or slight floor slope causes the flywheel to speed up continuously until reaching saturation (`~3000 RPM`). The outer loop prevents saturation by trimming the balance setpoint:

1. **Slow Trend Extraction (`Slow_RPM_LPF`):** Passes `Motor_RPM` through `0.0025 / (1 - 0.9975 z^-1)` (~`2 s` time constant) to reject short push disturbances.
2. **Deadzone (`Trim_Deadzone`):** Ignores jitter inside `±30 RPM`.
3. **Anti-Windup (`Not_Sat_Check` & `Trim_Gate`):** Freezes integration whenever `|Speed_Cmd| >= 85%` or `Enable_Out = 0` so impact recoveries never corrupt the learned balance point.
4. **Bounded Integrator (`Trim_Gain`, `Trim_Sat`, `Trim_Reset_Sw`):** Integrates at `1.5e-7` per step, clamped to `±1.5°`, and adds it to `Nominal_Offset` (`-1.005°`). `Trim_Memory` resets to `0` only when the user turns `Enable_Switch` OFF; if the bike trips past `12°` while still enabled, the learned offset stays frozen for immediate upright recovery.

### 4.3. Safety Hysteresis & Motor Drivers
- **Arming Relay (`Arm_Hysteresis`):** Arms balancing only when `|e| <= 2.0°`, holds through disturbances up to `12.0°`, and cuts PWM to `0` (`Safety_Switch`) beyond `12.0°`.
- **Flywheel Driver (`Driver`):** Maps `|Speed_Cmd|` (`>= 1.0`) to `94.5 - 0.945 * |u|` at `20 kHz` active-low PWM (`100.0%` stop) with a 1-sample (`5 ms`) zero-PWM hold whenever `Prev_Dir != Dir_Neg` to protect the motor H-bridge.
- **Rear Drive & Steering (`DRIVE and MOTION`):** Drives the rear Nidec 24H wheel open-loop using the same `94.5%` deadband map and `5 ms` reversal blanking, and maps `Steer_Angle` (`-45°..+45°`) onto `50 Hz` servo duty `7.5 + Steer_Angle * (2.5 / 45.0)`, clamped to `[5.0%, 10.0%]`.
