function tbl = ReqQryTradingAccount(td, sec)

SetWait(0);

% 查询
global account_table;
account_table = [];
td.NewQuery();
td.ReqQuery('ReqQryTradingAccount');

% 需要循环等待才行
Wait(sec);

% 返回
tbl = account_table;

end