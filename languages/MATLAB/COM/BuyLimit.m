function OrderRef = BuyLimit(td,Instrument,Qty,Price)

td.NewOrder();
td.SetOrder('InstrumentID',Instrument);
td.SetOrder('ExchangeID','');
td.SetOrder('Type','Limit');
td.SetOrder('Side','Buy');
td.SetOrder('Qty',Qty);
td.SetOrder('Price',Price);
td.SetOrder('OpenClose','Open');
td.SetOrder('HedgeFlag','Speculation');

OrderRef = td.SendOrder();

end
