using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace XAPI
{
    [ComVisible(false)]
    public delegate void DelegateOnConnectionStatus(object sender, ConnectionStatus status, [In] ref RspUserLoginField userLogin, int size1);
    [ComVisible(false)]
    public delegate void DelegateOnRtnError(object sender, [In] ref ErrorField error);
    [ComVisible(false)]
    public delegate void DelegateOnLog(object sender, [In] ref LogField log);
    [ComVisible(false)]
    public delegate void DelegateOnRtnDepthMarketData(object sender, [In]ref DepthMarketDataNClass marketData);
    [ComVisible(false)]
    public delegate void DelegateOnRtnQuoteRequest(object sender, [In]ref QuoteRequestField quoteRequest);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryInstrument(object sender, [In] ref InstrumentField instrument, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryTradingAccount(object sender, [In] ref AccountField account, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryInvestorPosition(object sender, [In] ref PositionField position, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQrySettlementInfo(object sender, [In] ref SettlementInfoClass settlementInfo, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryInvestor(object sender, [In] ref InvestorField investor, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRtnOrder(object sender, [In] ref OrderField order);
    [ComVisible(false)]
    public delegate void DelegateOnRtnTrade(object sender, [In] ref TradeField trade);
    [ComVisible(false)]
    public delegate void DelegateOnRtnQuote(object sender, [In] ref QuoteField quote);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryOrder(object sender, [In] ref OrderField order, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryTrade(object sender, [In] ref TradeField trade, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryQuote(object sender, [In] ref QuoteField quote, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRtnInstrumentStatus(object sender, [In] ref InstrumentStatusField pInstrumentStatus);

    [ComVisible(false)]
    public delegate void DelegateOnRspQryHistoricalTicks(object sender, IntPtr pTicks, int size1, [In] ref HistoricalDataRequestField request, int size2, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryHistoricalBars(object sender, IntPtr pBars, int size1, [In] ref HistoricalDataRequestField request, int size2, bool bIsLast);
    [ComVisible(false)]
    public delegate bool DelegateOnFilterSubscribe(object sender, ExchangeType exchange, int instrument_part1, int instrument_part2, int instrument_part3, IntPtr pInstrument);
}
