function [PWM_Duty_PA8, Dir_PC7, Brake_PC8] = fcn_Nidec_Driver(Enable_Run, Speed_Cmd)
%#codegen
% =========================================================================
% KHOI DIEU KHIEN DONG CO KHONG CHOI THAN NIDEC 24H
% Chan tin hieu : PA8 - Xung PWM 20 kHz, PC7 - Chieu quay, PC8 - Phanh dien tu
% =========================================================================

persistent prev_dir stop_cnt
if isempty(prev_dir)
    prev_dir = 1.0;
    stop_cnt = 0.0;
end

ZERO_DUTY = 94.0;

% 1. Trang thai ngat dong co: Tha troi 0.5 giay dau truoc khi dong phanh
if Enable_Run < 0.5
    PWM_Duty_PA8 = 100.0;
    Dir_PC7      = prev_dir;
    stop_cnt     = stop_cnt + 1.0;

    if stop_cnt > 100.0
        Brake_PC8 = 0.0;
    else
        Brake_PC8 = 1.0;
    end
    return;
else
    stop_cnt = 0.0;
end

u = max(min(Speed_Cmd, 100.0), -100.0);

% 2. Vung chet quanh diem 0: Nha phanh va ngat xung
if abs(u) < 0.5
    PWM_Duty_PA8 = 100.0;
    Dir_PC7      = prev_dir;
    Brake_PC8    = 1.0;
    return;
end

% 3. Xac dinh chieu quay
if u >= 0.01
    desired_dir = 0.0;
else
    desired_dir = 1.0;
end

% 4. Bao ve khi dao chieu: Ngat xung 1 chu ky truoc khi cap cong suat
if desired_dir ~= prev_dir
    PWM_Duty_PA8 = 100.0;
    Dir_PC7      = desired_dir;
    Brake_PC8    = 1.0;
    prev_dir     = desired_dir;
    return;
end

% 5. Xuat xung dieu khien cong suat
Brake_PC8    = 1.0;
Dir_PC7      = desired_dir;
PWM_Duty_PA8 = ZERO_DUTY * (1.0 - abs(u) / 100.0);

end
