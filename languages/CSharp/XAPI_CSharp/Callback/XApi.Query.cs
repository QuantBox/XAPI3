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
        public DelegateOnRspQryTradingAccount OnRspQryTradingAccount
        {
            get { return OnRspQryTradingAccount_; }
            set { OnRspQryTradingAccount_ = value; }
        }
        public DelegateOnRspQryInvestorPosition OnRspQryInvestorPosition
        {
            get { return OnRspQryInvestorPosition_; }
            set { OnRspQryInvestorPosition_ = value; }
        }
        public DelegateOnRspQrySettlementInfo OnRspQrySettlementInfo
        {
            get { return OnRspQrySettlementInfo_; }
            set { OnRspQrySettlementInfo_ = value; }
        }
        public DelegateOnRspQryInvestor OnRspQryInvestor
        {
            get { return OnRspQryInvestor_; }
            set { OnRspQryInvestor_ = value; }
        }

        public DelegateOnRspQryOrder OnRspQryOrder
        {
            get { return OnRspQryOrder_; }
            set { OnRspQryOrder_ = value; }
        }

        public DelegateOnRspQryTrade OnRspQryTrade
        {
            get { return OnRspQryTrade_; }
            set { OnRspQryTrade_ = value; }
        }
        public DelegateOnRspQryQuote OnRspQryQuote
        {
            get { return OnRspQryQuote_; }
            set { OnRspQryQuote_ = value; }
        }

        private DelegateOnRspQryTradingAccount OnRspQryTradingAccount_;
        private DelegateOnRspQryInvestorPosition OnRspQryInvestorPosition_;
        private DelegateOnRspQrySettlementInfo OnRspQrySettlementInfo_;
        private DelegateOnRspQryInvestor OnRspQryInvestor_;
        private DelegateOnRspQryOrder OnRspQryOrder_;
        private DelegateOnRspQryTrade OnRspQryTrade_;
        private DelegateOnRspQryQuote OnRspQryQuote_;

        private Dictionary<string, StringBuilder> dict = new Dictionary<string, StringBuilder>();

        private void _OnRspQryTradingAccount(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryTradingAccount_ == null)
                return;

            AccountField obj = PInvokeUtility.GetObjectFromIntPtr<AccountField>(ptr1);

            OnRspQryTradingAccount_(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQryInvestorPosition(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryInvestorPosition_ == null)
                return;

            PositionField obj = PInvokeUtility.GetObjectFromIntPtr<PositionField>(ptr1);

            OnRspQryInvestorPosition_(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQrySettlementInfo(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQrySettlementInfo_ == null)
                return;

            SettlementInfoClass cls = PInvokeUtility.GetSettlementInfoClass(ptr1);
            OnRspQrySettlementInfo_(this, ref cls, size1, double1 != 0);
        }

        private void _OnRspQryInvestor(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryInvestor_ == null)
                return;

            InvestorField obj = PInvokeUtility.GetObjectFromIntPtr<InvestorField>(ptr1);

            OnRspQryInvestor_(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQryOrder(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryOrder_ == null)
                return;

            OrderField obj = PInvokeUtility.GetObjectFromIntPtr<OrderField>(ptr1);

            OnRspQryOrder_(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQryTrade(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryTrade_ == null)
                return;

            TradeField obj = PInvokeUtility.GetObjectFromIntPtr<TradeField>(ptr1);

            OnRspQryTrade_(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQryQuote(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryQuote_ == null)
                return;

            QuoteField obj = PInvokeUtility.GetObjectFromIntPtr<QuoteField>(ptr1);

            OnRspQryQuote_(this, ref obj, size1, double1 != 0);
        }
    }
}
