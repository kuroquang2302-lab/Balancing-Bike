function [Speed_Cmd, Enable_Out, Active_Offset] = ...
    fcn_Balance_Controller( ...
    Roll_Angle, Gyro_X_Rate, Motor_RPM, ...
    Enable_Switch, Kp, Kd, K_trim)

%#codegen
% BỘ ĐIỀU KHIỂN CÂN BẰNG 2 VÒNG LẶP ĐÃ SỬA LỖI CALIB, CHỐNG RUNG & CHỐT NGÃ

persistent current_offset saved_offset calib_sum calib_cnt is_calibrated fault_latched

if isempty(current_offset)
    current_offset = 1.30; % Offset tĩnh thực nghiệm mặc định
    saved_offset   = 1.30;
    calib_sum      = 0.0;
    calib_cnt      = 0.0;
    is_calibrated  = false;
    fault_latched  = false;
end

NOMINAL_OFFSET = 1.30;
CALIB_RANGE    = 4.5;    % Chỉ nhận calib khi xe dựng thẳng quanh 1.30 +- 4.5 deg
MAX_GA         = 95.0;
ANGLE_CUTOFF   = 8.0;
RPM_SOFT       = 2500.0;
RPM_LIMIT      = 3000.0;
RPM_DEADBAND   = 80.0;
OFFSET_LIMIT   = 3.0;
Kw_damp        = 0.002;  % Hệ số cản mềm tốc độ bánh đà
dt             = 0.005;

Speed_Cmd  = 0.0;
Enable_Out = 0.0;

% =========================================================
% 0. CHỜ BỘ LỌC MPU-6050 CALIB GYRO BIAS XONG (1.0 GIÂY ĐẦU)
% =========================================================
if abs(Gyro_X_Rate) > 500.0
    Active_Offset = saved_offset;
    return;
end

% =========================================================
% 1. CHẾ ĐỘ CHỜ HOẶC CHƯA CALIB LẦN ĐẦU: CĂN OFFSET GỐC AN TOÀN
% =========================================================
if (Enable_Switch < 0.5) || (~is_calibrated)
    current_offset = saved_offset;
    fault_latched  = false;

    % Chỉ lấy mẫu khi xe được giữ yên VÀ đang dựng thẳng đứng
    if abs(Gyro_X_Rate) < 1.5 && abs(Roll_Angle - NOMINAL_OFFSET) < CALIB_RANGE
        calib_sum = calib_sum + Roll_Angle;
        calib_cnt = calib_cnt + 1.0;
        if calib_cnt >= 100.0 % Giữ yên 0.5 giây (100 mẫu x 5ms)
            saved_offset   = calib_sum / calib_cnt;
            current_offset = saved_offset;
            calib_sum      = 0.0;
            calib_cnt      = 0.0;
            is_calibrated  = true;
        end
    else
        calib_sum = 0.0;
        calib_cnt = 0.0;
    end

    Active_Offset = current_offset;
    return;
end

% =========================================================
% 2. TÍNH SAI SỐ GÓC & BẢO VỆ NGÃ XE (CÓ CHỐT & TỰ RE-ARM KHI DỰNG YÊN)
% =========================================================
err = Roll_Angle - current_offset;

if fault_latched
    current_offset = saved_offset;
    % Cho phép mở khóa lại nếu người dùng dựng xe thẳng đứng và giữ yên 1.0 giây
    if abs(Gyro_X_Rate) < 1.5 && abs(Roll_Angle - saved_offset) < 2.0
        calib_cnt = calib_cnt + 1.0;
        if calib_cnt >= 200.0
            fault_latched = false;
            calib_cnt     = 0.0;
        end
    else
        calib_cnt = 0.0;
    end
    Active_Offset = current_offset;
    return;
end

if abs(err) > ANGLE_CUTOFF
    fault_latched  = true;
    calib_cnt      = 0.0;
    current_offset = saved_offset;
    Active_Offset  = current_offset;
    return;
end

% =========================================================
% 3. VÒNG TRONG: PD CỨU GÓC + CẢN MỀM VẬN TỐC BÁNH ĐÀ
% =========================================================
Enable_Out = 1.0;
u = -(Kp * err) - (Kd * Gyro_X_Rate) - (Kw_damp * Motor_RPM);

% Kẹp công suất ga
cmd_raw = max(min(u, MAX_GA), -MAX_GA);

% =========================================================
% 4. VÒNG NGOÀI: XẢ RPM BÁNH ĐÀ BẰNG DỊCH CHUYỂN OFFSET
% =========================================================
if abs(err) < 2.5 && abs(Motor_RPM) > RPM_DEADBAND
    shift_rate = -K_trim * (Motor_RPM / 1000.0) * dt;
    shift_rate = max(min(shift_rate, 0.003), -0.003); % Tối đa 0.6 deg/s

    current_offset = current_offset + shift_rate;

    % Giới hạn offset trong phạm vi an toàn quanh điểm calib gốc
    current_offset = max(min(current_offset, ...
                             saved_offset + OFFSET_LIMIT), ...
                         saved_offset - OFFSET_LIMIT);
end

Active_Offset = current_offset;

% =========================================================
% 5. BẢO VỆ QUÁ TỐC MỀM (GIẢM TUYẾN TÍNH TỪ 2500 -> 3000 RPM)
% =========================================================
if cmd_raw * Motor_RPM > 0.0
    abs_rpm = abs(Motor_RPM);
    if abs_rpm >= RPM_LIMIT
        scale = 0.0;
    elseif abs_rpm > RPM_SOFT
        scale = (RPM_LIMIT - abs_rpm) / (RPM_LIMIT - RPM_SOFT);
    else
        scale = 1.0;
    end
    Speed_Cmd = cmd_raw * scale;
else
    Speed_Cmd = cmd_raw;
end

end
