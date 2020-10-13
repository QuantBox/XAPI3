%% 查询委托
function tbl = ReqQryOrder(td,sec)

SetWait(0);

% 查询
global order_table;
order_table = [];
td.NewQuery();
td.SetQuery('Int32ID',-1);
td.ReqQuery('ReqQryOrder');

% 需要循环等待才行
Wait(sec);

% 返回
tbl = order_table;

end