function makeCmdOut = setup_for_mbed(args)
% Abgeleitet von:
% SETUP_FOR_DEFAULT - sets up a default batch file for RTW builds
% 
%--------------------------------------------------------------------------
% Zuerst das Normale bat erstellen zur exe erzeugung
%--------------------------------------------------------------------------
% SETUP_FOR_DEFAULT - sets up a default batch file for RTW builds
% Copyright 2002-2013 The MathWorks, Inc.  
  makeCmd        = args.makeCmd;
  modelName      = args.modelName;
  verbose        = args.verbose;
  
  pathVmbCompiler = get_param(gcs,'pathVmbCompiler');
  nameVmbModul    = modelName;
  
  % args.compilerEnvVal not used

    
  cmdFile = ['.\',modelName, '.bat'];
  cmdFileFid = fopen(cmdFile,'wt');
  if ~verbose
      fprintf(cmdFileFid, '@echo off\n');
  end
  fprintf(cmdFileFid, 'set MATLAB=%s\n', matlabroot);
  
  % Write out any build hook environment commands
  if ~isempty(args.aBuildHookEnvVars)
      coder.coverage.BuildHook.writePcBuildEnvironmentCmds...
          (cmdFileFid, args.aBuildHookEnvVars, args.aBuildHookPathPrepends);
  end
%--------------------------------------------------------------------------
% Jetzt eigenen Befehl dran bauen
%--------------------------------------------------------------------------
% makeCmdOwn     = ['cd D:\venus\vmbextensions & compile.bat 040_IntegratorKF & cd D:\Test1\040_IntegratorKF & echo ### Created the Lib :-D']; % \\TestBerechnung_venustarget
makeCmdOwn     = {['cd ',pathVmbCompiler];...
                  ['compile.bat ',nameVmbModul, ' & echo ### Created the Lib :-D'];...
                  };

%--------------------------------------------------------------------------
% Datei schliessen und BefehlDatei Uebergeben
%-------------------------------------------------------------------------- 
  
  %fprintf(cmdFileFid, '%s\n', makeCmd );
  for i = 1:length(makeCmdOwn(:,1))
    fprintf(cmdFileFid, '%s\n', makeCmdOwn{i,1});
  end
  fclose(cmdFileFid);
  makeCmdOut = cmdFile;

%endfunction setup_for_default
