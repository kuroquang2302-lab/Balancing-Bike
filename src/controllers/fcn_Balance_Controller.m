function [Speed_Cmd, Enable_Out, Active_Offset] = fcn_Balance_Controller( ...
    Roll_Angle, Gyro_X_Rate, Motor_RPM, Enable_Switch, Kp, Kd, K_trim)
%#codegen
% =========================================================================
% BO DIEU KHIEN CAN BANG XE DAP HAI VONG LAP
% Vong trong : Dieu khien PD goc nghieng ket hop giam chan van toc banh da
% Vong ngoai : Dieu chinh diem dat dong khu bao hoa van toc banh da
% =========================================================================

persistent current_offset saved_offset calib_sum calib_cnt is_calibrated fault_latched

if isempty(current_offset)
    current_offset = 1.30;
    saved_offset   = 1.30;
    calib_sum      = 0.0;
    calib_cnt      = 0.0;
    is_calibrated  = false;
    fault_latched  = false;
end

NOMINAL_OFFSET = 1.30;
CALIB_RANGE    = 4.5;
MAX_GA         = 95.0;
ANGLE_CUTOFF   = 8.0;
RPM_SOFT       = 2500.0;
RPM_LIMIT      = 3000.0;
RPM_DEADBAND   = 80.0;
OFFSET_LIMIT   = 3.0;
Kw_damp        = 0.002;
dt             = 0.005;

Speed_Cmd  = 0.0;
Enable_Out = 0.0;

% 1. Cho bo loc cam bien hoan tat hieu chuan lech tinh
if abs(Gyro_X_Rate) > 500.0
    Active_Offset = saved_offset;
    return;
end

% 2. Che do cho va hieu chuan goc can bang goc
if Enable_Switch < 0.5 || ~is_calibrated
    current_offset = saved_offset;
    fault_latched  = false;

    if abs(Gyro_X_Rate) < 1.5 && abs(Roll_Angle - NOMINAL_OFFSET) < CALIB_RANGE
        calib_sum = calib_sum + Roll_Angle;
        calib_cnt = calib_cnt + 1.0;
        if calib_cnt >= 100.0
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

% 3. Kiem tra sai so goc va khoa an toan khi xe nga
err = Roll_Angle - current_offset;

if fault_latched
    current_offset = saved_offset;
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

% 4. Vong trong: Dieu khien PD goc nghieng va giam chan banh da
Enable_Out = 1.0;
u = -Kp * err - Kd * Gyro_X_Rate - Kw_damp * Motor_RPM;
cmd_raw = max(min(u, MAX_GA), -MAX_GA);

% 5. Vong ngoai: Dich chuyen diem dat khu bao hoa toc do banh da
if abs(err) < 2.5 && abs(Motor_RPM) > RPM_DEADBAND
    shift_rate = -K_trim * Motor_RPM / 1000.0 * dt;
    shift_rate = max(min(shift_rate, 0.003), -0.003);

    current_offset = current_offset + shift_rate;
    current_offset = max(min(current_offset, saved_offset + OFFSET_LIMIT), ...
                         saved_offset - OFFSET_LIMIT);
end

Active_Offset = current_offset;

% 6. Gioi han mem cong suat khi tiem can toc do toi da
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
