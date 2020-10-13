%% 交易
function td = CTP_Connect()

% 只能实例化一次
td = actxserver('XAPI.COM');
td.SetLibPath(fullfile(cd,'XAPI\x86\CTP\CTP_Trade_x86.dll'));
td.SetLibPath('C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Trade_x86.dll');

td.SetServerInfo('BrokerID','4500');
td.SetServerInfo('Address','tcp://180.166.45.116:41205');

td.SetUserInfo('UserID','123');
td.SetUserInfo('Password','123');

registerevent(td,{'OnConnectionStatus' @OnTdConnectionStatus});
registerevent(td,{'OnRtnOrder' @OnRtnOrder});
registerevent(td,{'OnRspQryOrder' @OnRspQryOrder});
registerevent(td,{'OnRspQryInvestorPosition' @OnRspQryInvestorPosition});
registerevent(td,{'OnRtnDepthMarketData' @OnRtnDepthMarketData});
registerevent(td,{'OnRspQrySettlementInfo' @OnRspQrySettlementInfo});
registerevent(td,{'OnRspQryInstrument' @OnRspQryInstrument});
registerevent(td,{'OnRspQryTradingAccount' @OnRspQryTradingAccount});

td.Connect();

end

