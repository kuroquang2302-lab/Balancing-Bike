classdef EncoderBlock_TIM3 < matlab.System ...
        & matlabshared.svd.BlockSampleTime ...
        & coder.ExternalDependency
    % TIM3 hardware quadrature encoder driver on PA6 and PA7 for reaction wheel motor
    %#codegen
    %#ok<*EMCA>

    properties (Nontunable)
        SampleTime = -1;
    end

    methods
        function obj = EncoderBlock_TIM3(varargin)
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
                coder.cinclude('soc_stm_encoder_tim3.h');
                coder.ceval('initEncoder_TIM3');
            end
        end

        function [EncoderCount, IndexCount, ValidIndex] = stepImpl(~)
            EncoderCount = uint16(0);
            IndexCount   = uint16(0);
            ValidIndex   = uint16(0);
            if ~isempty(coder.target)
                EncoderCount = coder.ceval('getEncoderCount_TIM3');
                ValidIndex   = coder.ceval('getIndexCount_TIM3', coder.wref(IndexCount));
            end
        end

        function releaseImpl(~)
            if ~isempty(coder.target)
                coder.ceval('releaseEncoder_TIM3');
            end
        end

        function num = getNumInputsImpl(~)
            num = 0;
        end

        function num = getNumOutputsImpl(~)
            num = 3;
        end

        function flag = isOutputSizeLockedImpl(~, ~)
            flag = true;
        end

        function varargout = isOutputFixedSizeImpl(~, ~)
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
            varargout{1} = [1, 1];
            varargout{2} = [1, 1];
            varargout{3} = [1, 1];
        end

        function varargout = getOutputDataTypeImpl(~)
            varargout{1} = 'uint16';
            varargout{2} = 'uint16';
            varargout{3} = 'uint16';
        end

        function icon = getIconImpl(~)
            icon = sprintf('STM32 Nucleo\nEncoder TIM3\nPA6 / PA7');
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
                'Title', 'STM32 Nucleo Encoder TIM3', ...
                'Text', ['Hardware 4x Quadrature Encoder Counter on TIM3 peripheral.' newline ...
                'Pin mapping: PA6 (Channel A), PA7 (Channel B).']);
        end
    end

    methods (Static)
        function name = getDescriptiveName()
            name = 'STM32 NUCLEO QEP TIM3';
        end

        function b = isSupportedContext(context)
            b = context.isCodeGenTarget('rtw');
        end

        function updateBuildInfo(buildInfo, context)
            if context.isCodeGenTarget('rtw')
                srcDir     = fullfile(fileparts(mfilename('fullpath')), 'src');
                includeDir = fullfile(fileparts(mfilename('fullpath')), 'include');
                addIncludePaths(buildInfo, includeDir);
                addSourceFiles(buildInfo, 'soc_stm_encoder_tim3.cpp', srcDir);
            end
        end
    end
end
