function [Roll_Angle, Gyro_X_Rate] = fcn_MPU6050_Filter(I2C_Bytes)
%#codegen
% =========================================================================
% BO LOC KALMAN THICH NGHI CAM BIEN QUAN TINH MPU-6050
% Dau vao : Mang 14 byte du lieu tho doc qua giao tiep I2C
% Dau ra  : Goc nghieng Roll_Angle va van toc goc Gyro_X_Rate
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

% 1. Giai ma du lieu nhi phan bu 2
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

% 2. Bo qua mau loi khi duong truyen I2C bi gian doan
if raw_ay == 0.0 && raw_az == 0.0 && raw_gx == 0.0
    Roll_Angle  = angle;
    Gyro_X_Rate = gyro_lpf;
    return;
end

acc_roll = atan2(raw_ay, raw_az) * 180.0 / pi;
gyro_x   = raw_gx / 131.0;

% 3. Uoc luong do lech tinh con quay hoi chuyen trong 200 chu ky dau
if Calib_Count < 200.0
    bias        = bias + gyro_x / 200.0;
    angle       = acc_roll;
    Calib_Count = Calib_Count + 1.0;
    Roll_Angle  = acc_roll;
    Gyro_X_Rate = 999.0;
    gyro_lpf    = 0.0;
    return;
end

% 4. Buoc du doan trang thai goc nghieng
rate  = gyro_x - bias;
angle = angle + dt * rate;
P11   = P11 + Q_angle * dt;

% 5. Buoc cap nhat thich nghi theo bien do gia toc tong hop
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

% 6. Loc thong thap van toc goc
gyro_lpf = 0.60 * rate + 0.40 * gyro_lpf;

Roll_Angle  = angle;
Gyro_X_Rate = gyro_lpf;

end
