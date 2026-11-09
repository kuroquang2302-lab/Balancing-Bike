# Literature Review & Prior Art Analysis

---

## 1. Reaction Wheel Dynamics & Actuator Limits
- **Author / Source:** Tom Stanton
- **Reference:** [Can Reaction Wheels control a Drone?](https://youtu.be/4kfBEaTncjI)
- **Technical Takeaways:**
  - Reaction wheel torque is governed strictly by angular acceleration ($\tau = J \dot{\omega}$). Once the motor approaches maximum terminal velocity dictated by supply voltage, $\dot{\omega} \to 0$, causing instantaneous loss of stabilizing control torque.
  - Sensorless BLDC Electronic Speed Controllers (ESCs) suffer from commutation slip and slow zero-crossing response when reversing direction through $0\text{ RPM}$. Self-balancing systems fundamentally require sensored BLDC motors with integrated Hall effect or optical feedback.

## 2. Field-Oriented Control (FOC) & Dynamic Setpoint Shifting
- **Author / Source:** Every Flavor of Robot
- **Reference:** [Reaction Cube Situation Is Crazy](https://youtu.be/OeoIK913tRk)
- **Technical Takeaways:**
  - When controlling motor torque directly via quadrature current ($\tau \propto i_q$), a standard PD controller creates a natural second-order mass-spring-damper system around vertical equilibrium.
  - To eliminate monotonic flywheel speed drift caused by physical center-of-mass misalignments, an outer loop must slowly steer the balance setpoint $\theta_{\text{offset}}$ against flywheel spin, using gravity to decelerate the rotor back toward $0\text{ RPM}$.

## 3. Velocity-Mode Driver Characteristics & Control Derivatives
- **Author / Source:** James Bruton
- **Reference:** [What is the Difference Between Reaction Wheels and Gyroscopes?](https://youtu.be/pJfMFUcquWM)
- **Technical Takeaways:**
  - Contrasts reaction torque ($\tau = J \dot{\omega}$) with gyroscopic precession torque in Control Moment Gyroscopes (CMGs).
  - On velocity-mode actuators where motor speed tracks input command ($\omega \approx K_v u$), generated reaction torque is proportional to the time derivative of the control command ($\tau \propto \dot{u}$). Consequently, a proportional angle term $-K_p e$ behaves strictly as velocity damping $-K_p \dot{e}$, while an integral angle term $-K_i \int e\,dt$ differentiates into a static restoring torque $-K_i e$.

## 4. Inertia Ratio Optimization & Steady-State Error Removal
- **Author / Source:** Sydney Madueke
- **Reference:** [Building a SELF-BALANCING reaction wheel](https://youtu.be/Cegxa2BuCUc)
- **Technical Takeaways:**
  - Minimizing chassis moment of inertia relative to flywheel inertia enables higher instantaneous angular acceleration at reduced operating voltages.
  - Incorporating integral action with anti-windup bounds eliminates steady-state tilt error under sustained external disturbances.

## 5. Mechanical Baseline & State-Feedback Architecture
- **Author / Source:** RemRC
- **Reference:** [Self-Balancing-Bike](https://github.com/remrc/Self-Balancing-Bike), [3-Reaction-Wheels-Self-Balancing](https://github.com/remrc/3-Reaction-Wheels-Self-Balancing), and [Thingiverse 5887157](https://www.thingiverse.com/thing:5887157)
- **Technical Takeaways:**
  - Provided the original 3D-printable CAD design utilizing Nidec 24H motors and an MG995 steering servo.
  - The legacy Arduino Nano firmware merged four state variables into a single unconstrained linear state-feedback equation, causing flywheel speed braking terms to attenuate tilt recovery torque during large angular deviations.

## 6. Hardware Protection & Nidec 24H Interfacing
- **Author / Source:** BD594 (James Cochrane)
- **Reference:** [Build Your Own - 1D Wannabe Cubli Reaction Wheel](https://www.youtube.com/watch?v=vqV6nazapFQ) and [DIY 1D Wannabe Cubli](https://github.com/squirtifier/DIY---1D-Wannabe-Cubli-Reaction-Wheel)
- **Technical Takeaways:**
  - Pinout documentation for the Nidec 24H driver board, flyback protection diode selection on the main 12V bus, and $10\text{ k}\Omega$ pull-up resistor requirements for the open-drain magnetic encoder channels.

## 7. Standardized Mechanical Assembly
- **Author / Source:** Dmitrii Tomin
- **Reference:** [Printables 956136](https://www.printables.com/model/956136-3d-printed-self-balancing-bike) and [self-balancing_bike](https://github.com/Dmitrii-Tomin/self-balancing_bike)
- **Technical Takeaways:**
  - Documented a 23-step assembly process for 608ZZ bearings, GT2 rear drive belt tensioning, steering linkage geometry, and reaction wheel balancing.

## 8. Comparative Benchmark: DC Motor Single-Loop PID
- **Author / Source:** *Journal of Fuzzy Systems and Control (2024)*
- **Reference:** *PID Control for Balancing Bike Model using Reaction Wheel*, Vol. 2, No. 2, DOI: `10.59247/jfsc.v2i2.188`
- **Technical Takeaways:**
  - Evaluated a reaction-wheel balancing bicycle using a brushed DC motor with an L298N H-bridge and an Arduino Nano without flywheel speed feedback.
  - Experimental results demonstrated that increasing the integral gain $K_i$ in a single-loop structure introduced excessive phase lag, resulting in an extended settling time of $55\text{ s}$ and persistent limit-cycle oscillations around the operating point.
