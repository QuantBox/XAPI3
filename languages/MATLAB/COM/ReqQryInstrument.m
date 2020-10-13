%% 查询持仓
function tbl = ReqQryInstrument(td, sec)

SetWait(0);

% 查询
global instrument_table;
instrument_table = [];
td.NewQuery();
td.ReqQuery('ReqQryInstrument');

% 需要循环等待才行
Wait(sec);

% 返回
tbl = instrument_table;

end