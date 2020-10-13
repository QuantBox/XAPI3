function OrderRef = SellLimit(td,Instrument,Qty,Price)

td.NewOrder();
td.SetOrder('InstrumentID',Instrument);
td.SetOrder('ExchangeID','');
td.SetOrder('Type','Limit');
td.SetOrder('Side','Sell');
td.SetOrder('Qty',Qty);
td.SetOrder('Price',Price);
td.SetOrder('OpenClose','Close');
td.SetOrder('HedgeFlag','Speculation');

OrderRef = td.SendOrder();

end
