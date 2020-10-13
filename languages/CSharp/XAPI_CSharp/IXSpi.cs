using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace XAPI
{
    [ComVisible(false)]
    public interface IXSpi
    {
        DelegateOnConnectionStatus OnConnectionStatus { get; set; }
        DelegateOnRtnError OnRtnError { get; set; }
        DelegateOnLog OnLog { get; set; }

        DelegateOnRtnInstrumentStatus OnRtnInstrumentStatus { get; set; }

        /// <summary>
        /// 
        /// </summary>
        DelegateOnRtnDepthMarketData OnRtnDepthMarketData { get; set; }
        
        /// <summary>
        /// 
        /// </summary>
        DelegateOnRspQryInstrument OnRspQryInstrument { get; set; }

        /// <summary>
        /// 
        /// </summary>
        DelegateOnRspQryInvestor OnRspQryInvestor { get; set; }
        DelegateOnRspQryTradingAccount OnRspQryTradingAccount { get; set; }
        DelegateOnRspQryInvestorPosition OnRspQryInvestorPosition { get; set; }
        DelegateOnRspQrySettlementInfo OnRspQrySettlementInfo { get; set; }

        DelegateOnRspQryOrder OnRspQryOrder { get; set; }
        DelegateOnRspQryTrade OnRspQryTrade { get; set; }
        DelegateOnRspQryQuote OnRspQryQuote { get; set; }

        /// <summary>
        /// 
        /// </summary>
        DelegateOnRtnOrder OnRtnOrder { get; set; }
        DelegateOnRtnTrade OnRtnTrade { get; set; }
        DelegateOnRtnQuote OnRtnQuote { get; set; }

        DelegateOnRtnQuoteRequest OnRtnQuoteRequest { get; set; }

        /// <summary>
        /// 
        /// </summary>
        DelegateOnRspQryHistoricalTicks OnRspQryHistoricalTicks { get; set; }
        DelegateOnRspQryHistoricalBars OnRspQryHistoricalBars { get; set; }
    }
}
