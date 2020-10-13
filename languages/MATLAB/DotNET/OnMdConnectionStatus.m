function OnMdConnectionStatus(sender,arg)

import XAPI.*;

global md;

disp('MD')
% 交易连接回报
disp(arg.status);

if(arg.size1>0)
    disp(Extensions_GBK.Text(arg.userLogin));

switch arg.status
    case XAPI.ConnectionStatus.Done
        % 订阅行情，支持";"分隔
        md.Subscribe('IF1602;IF1603;IF1606','');
end

end
