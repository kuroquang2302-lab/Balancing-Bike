# Engineering Bug Log & Root-Cause Analyses

### 1. Inverted DIR Pin Logic on Nidec 24H Motor Driver
- **Symptom:** Applying a positive control command caused the reaction wheel to accelerate in the opposite direction, aggravating frame lean rather than correcting it.
- **Root Cause:** The internal ASIC of the Nidec 24H defines logic `0` on pin `PC7` as clockwise (CW) rotation and logic `1` as counter-clockwise (CCW).
- **Resolution:** Standardized inside `fcn_Nidec_Driver.m`: set `Dir_PC7 = 0.0` when `u >= 0.01` and `Dir_PC7 = 1.0` when `u < 0.01`.

### 2. Mechanical Chattering Across Zero-Crossing Deadband
- **Symptom:** Noticeable violent acoustic and mechanical shuddering whenever the control signal crossed zero or remained at low amplitude.
- **Root Cause:** An early driver iteration pulled `Brake_PC8 = 0.0` (engaging electromagnetic braking) whenever $|u| < 0.5\%$.
- **Resolution:** Maintained `Brake_PC8 = 1.0` (brake disengaged) and set `PWM_Duty_PA8 = 100.0%` (active-low 0% power) across the deadband $|u| < 0.5\%$, allowing the flywheel to coast smoothly. Active braking is only engaged after continuous disarming for $> 100$ control cycles ($0.5\text{ s}$).

### 3. Peripheral Pin Conflict Between Library Encoder Block and ADC/PWM
- **Symptom:** Standard Simulink external encoder library disabled PWM output on pin `PA8` and corrupted steering feedback readings on `PA4`.
- **Root Cause:** The generic library code re-initialized `PA8` as an encoder input and configured an external interrupt (EXTI) on `PA4`.
- **Resolution:** Developed a dedicated `EncoderBlock_TIM3` System Object directly mapping `PA6` and `PA7` to `TIM3` registers, stripping all conflicting interrupt handlers on `PA4`.

### 4. Flywheel Velocity Saturation Caused by Static Setpoint Offset
- **Symptom:** A static lean error as small as $0.2^\circ$ caused the reaction wheel to accelerate monotonically until reaching maximum motor RPM ($3136\text{ RPM}$).
- **Root Cause:** In the presence of a non-zero static gravitational torque, the inner loop must maintain continuous non-zero motor acceleration, eventually hitting the supply voltage ceiling.
- **Resolution:** Implemented an outer momentum dumping loop that dynamically shifts `current_offset` proportionally to flywheel velocity via gain `-K_trim`, automatically aligning the frame with the true center of mass.

### 5. False Calibration When Powering On While Lying Horizontal
- **Symptom:** When starting the bicycle lying on its side, the controller recorded a large tilt angle as its zero-point reference.
- **Root Cause:** The initial calibration state machine only checked for low gyro rate without verifying that the roll angle was within a feasible upright range.
- **Resolution:** Restricted calibration sample accumulation to states satisfying both $|\text{Roll\_Angle} - \text{NOMINAL\_OFFSET}| < 4.5^\circ$ and $|\text{Gyro\_X\_Rate}| < 1.5^\circ/\text{s}$.

### 6. Unintended Motor Re-engagement When Lifting Bike After a Fall
- **Symptom:** After falling past $8.0^\circ$, picking up the bicycle caused the motor to suddenly spin up at maximum power as the frame crossed the vertical axis.
- **Root Cause:** The firmware lacked a latched fault state machine after an over-tilt shutdown.
- **Resolution:** Added `fault_latched` state. Once an over-tilt event occurs ($|e| > 8.0^\circ$), motor output remains permanently disabled until the user cycles `Enable_Switch` or holds the bike stationary in the upright position for 200 consecutive cycles.

### 7. Limit-Cycle Chattering at 3000 RPM Overspeed Threshold
- **Symptom:** When flywheel speed approached $3000\text{ RPM}$, the control command chattered abruptly between maximum output and $0\%$.
- **Root Cause:** Binary threshold comparison cut power discontinuously at the ceiling.
- **Resolution:** Implemented a continuous linear scaling window from $2500\text{ RPM}$ to $3000\text{ RPM}$ for commands acting in the direction of motor rotation, while preserving 100% recovery braking torque for opposing commands.

### 8. Premature Calibration Latch and Tangential Acceleration Contamination
- **Symptom:** Balance setpoint erroneously locked to $0.0^\circ$ when `Enable_Switch` was tied high at startup, and estimated roll angle drifted during rapid lateral oscillation.
- **Root Cause:** The Kalman filter output $0.0$ while estimating initial gyro bias, causing the controller to latch premature calibration data. Furthermore, centripetal/tangential accelerations at the 60 mm sensor mounting height corrupted the gravity vector.
- **Resolution:** Programmed the filter to output a busy sentinel flag (`Gyro_X_Rate = 999.0`) during the first 200 samples to hold the controller in standby. Implemented dynamic measurement covariance scaling: $R$ increases up to 10x whenever total acceleration deviates from $1.0g$ by $> 0.08g$.

### 9. Lost Restoring Torque at Motion Reversal in Velocity-Mode BLDC
- **Symptom:** When roll angle exceeded $2.0^\circ$ and angular velocity dropped to $0^\circ/\text{s}$ at the apex of a lean cycle, the motor ceased to accelerate, allowing the bike to fall.
- **Root Cause:** For velocity-mode actuators where $\tau \propto \dot{u}$, the time derivative of a pure PD control law equals zero when $\dot{\theta} = 0$ and $\ddot{\theta} = 0$.
- **Resolution:** Introduced non-linear gravity feedforward $u_{\text{grav}} = -K_g \sin(e)$ and anti-windup error integration $u_{\text{int}} = -K_i \int e\,dt$ to guarantee non-zero $\dot{u}$ at turning points.

### 10. Hardware Reset Short Circuit on ESP32-C3 Super Mini Board
- **Symptom:** Power LED illuminated and 3.3V LDO measured stable at 3.34V, but status LED on GPIO8 remained unlit and the host PC failed to enumerate the USB JTAG/Serial device.
- **Root Cause:** A 100 nF surface-mount ceramic bypass capacitor in parallel with the RST tactile switch had suffered mechanical cracking during PCB manufacture, shorting the `CHIP_EN` line directly to ground ($0\text{ V}$).
- **Resolution:** Probed continuity along the `CHIP_EN` net, isolated the fractured MLCC capacitor, and desoldered it from the board. The internal $10\text{ k}\Omega$ pull-up restored `CHIP_EN` to 3.3V, successfully recovering firmware upload and normal MCU operation.
