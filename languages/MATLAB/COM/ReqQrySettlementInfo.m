%% 查询持仓
function tbl = ReqQrySettlementInfo(td,date,sec)

SetWait(0);

% 查询
global settlementInfo_table;
settlementInfo_table = [];
td.NewQuery();
td.SetQuery('DateStart',date);
td.ReqQuery('ReqQrySettlementInfo');

% 需要循环等待才行
Wait(sec);

% 返回
tbl = settlementInfo_table;

end