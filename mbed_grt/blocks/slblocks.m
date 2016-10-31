function blkStruct = slblocks
blkStruct.Name = 'MBED Library';    %Display name
blkStruct.OpenFcn = 'mbed_lib';          %Library name
blkStruct.MaskDisplay = '';
Browser(1).Library = 'mbed_lib';         %Library name

%   Copyright 2012-2014 The MathWorks, Inc.

Browser(1).Name='MBED Library';     %Display name
blkStruct.Browser = Browser;
