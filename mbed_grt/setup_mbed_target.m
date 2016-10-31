%Setup the MBED Target
strPath = pwd;
addpath(genpath('strPath'));

strPathBlocks = fullfile(strPath,'blocks');


%Build all S-Functions
cd(strPathBlocks)
strctFileList = dir('*.c');

for i = 1:numel(strctFileList)
    try
        fprintf('Building S-Function: %s\n', strctFileList(i).name);
        mex(strctFileList(i).name);
    catch
    end    
end

cd(strPathBlocks)