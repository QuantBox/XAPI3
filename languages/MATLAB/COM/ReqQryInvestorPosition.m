%% 查询持仓
function tbl = ReqQryInvestorPosition(td, sec)

SetWait(0);

% 查询
global position_table;
position_table = [];
td.NewQuery();
td.SetQuery('Int32ID',-1);
td.ReqQuery('ReqQryInvestorPosition');

% 需要循环等待才行
Wait(sec);

% 返回
tbl = position_table;

end