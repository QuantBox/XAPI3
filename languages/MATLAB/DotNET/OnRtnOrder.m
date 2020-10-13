function OnRtnOrder(sender,arg)

import XAPI.*;

% 委托回报
global orders;

% 打印内容
disp(arg.order.InstrumentID);
disp(arg.order.Status);
disp(arg.order.ExecType);
disp(Extensions_GBK.Text(arg.order));

disp(arg.order.LocalID);
disp(arg.order.ID);

end
