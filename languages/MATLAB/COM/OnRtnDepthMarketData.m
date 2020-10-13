function OnRtnDepthMarketData(varargin)
% 行情回报

% 打印行情
event_struct = varargin{1,end-1};
marketData = event_struct.marketData;

disp(marketData.Symbol);
disp(marketData.InstrumentID);
disp(marketData.ExchangeID);
disp(marketData.LastPrice);
disp(marketData.AskPrice1);
disp(marketData.BidPrice1);

end
