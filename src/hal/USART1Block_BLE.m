classdef USART1Block_BLE < matlab.System ...
        & matlabshared.svd.BlockSampleTime ...
        & coder.ExternalDependency
    % Khoi giao tiep BLE qua ngoai vi USART1 tren chan PA9 va PA10
    %#codegen
    %#ok<*EMCA>

    properties (Nontunable)
        SampleTime = 0.005;
    end

    methods
        function obj = USART1Block_BLE(varargin)
            coder.allowpcode('plain');
            setProperties(obj, nargin, varargin{:});
        end

        function set.SampleTime(obj, newTime)
            coder.extrinsic('error');
            coder.extrinsic('message');
            newTime = matlabshared.svd.internal.validateSampleTime(newTime);
            obj.SampleTime = newTime;
        end
    end

    methods (Access = protected)
        function setupImpl(~)
            if ~isempty(coder.target)
                coder.cinclude('soc_stm_usart1.h');
                coder.ceval('initUSART1_BLE');
            end
        end

        function [Enable_Switch, Drive_Speed, Steer_Angle, Kp, Kd, K_trim, Nominal_Offset] = ...
                stepImpl(~, Enable_Out, Active_Offset, Roll_Angle, Motor_RPM)
            Enable_Switch  = 0.0;
            Drive_Speed    = 0.0;
            Steer_Angle    = 0.0;
            Kp             = 46.0;
            Kd             = 2.00;
            K_trim         = 0.036;
            Nominal_Offset = -1.005;

            if ~isempty(coder.target)
                coder.ceval('stepUSART1_BLE', ...
                    double(Enable_Out), double(Active_Offset), ...
                    double(Roll_Angle), double(Motor_RPM), ...
                    coder.wref(Enable_Switch), ...
                    coder.wref(Drive_Speed), ...
                    coder.wref(Steer_Angle), ...
                    coder.wref(Kp), ...
                    coder.wref(Kd), ...
                    coder.wref(K_trim), ...
                    coder.wref(Nominal_Offset));
            end
        end

        function releaseImpl(~)
            if ~isempty(coder.target)
                coder.ceval('releaseUSART1_BLE');
            end
        end

        function num = getNumInputsImpl(~)
            num = 4;
        end

        function num = getNumOutputsImpl(~)
            num = 7;
        end

        function [name1, name2, name3, name4] = getInputNamesImpl(~)
            name1 = 'Enable_Out';
            name2 = 'Active_Offset';
            name3 = 'Roll_Angle';
            name4 = 'Motor_RPM';
        end

        function [n1, n2, n3, n4, n5, n6, n7] = getOutputNamesImpl(~)
            n1 = 'Enable_Switch';
            n2 = 'Drive_Speed';
            n3 = 'Steer_Angle';
            n4 = 'Kp';
            n5 = 'Kd';
            n6 = 'K_trim';
            n7 = 'Nominal_Offset';
        end

        function flag = isOutputSizeLockedImpl(~, ~)
            flag = true;
        end

        function varargout = isOutputFixedSizeImpl(~)
            for i = 1:7
                varargout{i} = true;
            end
        end

        function varargout = isOutputComplexImpl(~)
            for i = 1:7
                varargout{i} = false;
            end
        end

        function varargout = getOutputSizeImpl(~)
            for i = 1:7
                varargout{i} = [1, 1];
            end
        end

        function varargout = getOutputDataTypeImpl(~)
            for i = 1:7
                varargout{i} = 'double';
            end
        end

        function icon = getIconImpl(~)
            icon = sprintf('STM32 Nucleo\nUSART1 BLE\nPA9 / PA10');
        end
    end

    methods (Static, Access = protected)
        function simMode = getSimulateUsingImpl(~)
            simMode = 'Interpreted execution';
        end

        function isVisible = showSimulateUsingImpl
            isVisible = false;
        end

        function sts = getSampleTimeImpl(obj)
            if isequal(obj.SampleTime, -1) || isequal(obj.SampleTime, [-1, 0])
                sts = matlab.system.SampleTimeSpecification('Type', 'Inherited');
            elseif isequal(obj.SampleTime, [0, 1])
                sts = matlab.system.SampleTimeSpecification('Type', 'Fixed In Minor Step');
            else
                if isscalar(obj.SampleTime)
                    sampleTime = obj.SampleTime;
                    offset = 0;
                else
                    sampleTime = obj.SampleTime(1);
                    offset = obj.SampleTime(2);
                end
                sts = matlab.system.SampleTimeSpecification('Type', 'Discrete', ...
                    'SampleTime', sampleTime, 'Offset', offset);
            end
        end

        function header = getHeaderImpl()
            header = matlab.system.display.Header(mfilename('class'), ...
                'ShowSourceLink', false, ...
                'Title', 'STM32 Nucleo USART1 BLE Bridge', ...
                'Text', ['14-byte BLE through USART1 + DMA2.' newline ...
                'PA9 - USART1_TX, PA10 - USART1_RX (115200 bps).']);
        end
    end

    methods (Static)
        function name = getDescriptiveName()
            name = 'STM32 NUCLEO USART1 BLE';
        end

        function b = isSupportedContext(context)
            b = context.isCodeGenTarget('rtw');
        end

        function updateBuildInfo(buildInfo, context)
            if context.isCodeGenTarget('rtw')
                srcDir     = fullfile(fileparts(mfilename('fullpath')), 'src');
                includeDir = fullfile(fileparts(mfilename('fullpath')), 'include');
                addIncludePaths(buildInfo, includeDir);
                addSourceFiles(buildInfo, 'soc_stm_usart1.cpp', srcDir);
            end
        end
    end
end
