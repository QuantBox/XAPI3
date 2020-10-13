using System;
using System.Runtime.InteropServices;

namespace XAPI.COM
{
    /// <summary>
    /// 将object sender改成XApiCom sender后，在MATLAB下只是OnData事件可以注册后收到消息，而其它的事件都无法直接收到
    /// 如果将OnConnectionStatus这类的事件修改成不带类成员的这种，一些信息又无法传递
    /// </summary>
    [ComVisible(true)]
    [Guid("4D7F9B54-3098-45AE-8EEB-4ED24AC47613")]
    [InterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
    public interface IXApiEvents
    {
        [DispId(2)]
        void OnConnectionStatus(XApiCom sender, int status, string status_String, RspUserLoginClass userLogin, int size1);
        [DispId(3)]
        void OnRtnError(XApiCom sender, ErrorClass error);
        [DispId(4)]
        void OnLog(XApiCom sender, LogClass log);

        [DispId(5)]
        void OnRtnDepthMarketData(XApiCom sender, DepthMarketDataNClass marketData);
        //public delegate void DelegateOnRtnQuoteRequest(object sender, [In]ref QuoteRequestField quoteRequest);

        [DispId(6)]
        void OnRspQryInstrument(XApiCom sender, InstrumentClass instrument, int size1, bool bIsLast);
        [DispId(7)]
        void OnRspQryTradingAccount(XApiCom sender, AccountClass account, int size1, bool bIsLast);
        [DispId(8)]
        void OnRspQryInvestorPosition(XApiCom sender, PositionClass position, int size1, bool bIsLast);
        [DispId(9)]
        void OnRspQrySettlementInfo(object sender, SettlementInfoClass settlementInfo, int size1, bool bIsLast);
        //public delegate void DelegateOnRspQryInvestor(object sender, InvestorField investor, int size1, bool bIsLast);
        [DispId(10)]
        void OnRtnOrder(XApiCom sender, OrderClass order);
        [DispId(11)]
        void OnRtnTrade(XApiCom sender, TradeClass trade);
        //public delegate void DelegateOnRtnQuote(object sender, QuoteField quote);
        [DispId(12)]
        void OnRspQryOrder(XApiCom sender, OrderClass order, int size1, bool bIsLast);
        [DispId(13)]
        void OnRspQryTrade(XApiCom sender, TradeClass trade, int size1, bool bIsLast);
        //public delegate void DelegateOnRspQryQuote(object sender, QuoteField quote, int size1, bool bIsLast);

        //public delegate void DelegateOnRspQryHistoricalTicks(object sender, IntPtr pTicks, int size1, HistoricalDataRequestField request, int size2, bool bIsLast);
        //public delegate void DelegateOnRspQryHistoricalBars(object sender, IntPtr pBars, int size1, HistoricalDataRequestField request, int size2, bool bIsLast);

        //public delegate bool DelegateOnFilterSubscribe(object sender, ExchangeType exchange, int instrument_part1, int instrument_part2, int instrument_part3, IntPtr pInstrument);
        
    }
}
