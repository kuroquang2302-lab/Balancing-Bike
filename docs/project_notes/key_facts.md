# Technical Specifications & Empirical Parameters

## 1. Mechanical Properties
- **Total System Mass:** $750\text{ g} \dots 800\text{ g}$ (including 3D-printed chassis, dual Nidec 24H motors, MG995 servo, 3S LiPo battery, and Nucleo-64 board).
- **Reaction Flywheel:** Mass $100.0\text{ g}$, outer diameter $130\text{ mm}$, moment of inertia $J = 3.1 \times 10^{-4}\text{ kg}\cdot\text{m}^2$.
- **Wheelbase:** $0.22\text{ m}$ between front and rear axle centers.
- **Rear Wheel Drive:** GT2 timing belt, $6\text{ mm}$ width, $240\text{ mm}$ pitch length.

## 2. MPU-6050 Inertial Measurement Unit
- **Mounting Position:** Elevated $60\text{ mm}$ above ground contact line; X-axis aligned longitudinally with bicycle roll axis, Y-axis aligned laterally.
- **Nominal Static Balance Angle:** $+1.30^\circ$.
- **Post-Calibration Stationary Gyro Bias Drift:** $< 0.02^\circ/\text{s}$.
- **Sampling & State Estimation Period:** $T_s = 5\text{ ms}$ ($200\text{ Hz}$).

## 3. Nidec 24H BLDC Actuator Characteristics @ 12V Supply
- **Control Signal:** $20\text{ kHz}$ active-low PWM; motor starts rotating at $94.0\% \dots 94.5\%$ duty cycle.
- **Integrated Magnetic Encoder:** 100 pulses/revolution magnetic ring; $4\text{x}$ quadrature decoding on TIM3/TIM2 yields $400\text{ PPR}$.
- **Linear Command vs. No-Load RPM at 12V:**
  - $1\% \to 111\text{ RPM}$
  - $10\% \to 434\text{ RPM}$
  - $30\% \to 989\text{ RPM}$
  - $50\% \to 1560\text{ RPM}$
  - $70\% \to 2180\text{ RPM}$
  - $100\% \to 3136\text{ RPM}$ (no load), $\approx 2500\text{ RPM}$ with reaction flywheel load
- **Steady-State Velocity Gain:** $K_v \approx 31.36\text{ RPM}/\%$.
- **Current Consumption:** $10\text{ mA} \dots 80\text{ mA}$ steady-state; transient peak during reversal at $30\%$ command reaches $0.3\text{ A} \dots 0.5\text{ A}$.

## 4. Back-EMF Voltage Saturation Constraints
- Under rated $24\text{ V}$ operation, maximum no-load speed reaches $\approx 6000\text{ RPM}$; at $1500\text{ RPM}$, back-EMF accounts for only $25\%$ of supply voltage.
- Under the $12\text{ V}$ supply of a 3S LiPo battery, top speed is capped at $3136\text{ RPM}$. Above $1500\text{ RPM}$, back-EMF exceeds $50\%$ of supply voltage, significantly attenuating angular acceleration torque $\dot{\omega}$. This physical constraint necessitates the outer momentum dumping loop to continuously drain flywheel speed back toward $0\text{ RPM}$.
