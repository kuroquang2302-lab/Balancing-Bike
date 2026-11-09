function [Roll_Angle, Gyro_X_Rate] = fcn_MPU6050_Filter(I2C_Bytes)
%#codegen
% =========================================================================
% ADAPTIVE 1D KALMAN FILTER FOR MPU-6050 INERTIAL MEASUREMENT UNIT
% Input  : 14-byte raw sensor data array read over I2C Fast-Mode (400 kHz)
% Output : Estimated Roll_Angle (deg) and Gyro_X_Rate (deg/s)
% =========================================================================

dt      = 0.005;
Q_angle = 0.005;
R_meas  = 0.25;

persistent angle bias P11 Calib_Count gyro_lpf is_init
if isempty(is_init)
    angle       = 0.0;
    bias        = 0.0;
    P11         = 1.0;
    Calib_Count = 0.0;
    gyro_lpf    = 0.0;
    is_init     = true;
end

% 1. Decode two's complement 16-bit binary accelerometer and gyroscope values
raw_ay = double(I2C_Bytes(3)) * 256.0 + double(I2C_Bytes(4));
if raw_ay >= 32768.0
    raw_ay = raw_ay - 65536.0;
end

raw_az = double(I2C_Bytes(5)) * 256.0 + double(I2C_Bytes(6));
if raw_az >= 32768.0
    raw_az = raw_az - 65536.0;
end

raw_gx = double(I2C_Bytes(9)) * 256.0 + double(I2C_Bytes(10));
if raw_gx >= 32768.0
    raw_gx = raw_gx - 65536.0;
end

% 2. Discard corrupt frames during I2C bus dropouts
if raw_ay == 0.0 && raw_az == 0.0 && raw_gx == 0.0
    Roll_Angle  = angle;
    Gyro_X_Rate = gyro_lpf;
    return;
end

acc_roll = atan2(raw_ay, raw_az) * 180.0 / pi;
gyro_x   = raw_gx / 131.0;

% 3. Estimate static gyroscope bias during initial 200 cycles
if Calib_Count < 200.0
    bias        = bias + gyro_x / 200.0;
    angle       = acc_roll;
    Calib_Count = Calib_Count + 1.0;
    Roll_Angle  = acc_roll;
    Gyro_X_Rate = 999.0;
    gyro_lpf    = 0.0;
    return;
end

% 4. State prediction step
rate  = gyro_x - bias;
angle = angle + dt * rate;
P11   = P11 + Q_angle * dt;

% 5. Adaptive measurement update with total acceleration check
total_acc = sqrt(raw_ay * raw_ay + raw_az * raw_az) / 16384.0;
acc_dev   = abs(total_acc - 1.0);

if acc_dev > 0.20
    current_R = R_meas * 10.0;
elseif acc_dev > 0.08
    current_R = R_meas * 3.0;
else
    current_R = R_meas;
    if abs(rate) < 1.0
        bias = 0.9995 * bias + 0.0005 * gyro_x;
    end
end

y     = acc_roll - angle;
K1    = P11 / (P11 + current_R);
angle = angle + K1 * y;
P11   = (1.0 - K1) * P11;

% 6. Low-pass filter angular velocity output
gyro_lpf = 0.60 * rate + 0.40 * gyro_lpf;

Roll_Angle  = angle;
Gyro_X_Rate = gyro_lpf;

end
