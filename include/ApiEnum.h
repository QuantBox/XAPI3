#ifndef _API_ENUM_H_
#define _API_ENUM_H_

/// 连接状态,此枚举主要供程序进行识别使用
enum ConnectionStatus :char
{
	ConnectionStatus_Uninitialized,	/// 未初始化
	ConnectionStatus_Initialized,	/// 已经初始化
	ConnectionStatus_Disconnected,	/// 连接已经断开，表示连接过程中遇到情况失败了
	ConnectionStatus_Connecting,	/// 连接中
	ConnectionStatus_Connected,		/// 连接成功
	ConnectionStatus_Authorizing,	/// 授权中
	ConnectionStatus_Authorized,	/// 授权成功
	ConnectionStatus_Logining,		/// 登录中
	ConnectionStatus_Logined,		/// 登录成功
	ConnectionStatus_Confirming,	/// 结算单确认中
	ConnectionStatus_Confirmed,		/// 已经确认
	ConnectionStatus_Doing,			/// 进行中
	ConnectionStatus_Done,			/// 完成，表示登录的重要过程都完成了，可通知Provider已经连上
	ConnectionStatus_Unknown,		/// 未知，一般不用这个状态
};

// API功能类型，功能可叠加
enum ApiType :char
{
	ApiType_None = 0,				/// 无
	ApiType_Trade = 1,				/// 交易，只下单撤单
	ApiType_MarketData = 2,			/// 行情
	ApiType_Level2 = 4,				///	Level2行情，这是否有保留的必要？
	ApiType_QuoteRequest = 8,
	ApiType_HistoricalData = 16,
	ApiType_Instrument = 32,
	ApiType_Query = 64,
};

/// 流恢复类型
enum ResumeType :char
{
	ResumeType_Restart,
	ResumeType_Resume,
	ResumeType_Quick,
	ResumeType_Undefined,
};

/// 日志记录级别
enum LogLevel :char
{
	LogLevel_Trace,
	LogLevel_Debug,
	LogLevel_Info,
	LogLevel_Warn,
	LogLevel_Error,
	LogLevel_Fatal,
};

/// 期权类型
enum PutCall :char
{
	PutCall_Put,
	PutCall_Call,
};

/// 委托状态，与OpenQuant2014中的值一样
enum OrderStatus :char
{
	OrderStatus_NotSent,
	OrderStatus_PendingNew,
	OrderStatus_New,
	OrderStatus_Rejected,
	OrderStatus_PartiallyFilled,
	OrderStatus_Filled,
	OrderStatus_PendingCancel,
	OrderStatus_Cancelled,
	OrderStatus_Expired,
	OrderStatus_PendingReplace,
	OrderStatus_Replaced,
};

/// 方向
/// 新股申购、回购，质押都暂时不写入，等业务掌握熟了后再加
enum OrderSide :char
{
	OrderSide_Buy,
	OrderSide_Sell,
	OrderSide_LOFCreation,		///申购,LOF申购
	OrderSide_LOFRedemption,	///赎回,LOF赎回
	OrderSide_ETFCreation,		///ETF申购
	OrderSide_ETFRedemption,	///ETF赎回
	OrderSide_Merge,			///合并
	OrderSide_Split,			///拆分
	OrderSide_CBConvert,		///可转债转股，参考于https://en.wikipedia.org/wiki/Convertible_bond
	OrderSide_CBRedemption,		///可转债回售，参考于https://en.wikipedia.org/wiki/Convertible_bond
	OrderSide_Unknown,			///出现这个输出时，需要技术人员去查找原因修正代码
};

/// 报单类型，与OpenQuant2014中的值一样
/// 各交易所的报单指令相当多，实际交易时只取市价和限价两种
enum OrderType :char
{
	OrderType_Market,
	OrderType_Stop,
	OrderType_Limit,
	OrderType_StopLimit,
	OrderType_MarketOnClose,
	OrderType_Pegged,
	OrderType_TrailingStop,
	OrderType_TrailingStopLimit,
};

/// TimeInForce，与OpenQuant2014中的值一样
/// 实际只识别IOC和FOK，其它都当成普通类型
enum TimeInForce :char
{
	TimeInForce_ATC,
	TimeInForce_Day,
	TimeInForce_GTC,
	TimeInForce_IOC,
	TimeInForce_OPG,
	TimeInForce_OC,
	TimeInForce_FOK,
	TimeInForce_GTX,
	TimeInForce_GTD,
	TimeInForce_GFS,
	TimeInForce_AUC,
};

/// 持仓方向，与OpenQuant2014中的值一样
enum PositionSide :char
{
	PositionSide_Long,
	PositionSide_Short,
};

/// 执行类型，与OpenQuant2014中的值一样
/// 一般与OrderStatus对应，但ExecCancelReject与ExecReplaceReject时与OrderStatus不对应
enum ExecType : char
{
	ExecType_New,
	ExecType_Stopped,
	ExecType_Rejected,
	ExecType_Expired,
	ExecType_Trade,
	ExecType_PendingCancel,
	ExecType_Cancelled,
	ExecType_CancelReject,
	ExecType_PendingReplace,
	ExecType_Replace,
	ExecType_ReplaceReject,
	ExecType_TradeCorrect,
	ExecType_TradeCancel,
	ExecType_OrderStatus,
	ExecType_PendingNew,
	ExecType_ClearingHold,
};

///开平标志，没有提供专门的平昨，而是用Close
enum OpenCloseType :char
{
	OpenCloseType_Undefined = 0,	// 未指定，用来自行处理
	OpenCloseType_Open,
	OpenCloseType_Close,
	OpenCloseType_CloseToday,
};

