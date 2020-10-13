function OnTdConnectionStatus(sender,arg)

import XAPI.*;

global td;

disp('TD')
% 交易连接回报
disp(arg.status);

if(arg.size1>0)
    disp(Extensions_GBK.Text(arg.userLogin));

end
