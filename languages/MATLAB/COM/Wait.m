% 需要循环等待才行
function Wait(sec)

global wait_lock;
global timing_lock;

while isempty(wait_lock) || wait_lock==0
    d = now - timing_lock;
    disp(datestr(d,'MM:SS.FFF'))
    if (d>sec/86400)
        break;
    end
    
    pause(1);
end

end