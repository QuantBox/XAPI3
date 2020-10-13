using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace XAPI
{
    public enum QueryType : byte
    {
        ReqQryInstrument = 32,
        ReqQryTradingAccount,
        ReqQryInvestorPosition,

        ReqQryOrder,
        ReqQryTrade,
        ReqQryQuote,

        ReqQryInstrumentCommissionRate,
        ReqQryInstrumentMarginRate,
        ReqQrySettlementInfo,
        ReqQryInvestor,

        ReqQryHistoricalTicks,
        ReqQryHistoricalBars,
    }
}
