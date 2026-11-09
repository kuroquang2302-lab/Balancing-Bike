# Development Roadmap & System Milestones

### Phase 1: Peripheral Interfacing & Inertial State Estimation
- Configure I2C Fast-Mode at $400\text{ kHz}$ to stream 14-byte raw sensor frames from the MPU-6050 and build the discrete 1D adaptive Kalman filter.
- Configure STM32F411RE hardware peripherals: `TIM1` (20 kHz PWM), `TIM2` (rear encoder), `TIM3` (flywheel encoder), `TIM4` (servo PWM), and `ADC1_IN4` (potentiometer feedback).

### Phase 2: Actuator Drivers & Calibration State Machine
- Develop `fcn_Nidec_Driver` with active-low $20\text{ kHz}$ PWM, deadband compensation, one-sample reversal blanking, and two-stage braking.
- Implement the automatic static center-of-mass calibration state machine, over-tilt shutdown logic, and latched fault reset routines.

### Phase 3: Adaptive Kalman Tuning & Phase-Lag Minimization
- Establish the filter-to-controller handshake using the `Gyro_X_Rate = 999.0` busy flag during initial 200-sample gyro drift profiling.
- Implement adaptive measurement noise covariance ($R$) scaling based on total acceleration magnitude ($|\mathbf{a}| \ne 1g$) to reject centripetal and tangential disturbances; tune low-pass gyro rate filter coefficient to $0.60$.

### Phase 4: Cascaded Two-Loop Balance Controller Integration
- Synthesize non-linear gravity feedforward, inner PID with anti-windup clamping, and an $8.0\% / 5\text{ ms}$ slew-rate limiter.
- Combine outer momentum dumping loop using TIM3 velocity feedback with soft linear power derating between $2500\text{ RPM}$ and $3000\text{ RPM}$.

### Phase 5: Wireless BLE 5.0 Co-Processor & Telemetry Bridge
- Develop BLE 5.0 server firmware on ESP32-C3 Super Mini implementing the Nordic UART Service (NUS) for iOS interoperability.
- Implement fixed 14-byte XOR-checksummed binary packet protocol over `USART1` (`PA9`/`PA10`) at $50\text{ Hz}$ for gamepad steering, throttle, live parameter tuning, and acoustic buzzer feedback.
