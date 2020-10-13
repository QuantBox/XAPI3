using Ideafixxxer.Generics;
using XAPI.Callback;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
//using System.EnterpriseServices;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;

namespace XAPI.COM
{
    [ComVisible(true)]
    [Guid("825E3182-8444-4580-8A8C-965485FBF451")]
    [ClassInterface(ClassInterfaceType.None)]
    [ComSourceInterfaces(typeof(IXApiEvents))]
    [ProgId("XAPI.COM")]
    //[EventTrackingEnabled(true)]
    //[Description("Interface Serviced Component")]
    public class XApiCom : UserControl,/*ServicedComponent,*/ IXApi
        //,IObjectSafety // implement IObjectSafety to supress the unsafe for scripting 
                       // warning message
    {
        #region Constants
        // Constants for implementation of the IObjectSafety interface.
        private const int INTERFACESAFE_FOR_UNTRUSTED_CALLER = 0x00000001;
        private const int INTERFACESAFE_FOR_UNTRUSTED_DATA = 0x00000002;
        private Label label1;
        private const int S_OK = 0;
        #endregion

        #region IObjectSafety Methods
        public int GetInterfaceSafetyOptions(ref Guid riid, out int pdwSupportedOptions, out int pdwEnabledOptions)
        {
            pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
            pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
            return S_OK;   // return S_OK
        }

        public int SetInterfaceSafetyOptions(ref Guid riid, int dwOptionSetMask, int dwEnabledOptions)
        {
            return S_OK;   // return S_OK
        }
        #endregion

        public event DelegateOnConnectionStatus OnConnectionStatus;
        public event DelegateOnRtnError OnRtnError;
        public event DelegateOnLog OnLog;
        public event DelegateOnRtnDepthMarketData OnRtnDepthMarketData;
        public event DelegateOnRspQryInstrument OnRspQryInstrument;
        public event DelegateOnRspQryTradingAccount OnRspQryTradingAccount;
        public event DelegateOnRspQryInvestorPosition OnRspQryInvestorPosition;
        public event DelegateOnRspQrySettlementInfo OnRspQrySettlementInfo;
        public event DelegateOnRtnOrder OnRtnOrder;
        public event DelegateOnRtnTrade OnRtnTrade;
        public event DelegateOnRspQryOrder OnRspQryOrder;
        public event DelegateOnRspQryTrade OnRspQryTrade;

        private readonly ConcurrentQueue<QueueData> MessageQueue;

        private readonly XApi api;
        private ServerInfoField _Server = new ServerInfoField();
        private UserInfoField _User = new UserInfoField();
        private OrderField _Order;
        private ReqQueryField _Query;

        //private static int cookie;
        //private static RegistrationServices msRegSvc = new RegistrationServices();

        // http://www.codeproject.com/Articles/24089/Create-ActiveX-in-NET-Step-by-Step
        // http://www.codeproject.com/Articles/1256/Exposing-Windows-Forms-Controls-as-ActiveX-control
        ///<summary>
        ///Register the class as a control and set its CodeBase entry
        ///</summary>
        ///<param name="key">The registry key of the control</param>
        [ComRegisterFunction()]
        public static void RegisterClass(string key)
        {
            // Strip off HKEY_CLASSES_ROOT\ from the passed key as I don't need it
            StringBuilder sb = new StringBuilder(key);
            sb.Replace(@"HKEY_CLASSES_ROOT\", "");

            // Open the CLSID\{guid} key for write access
            RegistryKey k = Registry.ClassesRoot.OpenSubKey(sb.ToString(), true);

            // And create the 'Control' key - this allows it to show up in 
            // the ActiveX control container 
            RegistryKey ctrl = k.CreateSubKey("Control");
            ctrl.Close();

            // Next create the CodeBase entry - needed if not string named and GACced.
            RegistryKey inprocServer32 = k.OpenSubKey("InprocServer32", true);
            inprocServer32.SetValue("CodeBase", Assembly.GetExecutingAssembly().CodeBase);
            inprocServer32.Close();

            // 是否有必要将InprocServer32给删了来支持进程外呢？
            RegistryKey localServer32 = k.CreateSubKey("LocalServer32");
            localServer32.SetValue("", Assembly.GetExecutingAssembly().Location);
            inprocServer32.Close();

            // Finally close the main key
            k.Close();
        }

        ///<summary>
        ///Called to unregister the control
        ///</summary>
        ///<param name="key">The registry key</param>
        [ComUnregisterFunction()]
        public static void UnregisterClass(string key)
        {
            StringBuilder sb = new StringBuilder(key);
            sb.Replace(@"HKEY_CLASSES_ROOT\", "");

            // Open HKCR\CLSID\{guid} for write access
            RegistryKey k = Registry.ClassesRoot.OpenSubKey(sb.ToString(), true);

            // Delete the 'Control' key, but don't throw an exception if it does not exist
            k.DeleteSubKey("Control", false);

            // Next open up InprocServer32
            RegistryKey inprocServer32 = k.OpenSubKey("InprocServer32", true);

            // And delete the CodeBase key, again not throwing if missing 
            k.DeleteSubKey("CodeBase", false);

            // Finally close the main key 
            k.Close();
        }

        public XApiCom()
        {
            MessageQueue = new ConcurrentQueue<QueueData>();

            api = new XApi();

            api.OnConnectionStatus = OnConnectionStatus_callback;
            api.OnRtnError = OnRtnError_callback;
            api.OnLog = OnLog_callback;

            api.OnRtnDepthMarketData = OnRtnDepthMarketData_callback;
            //base.OnRtnQuoteRequest = OnRtnQuoteRequest_callback;

            api.OnRspQryInstrument = OnRspQryInstrument_callback;
            api.OnRspQryTradingAccount = OnRspQryTradingAccount_callback;
            api.OnRspQryInvestorPosition = OnRspQryInvestorPosition_callback;
            api.OnRspQrySettlementInfo = OnRspQrySettlementInfo_callback;

            api.OnRspQryOrder = OnRspQryOrder_callback;
            api.OnRspQryTrade = OnRspQryTrade_callback;
            //base.OnRspQryQuote = OnRspQryQuote_callback;

            api.OnRtnOrder = OnRtnOrder_callback;
            api.OnRtnTrade = OnRtnTrade_callback;
            //base.OnRtnQuote = OnRtnQuote_callback;

            //base.OnRspQryHistoricalTicks = OnRspQryHistoricalTicks_callback;
            //base.OnRspQryHistoricalBars = OnRspQryHistoricalBars_callback;

            //base.OnRspQryInvestor = OnRspQryInvestor_callback;

            InitializeComponent();


            Console.WriteLine("ReferenceCountedObjectBase contructor.");
            // We increment the global count of objects.
            ManagedCOMLocalServer.InterlockedIncrementObjectsCount();
        }

        ~XApiCom()
        {
            Console.WriteLine("ReferenceCountedObjectBase destructor.");
            // We decrement the global count of objects.
            ManagedCOMLocalServer.InterlockedDecrementObjectsCount();
            // We then immediately test to see if we the conditions
            // are right to attempt to terminate this server application.
            ManagedCOMLocalServer.AttemptToTerminateServer();
        }

        public void SetLibPath(string LibPath)
        {
            api.LibPath = LibPath;
        }

        public void SetServerInfo(string key, object value)
        {
            Type type = typeof(ServerInfoField);
            FieldInfo field = type.GetField(key,BindingFlags.Public | BindingFlags.Instance);
            if (field == null)
            {
                throw new ArgumentException(key +" is not exist!", key);
            }
            try
            {
                object obj = Helper.ChangeType(value, field.FieldType);
                field.SetValue(_Server, obj);
            }
            catch(Exception ex)
            {
                throw new InvalidCastException(ex.Message);
            }
        }

        public void SetUserInfo(string key, object value)
        {
            Type type = typeof(UserInfoField);
            FieldInfo field = type.GetField(key, BindingFlags.Public | BindingFlags.Instance);
            if (field == null)
            {
                throw new ArgumentException(key + " is not exist!", key);
            }
            try
            {
                object obj = Helper.ChangeType(value, field.FieldType);
                field.SetValue(_User, obj);
            }
            catch (Exception ex)
            {
                throw new InvalidCastException(ex.Message);
            }
        }

        public void Connect()
        {
            api.Server = _Server;
            api.User = _User;
            api.Connect();
        }

        public void Disconnect()
        {
            api.Disconnect();
        }

        public void Subscribe(string szInstrument, string szExchange)
        {
            api.Subscribe(szInstrument, szExchange);
        }

        public void Unsubscribe(string szInstrument, string szExchange)
        {
            api.Unsubscribe(szInstrument, szExchange);
        }

        public void NewOrder()
        {
            _Order = new OrderField();
        }
        public void SetOrder(string key, object value)
        {
            Type type = typeof(OrderField);
            FieldInfo field = type.GetField(key, BindingFlags.Public | BindingFlags.Instance);
            if (field == null)
            {
                throw new ArgumentException(key + " is not exist!", key);
            }
            try
            {
                object obj = Helper.ChangeType(value, field.FieldType);
                field.SetValue(_Order, obj);
            }
            catch (Exception ex)
            {
                throw new InvalidCastException(ex.Message);
            }
        }

        public string SendOrder()
        {
            return api.SendOrder(_Order);
        }
        public string CancelOrder(string id)
        {
            return api.CancelOrder(id);
        }

        public void NewQuery()
        {
            _Query = new ReqQueryField();
        }
        public void SetQuery(string key, object value)
        {
            Type type = typeof(ReqQueryField);
            FieldInfo field = type.GetField(key, BindingFlags.Public | BindingFlags.Instance);
            if (field == null)
            {
                throw new ArgumentException(key + " is not exist!", key);
            }
            try
            {
                object obj = Helper.ChangeType(value, field.FieldType);
                field.SetValue(_Query, obj);
            }
            catch (Exception ex)
            {
                throw new InvalidCastException(ex.Message);
            }
        }

        public void ReqQuery(string type)
        {
            QueryType obj = Enum<QueryType>.Parse(type);
            api.ReqQuery(obj, _Query);
        }

        public QueueData TryDequeue()
        {
            QueueData qd;
            if (MessageQueue.TryDequeue(out qd))
            {
                return qd;
            }
            return null;
        }

        /// <summary>
        /// 给COM使用，进行内存回收
        /// </summary>
        public void GCCollect()
        {
            api.GCCollect();
        }

        private void OnConnectionStatus_callback(object sender, XAPI.ConnectionStatus status, [In] ref RspUserLoginField userLogin, int size1)
        {
            RspUserLoginClass cls = new RspUserLoginClass();

            if (size1 > 0)
            {
                RspUserLoginField field = userLogin;

                cls.TradingDay = field.TradingDay;
                cls.LoginTime = field.LoginTime;
                cls.SessionID = field.SessionID;
                cls.UserID = field.UserID;
                cls.AccountID = field.AccountID;
                cls.InvestorName = field.InvestorName();
                cls.XErrorID = field.XErrorID;
                cls.RawErrorID = field.RawErrorID;
                cls.Text = field.Text();
            }

            if (null == OnConnectionStatus)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnConnectionStatus;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnConnectionStatus);
                qd.Sender = this;
                qd.Data1 = status;
                qd.Data2 = Enum<XAPI.ConnectionStatus>.ToString(status);
                qd.Data3 = cls;
                qd.Data4 = size1;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnConnectionStatus(this, (int)status, Enum<XAPI.ConnectionStatus>.ToString(status), cls, size1);
            }
        }

        private void OnRtnError_callback(object sender, [In] ref ErrorField error)
        {
            ErrorClass cls = null;

            cls = new ErrorClass();
            ErrorField field = error;

            cls.XErrorID = field.XErrorID;
            cls.RawErrorID = field.RawErrorID;
            cls.Text = field.Text();
            cls.Source = field.Source;

            if (null == OnRtnError)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRtnError;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRtnError);
                qd.Sender = this;
                qd.Data1 = cls;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRtnError(this, cls);
            }
        }

        private void OnLog_callback(object sender, [In] ref LogField log)
        {
            LogClass cls = null;

            cls = new LogClass();
            LogField field = log;

            cls.Level = (int)field.Level;
            cls.Level_String = Enum<XAPI.LogLevel>.ToString(field.Level);
            cls.Message = field.Message();

            if (null == OnLog)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnLog;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnLog);
                qd.Sender = this;
                qd.Data1 = cls;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnLog(this, cls);
            }
        }

        private void OnRtnDepthMarketData_callback(object sender, [In] ref XAPI.DepthMarketDataNClass marketData)
        {
            DepthMarketDataNClass cls = new DepthMarketDataNClass();
            XAPI.DepthMarketDataNClass field = marketData;

            cls.TradingDay = field.TradingDay;
            cls.ActionDay = field.ActionDay;
            cls.UpdateTime = field.UpdateTime;
            cls.UpdateMillisec = field.UpdateMillisec;
            cls.Exchange = (int)field.Exchange;
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.LastPrice = field.LastPrice;
            cls.Volume = field.Volume;
            cls.Turnover = field.Turnover;
            cls.OpenInterest = field.OpenInterest;
            cls.AveragePrice = field.AveragePrice;
            cls.OpenPrice = field.OpenPrice;
            cls.HighestPrice = field.HighestPrice;
            cls.LowestPrice = field.LowestPrice;
            cls.ClosePrice = field.ClosePrice;
            cls.SettlementPrice = field.SettlementPrice;
            cls.UpperLimitPrice = field.UpperLimitPrice;
            cls.LowerLimitPrice = field.LowerLimitPrice;
            cls.PreClosePrice = field.PreClosePrice;
            cls.PreSettlementPrice = field.PreSettlementPrice;
            cls.PreOpenInterest = field.PreOpenInterest;
            cls.TradingPhase = (int)field.TradingPhase;
            cls.TradingPhase_String = Enum<XAPI.TradingPhaseType>.ToString(field.TradingPhase);

            if (field.Bids != null && field.Bids.Length > 0)
            {
                cls.BidPrice1 = field.Bids[0].Price;
                cls.BidSize = field.Bids[0].Size;
            }

            if (field.Asks != null && field.Asks.Length > 0)
            {
                cls.AskPrice1 = field.Asks[0].Price;
                cls.AskSize = field.Asks[0].Size;
            }

            if (null == OnRtnDepthMarketData)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRtnDepthMarketData;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRtnDepthMarketData);
                qd.Sender = this;
                qd.Data1 = cls;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRtnDepthMarketData(this, cls);
            }
        }

        private void OnRtnOrder_callback(object sender, [In] ref OrderField order)
        {
            OrderField field = order;

            OrderClass cls = new OrderClass();

            cls.InstrumentName = field.InstrumentName();
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;
            cls.Side = (int)field.Side;
            cls.Side_String = Enum<XAPI.OrderSide>.ToString(field.Side);
            cls.Qty = field.Qty;
            cls.Price = field.Price;
            cls.OpenClose = (int)field.OpenClose;
            cls.OpenClose_String = Enum<XAPI.OpenCloseType>.ToString(field.OpenClose);
            cls.HedgeFlag = (int)field.HedgeFlag;
            cls.HedgeFlag_String = Enum<XAPI.HedgeFlagType>.ToString(field.HedgeFlag);
            cls.Date = field.Date;
            cls.Time = field.Time;
            cls.ID = field.ID;
            cls.OrderID = field.OrderID;
            cls.LocalID = field.LocalID;
            cls.Type = (int)field.Type;
            cls.Type_String = Enum<XAPI.OrderType>.ToString(field.Type);
            cls.StopPx = field.StopPx;
            cls.TimeInForce = (int)field.TimeInForce;
            cls.TimeInForce_String = Enum<XAPI.TimeInForce>.ToString(field.TimeInForce);
            cls.Status = (int)field.Status;
            cls.Status_String = Enum<XAPI.OrderStatus>.ToString(field.Status);
            cls.ExecType = (int)field.ExecType;
            cls.ExecType_String = Enum<XAPI.ExecType>.ToString(field.ExecType);
            cls.LeavesQty = field.LeavesQty;
            cls.CumQty = field.CumQty;
            cls.AvgPx = field.AvgPx;
            cls.XErrorID = field.XErrorID;
            cls.RawErrorID = field.RawErrorID;
            cls.Text = field.Text();
            cls.ReserveInt32 = field.ReserveInt32;
            cls.ReserveChar64 = field.ReserveChar64;

            cls.PortfolioID1 = field.PortfolioID1;
            cls.PortfolioID2 = field.PortfolioID2;
            cls.PortfolioID3 = field.PortfolioID3;
            cls.Business = (int)field.Business;
            cls.Business_String = Enum<XAPI.BusinessType>.ToString(field.Business);

            if (null == OnRtnOrder)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRtnOrder;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRtnOrder);
                qd.Sender = this;
                qd.Data1 = cls;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRtnOrder(this, cls);
            }
        }

        private void OnRtnTrade_callback(object sender, [In] ref TradeField trade)
        {
            if (null == OnRtnTrade)
                return;

            TradeField field = trade;

            TradeClass cls = new TradeClass();

            cls.InstrumentName = field.InstrumentName();
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;
            cls.Side = (int)field.Side;
            cls.Side_String = Enum<XAPI.OrderSide>.ToString(field.Side);
            cls.Qty = field.Qty;
            cls.Price = field.Price;
            cls.OpenClose = (int)field.OpenClose;
            cls.OpenClose_String = Enum<XAPI.OpenCloseType>.ToString(field.OpenClose);
            cls.HedgeFlag = (int)field.HedgeFlag;
            cls.HedgeFlag_String = Enum<XAPI.HedgeFlagType>.ToString(field.HedgeFlag);
            cls.Date = field.Date;
            cls.Time = field.Time;
            cls.ID = field.ID;
            cls.TradeID = field.TradeID;
            cls.ReserveInt32 = field.ReserveInt32;
            cls.ReserveChar64 = field.ReserveChar64;

            cls.PortfolioID1 = field.PortfolioID1;
            cls.PortfolioID2 = field.PortfolioID2;
            cls.PortfolioID3 = field.PortfolioID3;
            cls.Business = (int)field.Business;
            cls.Business_String = Enum<XAPI.BusinessType>.ToString(field.Business);

            if (null == OnRtnTrade)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRtnTrade;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRtnTrade);
                qd.Sender = this;
                qd.Data1 = cls;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRtnTrade(this, cls);
            }
        }

        private void OnRspQryInstrument_callback(object sender, [In] ref InstrumentField instrument, int size1, bool bIsLast)
        {
            if (null == OnRspQryInstrument)
                return;

            InstrumentField field = instrument;

            InstrumentClass cls = new InstrumentClass();

            cls.InstrumentName = field.InstrumentName();
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;

            cls.ExchangeInstID = field.ExchangeInstID;

            cls.Type = (int)field.Type;
            cls.Type_String = Enum<XAPI.InstrumentType>.ToString(field.Type);

            cls.VolumeMultiple = field.VolumeMultiple;
            cls.PriceTick = field.PriceTick;
            cls.ExpireDate = field.ExpireDate;
            cls.StrikePrice = field.StrikePrice;

            cls.OptionsType = (int)field.OptionsType;
            cls.OptionsType_String = Enum<XAPI.PutCall>.ToString(field.OptionsType);

            cls.ProductID = field.ProductID;
            cls.UnderlyingInstrID = field.UnderlyingInstrID;

            cls.InstLifePhase = (int)field.InstLifePhase;
            cls.InstLifePhase_String = Enum<XAPI.InstLifePhaseType>.ToString(field.InstLifePhase);

            if (null == OnRspQryInstrument)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRspQryInstrument;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRspQryInstrument);
                qd.Sender = this;
                qd.Data1 = cls;
                qd.Data2 = size1;
                qd.Data3 = bIsLast;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRspQryInstrument(this, cls, size1, bIsLast);
            }
        }

        private void OnRspQryInvestorPosition_callback(object sender, [In] ref PositionField position, int size1, bool bIsLast)
        {
            if (null == OnRspQryInvestorPosition)
                return;

            PositionField field = position;

            PositionClass cls = new PositionClass();

            cls.InstrumentName = field.InstrumentName();
            cls.Symbol = field.Symbol;
            cls.InstrumentID = field.InstrumentID;
            cls.ExchangeID = field.ExchangeID;
            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;

            cls.Side = (int)field.Side;
            cls.Side_String = Enum<XAPI.PositionSide>.ToString(field.Side);
            cls.HedgeFlag = (int)field.HedgeFlag;
            cls.HedgeFlag_String = Enum<XAPI.HedgeFlagType>.ToString(field.HedgeFlag);

            cls.Date = field.Date;
            cls.PositionCost = field.PositionCost;
            cls.Position = field.Position;
            cls.TodayPosition = field.TodayPosition;
            cls.HistoryPosition = field.HistoryPosition;
            cls.HistoryFrozen = field.HistoryFrozen;
            cls.TodayBSPosition = field.TodayBSPosition;
            cls.TodayBSFrozen = field.TodayBSFrozen;
            cls.TodayPRPosition = field.TodayPRPosition;
            cls.TodayPRFrozen = field.TodayPRFrozen;
            cls.ID = field.ID;

            cls.PortfolioID1 = field.PortfolioID1;
            cls.PortfolioID2 = field.PortfolioID2;
            cls.PortfolioID3 = field.PortfolioID3;
            cls.Business = (int)field.Business;
            cls.Business_String = Enum<XAPI.BusinessType>.ToString(field.Business);


            if (null == OnRspQryInvestorPosition)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRspQryInvestorPosition;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRspQryInvestorPosition);
                qd.Sender = this;
                qd.Data1 = cls;
                qd.Data2 = size1;
                qd.Data3 = bIsLast;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRspQryInvestorPosition(this, cls, size1, bIsLast);
            }
        }

        private void OnRspQrySettlementInfo_callback(object sender, [In] ref XAPI.SettlementInfoClass settlementInfo, int size1, bool bIsLast)
        {
            if (null == OnRspQrySettlementInfo)
                return;

            XAPI.SettlementInfoClass field = settlementInfo;

            XAPI.COM.SettlementInfoClass cls = new XAPI.COM.SettlementInfoClass();
            cls.TradingDay = field.TradingDay;
            cls.Content = field.Content;

            if (null == OnRspQrySettlementInfo)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRspQrySettlementInfo;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRspQrySettlementInfo);
                qd.Sender = this;
                qd.Data1 = cls;
                qd.Data2 = size1;
                qd.Data3 = bIsLast;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRspQrySettlementInfo(this, cls, size1, bIsLast);
            }
        }


        private void OnRspQryTradingAccount_callback(object sender, [In] ref AccountField account, int size1, bool bIsLast)
        {
            if (null == OnRspQryTradingAccount)
                return;

            AccountField field = account;

            AccountClass cls = new AccountClass();

            cls.ClientID = field.ClientID;
            cls.AccountID = field.AccountID;
            cls.CurrencyID = field.CurrencyID;

            cls.PreBalance = field.PreBalance;
            cls.CurrMargin = field.CurrMargin;
            cls.CloseProfit = field.CloseProfit;
            cls.PositionProfit = field.PositionProfit;
            cls.Balance = field.Balance;
            cls.Available = field.Available;
            cls.Deposit = field.Deposit;
            cls.Withdraw = field.Withdraw;
            cls.WithdrawQuota = field.WithdrawQuota;
            cls.FrozenTransferFee = field.FrozenTransferFee;
            cls.FrozenStampTax = field.FrozenStampTax;
            cls.FrozenCommission = field.FrozenCommission;
            cls.FrozenCash = field.FrozenCash;
            cls.StampTax = field.StampTax;
            cls.Commission = field.Commission;
            cls.CashIn = field.CashIn;

            if (null == OnRspQryTradingAccount)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRspQryTradingAccount;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRspQryTradingAccount);
                qd.Sender = this;
                qd.Data1 = cls;
                qd.Data2 = size1;
                qd.Data3 = bIsLast;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRspQryTradingAccount(this, cls, size1, bIsLast);
            }
        }

        private void OnRspQryOrder_callback(object sender, [In] ref OrderField order, int size1, bool bIsLast)
        {
            if (null == OnRspQryOrder)
                return;

            OrderClass cls = new OrderClass();

            if (size1 > 0)
            {
                OrderField field = order;
                
                cls.InstrumentName = field.InstrumentName();
                cls.Symbol = field.Symbol;
                cls.InstrumentID = field.InstrumentID;
                cls.ExchangeID = field.ExchangeID;
                cls.ClientID = field.ClientID;
                cls.AccountID = field.AccountID;
                cls.Side = (int) field.Side;
                cls.Side_String = Enum<XAPI.OrderSide>.ToString(field.Side);
                cls.Qty = field.Qty;
                cls.Price = field.Price;
                cls.OpenClose = (int) field.OpenClose;
                cls.OpenClose_String = Enum<XAPI.OpenCloseType>.ToString(field.OpenClose);
                cls.HedgeFlag = (int) field.HedgeFlag;
                cls.HedgeFlag_String = Enum<XAPI.HedgeFlagType>.ToString(field.HedgeFlag);
                cls.Date = field.Date;
                cls.Time = field.Time;
                cls.ID = field.ID;
                cls.OrderID = field.OrderID;
                cls.LocalID = field.LocalID;
                cls.Type = (int) field.Type;
                cls.Type_String = Enum<XAPI.OrderType>.ToString(field.Type);
                cls.StopPx = field.StopPx;
                cls.TimeInForce = (int) field.TimeInForce;
                cls.TimeInForce_String = Enum<XAPI.TimeInForce>.ToString(field.TimeInForce);
                cls.Status = (int) field.Status;
                cls.Status_String = Enum<XAPI.OrderStatus>.ToString(field.Status);
                cls.ExecType = (int) field.ExecType;
                cls.ExecType_String = Enum<XAPI.ExecType>.ToString(field.ExecType);
                cls.LeavesQty = field.LeavesQty;
                cls.CumQty = field.CumQty;
                cls.AvgPx = field.AvgPx;
                cls.XErrorID = field.XErrorID;
                cls.RawErrorID = field.RawErrorID;
                cls.Text = field.Text();
                cls.ReserveInt32 = field.ReserveInt32;
                cls.ReserveChar64 = field.ReserveChar64;

                cls.PortfolioID1 = field.PortfolioID1;
                cls.PortfolioID2 = field.PortfolioID2;
                cls.PortfolioID3 = field.PortfolioID3;
                cls.Business = (int)field.Business;
                cls.Business_String = Enum<XAPI.BusinessType>.ToString(field.Business);
            }

            if (null == OnRspQryOrder)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRspQryOrder;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRspQryOrder);
                qd.Sender = this;
                qd.Data1 = cls;
                qd.Data2 = size1;
                qd.Data3 = bIsLast;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRspQryOrder(this, cls, size1, bIsLast);
            }
        }

        private void OnRspQryTrade_callback(object sender, [In] ref TradeField trade, int size1, bool bIsLast)
        {
            if (null == OnRspQryTrade)
                return;

            TradeClass cls = new TradeClass();

            if (size1 > 0)
            {
                TradeField field = trade;

                cls.InstrumentName = field.InstrumentName();
                cls.Symbol = field.Symbol;
                cls.InstrumentID = field.InstrumentID;
                cls.ExchangeID = field.ExchangeID;
                cls.ClientID = field.ClientID;
                cls.AccountID = field.AccountID;
                cls.Side = (int)field.Side;
                cls.Side_String = Enum<XAPI.OrderSide>.ToString(field.Side);
                cls.Qty = field.Qty;
                cls.Price = field.Price;
                cls.OpenClose = (int)field.OpenClose;
                cls.OpenClose_String = Enum<XAPI.OpenCloseType>.ToString(field.OpenClose);
                cls.HedgeFlag = (int)field.HedgeFlag;
                cls.HedgeFlag_String = Enum<XAPI.HedgeFlagType>.ToString(field.HedgeFlag);
                cls.Date = field.Date;
                cls.Time = field.Time;
                cls.ID = field.ID;
                cls.TradeID = field.TradeID;
                cls.ReserveInt32 = field.ReserveInt32;
                cls.ReserveChar64 = field.ReserveChar64;

                cls.PortfolioID1 = field.PortfolioID1;
                cls.PortfolioID2 = field.PortfolioID2;
                cls.PortfolioID3 = field.PortfolioID3;
                cls.Business = (int)field.Business;
                cls.Business_String = Enum<XAPI.BusinessType>.ToString(field.Business);
            }

            if (null == OnRspQryTrade)
            {
                QueueData qd = new QueueData();
                qd.Type = (int)ResponseType.OnRspQryTrade;
                qd.Type_String = Enum<XAPI.ResponseType>.ToString(ResponseType.OnRspQryTrade);
                qd.Sender = this;
                qd.Data1 = cls;
                qd.Data2 = size1;
                qd.Data3 = bIsLast;

                MessageQueue.Enqueue(qd);
            }
            else
            {
                OnRspQryTrade(this, cls, size1, bIsLast);
            }
        }

        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(20, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(31, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "XAPI";
            // 
            // XApiCom
            // 
            this.Controls.Add(this.label1);
            this.Name = "XApiCom";
            this.Size = new System.Drawing.Size(122, 33);
            this.ResumeLayout(false);
            this.PerformLayout();

        }
    }

    class SimpleCOMObjectClassFactory : ClassFactoryBase
    {
        public override void virtual_CreateInstance(IntPtr pUnkOuter, ref Guid riid, out IntPtr ppvObject)
        {
            Console.WriteLine("SimpleCOMObjectClassFactory.CreateInstance().");
            Console.WriteLine("Requesting Interface : " + riid.ToString());

            if (riid == Marshal.GenerateGuidForType(typeof(IXApi)) ||
                riid == ManagedCOMLocalServer.IID_IDispatch ||
                riid == ManagedCOMLocalServer.IID_IUnknown)
            {
                XApiCom SimpleCOMObject_New = new XApiCom();

                ppvObject = Marshal.GetComInterfaceForObject(SimpleCOMObject_New, typeof(IXApi));
            }
            else
            {
                throw new COMException("No interface", unchecked((int)0x80004002));
            }
        }
    }
}
