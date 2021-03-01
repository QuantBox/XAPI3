using XAPI.Callback;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

using System.IO;
using System.Threading;
using XAPI.Event;

namespace XAPI
{
    class Program
    {
        static void OnConnectionStatus(object sender, ConnectionStatus status, ref RspUserLoginField userLogin, int size1)
        {
            if (size1 > 0)
            {
                Console.WriteLine("333333" + status);
                Console.WriteLine("333333" + userLogin.ToFormattedStringLong());
            }
            else
            {
                Console.WriteLine("333333" + status);
            }
        }

        static void OnConnectionStatus2(object sender, ConnectionStatus status, ref RspUserLoginField userLogin, int size1)
        {
            if (size1 > 0)
            {
                Console.WriteLine("222222" + status);
                Console.WriteLine("222222" + userLogin.ToFormattedStringLong());
            }
            else
            {
                Console.WriteLine("222222" + status);
            }
        }

        static void OnRtnDepthMarketData(object sender, ref DepthMarketDataNClass marketData)
        {
            Debugger.Log(0, null, "CTP:C#");
            Console.WriteLine(marketData.InstrumentID);
            //Console.WriteLine(marketData.Exchange);
            Console.WriteLine(marketData.LastPrice);
            //Console.WriteLine(marketData.OpenInterest);
            if (marketData.Bids.Count() > 0)
            {
                Console.WriteLine(marketData.Bids[0].Price);
            }
            if (marketData.Asks.Count() > 0)
            {
                Console.WriteLine(marketData.Asks[0].Price);
            }
        }

        static void OnRspQryInstrument(object sender, ref InstrumentField instrument, int size1, bool bIsLast)
        {
            Console.WriteLine(instrument.InstrumentName);
        }

        static void OnRspQryTradingAccount(object sender, ref AccountField account, int size1, bool bIsLast)
        {

        }

        static void OnRspQrySettlementInfo(object sender, ref SettlementInfoClass settlementInfo, int size1, bool bIsLast)
        {
            Console.WriteLine(settlementInfo.Content);
        }

        static void OnRtnOrder(object sender, ref OrderField order)
        {
            Console.WriteLine("AA " + order.Status + order.ExecType + order.Text() + "AA " + order.ID + "AA " + order.OrderID);
        }

        static void OnRtnError(object sender, ref ErrorField error)
        {
            Console.WriteLine("BB" + error.XErrorID + error.RawErrorID + error.Text());
        }

        static void OnRtnTrade(object sender, ref TradeField trade)
        {
            Console.WriteLine("CC " + trade.Time + "CC " + trade.ID + "CC " + trade.TradeID);
        }

        static void Main(string[] args)
        {
            // 注意：为了进行进程外COM技术的支持，项目已经修改了启动对象，
            // 默认是从另一个Main中启动，如想测试此内容，请修改项目的属性，Application->Startup object
            int i;
            int iCount = args.Length;

            for (i = 0; i < iCount; i++)
            {
                Console.Write("Argument : ");
                Console.WriteLine(args[i]);
            }

            Console.WriteLine("Press [ENTER] to exit.");
            Console.ReadLine();

            //for (int i = 0; i < 10000; ++i)
            {
                test_CTP_Main2(args);
            }
            Console.ReadKey();
        }

        static IXApi api;

        static void test_CTP_Main(string[] args)
        {
            Type type = Type.GetType("XAPI.Callback.XApi, XAPI_CSharp");
            var a = (IXApi)Activator.CreateInstance(type, @"d:\Users\Kan\Documents\GitHub\XAPI3\bin\Debug64\x64\CTP_SE\CTP_SE_Quote_x64.dll");

            api = a;

            api.OnConnectionStatus = OnConnectionStatus;
            api.OnRtnDepthMarketData = OnRtnDepthMarketData;
            api.OnRspQryInstrument = OnRspQryInstrument;
            api.OnRspQrySettlementInfo = OnRspQrySettlementInfo;

            api.Connect(
                @"d:\Users\Kan\Documents\GitHub\XAPI3\bin\Debug64\x64\CTP_SE\md_1.json",
                @"d:\Users\Kan\Documents\GitHub\XAPI3\bin\Debug64\x64\CTP_SE\md_2.json",
                Path.GetTempPath());
            Thread.Sleep(3 * 1000);
            api.Subscribe("IF2103", "");

            Console.ReadKey();
            Thread.Sleep(1000 * 1000);

            api.Dispose();

            Thread.Sleep(5 * 1000);
        }

        static void test_CTP_Main2(string[] args)
        {
            Type type = Type.GetType("XAPI.Callback.XApi, XAPI_CSharp");
            var a = (IXApi)Activator.CreateInstance(type, @"d:\Users\Kan\Documents\GitHub\XAPI3\bin\Debug64\x64\CTP_SE\CTP_SE_Trade_x64.dll");

            api = a;

            api.OnConnectionStatus = OnConnectionStatus;
            api.OnRtnDepthMarketData = OnRtnDepthMarketData;
            api.OnRspQryInstrument = OnRspQryInstrument;
            api.OnRspQrySettlementInfo = OnRspQrySettlementInfo;

            api.Connect(
                @"d:\Users\Kan\Documents\GitHub\XAPI3\bin\Debug64\x64\CTP_SE\td_1.json",
                @"d:\Users\Kan\Documents\GitHub\XAPI3\bin\Debug64\x64\CTP_SE\td_2.json",
                Path.GetTempPath());
            Thread.Sleep(3 * 1000);

            Console.ReadKey();
            Thread.Sleep(1000 * 1000);

            api.Dispose();

            Thread.Sleep(5 * 1000);
        }

        static void api_OnRtnDepthMarketData(object sender, OnRtnDepthMarketDataNEventArgs e)
        {
            throw new NotImplementedException();
        }

        static void api_OnConnectionStatus(object sender, OnConnectionStatusEventArgs e)
        {
            throw new NotImplementedException();
        }

        static void test_CTP_COM_Main(string[] args)
        {
            //var api = new XAPI.COM.XApiCom();

            ////Type type = Type.GetType("XAPI.Callback.XApi, XAPI_CSharp");
            ////var a = (IXApi)Activator.CreateInstance(type, @"C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\x86\CTP\CTP_Quote_x86.dll");

            ////api = a;

            ////api.Server.BrokerID = "9999";
            ////api.Server.Address = "tcp://180.168.146.187:10010";
            ////api.Server.PrivateTopicResumeType = ResumeType.Undefined;

            ////api.User.UserID = "037505";
            ////api.User.Password = "123456";

            ////api.OnConnectionStatus = OnConnectionStatus;
            ////api.OnRtnDepthMarketData = OnRtnDepthMarketData;
            ////api.OnRspQryInstrument = OnRspQryInstrument;
            ////api.OnRspQrySettlementInfo = OnRspQrySettlementInfo;

            //api.Connect();
            //Thread.Sleep(3 * 1000);
            //api.Subscribe("IF000;IF_WI;IF_IH_1803;IF888", "");

            //Console.ReadKey();
            //Thread.Sleep(1000 * 1000);

            //api.Dispose();

            //Thread.Sleep(5 * 1000);
        }
    }
}
