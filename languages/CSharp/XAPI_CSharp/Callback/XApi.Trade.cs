using XAPI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace XAPI.Callback
{
    public partial class XApi
    {
        public DelegateOnRtnOrder OnRtnOrder
        {
            get { return OnRtnOrder_; }
            set { OnRtnOrder_ = value; }
        }
        public DelegateOnRtnTrade OnRtnTrade
        {
            get { return OnRtnTrade_; }
            set { OnRtnTrade_ = value; }
        }
        public DelegateOnRtnQuote OnRtnQuote
        {
            get { return OnRtnQuote_; }
            set { OnRtnQuote_ = value; }
        }

        private DelegateOnRtnOrder OnRtnOrder_;
        private DelegateOnRtnTrade OnRtnTrade_;
        private DelegateOnRtnQuote OnRtnQuote_;


        public string SendOrder(OrderField order)
        {
            OrderField[] orders = new OrderField[1];
            orders[0] = order;
            return SendOrder(orders);
        }

        public string CancelOrder(string szId)
        {
            string[] szIds = new string[1];
            szIds[0] = szId;
            return CancelOrder(szIds);
        }

        public string SendOrder(OrderField[] orders)
        {
            int OrderField_size = Marshal.SizeOf(typeof(OrderField));
            int OrderIDType_size = Marshal.SizeOf(typeof(OrderIDType));
            
            IntPtr OrderField_Ptr = Marshal.AllocHGlobal(OrderField_size * orders.Length);
            IntPtr OrderIDType_Ptr = Marshal.AllocHGlobal(OrderIDType_size * orders.Length);

            // 将结构体写成内存块
            for (int i = 0; i < orders.Length;++i)
            {
                Marshal.StructureToPtr(orders[i], new IntPtr(OrderField_Ptr.ToInt64() + i * OrderField_size), false);
            }

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqOrderInsert, Handle, IntPtr.Zero,
                0, 0,
                OrderField_Ptr, orders.Length, OrderIDType_Ptr, 0, IntPtr.Zero, 0);

            string ret = Marshal.PtrToStringAnsi(ptr);

            Marshal.FreeHGlobal(OrderField_Ptr);
            Marshal.FreeHGlobal(OrderIDType_Ptr);

            return ret;
        }

        public string CancelOrder(string[] szIds)
        {
            int OrderIDType_size = Marshal.SizeOf(typeof(OrderIDType));

            IntPtr Input_Ptr = Marshal.AllocHGlobal(OrderIDType_size * szIds.Length);
            IntPtr Output_Ptr = Marshal.AllocHGlobal(OrderIDType_size * szIds.Length);

            // 将结构体写成内存块
            for (int i = 0; i < szIds.Length; ++i)
            {
                OrderIDType _szId = new OrderIDType();
                _szId.ID = szIds[i];
                Marshal.StructureToPtr(_szId, new IntPtr(Input_Ptr.ToInt64() + i * OrderIDType_size), false);
            }

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqOrderAction, Handle, IntPtr.Zero, 0, 0,
                Input_Ptr, szIds.Length, Output_Ptr, 0, IntPtr.Zero, 0);

            string ret = Marshal.PtrToStringAnsi(ptr);

            Marshal.FreeHGlobal(Input_Ptr);
            Marshal.FreeHGlobal(Output_Ptr);

            return ret;
        }

        public string SendQuote(QuoteField quote)
        {
            int QuoteField_size = Marshal.SizeOf(typeof(QuoteField));
            int OrderIDType_size = Marshal.SizeOf(typeof(OrderIDType));

            IntPtr QuoteField_Ptr = Marshal.AllocHGlobal(QuoteField_size);
            IntPtr OrderIDType_Ptr = Marshal.AllocHGlobal(OrderIDType_size * 3);

            // 将结构体写成内存块
            for (int i = 0; i < 1; ++i)
            {
                Marshal.StructureToPtr(quote, new IntPtr(QuoteField_Ptr.ToInt64() + i * QuoteField_size), false);
            }

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqQuoteInsert, Handle, IntPtr.Zero,
                0, 0,
                QuoteField_Ptr, 1, OrderIDType_Ptr, 0, IntPtr.Zero, 0);

            string ret = Marshal.PtrToStringAnsi(ptr);

            Marshal.FreeHGlobal(QuoteField_Ptr);
            Marshal.FreeHGlobal(OrderIDType_Ptr);

            return ret;
        }

        public string CancelQuote(string szId)
        {
            IntPtr szIdPtr = Marshal.StringToHGlobalAnsi(szId);
            int OrderIDType_size = Marshal.SizeOf(typeof(OrderIDType));
            IntPtr OrderIDType_Ptr = Marshal.AllocHGlobal(OrderIDType_size);

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqQuoteAction, Handle, IntPtr.Zero, 0, 0,
                szIdPtr, 1, OrderIDType_Ptr, 0, IntPtr.Zero, 0);

            string ret = Marshal.PtrToStringAnsi(ptr);

            Marshal.FreeHGlobal(szIdPtr);
            Marshal.FreeHGlobal(OrderIDType_Ptr);

            return ret;
        }

        private void _OnRtnOrder(IntPtr ptr1, int size1)
        {
            // 求快，不加
            if (OnRtnOrder_ == null)
                return;

            OrderField obj = (OrderField)Marshal.PtrToStructure(ptr1, typeof(OrderField));

            OnRtnOrder_(this, ref obj);
        }

        private void _OnRtnTrade(IntPtr ptr1, int size1)
        {
            // 求快，不加
            if (OnRtnTrade_ == null)
                return;

            TradeField obj = (TradeField)Marshal.PtrToStructure(ptr1, typeof(TradeField));

            OnRtnTrade_(this, ref obj);
        }

        private void _OnRtnQuote(IntPtr ptr1, int size1)
        {
            if (OnRtnQuote_ == null)
                return;

            QuoteField obj = (QuoteField)Marshal.PtrToStructure(ptr1, typeof(QuoteField));

            OnRtnQuote_(this, ref obj);
        }
    }
}
