# Project Milestones & Future Work

## Completed Milestones
1. **Hardware Bring-Up & Peripheral Drivers:** Built register-level C++ Simulink System Objects for `TIM3` / `TIM2` 4x quadrature encoders and `USART1` `DMA2_Stream2` telemetry/command framing on `STM32F411RE`.
2. **Mechanical Frame Migration:** Transitioned to RemRC `5887157` (`6 mm` bolted PETG truss frame, `GT2-280mm` belt) + Pontifex42 `5995416` (`685ZZ` bearing front wheel) with balanced side ballast.
3. **Pure-Block Simulink Controller (`BalancingBike.slx`):** Replaced legacy MATLAB Function scripts with native Simulink blocks running at `200 Hz` (`Kp = 46.0`, `Kd = 2.0`, `Kw = 0.036`, `Const_Offset = -1.005°`, `±1.5°` integral trim, `2°/12°` hysteresis arming). Verified >`5.5 min` continuous stationary balancing and ~`0.9 s` disturbance recovery.
4. **ESP32-C3 BLE Bridge & iPhone Bluefy Web UI:** Implemented `BalancingBike-C3` NimBLE server (`MTU = 185`, `50 Hz` `14-byte` XOR frames to STM32) and published the two-handed `docs/index.html` controller (`Joy`/`Pad` modes, live `Roll`/`RPM`/`Ofs`, `E-STOP`, live `KP`/`KD` tuning).
5. **Untethered Driving & Steering Validation:** Wired `Drive_Speed` and `Steer_Angle` into `DRIVE and MOTION` and verified stable `5 min` wireless forward, reverse, and cornering runs without tipping.

## Future Work
- **Closed-Loop Rear Speed Control (`ENCODER_TIM2`):** Tune a light speed regulator or cornering lean feedforward using `RPM_Back`, which is currently decoded and available inside `DRIVE and MOTION`.
- **Nonlinear / Fuzzy Controller Benchmarking:** Benchmark a `PD-Fuzzy` or adaptive inner loop against `PD + Kw` on the same `STM32F411RE` hardware under step disturbances (`POT%`, settling time, `IAE`/`ISE`).
- **Dynamic Model vs. Hardware Correlation:** Formulate the full nonlinear Euler-Lagrange equations and overlay simulated step responses directly against logged SDI hardware traces.
