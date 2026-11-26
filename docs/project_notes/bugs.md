# Engineering Bug Log & Root-Cause Analyses

### 1. Inverted DIR Pin Logic & Zero-Crossing Chatter on Nidec 24H
- **Symptom:** Positive commands drove the flywheel backwards in early tests, and engaging `BRAKE = 0` at low duty caused harsh chattering near `0 RPM`.
- **Root Cause:** Nidec 24H defines `DIR = 0` as CW and `1` as CCW, and `BRAKE = 0` locks the rotor mechanically when crossing zero speed.
- **Resolution:** Standardized polarity (`Dir_Neg: < 0`) inside `Driver` and `DRIVE and MOTION`, set `PWM = 100.0%` with `BRAKE = 1` across `|u| < 1.0`, and added `Prev_Dir` 1-sample (`5 ms`) PWM blanking on direction reversals.

### 2. Flywheel Coasts Down or Drifts to Saturation Under Pure PD
- **Symptom:** Under pure `PD` (or low `Kw = 0.002`), the bicycle tipped over after a few seconds as the flywheel slowed or saturated one way.
- **Root Cause:** Because the Nidec 24H ASIC regulates velocity proportionally to PWM (`Kv ≈ 31.36 RPM/%`), back-EMF acts like an unwanted brake unless canceled by `+Kw * Motor_RPM`, and static weight offsets shift the true equilibrium away from nominal.
- **Resolution:** Raised `Const_Kw` to `0.036` (`> 1/Kv`) inside the inner loop and tuned the outer `Integral Trim` loop (`Slow_RPM_LPF`, `±30 RPM` deadzone, `Not_Sat_Check < 85%`, `±1.5°` clamp).

### 3. Peripheral Pin Conflict & 16-Bit Encoder Overflow Spikes
- **Symptom:** Generic Simulink encoder blocks disabled `PA8` PWM output and conflicted with `PA4`; raw counter differences spiked when crossing `65535 -> 0`.
- **Root Cause:** Third-party library wrappers claimed `PA8` and attached EXTI interrupts on `PA4`, and raw `uint16` deltas wrap by `±65535`.
- **Resolution:** Implemented custom register-level `EncoderBlock_TIM3` and `EncoderBlock_TIM2` System Objects with modulo-`65536` signed unwrapping inside Simulink (`+32768 -> mod 65536 -> -32768 -> *30`).

### 4. Algebraic Loop When Wiring `USART1_BLE` Into `BalancingBike.slx`
- **Symptom:** Simulink refused code generation due to an algebraic loop along `Controller/Enable_Out -> USART1_BLE -> Enable_Merge -> Controller/Enable_Switch`.
- **Root Cause:** Feeding `Controller` outputs directly back into `USART1_BLE` in the same discrete sample created zero-delay mutual dependency.
- **Resolution:** Added 1-sample `Unit Delay` blocks (`Delay_BLE_EnOut` IC `0.0`, `Delay_BLE_Ofs` IC `-1.005`) strictly on the telemetry return lines into `USART1_BLE`, keeping zero delay on the incoming control path.

### 5. Bluefy iOS Multi-Touch Freeze, Landscape Shift, and Width Stretching
- **Symptom:** On Bluefy for iOS, using `PointerEvent` (`setPointerCapture`) locked out the second thumb when steering and throttling together, `viewport-fit=cover` shifted landscape touch hitboxes sideways past the notch, and long BLE status strings widened the CSS Grid and shrank buttons upon connecting.
- **Root Cause:** iOS WKWebView multi-touch pointer capture conflicts across sibling controls, Bluefy's container offset disagrees with `viewport-fit=cover` in landscape, and `1fr` CSS Grid tracks expand to `min-content` without `minmax(0, 1fr)`.
- **Resolution:** Rewrote touch controls in `docs/index.html` using native multi-touch `TouchEvent` tracking (`Touch.identifier`) with `requestAnimationFrame` visual coalescing and cached `getBoundingClientRect()`, removed `viewport-fit=cover`, and locked columns with `minmax(0, ...)`.

### 6. `Ofs` Telemetry Flickering to `-3.30°` and BLE Notify Stutter
- **Symptom:** Pushing forward/back/left/right caused `Ofs` on the Web UI to briefly flash `-3.30°` before reverting to `-1.01°`, with occasional control jitter.
- **Root Cause:** Legacy ESP32 firmware initialized `nominal_ofs = -3.30f` and echoed `PAD_PRESS`/`PAD_REL` status strings on every `!B5..B8` button event, while `docs/index.html` overwrote `valOfs` from those status strings. Meanwhile, `RxCallbacks::onWrite` (NimBLE task) called `notify()` concurrently with `loop()` (Arduino task) under default 23-byte MTU.
- **Resolution:** Synced `ESP32-C3` defaults (`46.0f, 2.00f, 0.036f, -1.01f`), restricted `valOfs` updates strictly to `[STM32]` telemetry, set `NimBLEDevice::setMTU(185)`, suppressed continuous motion notify replies, and queued all BLE notifications to execute safely inside `loop()`.

### 7. Hardware Reset Short Circuit on `ESP32-C3 Super Mini`
- **Symptom:** Board powered up at `3.34 V` on the 3.3V rail, but USB enumeration failed and the status LED stayed off.
- **Root Cause:** A cracked `100 nF` MLCC bypass capacitor across the reset switch shorted `CHIP_EN` directly to `GND`.
- **Resolution:** Isolated and removed the fractured capacitor so the internal pull-up restored `CHIP_EN` to `3.3 V`.
