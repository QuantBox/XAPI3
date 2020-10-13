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
        public DelegateOnRspQryHistoricalTicks OnRspQryHistoricalTicks { get; set; }
        public DelegateOnRspQryHistoricalBars OnRspQryHistoricalBars { get; set; }

        private void _OnRspQryHistoricalTicks(IntPtr ptr1, int size1, IntPtr ptr2, int size2, double double1)
        {
            if (OnRspQryHistoricalTicks == null)
                return;

            HistoricalDataRequestField obj = PInvokeUtility.GetObjectFromIntPtr<HistoricalDataRequestField>(ptr2);

            OnRspQryHistoricalTicks(this, ptr1, size1,ref obj, size2, double1 != 0);
        }
        private void _OnRspQryHistoricalBars(IntPtr ptr1, int size1, IntPtr ptr2, int size2, double double1)
        {
            if (OnRspQryHistoricalBars == null)
                return;

            HistoricalDataRequestField obj = PInvokeUtility.GetObjectFromIntPtr<HistoricalDataRequestField>(ptr2);

            OnRspQryHistoricalBars(this, ptr1, size1,ref obj, size2, double1 != 0);
        }
    }
}
