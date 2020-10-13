using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace XAPI
{
    public enum ResponseType : byte
    {
        OnConnectionStatus = 64,
        OnRtnError,
        OnLog,

        OnRtnDepthMarketData,
        OnRspQryInstrument,
        OnRspQryTradingAccount,
        OnRspQryInvestorPosition,
        OnRspQrySettlementInfo,

        OnRspQryOrder,
        OnRspQryTrade,
        OnRspQryQuote,

        OnRtnOrder,
        OnRtnTrade,
        OnRtnQuote,
        
        OnRtnQuoteRequest,

        OnRspQryHistoricalTicks,
        OnRspQryHistoricalBars,
        OnRspQryInvestor,
        OnRtnInstrumentStatus,

        OnFilterSubscribe,
    }
}
