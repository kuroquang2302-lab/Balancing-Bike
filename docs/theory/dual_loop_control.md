# Theoretical Modeling & Cascaded Two-Loop Control Architecture

## 1. Reaction Wheel Inverted Pendulum Dynamics

The lateral roll dynamics of the bicycle chassis about its ground contact line are governed by the non-linear inverted pendulum equation:

$$I_{\text{bike}} \ddot{\theta} = m g h \sin(\theta - \theta_{\text{CM}}) - \tau_{\text{wheel}}$$

where:
- $\theta$: Roll angle relative to the gravity vector.
- $\theta_{\text{CM}}$: Static angular offset of the physical center of mass.
- $m$: Total system mass ($0.75\text{ kg} \dots 0.80\text{ kg}$).
- $h$: Effective height of the center of mass above the ground contact patch ($0.06\text{ m}$).
- $I_{\text{bike}}$: Lumped moment of inertia of the bicycle frame.
- $\tau_{\text{wheel}}$: Reaction torque applied by the flywheel to the frame.

The reaction torque delivered to the chassis is directly proportional to the angular acceleration of the flywheel:

$$\tau_{\text{wheel}} = J_{\text{wheel}} \dot{\omega}_{\text{wheel}}$$

where $J_{\text{wheel}} = 3.1 \times 10^{-4}\text{ kg}\cdot\text{m}^2$ is the moment of inertia of the reaction wheel.

---

## 2. Velocity-Mode Actuator Dynamics (Nidec 24H)

The internal Application-Specific Integrated Circuit (ASIC) of the Nidec 24H BLDC motor regulates angular velocity proportionally to the input PWM duty cycle:

$$\omega_{\text{wheel}} \approx K_v u$$

where $K_v \approx 31.36\text{ RPM}/\%$ is the steady-state velocity gain at $12\text{ V}$.

Differentiating with respect to time yields the instantaneous reaction torque acting on the bicycle frame:

$$\tau_{\text{wheel}} = J_{\text{wheel}} K_v \frac{du}{dt}$$

### Physical Implications of Pure PD Control
If a standard PD control law $u = -K_p e - K_d \dot{e}$ is applied (where $e = \theta - \theta_{\text{offset}}$), its time derivative becomes:

$$\frac{du}{dt} = -K_p \dot{e} - K_d \ddot{e}$$

At the turning point of a roll disturbance (where roll velocity $\dot{e} = 0$), the proportional term $-K_p \dot{e}$ vanishes completely. Consequently, the motor ceases to accelerate, producing zero restoring torque at the point of maximum lean.

To restore a static restoring torque proportional to angle, an integral error term $-K_i \int e\,dt$ and non-linear gravity feedforward $-K_g \sin(e)$ must be incorporated:

$$\frac{d}{dt} \left( -K_i \int e\,dt \right) = -K_i e$$

Taking the time derivative of the integral term yields a term proportional to displacement $-K_i e$, directly resisting gravitational torque.

---

## 3. Cascaded Two-Loop Control Scheme

### 3.1. Inner Balance Loop ($f_s = 200\text{ Hz}, T_s = 5\text{ ms}$)
The inner loop computes the total control command by combining gravity feedforward, roll angle PID, and flywheel velocity damping:

$$u = -K_g \sin(e) - K_p e - K_i \int e\,dt - K_d \dot{\theta} - K_w \omega_{\text{wheel}}$$

- **Anti-Windup Integration:** The integral accumulator is bounded at $\pm 25.0\%$ equivalent duty cycle. When $|e| < 0.15^\circ$, an exponential decay factor of $0.995$ is applied to discharge accumulator windup and prevent overshoot across the vertical equilibrium.
- **Slew-Rate Limiter:** Maximum control variation between successive $5\text{ ms}$ steps is clamped to $|\Delta u| \le 8.0\% / 5\text{ ms}$, eliminating sharp torque steps and protecting the motor driver.

### 3.2. Soft High-Speed Power Derating
As the flywheel approaches its physical speed limit ($3000\text{ RPM}$ under 12V load), back-EMF severely degrades available acceleration torque. To prevent sudden control cutoffs and limit-cycle oscillations:
- For motor commands acting in the same direction as current spin ($u \cdot \omega_{\text{wheel}} > 0$):
  - When $|\omega_{\text{wheel}}| \le 2500\text{ RPM}$: Full command applied ($\text{scale} = 1.0$).
  - When $2500\text{ RPM} < |\omega_{\text{wheel}}| < 3000\text{ RPM}$: Linear scaling factor applied:

$$\text{scale} = \frac{3000 - |\omega_{\text{wheel}}|}{3000 - 2500}$$

  - When $|\omega_{\text{wheel}}| \ge 3000\text{ RPM}$: Command suppressed to $0\%$ to prevent overspeed.
- Reverse-torque braking commands ($u \cdot \omega_{\text{wheel}} < 0$) are never derated, allowing full recovery torque at all times.

### 3.3. Outer Momentum Dumping Loop
Physical center-of-mass shifts cause the inner loop to maintain a steady-state lean error, forcing the flywheel to accelerate continuously until voltage saturation occurs. 

The outer loop measures the true flywheel velocity $\omega_{\text{wheel}}$ via the TIM3 4x quadrature encoder and dynamically trims the balance setpoint $\theta_{\text{offset}}$:

$$\dot{\theta}_{\text{offset}} = -K_{\text{trim}} \frac{\omega_{\text{wheel}}}{1000}$$

- The trim rate is saturated at $\pm 0.003^\circ / 5\text{ ms}$ ($0.6^\circ/\text{s}$).
- The allowable setpoint range is strictly bounded within $\pm 3.0^\circ$ of the initial static calibration point $\theta_{\text{saved}}$.
- By steering the frame slightly into the direction of motor acceleration, gravity exerts a natural opposing torque that decelerates the flywheel back to $0\text{ RPM}$.
