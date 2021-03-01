using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace XAPI
{
    [ComVisible(false)]
    public interface IXApi:IXSpi, IDisposable
    {
        ApiType GetApiTypes { get; }
        string GetApiName { get; }
        string GetApiVersion { get; }

        bool IsConnected { get; }
        void Connect(string szServerPath, string szUserPath, string szPath);
        void Disconnect();

        void ReqQuery(QueryType type, ReqQueryField query);

        void Subscribe(string szInstrument, string szExchange);
        void Unsubscribe(string szInstrument, string szExchange);

        void SubscribeQuote(string szInstrument, string szExchange);
        void UnsubscribeQuote(string szInstrument, string szExchange);

        string SendOrder(OrderField[] orders);
        string CancelOrder(string[] szId);
        string SendQuote(QuoteField quote);
        string CancelQuote(string szId);

        RspUserLoginField UserLogin { get; set; }

        object Log { get; set; }

        int ReconnectInterval { get; set; }
    }
}
