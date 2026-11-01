function [Roll_Angle, Gyro_X_Rate] = fcn_MPU6050_Filter(I2C_Bytes)
%#codegen
% BỘ LỌC KALMAN MPU-6050 ĐÃ TỐI ƯU CHỐNG RUNG & CHỐNG NHIỄM ĐỘC BIAS

    dt = 0.005;        % Chu kỳ lấy mẫu 5ms (200 Hz)
    Q_angle = 0.005;   % Độ tin cậy dự đoán góc từ Gyro
    R_meas  = 0.25;    % Tăng R_meas để lọc rung cơ khí + gia tốc tiếp tuyến ở độ cao 6cm

    persistent angle bias P11 Calib_Count gyro_lpf is_init
    if isempty(is_init)
        angle       = 0.0;
        bias        = 0.0;
        P11         = 1.0;
        Calib_Count = 0.0;
        gyro_lpf    = 0.0;
        is_init     = true;
    end

    % 1. Giải mã Byte I2C (Big-Endian Two's Complement)
    raw_ay = double(I2C_Bytes(3)) * 256.0 + double(I2C_Bytes(4));
    if raw_ay >= 32768.0, raw_ay = raw_ay - 65536.0; end

    raw_az = double(I2C_Bytes(5)) * 256.0 + double(I2C_Bytes(6));
    if raw_az >= 32768.0, raw_az = raw_az - 65536.0; end

    raw_gx = double(I2C_Bytes(9)) * 256.0 + double(I2C_Bytes(10));
    if raw_gx >= 32768.0, raw_gx = raw_gx - 65536.0; end

    % Bảo vệ lỗi rớt gói I2C (toàn byte 0): Giữ nguyên giá trị chu kỳ trước
    if (raw_ay == 0.0) && (raw_az == 0.0) && (raw_gx == 0.0)
        Roll_Angle  = angle;
        Gyro_X_Rate = gyro_lpf;
        return;
    end

    acc_roll = atan2(raw_ay, raw_az) * (180.0 / pi);
    gyro_x   = raw_gx / 131.0;

    % 2. Calib Gyro Bias trong 1.0 giây đầu (200 mẫu) khi xe giữ yên
    if Calib_Count < 200.0
        bias        = bias + (gyro_x / 200.0);
        angle       = acc_roll; % Khởi tạo góc thật từ Accel ngay lập tức
        Calib_Count = Calib_Count + 1.0;
        Roll_Angle  = acc_roll;
        % Xuất 999.0 để báo cho Controller chưa chốt saved_offset lúc này
        Gyro_X_Rate = 999.0;
        gyro_lpf    = 0.0;
        return;
    end

    % 3. Bước Dự đoán (Predict Step) - Khóa cứng bias động chống nhiễm độc
    rate  = gyro_x - bias;
    angle = angle + dt * rate;
    P11   = P11 + Q_angle * dt;

    % 4. Bước Cập nhật Thích nghi (Loại bỏ gia tốc tiếp tuyến & rung động cơ)
    total_acc = sqrt(raw_ay * raw_ay + raw_az * raw_az) / 16384.0;
    acc_dev   = abs(total_acc - 1.0);

    if acc_dev > 0.20
        % Khi bị giật mạnh hoặc rung ly tâm (>0.2g): Giảm 10 lần trọng số Accel
        current_R = R_meas * 10.0;
    elseif acc_dev > 0.08
        current_R = R_meas * 3.0;
    else
        current_R = R_meas;
        % Chỉ tinh chỉnh cực chậm bias khi xe thực sự đứng yên và không chịu gia tốc lạ
        if abs(rate) < 1.0
            bias = 0.9995 * bias + 0.0005 * gyro_x;
        end
    end

    y   = acc_roll - angle;
    K1  = P11 / (P11 + current_R);
    angle = angle + K1 * y;
    P11   = (1.0 - K1) * P11;

    % 5. Lọc thông thấp nhẹ cho Gyro (Alpha = 0.60: giảm trễ pha từ 15ms xuống 3ms)
    gyro_lpf = (0.60 * rate) + (0.40 * gyro_lpf);

    % 6. Xuất kết quả
    Roll_Angle  = angle;
    Gyro_X_Rate = gyro_lpf;
end