///投机套保标志
enum HedgeFlagType :char
{
	///投机
	HedgeFlagType_Speculation,
	///套利
	HedgeFlagType_Arbitrage,
	///套保
	HedgeFlagType_Hedge,
	///备兑,参考于CTPZQ
	HedgeFlagType_Covered,
	///做市商,参考于Femas
	HedgeFlagType_MarketMaker,
};

/// 合约类型，与OpenQuant2014中的值一样
enum InstrumentType :char
{
	InstrumentType_Stock = 0,
	InstrumentType_Future,
	InstrumentType_Option,
	InstrumentType_FutureOption,
	InstrumentType_Bond,
	InstrumentType_FX,
	InstrumentType_Index,
	InstrumentType_ETF,
	InstrumentType_MultiLeg,
	InstrumentType_Synthetic,
	InstrumentType_LOF,		///多加LOF
};

//////////////////////////////////////////////////////////////////////////
/// 证件类型，参考于CTP，去除了一些感觉不大可能用到的类型
/// 一般登录时返回，原计划是对机构账号与个人账号进行区别收费
enum IdCardType:char
{
	IdCardType_EID,				///组织机构代码
	IdCardType_IDCard,			///中国公民身份证
	IdCardType_Passport,		///护照
	IdCardType_LicenseNo,		///营业执照号
	IdCardType_TaxNo,			///税务登记号/当地纳税ID
	IdCardType_DrivingLicense,	///驾照
	IdCardType_SocialID,		///当地社保ID
	IdCardType_LocalID,			///当地身份证
	IdCardType_OtherCard,		///其他证件
};

/// 交易所类型，按交易所官网的缩写决定
/// 例如：
/// 上交所有SH(国内网站)/SS(Yahoo)/SSE(LTS)/SHA(Google)
/// 深交所有SZ(国内网站)/SZE(LTS)/SZSE(官网)
/// 港交所有HK(国内网站)/HKEx(官网)
enum ExchangeType :char
{
	ExchangeType_Undefined = 0,	/// 未定义
	ExchangeType_SHFE,			/// 上期所
	ExchangeType_DCE,			/// 大商所
	ExchangeType_CZCE,			/// 郑商所
	ExchangeType_CFFEX,			/// 中金所
	ExchangeType_INE,			/// 能源中心
	ExchangeType_SSE,			/// 上交所
	ExchangeType_SZSE,			/// 深交所
	ExchangeType_SGE,			/// 上海黄金交易所
	ExchangeType_NEEQ,			/// 全国中小企业股份转让系统,三板
	ExchangeType_HKEx,			/// 港交所
};


///合约生命周期状态类型
enum InstLifePhaseType :char
{
	InstLifePhaseType_NotStart,		///未上市
	InstLifePhaseType_Started,		///上市
	InstLifePhaseType_Pause,		///停牌
	InstLifePhaseType_Expired,		///到期

	InstLifePhaseType_Issue,		///发行,参考于XSpeed
	InstLifePhaseType_FirstList,	///首日上市,参考于XSpeed
	InstLifePhaseType_UnList,		///退市,参考于XSpeed
};

///交易阶段类型
enum TradingPhaseType :char
{
	TradingPhaseType_BeforeTrading,		///开盘前
	TradingPhaseType_NoTrading,			///非交易
	TradingPhaseType_Continuous,		///连续交易
	TradingPhaseType_AuctionOrdering,	///集合竞价报单
	TradingPhaseType_AuctionBalance,	///集合竞价价格平衡
	TradingPhaseType_AuctionMatch,		///集合竞价撮合
	TradingPhaseType_Closed,			///收盘
	TradingPhaseType_Suspension,		///停牌时段,参考于LTS
	TradingPhaseType_Fuse,				///熔断时段,参考于LTS
};

/// 业务类型，主要是有些API实际上已经对接了各种市场
// 不同的市场需要不同的函数，统一到一起实在太难,所以这个地方交给用户自己来指定
enum BusinessType :char
{
	BusinessType_Undefined = 0,		/// 未指定
	BusinessType_Future,
	BusinessType_Stock,
	BusinessType_Option,
};











//////////////////////////////////////////////////////////////////////////
// 只在历史下载中出现，可能需要删除

enum BarType :char
{
	BarType_Time = 1,
	Tick,
	Volume,
	Range,
	Session,
};

enum DataObjetType : char
{
	DataObject,
	Tick_,
	Bid,
	Ask,
	Trade_,
	Quote,
	Bar,
	Level2_,
	Level2Snapshot,
	Level2Update,
};

/// 行情数据深度级别
enum DepthLevelType :char
{
	DepthLevelType_L0,
	DepthLevelType_L1,
	DepthLevelType_L3,
	DepthLevelType_L5,
	DepthLevelType_L10,
	DepthLevelType_FULL,
};


/*
发现融资融券的只有部分功能加入，还有一些功能缺失
这下回到XAPI的定位问题，到底是一个只服务自动套利交易的API,还是全功能的API
*/
enum SecurityType :char
{
	CS, // Common Stock
	CB, // Convertible Bond，标记后，深圳卖出当前可转债表示转股
	ETF_, // 标记后，买入表示申购，卖出表示赎回
	LOF, // 标记后，买入表示申购，卖出表示赎回
	SF, // 标记后，买入表示合并，卖出表示分拆
	CashMargin, // 标记后，开仓表示"融"，平仓表示"还"
};

#endif
