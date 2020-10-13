using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XAPI
{
    public class XApiHelper
    {
        public static IXApi CreateInstance(string typeName = "XAPI.Callback.XApi, XAPI_CSharp", string c_dll_path = "")
        {
            //Type type = Type.GetType("XAPI.Callback.XApi, XAPI_CSharp");
            //var api = (IXApi)Activator.CreateInstance(type, @"C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Trade_x86.dll");
            //return api;
            Type type = Type.GetType(typeName);
            if (type == null)
                return null;
            IXApi api = null;
            if (string.IsNullOrEmpty(c_dll_path))
            {
                api = (IXApi)Activator.CreateInstance(type);
            }
            else
            {
                api = (IXApi)Activator.CreateInstance(type, c_dll_path);
            }
            return api;
        }
    }
}
