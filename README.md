# Self-Balancing Bicycle on STM32 and MATLAB Simulink

![System Schematic](Schematic.png)

## Overview

This project takes mechanical inspiration from the three-dimensional printed self-balancing bicycle design by RemRC and adapts it into a friendly STM32F411RE and MATLAB Simulink control project. Instead of maintaining handwritten MATLAB scripts inside the model, the estimation, balancing, and motor driving logic run completely through native Simulink blocks at a fixed 200 hertz control rate.

Moving from script code to native visual blocks makes signal flow easy to inspect, allows live parameter tuning inside Simulink, and generates deterministic C code directly for the STM32F411RE Nucleo-64 microcontroller.

## Upgrade to Native Simulink Blocks

Earlier versions placed sensor filtering, balance equations, and motor logic inside custom MATLAB Function code blocks. That approach hid internal states and slowed down tuning on real hardware.

The upgraded model replaces every custom script block with standard Simulink math, logic, discrete filter, relay, and saturation blocks inside `BalancingBike.slx`. Every intermediate signal can now be monitored in real time while the controller gains are adjusted directly from the Simulink workspace.

## Algorithms Behind the Blocks

### Motion Estimation and Filtering

The inertial sensor subsystem reads raw accelerometer and gyroscope bytes from the MPU6050 sensor every 5 milliseconds over the two-wire sensor bus. Signed measurements are scaled into physical units, and lateral tilt is computed from acceleration components through an inverse tangent operation.

A complementary filter combines 99 percent integrated gyroscope angular rate with 1 percent accelerometer tilt angle to track lean without drifting or picking up wheel vibration. In parallel, raw angular velocity passes through a first-order discrete low-pass filter so derivative control stays smooth and free of mechanical chatter.

### Dual-Loop Cascade Trim Controller

The inner balance loop applies proportional and derivative feedback on roll angle error and filtered angular velocity, paired with light flywheel speed damping. This loop delivers immediate reaction torque whenever the bicycle leans away from vertical.

An outer cascade trim loop passes flywheel velocity through a slow discrete low-pass filter and ignores small speed jitter inside a deadzone around zero revolutions per minute. When the physical center of gravity shifts away from the nominal angle, residual wheel speed integrates slowly into a bounded balance offset within negative 1.5 to positive 1.5 degrees. Gravity then helps slow the flywheel back toward zero speed.

For safe startup and recovery, an arming hysteresis relay activates balancing only when the bicycle enters a tight band near vertical and disarms cleanly if tilt exceeds safety bounds. An anti-windup gate freezes the learned trim offset during saturation or brief recoveries until balancing is turned off manually.

### Motor Driver Conditioning

The flywheel driver subsystem converts signed control commands into active-low pulse width modulation at 20 kilohertz, compensating for the internal motor deadband starting at 94.5 percent duty cycle. Whenever the commanded rotation reverses direction, logic blocks hold motor power at zero for one 5 millisecond sample before toggling direction to protect the internal motor electronics.

### Hardware Encoders and Wireless Bridge

Dedicated C++ System Object blocks configure Timer 3 and Timer 2 in four-times hardware quadrature encoder mode for zero-overhead wheel speed feedback. Wireless phone control and telemetry run separately on an ESP32-C3 Super Mini board over Bluetooth Low Energy, forwarding fixed 14-byte packets with exclusive-or error checking to the STM32 hardware serial port via direct memory access.

## Hardware Components

| Subsystem | Component | Role |
| :--- | :--- | :--- |
| Main controller | STM32F411RE Nucleo-64 | Executes the 200 hertz Simulink control loop |
| Wireless bridge | ESP32-C3 Super Mini | Manages Bluetooth Low Energy communication |
| Reaction flywheel | Nidec 24H brushless motor | Produces balancing torque along the roll axis |
| Rear wheel drive | Nidec 24H brushless motor | Drives forward and backward motion |
| Front steering | TowerPro MG995 servo | Controls front fork steering angle |
| Motion sensor | MPU6050 module | Measures acceleration and roll angular rate |
| Power supply | Three-cell lithium polymer battery | Powers motors directly and electronics at 5 volts |

## Repository Structure

- File `BalancingBike.slx` contains the complete pure-block Simulink model ready for hardware testing.
- Folder `src/hal` holds the hardware abstraction blocks and low-level C++ register drivers for encoders and wireless serial communication.
- Folder `src/esp32_ble_bridge` holds the PlatformIO firmware for the ESP32-C3 Super Mini wireless bridge.
- Folder `docs` contains pin assignments, wireless protocol documentation, and background design notes.

## Quick Start

1. Open MATLAB and add `src/hal` along with its subfolders to the MATLAB path.
2. Open `BalancingBike.slx` in Simulink.
3. Connect the STM32F411RE Nucleo-64 board through a universal serial bus cable and click Monitor and Tune for live tuning, or click Build, Deploy and Start for standalone battery operation.

## Credits

Mechanical geometry and three-dimensional printable frame concepts are adapted from RemRC. Reaction wheel modeling, encoder interfacing, and hardware protection practices draw from Tom Stanton, James Bruton, Sydney Madueke, Every Flavor of Robot, BD594, and Dmitrii Tomin.
