classdef EncoderBlock_TIM2 < matlab.System ...
        & matlabshared.svd.BlockSampleTime ...
        & coder.ExternalDependency
    % Khoi doc Encoder TIM2 (PA15, PB3) chuyen dung cho Nidec 24H Banh sau
    %#codegen
    %#ok<*EMCA>

    properties (Nontunable)
        SampleTime = 0.005;
    end

    methods
        function obj = EncoderBlock_TIM2(varargin)
            coder.allowpcode('plain');
            setProperties(obj,nargin,varargin{:});
        end
        function set.SampleTime(obj,newTime)
            coder.extrinsic('error');
            coder.extrinsic('message');
            newTime = matlabshared.svd.internal.validateSampleTime(newTime);
            obj.SampleTime = newTime;
        end
    end

    methods (Access=protected)
        function setupImpl(~)
            if ~isempty(coder.target)
                coder.cinclude('soc_stm_encoder_tim2.h');
                coder.ceval('initEncoder_TIM2');
            end
        end

        function [EncoderCount,IndexCount,ValidIndex] = stepImpl(~)
            EncoderCount = uint16(0);
            IndexCount = uint16(0);
            ValidIndex = uint16(0);
            if ~isempty(coder.target)
                EncoderCount = coder.ceval('getEncoderCount_TIM2');
                ValidIndex = coder.ceval('getIndexCount_TIM2',coder.wref(IndexCount));
            end
        end

        function releaseImpl(~)
            if ~isempty(coder.target)
                coder.ceval('releaseEncoder_TIM2');
            end
        end

        function num = getNumInputsImpl(~)
            num = 0;
        end

        function num = getNumOutputsImpl(~)
            num = 3;
        end

        function flag = isOutputSizeLockedImpl(~,~)
            flag = true;
        end

        function varargout = isOutputFixedSizeImpl(~,~)
            varargout{1} = true;
            varargout{2} = true;
            varargout{3} = true;
        end

        function varargout = isOutputComplexImpl(~)
            varargout{1} = false;
            varargout{2} = false;
            varargout{3} = false;
        end

        function varargout = getOutputSizeImpl(~)
            varargout{1} = [1,1];
            varargout{2} = [1,1];
            varargout{3} = [1,1];
        end

        function varargout = getOutputDataTypeImpl(~)
            varargout{1} = 'uint16';
            varargout{2} = 'uint16';
            varargout{3} = 'uint16';
        end

        function icon = getIconImpl(~)
            icon = sprintf('STM32 Nucleo\nEncoder TIM2\n(PA15 / PB3)');
        end
    end

    methods (Static, Access=protected)
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
    end

    methods(Static, Access=protected)
        function header = getHeaderImpl()
            header = matlab.system.display.Header(mfilename('class'),...
                'ShowSourceLink', false, ...
                'Title','STM32 Nucleo Encoder TIM2 (PA15 / PB3)', ...
                'Text', ['Quadrature Encoder Mode 3 TIM2 for Motor Nidec 2.' newline ...
                '- PA_15 = Encoder A (PURPLE)' newline ...
                '- PB_3  = Encoder B (GREY)' newline ...
                '- CAN BE USEED WITH EncoderBlock_TIM3.']);
        end
    end

    methods (Static)
        function name = getDescriptiveName()
            name = 'STM32 NUCLEO QEP TIM2';
        end

        function b = isSupportedContext(context)
            b = context.isCodeGenTarget('rtw');
        end

        function updateBuildInfo(buildInfo, context)
            if context.isCodeGenTarget('rtw')
                srcDir = fullfile(fileparts(mfilename('fullpath')),'src');
                includeDir = fullfile(fileparts(mfilename('fullpath')),'include');
                addIncludePaths(buildInfo,includeDir);
                addSourceFiles(buildInfo,'soc_stm_encoder_tim2.cpp',srcDir);
            end
        end
    end
end
