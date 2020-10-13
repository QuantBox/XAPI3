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
        public DelegateOnRspQryInstrument OnRspQryInstrument
        {
            get { return OnRspQryInstrument_; }
            set { OnRspQryInstrument_ = value; }
        }
        private DelegateOnRspQryInstrument OnRspQryInstrument_;
        

        private void _OnRspQryInstrument(IntPtr ptr1,int size1, double double1)
        {
            if (OnRspQryInstrument_ == null)
                return;

            InstrumentField obj = PInvokeUtility.GetObjectFromIntPtr<InstrumentField>(ptr1);

            OnRspQryInstrument_(this, ref obj, size1, double1 != 0);
        }
    }
}
