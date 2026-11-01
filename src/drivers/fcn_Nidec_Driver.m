function [PWM_Duty_PA8, Dir_PC7, Brake_PC8] = fcn_Nidec_Driver(Enable_Run, Speed_Cmd)
%#codegen
% =========================================================================
% NIDEC 24H BLDC MOTOR DRIVER WITH 2-STAGE BRAKE SEQUENCER
% Hardware: STM32F411RE Nucleo-64
% Pins: PA8 (PWM 20 kHz Active-Low), PC7 (DIR), PC8 (BRAKE)
% Logic:
%   - DIR: 0 = CW (positive command), 1 = CCW (negative command)
%   - BRAKE: 1 = Run/Coast, 0 = Active Electronic Braking
%   - Zero-Crossing: 1 cycle 100% duty coasting during direction reversal
%   - 2-Stage Stop: 0.5s coasting on landing, followed by hard brake stop
% =========================================================================

persistent prev_dir stop_cnt
if isempty(prev_dir)
    prev_dir = 1.0;
    stop_cnt = 0.0;
end

ZERO_DUTY = 94.0;

% 1. Disabled / Fallen State: 2-stage brake sequencer
if Enable_Run < 0.5
    PWM_Duty_PA8 = 100.0;
    Dir_PC7      = prev_dir;
    
    stop_cnt = stop_cnt + 1.0;
    if stop_cnt > 100.0 % After 0.5s (100 cycles x 5ms)
        Brake_PC8 = 0.0; % Active brake engages once bike is resting
    else
        Brake_PC8 = 1.0; % First 0.5s coasting to prevent rebound
    end
    return;
else
    stop_cnt = 0.0;
end

u = max(min(Speed_Cmd, 100.0), -100.0);

% 2. Near-Zero Deadband: Coasting
if abs(u) < 0.5
    PWM_Duty_PA8 = 100.0;
    Dir_PC7      = prev_dir;
    Brake_PC8    = 1.0;
    return;
end

% 3. Direction resolution
if u >= 0.01
    desired_dir = 0.0;
else
    desired_dir = 1.0;
end

% 4. Zero-crossing guard (1 sample deadtime)
if desired_dir ~= prev_dir
    PWM_Duty_PA8 = 100.0;
    Dir_PC7      = desired_dir;
    Brake_PC8    = 1.0;
    prev_dir     = desired_dir;
    return;
end

% 5. Active Drive
Brake_PC8    = 1.0;
Dir_PC7      = desired_dir;
PWM_Duty_PA8 = ZERO_DUTY * (1.0 - abs(u) / 100.0);

end
