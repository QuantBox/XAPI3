using XAPI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace XAPI.Callback
{
    [ComVisible(false)]
    public partial class XApi : BaseApi, IDisposable, IXApi
    {
        public XApi(string path)
            : base(path)
        {
        }

        public XApi()
            : base()
        {
        }

        public override void Connect()
        {
            base.Connect();

            lock (locker)
            {

                foreach (var kv in _SubscribedInstruments)
                {
                    foreach (var kvv in kv.Value.ToList())
                    {
                        Subscribe(kvv, kv.Key);
                    }
                }

                // 做市商询价
                foreach (var kv in _SubscribedQuotes)
                {
                    foreach (var kvv in kv.Value.ToList())
                    {
                        SubscribeQuote(kvv, kv.Key);
                    }
                }
            }
        }

        public void ReqQuery(QueryType type, ReqQueryField query)
        {
            int ReqQueryField_size = Marshal.SizeOf(typeof(ReqQueryField));
            IntPtr ReqQueryField_Ptr = Marshal.AllocHGlobal(ReqQueryField_size);
            Marshal.StructureToPtr(query, ReqQueryField_Ptr, false);

            proxy.XRequest((byte)type, Handle, IntPtr.Zero, 0, 0,
                ReqQueryField_Ptr, ReqQueryField_size, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(ReqQueryField_Ptr);
        }

        protected override IntPtr OnResponse(byte type, IntPtr pApi1, IntPtr pApi2, double double1, double double2, IntPtr ptr1, int size1, IntPtr ptr2, int size2, IntPtr ptr3, int size3)
        {
            switch ((ResponseType)type)
            {
                case ResponseType.OnRspQryInstrument:
                    _OnRspQryInstrument(ptr1, size1, double1);
                    break;
                case ResponseType.OnRspQryTradingAccount:
                    _OnRspQryTradingAccount(ptr1, size1, double1);
                    break;
                case ResponseType.OnRspQryInvestorPosition:
                    _OnRspQryInvestorPosition(ptr1, size1, double1);
                    break;
                case ResponseType.OnRspQrySettlementInfo:
                    _OnRspQrySettlementInfo(ptr1, size1, double1);
                    break;
                case ResponseType.OnRspQryInvestor:
                    _OnRspQryInvestor(ptr1, size1, double1);
                    break;
                case ResponseType.OnRtnInstrumentStatus:
                    _OnRtnInstrumentStatus(ptr1, size1);
                    break;
                case ResponseType.OnRtnOrder:
                    _OnRtnOrder(ptr1, size1);
                    break;
                case ResponseType.OnRtnTrade:
                    _OnRtnTrade(ptr1, size1);
                    break;
                case ResponseType.OnRtnQuote:
                    _OnRtnQuote(ptr1, size1);
                    break;
                case ResponseType.OnRspQryOrder:
                    _OnRspQryOrder(ptr1, size1, double1);
                    break;
                case ResponseType.OnRspQryTrade:
                    _OnRspQryTrade(ptr1, size1, double1);
                    break;
                case ResponseType.OnRspQryQuote:
                    _OnRspQryQuote(ptr1, size1, double1);
                    break;
                case ResponseType.OnRtnDepthMarketData:
                    _OnRtnDepthMarketData(ptr1, size1, double1);
                    break;
                case ResponseType.OnRtnQuoteRequest:
                    _OnRtnQuoteRequest(ptr1, size1);
                    break;

                case ResponseType.OnRspQryHistoricalTicks:
                    _OnRspQryHistoricalTicks(ptr1, size1, ptr2, size2, double1);
                    break;
                case ResponseType.OnRspQryHistoricalBars:
                    _OnRspQryHistoricalBars(ptr1, size1, ptr2, size2, double1);
                    break;
                case ResponseType.OnFilterSubscribe:
                    if (_OnFilterSubscribe(double1, size1, size2, size3, ptr1))
                    {
                        return new IntPtr(1);
                    }
                    return IntPtr.Zero;
                default:
                    base.OnResponse(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
                    break;
            }

            return IntPtr.Zero;
        }
    }
}
