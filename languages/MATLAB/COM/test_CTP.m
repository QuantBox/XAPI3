td = CTP_Connect();

% 需要循环等待才行
SetWait(0);
Wait(10);
return

%% 一定要等待登录成功才做
positions = ReqQryInvestorPosition(td,10)
orders = ReqQryOrder(td,10)

settlementInfo = ReqQrySettlementInfo(td,20161219,10)
disp(settlementInfo.Content)

instruments = ReqQryInstrument(td,10)

account = ReqQryTradingAccount(td,10)

return;