function mbed_make_rtw_hook(hookMethod, modelName, rtwRoot, templateMakefile, buildOpts, buildArgs)
%function mbed_make_rtw_hook(hookMethod,modelName,~,~,~,~)
%

% K.Bass 07.03.2014
% Get System Parameter
%bGenCodeOnly = strcmp(get_param(gcs,'GenCodeOnly'),'on');

switch hookMethod
    case 'error'
        % Called if an error occurs anywhere during the build.  If no error occurs
        % during the build, then this hook will not be called.  Valid arguments
        % at this stage are hookMethod and modelName. This enables cleaning up
        % any static or global data used by this hook file.
        disp(['### Simulink Coder build procedure for model: ''' modelName...
            ''' aborted due to an error.']);
        
    case 'entry'
        % Called at start of code generation process (before anything happens.)
        % Valid arguments at this stage are hookMethod, modelName, and buildArgs.
        
    case 'before_tlc'
        % Called just prior to invoking TLC Compiler (actual code generation.)
        % Valid arguments at this stage are hookMethod, modelName, and
        % buildArgs
        
    case 'after_tlc'
        % Called just after to invoking TLC Compiler (actual code generation.)
        % Valid arguments at this stage are hookMethod, modelName, and
        % buildArgs
        disp('rtw_hook after tlc');
        %Copy MBED Library in Model Folder
        %[err] = copyMbedLib();
        
    case {'before_make','before_makefilebuilder_make'}
        % Called after code generation is complete, and just prior to kicking
        % off make process (assuming code generation only is not selected.)  All
        % arguments are valid at this stage.
        
    case 'after_make'
        % Called after make process is complete. All arguments are valid at
        % this stage.
        
    case 'exit'
        % Called at the end of the RTW build process.  All arguments are valid
        % at this stage.
        
        disp(['### Successful completion of Simulink Coder build ',...
            'procedure for model: ', modelName]);
        
end

function [err] = copyMbedLib()
    strPathMbedH = which('mbed.h');
    if isempty(strPathMbedH)
        error('mbed.h not found');
    else
        posMbedLibFolder = strfind(strPathMbedH,'mbed-src');
        strMbedLibFolder = [strPathMbedH(1:(posMbedLibFolder-1)),'mbed-src'];
        strMbedLibTargetFolder = fullfile(cd,'mbed-src');
        disp(['### Copy mbed-src to model code directory ...'])
        [st] = copyfile(strMbedLibFolder, strMbedLibTargetFolder);
        if st == 0
            error('### copy error')
        end
        disp(['### Copy mbed-src successful ...'])
    end
       
    err = 0;
