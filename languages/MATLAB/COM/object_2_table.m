%% 转换对象为表格
function t = object_2_table(obj)
    fields = fieldnames(obj);
    %disp('>>>>>>>>>>')
    [m,n] = size(fields);
    %disp(['m=' num2str(m) 'n=' num2str(n)])
    data = cell(1,m);
    for i = 1:m
        tmp = getfield(obj,fields{i});
        if ischar(tmp)
            % 如果出现换行，打印表格时会显示[1x17 char]，不够友好
            data{i} = strjoin(strsplit(tmp,{'\r','\n'}),'\t');
        else
            data{i} = tmp;
        end
    end
    
    %disp('<<<<<<<<<<<<')
    
    t = cell2table(data);
    t.Properties.VariableNames = fields;
end
