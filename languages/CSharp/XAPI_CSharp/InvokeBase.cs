using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace XAPI
{
    [ComVisible(false)]
    public class InvokeBase
    {
        protected IntPtr hLib = IntPtr.Zero;
        public virtual Delegate Invoke(String APIName, Type t)
        {
            return null;
        }

        public virtual void Dispose()
        {

        }
    }
}
