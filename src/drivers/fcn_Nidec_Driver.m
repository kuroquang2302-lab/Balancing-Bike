function [PWM_Duty_PA8, Dir_PC7, Brake_PC8] = fcn_Nidec_Driver(Enable_Run, Speed_Cmd)
%#codegen
% =========================================================================
% NIDEC 24H BRUSHLESS DC MOTOR DRIVER INTERFACE
% Signal Pins: PA8 - 20 kHz PWM, PC7 - Direction (DIR), PC8 - Brake (BRAKE)
% =========================================================================

persistent prev_dir stop_cnt
if isempty(prev_dir)
    prev_dir = 1.0;
    stop_cnt = 0.0;
end

ZERO_DUTY = 94.5;

% 1. Disarmed state: Coast for 0.5s before asserting electromagnetic brake
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

% 2. Deadband around zero: Release brake and cut PWM power
if abs(u) < 0.5
    PWM_Duty_PA8 = 100.0;
    Dir_PC7      = prev_dir;
    Brake_PC8    = 1.0;
    return;
end

% 3. Determine rotational direction
if u >= 0.01
    desired_dir = 0.0;
else
    desired_dir = 1.0;
end

% 4. Reversal protection: Cut power for 1 discrete cycle during direction change
if desired_dir ~= prev_dir
    PWM_Duty_PA8 = 100.0;
    Dir_PC7      = desired_dir;
    Brake_PC8    = 1.0;
    prev_dir     = desired_dir;
    return;
end

% 5. Active drive output
Brake_PC8    = 1.0;
Dir_PC7      = desired_dir;
PWM_Duty_PA8 = ZERO_DUTY * (1.0 - abs(u) / 100.0);

end
