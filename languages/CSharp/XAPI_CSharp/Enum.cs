using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace XAPI
{
    /// <summary>
    /// 连接状态
    /// </summary>
    [ComVisible(false)]
    public enum ConnectionStatus : byte
    {
        /// <summary>
        /// 未初始化
        /// </summary>
        [Description("未初始化")]
        Uninitialized = 0,
        /// <summary>
        /// 已经初始化
        /// </summary>
        [Description("已经初始化")]
        Initialized,
        /// <summary>
        /// 连接已经断开
        /// </summary>
        [Description("连接已经断开")]
        Disconnected,
        /// <summary>
        /// 连接中...
        /// </summary>
        [Description("连接中...")]
        Connecting,
        /// <summary>
        /// 连接成功
        /// </summary>
        [Description("连接成功")]
        Connected,
        /// <summary>
        /// 授权中...
        /// </summary>
        [Description("授权中...")]
        Authorizing,
        /// <summary>
        /// 授权成功
        /// </summary>
        [Description("授权成功")]
        Authorized,
        /// <summary>
        /// 登录中...
        /// </summary>
        [Description("登录中...")]
        Logining,
        /// <summary>
        /// 登录成功
        /// </summary>
        [Description("登录成功")]
        Logined,
        /// <summary>
        /// 确认中...
        /// </summary>
        [Description("确认中...")]
        Confirming,
        /// <summary>
        /// 已经确认
        /// </summary>
        [Description("已经确认")]
        Confirmed,
        /// <summary>
        /// 已经确认
        /// </summary>
        [Description("操作中...")]
        Doing,
        /// <summary>
        /// 完成
        /// </summary>
        [Description("完成")]
        Done,
        /// <summary>
        /// 未知
        /// </summary>
        [Description("未知")]
        Unknown,
    }

    [ComVisible(false)]
    [Flags]
    public enum ApiType : byte
    {
        None = 0,
        Trade = 1,
        MarketData = 2,
        Level2 = 4,
        QuoteRequest = 8,
        HistoricalData = 16,
        Instrument = 32,
        Query = 64,
    };

    [ComVisible(false)]
    public enum ResumeType : byte
    {
        Restart,
        Resume,
        Quick,
        Undefined,
    };

    [ComVisible(false)]
    public enum LogLevel : byte
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal,
    };

    [ComVisible(false)]
    public enum PutCall : byte
    {
        Put,
        Call,
    };

    [ComVisible(false)]
    public enum OrderStatus : byte
    {
        NotSent,
        PendingNew,
        New,
        Rejected,
        PartiallyFilled,
        Filled,
        PendingCancel,
        Cancelled,
        Expired,
        PendingReplace,
        Replaced,
    };

    [ComVisible(false)]
    public enum OrderSide : byte
    {
        Buy,
        Sell,
        LOFCreation,		///LOF申购
        LOFRedemption,		///LOF赎回
        ETFCreation,	///ETF申购
        ETFRedemption,	///ETF赎回
        Merge,		    ///合并
        Split,		    ///拆分
        CBConvert,		///可转债转股，参考于https://en.wikipedia.org/wiki/Convertible_bond
        CBRedemption,	
        Unknown,
    };

    [ComVisible(false)]
    public enum OrderType : byte
    {
        Market,
        Stop,
        Limit,
        StopLimit,
        MarketOnClose,
        Pegged,
        TrailingStop,
        TrailingStopLimit,
    };

    [ComVisible(false)]
    public enum TimeInForce : byte
    {
        ATC,
        Day,
        GTC,
        IOC,
        OPG,
        OC,
        FOK,
        GTX,
        GTD,
        GFS,
        AUC,
    };

    [ComVisible(false)]
    public enum PositionSide : byte
    {
        Long,
        Short,
    };

    [ComVisible(false)]
    public enum ExecType : byte
    {
        New,
        Stopped,
        Rejected,
        Expired,
        Trade,
        PendingCancel,
        Cancelled,
        CancelReject,
        PendingReplace,
        Replace,
        ReplaceReject,
        TradeCorrect,
        TradeCancel,
        OrderStatus,
        PendingNew,
        ClearingHold,
    };

    [ComVisible(false)]
    public enum OpenCloseType : byte
    {
        Undefined,
        Open,
        Close,
        CloseToday,
    };

    [ComVisible(false)]
    public enum HedgeFlagType : byte
    {
        Speculation,
        Arbitrage,
        Hedge,
        Covered,
        MarketMaker,
    };

    [ComVisible(false)]
    public enum InstrumentType : byte
    {
	    Stock,
	    Future,
	    Option,
	    FutureOption,
	    Bond,
	    FX,
	    Index,
	    ETF,
	    MultiLeg,
	    Synthetic,
    };

    [ComVisible(false)]
    public enum IdCardType : byte
    {
        EID,			///组织机构代码
        IDCard,			///中国公民身份证
        Passport,		///护照
        LicenseNo,		///营业执照号
        TaxNo,			///税务登记号/当地纳税ID
        DrivingLicense,	///驾照
        SocialID,		///当地社保ID
        LocalID,		///当地身份证
        OtherCard,		///其他证件
    };

    [ComVisible(false)]
    public enum ExchangeType : byte
    {
        Undefined,	    /// 未定义
        SHFE,			/// 上期所
        DCE,			/// 大商所
        CZCE,			/// 郑商所
        CFFEX,			/// 中金所
        INE,			/// 能源中心
        SSE,			/// 上交所
        SZSE,			/// 深交所
        SGE,			/// 上海黄金交易所
        NEEQ,			/// 全国中小企业股份转让系统,三板
        HKEx,			/// 港交所
    };

    ///合约生命周期状态类型
    [ComVisible(false)]
    public enum InstLifePhaseType : byte
    {
        NotStart,		///未上市
        Started,		///上市
        Pause,		///停牌
        Expired,		///到期

        Issue,		///发行,参考于XSpeed
        FirstList,	///首日上市,参考于XSpeed
        UnList,		///退市,参考于XSpeed
    };


    ///交易阶段类型
    [ComVisible(false)]
    public enum TradingPhaseType : byte
    {
        BeforeTrading,		///开盘前
        NoTrading,			///非交易
        Continuous,		///连续交易
        AuctionOrdering,	///集合竞价报单
        AuctionBalance,	///集合竞价价格平衡
        AuctionMatch,		///集合竞价撮合
        Closed,			///收盘
        Suspension,		///停牌时段,参考于LTS
        Fuse,				///熔断时段,参考于LTS
    };

    [ComVisible(false)]
    public enum BusinessType : byte
    {
        Undefined,
        Future,
        Stock,
        Option,
    };




    /// <summary>
    /// /////////////////////////////////////////////////////////
    /// </summary>
    [ComVisible(false)]
    public enum BarType:byte
    {
        Time = 1,
        Tick,
        Volume,
        Range,
        Session,
    };

    [ComVisible(false)]
    public enum DataObjetType : byte
    {
        DataObject,
        Tick,
        Bid,
        Ask,
        Trade,
        Quote,
        Bar,
        Level2,
        Level2Snapshot,
        Level2Update,
    };

    /// <summary>
    /// 
    /// </summary>
    [ComVisible(false)]
    public enum DepthLevelType : byte
    {
        L0,
        L1,
        L3,
        L5,
        L10,
        FULL,
    };
}
