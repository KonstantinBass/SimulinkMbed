function [cellTagList] = fcnSpiConfUpdateTags()

p = Simulink.Mask.get(gcb);
%a=p.Parameters(2); % Parameter #2 ist die Liste der gefungdenen  Bloecke
%Suche nach Spi-Tags Liste
%diese kann mit->Name: 'puSpiTags' Identifiziert werden
for i=1:length(p.Parameters)
    bCmp(i) = strcmp(p.Parameters(i).Name,'puSpiTags');
end
idx = find(bCmp==1);
if isempty(idx)
    error('SPI-Tag liste kann nicht gefunden werden');
end
a=p.Parameters(idx);

strTrgtFcnName = 'sfcnSpiConf';
blocks = get_param(gcs,'Blocks');
cellTagList = {};
for i=1:length(blocks)
    try
        strFcnName = get_param([gcs,'/',blocks{i}],'FunctionName');
        bCmp = strcmp(strFcnName, strTrgtFcnName);
        if bCmp
            cellTagList{end+1} = blocks{i};
        end
    catch
        %
    end
end

close_system(gcb);
a.set('TypeOptions',cellTagList);
open_system(gcb,'mask');

