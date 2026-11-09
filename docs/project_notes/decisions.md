# Architecture Decision Records (ADRs)

### ADR-001: Cascaded Two-Loop Balance Control Architecture
- **Context:** A single-loop controller regulating only roll angle cannot prevent the reaction wheel from monotonically accelerating to voltage saturation when a steady-state center-of-mass offset exists.
- **Decision:** Split the control topology into two distinct loops: a deterministic high-rate inner loop ($200\text{ Hz}$) stabilizing roll attitude, and a low-rate outer loop trimming the equilibrium setpoint $\theta_{\text{offset}}$ based on flywheel velocity.
- **Consequences:** Maintains high-frequency disturbance rejection while completely eliminating steady-state flywheel speed drift.

### ADR-002: One-Sample PWM Blanking During Direction Reversal
- **Context:** Toggling the Nidec 24H `DIR` logic pin while delivering high duty cycle causes severe shoot-through current and inductive flyback spikes on the motor driver's internal H-bridge.
- **Decision:** Force `PWM_Duty = 100.0%` (active-low 0% power) for exactly 1 discrete sample ($5\text{ ms}$) whenever the commanded direction changes.
- **Consequences:** Eliminates transient rail voltage drops without introducing noticeable phase lag into the balance loop.

### ADR-003: Independent Hardware Timer Allocation on STM32F411RE
- **Context:** Default third-party library blocks share pin assignments between PWM generation and hardware encoder decoding, causing timer peripheral contention.
- **Decision:** Dedicate independent hardware timers: `TIM1` generates 20 kHz PWM for both BLDC motors; `TIM3` decodes the flywheel encoder; `TIM2` decodes the rear wheel encoder; `TIM4` generates 50 Hz PWM for the steering servo.
- **Consequences:** 100% hardware-driven pulse generation and quadrature decoding with 0% CPU interrupt overhead.

### ADR-004: Automatic Static Center-of-Mass Calibration State Machine
- **Context:** The true center of gravity varies between runs due to LiPo battery mounting tolerances and wiring shifts.
- **Decision:** Implement an automated calibration state machine that averages 100 consecutive roll samples when the bicycle is held stationary ($|\dot{\theta}| < 1.5^\circ/\text{s}$) within $\pm 4.5^\circ$ of nominal ($+1.30^\circ$) before arming motor power.
- **Consequences:** Calibrates the true mechanical balance point on power-up without requiring firmware re-flashing.

### ADR-005: Isolation of Steering Potentiometer Feedback on Pin PA4
- **Context:** Generic encoder implementations attach an external interrupt to pin `PA4`, which collides with `ADC1_IN4` used for servo horn potentiometer sensing.
- **Decision:** Strip all `PA4` interrupt routines from the C++ driver implementation of the `TIM3` and `TIM2` encoder blocks.
- **Consequences:** Preserves the analog channel on `PA4` for absolute steering angle feedback.

### ADR-006: Safety Tilt Cutoff at 8.0 Degrees
- **Context:** Beyond an $8.0^\circ$ roll deviation, gravitational torque exceeds the maximum recovery torque of the flywheel actuator under a 12V supply.
- **Decision:** Disarm motor output and latch a shutdown flag (`fault_latched`) instantaneously whenever roll error $|e| > 8.0^\circ$.
- **Consequences:** Prevents dangerous runaway motor spin-up after the chassis has fallen.

### ADR-007: Two-Stage Electromagnetic Braking Sequence
- **Context:** Engaging the hard electromagnetic brake (`BRAKE = 0`) instantly when disarming injects high mechanical shock into the 3D-printed chassis.
- **Decision:** When `Enable_Run < 0.5`, disable PWM drive to let the flywheel freewheel for 100 control cycles ($0.5\text{ s}$) before asserting hard electrical braking (`BRAKE = 0`).
- **Consequences:** Smooths actuator deceleration and protects mechanical gears and 3D-printed mounts from shock fatigue.

### ADR-008: Bounded Dynamic Setpoint Clamping
- **Context:** If the outer loop is permitted to trim the setpoint without restriction during external physical holding, the setpoint drifts away from the upright equilibrium.
- **Decision:** Anchor the baseline calibration point (`saved_offset`) and clamp `current_offset` strictly within $\pm 3.0^\circ$ of `saved_offset`.
- **Consequences:** Guarantees that dynamic trimming remains confined to a physically recoverable operating regime.

### ADR-009: Latched Fault Recovery & High-Speed Linear Power Derating
- **Context:** Binary power cutoff at $3000\text{ RPM}$ causes limit-cycle chattering, and unlatched tilt faults cause violent motor restarts when uprighting a fallen bike.
- **Decision:** Introduce a linear derating function between $2500\text{ RPM}$ and $3000\text{ RPM}$ for forward-torque commands, and require 200 consecutive stationary samples in the upright position before unlatching `fault_latched`.
- **Consequences:** Smooth power transitions near actuator velocity limits and safe manual recovery handling.

### ADR-010: Filter-to-Controller Handshake & Anomaly Rejection Covariance Scaling
- **Context:** During the first 200 samples of gyro bias accumulation, outputting $0.0$ causes the controller to latch a false equilibrium, and lateral accelerations corrupt attitude estimation.
- **Decision:** Emit a busy sentinel flag (`Gyro_X_Rate = 999.0`) from the Kalman filter during initial gyro calibration, and scale measurement covariance $R$ up to 10x during dynamic accelerations ($|\mathbf{a}| \ne 1g$).
- **Consequences:** Prevents premature setpoint latching and immunizes attitude estimation against translational vibration.

### ADR-011: Gravity Feedforward, Anti-Windup Angle Integration, and Slew-Rate Limiting
- **Context:** Because the Nidec 24H BLDC operates in velocity mode ($\tau \propto \dot{u}$), a pure PD controller produces zero acceleration torque at motion reversal points where $\dot{\theta} = 0$.
- **Decision:** Add non-linear gravity feedforward $-K_g \sin(e)$, error integration $-K_i \int e\,dt$ with $\pm 25.0\%$ anti-windup clamping, and an $8.0\% / 5\text{ ms}$ slew-rate limiter.
- **Consequences:** Generates continuous restoring torque proportional to angle without integral windup, matching Simulink interface requirements.

### ADR-012: Delegation of Wireless BLE Telemetry to ESP32-C3 Co-Processor
- **Context:** Running an asynchronous wireless protocol stack on the STM32 introduces non-deterministic interrupt latency that degrades the $200\text{ Hz}$ hard real-time balance loop. Additionally, iOS requires BLE 5.0 NUS rather than legacy SPP.
- **Decision:** Offload all BLE handling and string parsing to an auxiliary ESP32-C3 Super Mini, transmitting fixed 14-byte XOR-checksummed binary frames over USART1 (PA9/PA10) at 50 Hz.
- **Consequences:** Enables low-latency wireless control and live parameter tuning from iOS devices with zero timing jitter on the STM32 real-time core.
