%% 导入C#库，请按自己目录进行调整
cd 'D:\Kan\Documents\GitHub\XAPI2\languages\MATLAB\test'
NET.addAssembly(fullfile(cd,'XAPI_CSharp.dll'));

import XAPI.Event.*;

%% 行情
global md;
md = XApiWrapper(fullfile(cd,'XAPI\x86\CTP\CTP_Quote_x86.dll'));
md.Server.BrokerID = '9999';
md.Server.Address = 'tcp://180.168.146.187:10010';

md.User.UserID = '123456';
md.User.Password = '123456';

addlistener(md,'OnConnectionStatus',@OnMdConnectionStatus);
addlistener(md,'OnRtnDepthMarketData',@OnRtnDepthMarketData);

md.Connect();

%% 交易
global td;
td = XApiWrapper(fullfile(cd,'XAPI\x86\CTP\CTP_Trade_x86.dll'));
td.Server.BrokerID = '9999';
td.Server.Address = 'tcp://180.168.146.187:10000';

td.User.UserID = '123456';
td.User.Password = '123456';

addlistener(td,'OnConnectionStatus',@OnTdConnectionStatus);
addlistener(td,'OnRtnOrder',@OnRtnOrder);

td.Connect();

%% 下单
order1 = BuyLimit('IF1601',1,3000)
disp(order1)

%% 撤单
td.CancelOrder(order1);

%% 退出
% md.Dispose() %行情退出
% td.Dispose() %交易退出