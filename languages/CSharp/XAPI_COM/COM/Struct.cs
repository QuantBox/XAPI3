using System;
using System.Runtime.InteropServices;


namespace XAPI.COM
{
    ///持仓
    [ComVisible(true)]
    [Guid("07C1DA6E-A85E-4931-ABE6-03681A3DBB0B")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class PositionClass
    {
        public string InstrumentName;
        public string Symbol;
        public string InstrumentID;
        public string ExchangeID;
        public string ClientID;
        public string AccountID;

        public int Side;
        public string Side_String;
        public int HedgeFlag;
        public string HedgeFlag_String;
        public int Date;
        public double PositionCost;
        
        public double Position;
        public double TodayPosition;
        public double HistoryPosition;
        public double HistoryFrozen;

        ///今日买卖持仓
        public double TodayBSPosition;
        ///今日买卖持仓冻结
        public double TodayBSFrozen;
        ///今日申赎持仓
        public double TodayPRPosition;
        ///今日申赎持仓冻结
        public double TodayPRFrozen;

        public string ID;
        
        public string PortfolioID1;
        public string PortfolioID2;
        public string PortfolioID3;

        public int Business;
        public string Business_String;
    }

    /// <summary>
    /// 做市商双向报价
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct QuoteField
    {
        public byte[] InstrumentName;
        public string Symbol;
        public string InstrumentID;
        public string ExchangeID;
        public string ClientID;
        public string AccountID;


        public double AskQty;
        public double AskPrice;
        public OpenCloseType AskOpenClose;
        public HedgeFlagType AskHedgeFlag;

        public double BidQty;
        public double BidPrice;
        public OpenCloseType BidOpenClose;
        public HedgeFlagType BidHedgeFlag;

        public OrderStatus Status;
        public ExecType ExecType;

        
        public string QuoteReqID;
        public string LocalID;
        public string ID;
        public string AskID;
        public string BidID;
        public string AskOrderID;
        public string BidOrderID;

        public int XErrorID;
        public int RawErrorID;
        public byte[] Text;
    }

    /// <summary>
    /// 订单信息
    /// </summary>
    [ComVisible(true)]
    [Guid("23FC4E48-54FF-4A89-B924-B8668C5D9570")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class OrderClass
    {
        public string InstrumentName;
        public string Symbol;
        public string InstrumentID;
        public string ExchangeID;
        public string ClientID;
        public string AccountID;


        public int Side;
        public string Side_String;
        public double Qty;
        public double Price;
        public int OpenClose;
        public string OpenClose_String;
        public int HedgeFlag;
        public string HedgeFlag_String;
        public int Date;
        public int Time;

        
        public string ID;
        public string OrderID;
        public string LocalID;


        public int Type;
        public string Type_String;
        public double StopPx;
        public int TimeInForce;
        public string TimeInForce_String;

        public int Status;
        public string Status_String;
        public int ExecType;
        public string ExecType_String;
        public double LeavesQty;
        public double CumQty;
        public double AvgPx;

        public int XErrorID;
        public int RawErrorID;
        public string Text;

        public int ReserveInt32;
        public string ReserveChar64;
        
        public string PortfolioID1;
        public string PortfolioID2;
        public string PortfolioID3;

        public int Business;
        public string Business_String;
    }

    /// <summary>
    /// 成交回报
    /// </summary>
    [ComVisible(true)]
    [Guid("9FF68AA1-CB14-49E5-91F9-856400663C5E")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class TradeClass
    {
        public string InstrumentName;
        public string Symbol;
        public string InstrumentID;
        public string ExchangeID;
        public string ClientID;
        public string AccountID;

        public int Side;
        public string Side_String;
        public double Qty;
        public double Price;
        public int OpenClose;
        public string OpenClose_String;
        public int HedgeFlag;
        public string HedgeFlag_String;
        public int Date;
        public int Time;
        

        public string ID;
        public string TradeID;

        public double Commission;

        public int ReserveInt32;
        public string ReserveChar64;


        public string PortfolioID1;
        public string PortfolioID2;
        public string PortfolioID3;

        public int Business;
        public string Business_String;
    }
    

    /// <summary>
    /// 错误信息
    /// </summary>
    [ComVisible(true)]
    [Guid("5F68D385-04A7-4247-8C07-839C6CE68EFF")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class ErrorClass
    {
        /// <summary>
        /// 错误代码
        /// </summary>
        public int XErrorID;
        /// <summary>
        /// 错误代码
        /// </summary>
        public int RawErrorID;
        /// <summary>
        /// 错误信息
        /// </summary>
        public string Text;
        /// <summary>
        /// 信息来源
        /// </summary>
        public string Source;
    }


    /// <summary>
    /// 日志信息
    /// </summary>
    [ComVisible(true)]
    [Guid("4A4FDEFA-AEA6-496E-AE92-F00FD28A2075")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class LogClass
    {
        /// <summary>
        /// 日志级别
        /// </summary>
        public int Level;
        public string Level_String;
        /// <summary>
        /// 错误信息
        /// </summary>
        public string Message;
    }


    [ComVisible(true)]
    [Guid("D642BDD2-8BB7-4BC9-BDC4-29CF05D1EAD4")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class RspUserLoginClass
    {
        public int TradingDay;
        public int LoginTime;
        public string SessionID;
        public string UserID;
        public string AccountID;
        public string InvestorName;
        public int XErrorID;
        public int RawErrorID;
        public string Text;
    }

    

    /// <summary>
    /// DepthField行情
    /// </summary>
    [ComVisible(true)]
    [Guid("6A9597AE-9AB2-4C48-8A96-F386DBCD8AB6")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class DepthClass
    {
        public double Price;
        public int Size;
        public int Count;
    }


    /// <summary>
    /// 合约信息
    /// </summary>
    [ComVisible(true)]
    [Guid("37ED5322-1C11-477A-AC97-F947AD8532B2")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class InstrumentClass
    {
        public string InstrumentName;
        public string Symbol;
        public string InstrumentID;
        public string ExchangeID;
        public string ClientID;
        public string AccountID;

        
        public string ExchangeInstID;


        /// <summary>
        /// 类型
        /// </summary>
        public int Type;
        public string Type_String;
        /// <summary>
        /// 合约数量乘数
        /// </summary>
        public int VolumeMultiple;
        /// <summary>
        /// 最小变动价位
        /// </summary>
        public double PriceTick;
        /// <summary>
        /// 到期日
        /// </summary>
        public int ExpireDate;

        /// <summary>
        /// 执行价
        /// </summary>
        public double StrikePrice;
        /// <summary>
        /// 期权类型
        /// </summary>
        public int OptionsType;
        public string OptionsType_String;
        /// <summary>
        /// 产品代码
        /// </summary>
        public string ProductID;
        /// <summary>
        /// 基础商品代码
        /// </summary>
        public string UnderlyingInstrID;
        ///合约生命周期状态
        public int InstLifePhase;
        public string InstLifePhase_String;
    }

    /// <summary>
    /// 账号
    /// </summary>
    [ComVisible(true)]
    [Guid("6A75FF0C-C78F-4805-96AB-0FB4AF196152")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class AccountClass
    {
        public string ClientID;
        public string AccountID;
        public string CurrencyID;
        /// <summary>
        /// 上次结算准备金
        /// </summary>
        public double PreBalance;
        /// <summary>
        /// 当前保证金总额
        /// </summary>
        public double CurrMargin;
        /// <summary>
        /// 平仓盈亏
        /// </summary>
        public double CloseProfit;
        /// <summary>
        /// 持仓盈亏
        /// </summary>
        public double PositionProfit;
        /// <summary>
        /// 期货结算准备金
        /// </summary>
        public double Balance;
        /// <summary>
        /// 可用资金
        /// </summary>
        public double Available;
        /// <summary>
        /// 入金金额
        /// </summary>
        public double Deposit;
        /// <summary>
        /// 出金金额
        /// </summary>
        public double Withdraw;
        /// <summary>
        /// 可取资金
        /// </summary>
        public double WithdrawQuota;
        /// <summary>
        /// 冻结的过户费
        /// </summary>
        public double FrozenTransferFee;
        /// <summary>
        /// 冻结的印花税
        /// </summary>
        public double FrozenStampTax;
        /// <summary>
        /// 冻结的手续费
        /// </summary>
        public double FrozenCommission;
        /// <summary>
        /// 冻结的资金
        /// </summary>
        public double FrozenCash;
        /// <summary>
        /// 过户费
        /// </summary>
        public double TransferFee;
        /// <summary>
        /// 印花税
        /// </summary>
        public double StampTax;
        /// <summary>
        /// 手续费
        /// </summary>
        public double Commission;
        /// <summary>
        /// 资金差额
        /// </summary>
        public double CashIn;
    }



    /// <summary>
    /// 发给做市商的询价请求
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct QuoteRequestField
    {
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        /// <summary>
        /// 交易日
        /// </summary>
        public int TradingDay;
        /// <summary>
        /// 询价时间
        /// </summary>
        public int QuoteTime;

        /// <summary>
        /// 询价编号
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string QuoteID;
    }


    [ComVisible(true)]
    [Guid("E08904CE-0E96-48B5-9D87-F4C5ACE3E560")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class SettlementInfoClass
    {
        /// <summary>
        /// 交易日
        /// </summary>
        public int TradingDay;
        ///// <summary>
        ///// 消息正文
        ///// </summary>
        public string Content;
    }

    /// <summary>
    /// 投资者
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct InvestorField
    {
        public string InvestorID;
        public string BrokerID;
        public int IdentifiedCardType;
        public string IdentifiedCardType_String;
        public string IdentifiedCardNo;
        public string InvestorName;
    }


    //[ComVisible(false)]
    //[StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    //public struct OrderIDType
    //{
    //    public string ID;
    //}
    

    [ComVisible(true)]
    [Guid("734AF40E-D336-4C0C-A8B7-C7DB9C7FF726")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class DepthMarketDataNClass
    {
        public int TradingDay;
        public int ActionDay;
        public int UpdateTime;
        public int UpdateMillisec;
        public string Symbol;
        public string InstrumentID;
        public string ExchangeID;

        /// <summary>
        /// 交易所代码
        /// </summary>
        public int Exchange;


        /// <summary>
        /// 最新价
        /// </summary>
        public double LastPrice;
        /// <summary>
        /// 数量
        /// </summary>
        public double Volume;
        /// <summary>
        /// 成交金额
        /// </summary>
        public double Turnover;
        /// <summary>
        /// 持仓量
        /// </summary>
        public double OpenInterest;
        /// <summary>
        /// 当日均价
        /// </summary>
        public double AveragePrice;


        /// <summary>
        /// 今开盘
        /// </summary>
        public double OpenPrice;
        /// <summary>
        /// 最高价
        /// </summary>
        public double HighestPrice;
        /// <summary>
        /// 最低价
        /// </summary>
        public double LowestPrice;
        /// <summary>
        /// 今收盘
        /// </summary>
        public double ClosePrice;
        /// <summary>
        /// 本次结算价
        /// </summary>
        public double SettlementPrice;

        /// <summary>
        /// 涨停板价
        /// </summary>
        public double UpperLimitPrice;
        /// <summary>
        /// 跌停板价
        /// </summary>
        public double LowerLimitPrice;
        /// <summary>
        /// 昨收盘
        /// </summary>
        public double PreClosePrice;
        /// <summary>
        /// 上次结算价
        /// </summary>
        public double PreSettlementPrice;
        /// <summary>
        /// 昨持仓量
        /// </summary>
        public double PreOpenInterest;

        ///交易阶段类型
        public int TradingPhase;
        public string TradingPhase_String;


        public double AskPrice1;
        public int AskSize;
        public double BidPrice1;
        public int BidSize;
    }















    /// <summary>
    /// Tick行情
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct TickField
    {
        public int Date;
        public int Time;
        public int Millisecond;

        public double LastPrice;
        public double Volume;
        public double OpenInterest;
        public double BidPrice1;
        public double AskPrice1;
        public int BidSize1;
        public int AskSize1;
    }

    /// <summary>
    /// Bar行情
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct BarField
    {
        public int Date;
        public int Time;

        public double Open;
        public double High;
        public double Low;
        public double Close;
        public double Volume;
        public double OpenInterest;
        public double Turnover;
    }



    /// <summary>
    /// 合约信息
    /// </summary>
    [ComVisible(false)]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
    public struct HistoricalDataRequestField
    {
        public int Date1;
        public int Date2;
        public int Time1;
        public int Time2;

        public DataObjetType DataType;
        public BarType BarType;
        public int BarSize;

        public int RequestId;
        public int CurrentDate;
        public int lRequest;

        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
    }

    [ComVisible(true)]
    [Guid("42AB7905-CC91-480A-AEF0-FF8795872FF4")]
    [ClassInterface(ClassInterfaceType.AutoDual)]
    public class QueueData
    {
        public int Type;
        public string Type_String;
        public object Sender;
        public object Data1;
        public object Data2;
        public object Data3;
        public object Data4;
    }

}
