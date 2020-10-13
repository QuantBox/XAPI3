using XAPI.Callback;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace XAPI.Event
{
    [ComVisible(false)]
    public class XApiWrapper : XApi
    {
        public new event EventHandler<OnConnectionStatusEventArgs> OnConnectionStatus;
        public new event EventHandler<OnRtnErrorEventArgs> OnRtnError;
        public new event EventHandler<OnLogEventArgs> OnLog;

        public new event EventHandler<OnRtnDepthMarketDataNEventArgs> OnRtnDepthMarketData;
        public new event EventHandler<OnRtnQuoteRequestEventArgs> OnRtnQuoteRequest;

        public new event EventHandler<OnRspQryInstrumentEventArgs> OnRspQryInstrument;
        public new event EventHandler<OnRspQryTradingAccountEventArgs> OnRspQryTradingAccount;
        public new event EventHandler<OnRspQryInvestorPositionEventArgs> OnRspQryInvestorPosition;
        public new event EventHandler<OnRspQrySettlementInfoEventArgs> OnRspQrySettlementInfo;

        public new event EventHandler<OnRspQryOrderEventArgs> OnRspQryOrder;
        public new event EventHandler<OnRspQryTradeEventArgs> OnRspQryTrade;
        public new event EventHandler<OnRspQryQuoteEventArgs> OnRspQryQuote;

        public new event EventHandler<OnRtnOrderEventArgs> OnRtnOrder;
        public new event EventHandler<OnRtnTradeEventArgs> OnRtnTrade;
        public new event EventHandler<OnRtnQuoteEventArgs> OnRtnQuote;

        public new event EventHandler<OnRspQryHistoricalTicksEventArgs> OnRspQryHistoricalTicks;
        public new event EventHandler<OnRspQryHistoricalBarsEventArgs> OnRspQryHistoricalBars;

        public new event EventHandler<OnRspQryInvestorEventArgs> OnRspQryInvestor;


        public XApiWrapper(string path) : this()
        {
            LibPath = path;
        }

        public XApiWrapper() : base()
        {
            base.OnConnectionStatus = OnConnectionStatus_callback;
            base.OnRtnError = OnRtnError_callback;
            base.OnLog = OnLog_callback;

            base.OnRtnDepthMarketData = OnRtnDepthMarketData_callback;
            base.OnRtnQuoteRequest = OnRtnQuoteRequest_callback;

            base.OnRspQryInstrument = OnRspQryInstrument_callback;
            base.OnRspQryTradingAccount = OnRspQryTradingAccount_callback;
            base.OnRspQryInvestorPosition = OnRspQryInvestorPosition_callback;
            base.OnRspQrySettlementInfo = OnRspQrySettlementInfo_callback;

            base.OnRspQryOrder = OnRspQryOrder_callback;
            base.OnRspQryTrade = OnRspQryTrade_callback;
            base.OnRspQryQuote = OnRspQryQuote_callback;

            base.OnRtnOrder = OnRtnOrder_callback;
            base.OnRtnTrade = OnRtnTrade_callback;
            base.OnRtnQuote = OnRtnQuote_callback;

            base.OnRspQryHistoricalTicks = OnRspQryHistoricalTicks_callback;
            base.OnRspQryHistoricalBars = OnRspQryHistoricalBars_callback;

            base.OnRspQryInvestor = OnRspQryInvestor_callback;

        }

        public void Show()
        {
            // MessageBox.Show("123456789");
        }

        private void OnConnectionStatus_callback(object sender, ConnectionStatus status, ref RspUserLoginField userLogin, int size1)
        {
            if (null != OnConnectionStatus)
            {
                OnConnectionStatus(this, new OnConnectionStatusEventArgs(status, ref userLogin, size1));
            }
        }

        private void OnRtnError_callback(object sender, ref ErrorField error)
        {
            if (null != OnRtnError)
            {
                OnRtnError(this, new OnRtnErrorEventArgs(ref error));
            }
        }

        private void OnLog_callback(object sender, ref LogField log)
        {
            if (null != OnLog)
            {
                OnLog(this, new OnLogEventArgs(ref log));
            }
        }

        private void OnRtnDepthMarketData_callback(object sender, ref DepthMarketDataNClass marketData)
        {
            if (null != OnRtnDepthMarketData)
            {
                OnRtnDepthMarketData(this, new OnRtnDepthMarketDataNEventArgs(ref marketData));
            }
        }

        private void OnRtnQuoteRequest_callback(object sender, ref QuoteRequestField quoteRequest)
        {
            if (null != OnRtnQuoteRequest)
            {
                OnRtnQuoteRequest(this, new OnRtnQuoteRequestEventArgs(ref quoteRequest));
            }
        }

        private void OnRspQryInstrument_callback(object sender, ref InstrumentField instrument, int size1, bool bIsLast)
        {
            if (null != OnRspQryInstrument)
            {
                OnRspQryInstrument(this, new OnRspQryInstrumentEventArgs(ref instrument, size1, bIsLast));
            }
        }

        private void OnRspQryTradingAccount_callback(object sender, ref AccountField account, int size1, bool bIsLast)
        {
            if (null != OnRspQryTradingAccount)
            {
                OnRspQryTradingAccount(this, new OnRspQryTradingAccountEventArgs(ref account, size1, bIsLast));
            }
        }

        private void OnRspQryInvestorPosition_callback(object sender, ref PositionField position, int size1, bool bIsLast)
        {
            if (null != OnRspQryInvestorPosition)
            {
                OnRspQryInvestorPosition(this, new OnRspQryInvestorPositionEventArgs(ref position, size1, bIsLast));
            }
        }
        private void OnRspQrySettlementInfo_callback(object sender, ref SettlementInfoClass settlementInfo, int size1, bool bIsLast)
        {
            if (null != OnRspQrySettlementInfo)
            {
                OnRspQrySettlementInfo(this, new OnRspQrySettlementInfoEventArgs(ref settlementInfo, size1, bIsLast));
            }
        }

        private void OnRspQryOrder_callback(object sender, ref OrderField order, int size1, bool bIsLast)
        {
            if (null != OnRspQryOrder)
            {
                OnRspQryOrder(this, new OnRspQryOrderEventArgs(ref order, size1, bIsLast));
            }
        }

        private void OnRspQryTrade_callback(object sender, ref TradeField trade, int size1, bool bIsLast)
        {
            if (null != OnRspQryTrade)
            {
                OnRspQryTrade(this, new OnRspQryTradeEventArgs(ref trade, size1, bIsLast));
            }
        }

        private void OnRspQryQuote_callback(object sender, ref QuoteField quote, int size1, bool bIsLast)
        {
            if (null != OnRspQryQuote)
            {
                OnRspQryQuote(this, new OnRspQryQuoteEventArgs(ref quote, size1, bIsLast));
            }
        }

        private void OnRtnOrder_callback(object sender, ref OrderField order)
        {
            if (null != OnRtnOrder)
            {
                OnRtnOrder(this, new OnRtnOrderEventArgs(ref order));
            }
        }

        private void OnRtnTrade_callback(object sender, ref TradeField trade)
        {
            if (null != OnRtnTrade)
            {
                OnRtnTrade(this, new OnRtnTradeEventArgs(ref trade));
            }
        }

        private void OnRtnQuote_callback(object sender, ref QuoteField quote)
        {
            if (null != OnRtnQuote)
            {
                OnRtnQuote(this, new OnRtnQuoteEventArgs(ref quote));
            }
        }

        private void OnRspQryHistoricalTicks_callback(object sender, IntPtr pTicks, int size1, ref HistoricalDataRequestField request, int size2, bool bIsLast)
        {
            if (null != OnRspQryHistoricalTicks)
            {
                OnRspQryHistoricalTicks(this, new OnRspQryHistoricalTicksEventArgs(pTicks, size1, ref request, size2, bIsLast));
            }
        }

        private void OnRspQryHistoricalBars_callback(object sender, IntPtr pTicks, int size1, ref HistoricalDataRequestField request, int size2, bool bIsLast)
        {
            if (null != OnRspQryHistoricalBars)
            {
                OnRspQryHistoricalBars(this, new OnRspQryHistoricalBarsEventArgs(pTicks, size1, ref request, size2, bIsLast));
            }
        }

        private void OnRspQryInvestor_callback(object sender, ref InvestorField investor, int size1, bool bIsLast)
        {
            if (null != OnRspQryInvestor)
            {
                OnRspQryInvestor(this, new OnRspQryInvestorEventArgs(ref investor, size1, bIsLast));
            }
        }
    }
}
