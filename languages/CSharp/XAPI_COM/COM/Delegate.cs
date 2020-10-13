using System.Runtime.InteropServices;

namespace XAPI.COM
{
    // 中文问题，以及枚举问题，使用新Class来替代Struct
    [ComVisible(false)]
    public delegate void DelegateOnConnectionStatus(XApiCom sender, int status, string status_String, RspUserLoginClass userLogin, int size1);
    [ComVisible(false)]
    public delegate void DelegateOnRtnError(XApiCom sender, ErrorClass error);
    [ComVisible(false)]
    public delegate void DelegateOnLog(XApiCom sender, LogClass log);

    [ComVisible(false)]
    public delegate void DelegateOnRtnDepthMarketData(XApiCom sender, DepthMarketDataNClass marketData);
    //public delegate void DelegateOnRtnQuoteRequest(object sender, [In]ref QuoteRequestField quoteRequest);

    [ComVisible(false)]
    public delegate void DelegateOnRspQryInstrument(XApiCom sender, InstrumentClass instrument, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryTradingAccount(XApiCom sender, AccountClass account, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryInvestorPosition(XApiCom sender, PositionClass position, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQrySettlementInfo(object sender, SettlementInfoClass settlementInfo, int size1, bool bIsLast);
    //public delegate void DelegateOnRspQryInvestor(object sender, InvestorField investor, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRtnOrder(XApiCom sender, OrderClass order);
    [ComVisible(false)]
    public delegate void DelegateOnRtnTrade(XApiCom sender, TradeClass trade);
    //public delegate void DelegateOnRtnQuote(object sender, QuoteField quote);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryOrder(XApiCom sender, OrderClass order, int size1, bool bIsLast);
    [ComVisible(false)]
    public delegate void DelegateOnRspQryTrade(XApiCom sender, TradeClass trade, int size1, bool bIsLast);
    //public delegate void DelegateOnRspQryQuote(object sender, QuoteField quote, int size1, bool bIsLast);

    //public delegate void DelegateOnRspQryHistoricalTicks(object sender, IntPtr pTicks, int size1, HistoricalDataRequestField request, int size2, bool bIsLast);
    //public delegate void DelegateOnRspQryHistoricalBars(object sender, IntPtr pBars, int size1, HistoricalDataRequestField request, int size2, bool bIsLast);

    //public delegate bool DelegateOnFilterSubscribe(object sender, ExchangeType exchange, int instrument_part1, int instrument_part2, int instrument_part3, IntPtr pInstrument);
}
