function mbed_ec_select_callback_handler(hDlg, hSrc)
%MBED_EC_SELECT_CALLBACK_HANDLER callback handler for mbed target

%   Copyright 2009-2014 The MathWorks, Inc.
%   Geaendert durch K.Bass 14.02.2015

% The target is model reference compliant
slConfigUISetVal(hDlg, hSrc, 'ModelReferenceCompliant', 'on');
slConfigUISetEnabled(hDlg, hSrc, 'ModelReferenceCompliant', false);

%Interface
slConfigUISetEnabled(hDlg, hSrc, 'SupportContinuousTime','on');

%Report
slConfigUISetEnabled(hDlg, hSrc, 'GenerateReport','on');

% Hardware being used is the production hardware
slConfigUISetVal(hDlg, hSrc, 'ProdEqTarget', 'on');

% Setup C++ as default language
slConfigUISetVal(hDlg, hSrc, 'TargetLang', 'C++');

% Setup the hardware configuration
slConfigUISetVal(hDlg, hSrc, 'ProdHWDeviceType', 'Atmel->AVR');

% Set the TargetLibSuffix
slConfigUISetVal(hDlg, hSrc, 'TargetLibSuffix', '.a');

% For real-time builds, we must generate ert_main.c
slConfigUISetVal(hDlg, hSrc, 'ERTCustomFileTemplate', 'mbed_file_process.tlc');

%slConfigUISetVal(hDlg, hSrc, 'ConcurrentExecutionCompliant', 'on');
%GenerateSampleERTMain
slConfigUISetVal(hDlg, hSrc, 'ERTSrcFileBannerTemplate','mbed_code_template.cgt');
slConfigUISetVal(hDlg, hSrc, 'ERTHdrFileBannerTemplate','mbed_code_template.cgt');
slConfigUISetVal(hDlg, hSrc, 'ERTDataSrcFileTemplate','mbed_code_template.cgt');
slConfigUISetVal(hDlg, hSrc, 'ERTDataHdrFileTemplate','mbed_code_template.cgt');

%Package model code in zip file for relocation (packNGo)
% nur fuer online compiler wichtig
slConfigUISetVal(hDlg, hSrc, 'PackageGeneratedCodeAndArtifacts','on');
slConfigUISetVal(hDlg, hSrc, 'PackageName','SimulinkExportPackage');

%Generate code only
% nur fuer online compiler wichtig
slConfigUISetVal(hDlg, hSrc, 'GenCodeOnly','on');

%Code interface packaging -> 'C++ class' | 'Nonreusable function' | 'Reusable function'
% C++ class -> fuer RefModelle nicht erlaubt!
slConfigUISetVal(hDlg, hSrc, 'CodeInterfacePackaging','Nonreusable function');

end
