# Literature Review & Prior Art Analysis

## 1. Reaction Wheel Dynamics & Actuator Limits
- **Author / Source:** Tom Stanton
- **Reference:** [Can Reaction Wheels control a Drone?](https://youtu.be/4kfBEaTncjI)
- **Technical Takeaways:**
  - Reaction torque is governed by angular acceleration ($\tau = J \dot{\omega}$). Once motor speed approaches the ceiling set by supply voltage, $\dot{\omega} \to 0$ and stabilizing torque drops to zero.
  - Sensorless BLDC ESCs lose commutation cleanly through `0 RPM` reversals. Self-balancing designs require sensored BLDC motors with integrated Hall feedback.

## 2. Dynamic Setpoint Shifting (Momentum Dumping)
- **Author / Source:** Every Flavor of Robot
- **Reference:** [Reaction Cube Situation Is Crazy](https://youtu.be/OeoIK913tRk)
- **Technical Takeaways:**
  - To prevent monotonic flywheel speed drift caused by center-of-mass offsets, an outer loop slowly shifts the balance setpoint $\theta_{\text{offset}}$ against wheel spin so gravity decelerates the flywheel back toward `0 RPM`.

## 3. Velocity-Mode Driver Characteristics
- **Author / Source:** James Bruton
- **Reference:** [What is the Difference Between Reaction Wheels and Gyroscopes?](https://youtu.be/pJfMFUcquWM)
- **Technical Takeaways:**
  - Contrasts reaction torque ($\tau = J \dot{\omega}$) against gyroscopic precession torque in CMGs.
  - Because velocity-mode motor drivers regulate speed rather than current ($\omega \approx K_v u$), the controller must cancel internal back-EMF damping so attitude error produces angular acceleration rather than simple velocity drag.

## 4. Inertia Ratio & Anti-Windup
- **Author / Source:** Sydney Madueke
- **Reference:** [Building a SELF-BALANCING reaction wheel](https://youtu.be/Cegxa2BuCUc)
- **Technical Takeaways:**
  - Minimizing chassis inertia relative to flywheel rim inertia increases instantaneous acceleration torque at `12 V`.
  - Bounding integral trim with anti-windup prevents large external pushes from corrupting the equilibrium setpoint.

## 5. Mechanical Baseline & Front Wheel Bearings
- **Author / Source:** RemRC & Pontifex42
- **Reference:** [Self-Balancing-Bike](https://github.com/remrc/Self-Balancing-Bike), [Thingiverse 5887157](https://www.thingiverse.com/thing:5887157), and [Bearing Front Wheel 5995416](https://www.thingiverse.com/thing:5995416)
- **Technical Takeaways:**
  - Provided the `6 mm` bolted PETG truss geometry, hollow TPU tires, dual Nidec 24H layout, MG995 steering fork, and dual-`685ZZ` bearing front wheel hub.

## 6. Nidec 24H Interfacing & Protection
- **Author / Source:** BD594 (James Cochrane)
- **Reference:** [Build Your Own - 1D Wannabe Cubli Reaction Wheel](https://www.youtube.com/watch?v=vqV6nazapFQ) and [DIY 1D Wannabe Cubli](https://github.com/squirtifier/DIY---1D-Wannabe-Cubli-Reaction-Wheel)
- **Technical Takeaways:**
  - Pinout mapping for the Nidec 24H connector, flyback protection on the `12 V` bus, and `10 kΩ` pull-up resistors for open-drain encoder channels.

## 7. Standardized Mechanical Assembly Notes
- **Author / Source:** Dmitrii Tomin
- **Reference:** [Printables 956136](https://www.printables.com/model/956136-3d-printed-self-balancing-bike) and [self-balancing_bike](https://github.com/Dmitrii-Tomin/self-balancing_bike)
- **Technical Takeaways:**
  - Assembly workflow reference for belt tensioning, steering linkage geometry, and reaction wheel rim balancing.

## 8. Comparative Benchmark: Brushed DC Single-Loop PID
- **Author / Source:** *Journal of Fuzzy Systems and Control (2024)*
- **Reference:** *PID Control for Balancing Bike Model using Reaction Wheel*, Vol. 2, No. 2, DOI: `10.59247/jfsc.v2i2.188`
- **Technical Takeaways:**
  - Evaluated a single-loop PID controller on a brushed DC motor (`L298N` + Arduino Nano) without flywheel speed feedback. Increasing integral gain $K_i$ directly inside the angle loop introduced phase lag, producing a `55 s` settling time and sustained limit-cycle oscillation.
