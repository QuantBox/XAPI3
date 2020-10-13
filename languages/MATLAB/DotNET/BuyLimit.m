function OrderRef = BuyLimit(Instrument,Qty,Price)

import XAPI.*;

global td;

% ÏÂµ¥
order = XAPI.OrderField;
order.InstrumentID = Instrument;
order.Type = XAPI.OrderType.Limit;
order.Side = XAPI.OrderSide.Buy;
order.Qty = Qty;
order.Price = Price;
order.OpenClose = XAPI.OpenCloseType.Open;
order.HedgeFlag = XAPI.HedgeFlagType.Speculation;

OrderRef = td.SendOrder(order);

end
