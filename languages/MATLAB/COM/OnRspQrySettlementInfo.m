function OnRspQrySettlementInfo(varargin)

% 预处理
event_struct = varargin{1,end-1};
settlementInfo = event_struct.settlementInfo;
bIsLast = event_struct.bIsLast;

% 处理内容，应当将多个表格进行组合，然后统一进行打印
global settlementInfo_table;
settlementInfo_table = settlementInfo;

% 最后一条记录，可以返回了
SetWait(bIsLast);

end
