function [cellTagList] = fcnSpiConfUpdateTags()

p = Simulink.Mask.get(gcb);
a=p.Parameters(2); % Parameter #2 ist die Liste der gefungdenen  Bloecke

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

