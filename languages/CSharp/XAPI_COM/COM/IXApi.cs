using System;
using System.Runtime.InteropServices;

namespace XAPI.COM
{
    [ComVisible(true)]
    [Guid("DBCAEFA9-4337-47B8-9B47-66070604CE2C")]
    public interface IXApi
    {
        [DispId(1)]
        void SetLibPath(string LibPath);
        [DispId(2)]
        void SetServerInfo(string key,object value);
        [DispId(3)]
        void SetUserInfo(string key, object value);


        [DispId(10)]
        void Connect();
        [DispId(11)]
        void Disconnect();

        [DispId(20)]
        void Subscribe(string szInstrument, string szExchange);
        [DispId(21)]
        void Unsubscribe(string szInstrument, string szExchange);

        [DispId(31)]
        void NewOrder();
        [DispId(32)]
        void SetOrder(string key, object value);
        [DispId(33)]
        string SendOrder();
        [DispId(34)]
        string CancelOrder(string id);

        [DispId(40)]
        void NewQuery();
        [DispId(41)]
        void SetQuery(string key, object value);
        [DispId(42)]
        void ReqQuery(string type);
        [DispId(50)]
        QueueData TryDequeue();

        [DispId(60)]
        void GCCollect();
    }
}
