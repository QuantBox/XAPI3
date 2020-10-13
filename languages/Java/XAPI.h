#pragma once

#ifndef _API_STRUCT_H_
#define _API_STRUCT_H_

#ifndef _API_DATA_TYPE_H_
#define _API_DATA_TYPE_H_


/////////////////////////////////////////////////////////////////////////
///SymbolType是一个合约唯一符号类型，通过某种方式算出，不同API的同一合约完全相同
/////////////////////////////////////////////////////////////////////////
typedef char SymbolType[64];

/////////////////////////////////////////////////////////////////////////
///InstrumentIDType是一个合约代码类型，API对应的合约代码
/////////////////////////////////////////////////////////////////////////
typedef char InstrumentIDType[31];

/////////////////////////////////////////////////////////////////////////
///ExchangeIDType是一个交易所代码类型，API对应的交易所代码，可能各不相同
/////////////////////////////////////////////////////////////////////////
typedef char ExchangeIDType[9];

/////////////////////////////////////////////////////////////////////////
///DateTimeType是一个日期时间类型
/////////////////////////////////////////////////////////////////////////
typedef char DateTimeType[32];
/////////////////////////////////////////////////////////////////////////
///DateTimeLongType是一个日期时间类型,yyyyMMddHHmmss
/////////////////////////////////////////////////////////////////////////
typedef long DateTimeLongType;

/////////////////////////////////////////////////////////////////////////
///DateType是一个日期类型
/////////////////////////////////////////////////////////////////////////
typedef char DateType[9];
/////////////////////////////////////////////////////////////////////////
///DateIntType是一个日期类型,yyyyMMdd
/////////////////////////////////////////////////////////////////////////
typedef int DateIntType;

/////////////////////////////////////////////////////////////////////////
///TimeType是一个时间类型
/////////////////////////////////////////////////////////////////////////
typedef char TimeType[9];
/////////////////////////////////////////////////////////////////////////
///TimeIntType是一个时间类型,HHmmss
/////////////////////////////////////////////////////////////////////////
typedef int TimeIntType;


/////////////////////////////////////////////////////////////////////////
///Int32Type
/////////////////////////////////////////////////////////////////////////
typedef int Int32Type;

/////////////////////////////////////////////////////////////////////////
///Char256Type
/////////////////////////////////////////////////////////////////////////
typedef char Char256Type[256];
/////////////////////////////////////////////////////////////////////////
///Char128Type
/////////////////////////////////////////////////////////////////////////
typedef char Char128Type[128];
/////////////////////////////////////////////////////////////////////////
///Char64Type
/////////////////////////////////////////////////////////////////////////
typedef char Char64Type[64];
/////////////////////////////////////////////////////////////////////////
///Char32Type
/////////////////////////////////////////////////////////////////////////
typedef char Char32Type[32];
/////////////////////////////////////////////////////////////////////////
///BooleanType
/////////////////////////////////////////////////////////////////////////
typedef bool BooleanType;

/////////////////////////////////////////////////////////////////////////
///PriceType是一个价格类型
/////////////////////////////////////////////////////////////////////////
typedef double PriceType;

/////////////////////////////////////////////////////////////////////////
///VolumeType是一个交易量类型
/////////////////////////////////////////////////////////////////////////
typedef int VolumeType;
/////////////////////////////////////////////////////////////////////////
///LargeVolumeType是一个大额数量类型
/////////////////////////////////////////////////////////////////////////
typedef double LargeVolumeType;

/////////////////////////////////////////////////////////////////////////
///QtyType是一个交易量类型
/////////////////////////////////////////////////////////////////////////
typedef double QtyType;
/////////////////////////////////////////////////////////////////////////
///MoneyType是一个资金类型
/////////////////////////////////////////////////////////////////////////
typedef double MoneyType;
/////////////////////////////////////////////////////////////////////////
///CurrencyIDType是一个币种类型
/////////////////////////////////////////////////////////////////////////
typedef char CurrencyIDType[4];

/////////////////////////////////////////////////////////////////////////
///IDChar32Type
/////////////////////////////////////////////////////////////////////////
typedef char IDChar32Type[32];

/////////////////////////////////////////////////////////////////////////
///PasswordType是一个密码类型
/////////////////////////////////////////////////////////////////////////
typedef char PasswordType[41];




/////////////////////////////////////////////////////////////////////////
/// PartyNameType是一个参与人名称类型
/////////////////////////////////////////////////////////////////////////
typedef char PartyNameType[81];

/////////////////////////////////////////////////////////////////////////
/// IdentifiedCardNoType是一个证件号码类型
/////////////////////////////////////////////////////////////////////////
typedef char IdentifiedCardNoType[51];

/////////////////////////////////////////////////////////////////////////
///ProductInfoType是一个产品信息类型
/////////////////////////////////////////////////////////////////////////
typedef char ProductInfoType[11];

/////////////////////////////////////////////////////////////////////////
///BrokerIDType是一个经纪公司代码类型
/////////////////////////////////////////////////////////////////////////
typedef char BrokerIDType[11];

/////////////////////////////////////////////////////////////////////////
///AuthCodeType是一个客户端认证码类型
/////////////////////////////////////////////////////////////////////////
typedef char AuthCodeType[17];

/////////////////////////////////////////////////////////////////////////
///AddressType是一个地址类型
/////////////////////////////////////////////////////////////////////////
typedef char AddressType[512];

/////////////////////////////////////////////////////////////////////////
///VolumeMultipleType是一个合约数量乘数类型
/////////////////////////////////////////////////////////////////////////
typedef int VolumeMultipleType;

/////////////////////////////////////////////////////////////////////////
///InstrumentNameType是一个合约名称类型
/////////////////////////////////////////////////////////////////////////
typedef char InstrumentNameType[64];

/////////////////////////////////////////////////////////////////////////
///ContentType是一个消息正文类型
/////////////////////////////////////////////////////////////////////////
typedef char ContentType[501];

/////////////////////////////////////////////////////////////////////////
///SessionIDType是一个会话编号类型
/////////////////////////////////////////////////////////////////////////
typedef char SessionIDType[32];

/////////////////////////////////////////////////////////////////////////
///OrderIDType是一个订单唯一编号类型
/////////////////////////////////////////////////////////////////////////
typedef char OrderIDType[64];

/////////////////////////////////////////////////////////////////////////
///TradeIDType是一个成交编号类型
/////////////////////////////////////////////////////////////////////////
typedef char TradeIDType[64];

/////////////////////////////////////////////////////////////////////////
///PositionIDType是一个持仓编号类型
/////////////////////////////////////////////////////////////////////////
typedef char PositionIDType[64];

/////////////////////////////////////////////////////////////////////////
///BarSizeType是一个BarSize类型
/////////////////////////////////////////////////////////////////////////
typedef long BarSizeType;

typedef int SizeType;
#endif


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
	ApiType_Nono = 0,				/// 无
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
};

///开平标志，没有提供专门的平昨，而是用Close
enum OpenCloseType :char
{
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

#pragma pack(push,1)

//////////////////////////////////////////////////////////////////////////
///字段分为两大类
/// 1.由API直接提供有效值
/// 2.计算出来的值，字段正好预留了此字段，可以直接填充，这样使用者就不需要定义自己的字段了

///查询持仓,查委托，查成交,查合约，查资金等
///定义一个通用的结构，省事
struct ReqQueryField
{
	///合约名称，继续保留在这只是为了与其它结构体风格统一，一般底层不用它
	InstrumentNameType	InstrumentName;
	///唯一符号，同样是为了风格统一，实际一般不用
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType		ExchangeID;
	///客户号，多账号支持使用
	IDChar32Type		ClientID;
	///账号，证券中用于股东代码
	IDChar32Type		AccountID;

	///币种
	CurrencyIDType		CurrencyID;

	///开始日期
	DateIntType			DateStart;
	///结束日期
	DateIntType			DateEnd;
	///开始时间
	TimeIntType			TimeStart;
	///结束时间
	TimeIntType			TimeEnd;

	///一般是指定查询时使用
	OrderIDType			Char64ID;
	Int32Type			Int32ID;

	///一般是由API底层查询预留
	OrderIDType			Char64PositionIndex;
	Int32Type			Int32PositionIndex;
};

///持仓
// TODO:如何记录股票的买卖，申购赎回，以及对应的冻结
// 融资融券是如何记录，使用AccountID来区分普通账户与信用账户吗？
// 能否记录港股？
struct PositionField
{
	///合约名称，主要考虑到股票的数字辨识度不够，加上中文
	InstrumentNameType	InstrumentName;
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType		ExchangeID;
	///客户号，多账号支持使用
	IDChar32Type		ClientID;
	///账号，证券中用于股东代码
	IDChar32Type		AccountID;

	///持仓方向，如果有双向持仓，那应当至少是两个PositionField
	PositionSide	Side;
	///投保，计划用它来区分个股期权的备兑
	HedgeFlagType	HedgeFlag;
	///交易日
	DateIntType		Date;
	///持仓成本，可用来计算持仓均价
	MoneyType		PositionCost;

	///总持仓=今持仓+昨持仓
	QtyType			Position;
	///今持仓=买卖+申赎
	QtyType			TodayPosition;
	///昨持仓,如果买卖、申赎成功了，历史持仓就没有区别了
	QtyType			HistoryPosition;
	///昨日持仓冻结，要卖出时要先冻结
	QtyType			HistoryFrozen;

	///今日买卖持仓
	QtyType			TodayBSPosition;
	///今日买卖持仓冻结
	QtyType			TodayBSFrozen;
	///今日申赎持仓
	QtyType			TodayPRPosition;
	///今日申赎持仓冻结
	QtyType			TodayPRFrozen;
};

///做市商双向报价
struct QuoteField
{
	///合约名称
	InstrumentNameType	InstrumentName;
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType		ExchangeID;
	///客户号，多账号支持使用
	IDChar32Type		ClientID;
	///账号，证券中用于股东代码
	IDChar32Type		AccountID;



	QtyType			AskQty;
	PriceType		AskPrice;
	OpenCloseType	AskOpenClose;
	HedgeFlagType	AskHedgeFlag;

	QtyType			BidQty;
	PriceType		BidPrice;
	OpenCloseType	BidOpenClose;
	HedgeFlagType	BidHedgeFlag;

	OrderStatus		Status;
	ExecType		ExecType;


	///询价编号
	OrderIDType		QuoteReqID;

	OrderIDType		LocalID;
	OrderIDType		ID;
	OrderIDType		AskID;
	OrderIDType		BidID;
	OrderIDType		AskOrderID;
	OrderIDType		BidOrderID;

	///XAPI中错误代码，转成统一的值后，应用程序可以直接用它进行判断
	Int32Type		XErrorID;
	///原始错误代码
	Int32Type		RawErrorID;
	Char256Type		Text;
};


///委托，委托回报都是同一结构体
struct OrderField
{
	///合约名称
	InstrumentNameType	InstrumentName;
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType		ExchangeID;
	///客户号，多账号支持使用
	IDChar32Type		ClientID;
	///账号，证券中用于股东代码
	IDChar32Type		AccountID;


	///订单方向，用来来区分买卖与申赎
	OrderSide		Side;
	///数量
	QtyType			Qty;
	///价格
	PriceType		Price;
	///开平
	OpenCloseType	OpenClose;
	///投机套保
	HedgeFlagType	HedgeFlag;
	///日期
	DateIntType		Date;
	///时间
	TimeIntType		Time;


	///系统ID，唯一，在CTP一类可指定OrderRef的API中，这是拼接成的字符串，在Tdx一类后生成ID的API中，这是柜台直接传来的唯一值
	OrderIDType			ID;

	//////////////////////////////////////////////////////////////////////////

	///交易所或柜台生成的ID，可供比较报单先后。不同交易所或柜台可能出现相同OrderID
	OrderIDType			OrderID;
	///本地ID,只在委托时定位使用，收到回报请使用ID，在CTP一类API中ID==LocalID
	OrderIDType			LocalID;

	///订单类型
	OrderType		Type;
	///止损价
	PriceType		StopPx;
	///有效时间类型
	TimeInForce		TimeInForce;
	///订单状态
	OrderStatus		Status;
	///执行类型
	ExecType		ExecType;
	///剩余数量
	QtyType			LeavesQty;
	///成交数量
	QtyType			CumQty;
	///成交均价
	PriceType		AvgPx;


	///XAPI中错误代码，转成统一的值后，应用程序可以直接用它进行判断
	Int32Type		XErrorID;
	///原始错误代码
	Int32Type		RawErrorID;
	///信息
	Char256Type		Text;

	///预留数字
	Int32Type		ReserveInt32;
	///预留字符串
	Char64Type		ReserveChar64;
};


///成交回报
struct TradeField
{
	///合约名称
	InstrumentNameType	InstrumentName;
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType		ExchangeID;
	///客户号，多账号支持使用
	IDChar32Type		ClientID;
	///账号，证券中用于股东代码
	IDChar32Type		AccountID;


	///订单方向
	OrderSide		Side;
	///数量
	QtyType			Qty;
	///价格
	PriceType		Price;
	///开平
	OpenCloseType	OpenClose;
	///投机套保
	HedgeFlagType	HedgeFlag;
	///日期
	DateIntType		Date;
	///时间
	TimeIntType		Time;


	///所对应的Order的ID
	OrderIDType		ID;

	//////////////////////////////////////////////////////////////////////////

	///交易所产生的成交ID，可用于判断自成交
	TradeIDType		TradeID;
	///手续费
	MoneyType		Commission;

	///预留数字
	Int32Type		ReserveInt32;
	///预留字符串
	Char64Type		ReserveChar64;
};

///服务器信息
struct ServerInfoField
{
	///是否UDP
	BooleanType			IsUsingUdp;
	///是否多播
	BooleanType			IsMulticast;
	///订阅ID
	Int32Type			TopicId;
	///端口号
	Int32Type			Port;

	ResumeType			MarketDataTopicResumeType;
	ResumeType			PrivateTopicResumeType;
	ResumeType			PublicTopicResumeType;
	ResumeType			UserTopicResumeType;

	///经纪商ID
	BrokerIDType		BrokerID;
	///产品信息
	ProductInfoType		UserProductInfo;
	///授权码
	AuthCodeType		AuthCode;
	///地址信息
	AddressType			Address;
	///配置文件路径
	Char256Type			ConfigPath;
	///扩展信息
	Char128Type			ExtInfoChar128;
};


// 用户信息
struct UserInfoField
{
	///用户代码
	IDChar32Type	UserID;
	///密码
	PasswordType	Password;
	///扩展信息，Tdx中用来做通讯密码
	Char64Type		ExtInfoChar64;
	///扩展信息，32位的数字
	Int32Type		ExtInfoInt32;
};


// 错误信息
struct ErrorField
{
	///XAPI中错误代码，转成统一的值后，应用程序可以直接用它进行判断
	Int32Type		XErrorID;
	///原始错误代码
	Int32Type		RawErrorID;
	// 错误信息
	Char256Type		Text;
	// 消息来源
	Char64Type		Source;
};

///日志信息
struct LogField
{
	///日志级别
	LogLevel	Level;
	///消息内容
	Char256Type	Message;
};


/// 登录回报
/// 通过size1>0来判断是否有内容
/// 不管成功还是失败都可以输出，这个结构体中大部分内容是给人看的
struct RspUserLoginField
{
	///交易日
	DateIntType		TradingDay;
	///时间
	TimeIntType		LoginTime;
	///会话ID
	SessionIDType	SessionID;
	///用户账号
	IDChar32Type	UserID;
	///股东账号
	IDChar32Type	AccountID;
	///投资者名称
	PartyNameType	InvestorName;
	///XAPI中错误代码，转成统一的值后，应用程序可以直接用它进行判断
	Int32Type		XErrorID;
	///原始错误代码
	Int32Type		RawErrorID;
	///信息
	Char256Type		Text;
};


///深度行情N档
struct DepthMarketDataNField
{
	///占用总字节大小
	SizeType			Size;
	///交易日，用于给数据接收器划分到同一文件使用，基本没啥别的用处
	DateIntType			TradingDay;
	///日历日，实际日期
	DateIntType			ActionDay;
	///时间
	TimeIntType			UpdateTime;
	///毫秒
	TimeIntType			UpdateMillisec;

	///唯一符号，这里的用途主要给map做映射，找到通知哪个合约来行情了使用
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;

	ExchangeIDType		ExchangeID;
	///交易所代码，枚举类型，主要是用于过滤使用，效率比字符串比较要高
	ExchangeType		Exchange;

	///最新价
	PriceType			LastPrice;
	///数量
	LargeVolumeType		Volume;
	///成交金额
	MoneyType			Turnover;
	///持仓量
	LargeVolumeType		OpenInterest;
	///当日均价
	PriceType			AveragePrice;


	///今开盘
	PriceType			OpenPrice;
	///最高价
	PriceType			HighestPrice;
	///最低价
	PriceType			LowestPrice;
	///今收盘
	PriceType			ClosePrice;
	///本次结算价
	PriceType			SettlementPrice;

	///涨停板价
	PriceType			UpperLimitPrice;
	///跌停板价
	PriceType			LowerLimitPrice;
	///昨收盘
	PriceType			PreClosePrice;
	///上次结算价
	PriceType			PreSettlementPrice;
	///昨持仓量
	LargeVolumeType		PreOpenInterest;
	///交易阶段类型
	TradingPhaseType	TradingPhase;

	///买档个数
	SizeType			BidCount;
};

///深度行情中的某一档
struct DepthField
{
	///价格
	PriceType		Price;
	///量
	VolumeType		Size;
	///笔数
	VolumeType		Count;
};

///合约
///TODO:是否要再补充一些？
///对于ETF,ETF列表，开放式基金、分级基金这一类的是只在当前结构体中加，还是有什么别的方案处理？
struct InstrumentField
{
	///合约名称
	InstrumentNameType	InstrumentName;
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType		ExchangeID;
	///客户号，多账号支持使用
	IDChar32Type		ClientID;
	///账号，证券中用于股东代码
	IDChar32Type		AccountID;
	///合约在交易所的代码，个股期权交易代码会变化
	InstrumentIDType	ExchangeInstID;

	///合约类型
	InstrumentType		Type;
	///合约数量乘数
	VolumeMultipleType	VolumeMultiple;
	///最小变动价位
	PriceType			PriceTick;
	///到期日
	DateIntType			ExpireDate;
	///执行价
	PriceType			StrikePrice;
	///期权类型
	PutCall				OptionsType;

	///产品代码
	InstrumentIDType	ProductID;

	///基础商品代码
	InstrumentIDType	UnderlyingInstrID;
	///合约生命周期状态
	InstLifePhaseType	InstLifePhase;
};

///账号资金
struct AccountField
{
	///客户号，多账号支持使用
	IDChar32Type		ClientID;
	///账号，证券中用于股东代码
	IDChar32Type		AccountID;
	///币种
	CurrencyIDType		CurrencyID;
	///上次结算准备金
	MoneyType			PreBalance;
	///当前保证金总额
	MoneyType			CurrMargin;
	///平仓盈亏
	MoneyType			CloseProfit;
	///持仓盈亏
	MoneyType			PositionProfit;
	///期货结算准备金
	MoneyType			Balance;
	///可用资金
	MoneyType			Available;

	///入金金额
	MoneyType			Deposit;
	///出金金额
	MoneyType			Withdraw;
	///可取资金
	MoneyType			WithdrawQuota;

	///冻结的过户费
	MoneyType			FrozenTransferFee;
	///冻结的印花税
	MoneyType			FrozenStampTax;
	///冻结的手续费
	MoneyType			FrozenCommission;
	///冻结的资金
	MoneyType			FrozenCash;

	///过户费
	MoneyType			TransferFee;
	///印花税
	MoneyType			StampTax;
	///手续费
	MoneyType			Commission;
	///资金差额
	MoneyType			CashIn;
};


///发给做市商的询价请求
struct QuoteRequestField
{
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType		ExchangeID;
	///交易日
	DateIntType			TradingDay;
	///询价时间
	TimeIntType			QuoteTime;
	///询价编号
	OrderIDType			QuoteID;
};

///结算信息
struct SettlementInfoField
{
	///大小
	SizeType	Size;
	///交易日
	DateIntType	TradingDay;
	///消息正文，以下是消息正文
};

///投资者
struct InvestorField
{
	///用户代码
	IDChar32Type			InvestorID;

	BrokerIDType			BrokerID;

	///证件类型
	IdCardType				IdentifiedCardType;
	///证件号码
	IdentifiedCardNoType	IdentifiedCardNo;
	///投资者名称
	PartyNameType			InvestorName;
};







//////////////////////////////////////////////////////////////////////////
/// TODO:以下可能是要清理不用的
struct ConfigInfoField
{
	bool DirectOutput;
};

///Tick行情
struct TickField
{
	///交易所时间
	DateIntType			Date;
	TimeIntType			Time;
	TimeIntType			Millisecond;

	PriceType	LastPrice;
	///数量
	LargeVolumeType	Volume;
	///持仓量
	LargeVolumeType	OpenInterest;
	PriceType	BidPrice1;
	PriceType	AskPrice1;
	VolumeType	BidSize1;
	VolumeType	AskSize1;
};


///Bar行情
struct BarField
{
	///交易所时间
	DateIntType			Date;
	TimeIntType			Time;

	///开
	PriceType	Open;
	///高
	PriceType	High;
	///低
	PriceType	Low;
	///收
	PriceType	Close;
	///数量
	LargeVolumeType	Volume;
	///持仓量
	LargeVolumeType	OpenInterest;
	///成交金额
	MoneyType	Turnover;
};


///历史数据请求，这个需要后期再改，目前肯定无法使用
struct HistoricalDataRequestField
{
	int Date1;
	int Date2;
	int Time1;
	int Time2;

	DataObjetType DataType;
	BarType BarType;
	long BarSize;

	int RequestId;
	int CurrentDate;
	int lRequest;

	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType	ExchangeID;
};

#pragma pack(pop)//恢复对齐状态

#endif











#ifndef _QUEUE_ENUM_H_
#define _QUEUE_ENUM_H_

/// 基本请求
enum RequestType :char
{
	RequestType_GetApiType = 0,
	RequestType_GetApiVersion,
	RequestType_GetApiName,

	RequestType_Create, // 创建
	RequestType_Release, // 销毁
	RequestType_Register, // 注册接收队列回调
	RequestType_Config,		// 配置参数

	RequestType_Connect, // 开始/连接
	RequestType_Disconnect, // 停止/断开

	RequestType_Clear, // 清理
	RequestType_Process, // 处理

	RequestType_Subscribe,	// 订阅
	RequestType_Unsubscribe, // 取消订阅

	RequestType_SubscribeQuote, // 订阅询价
	RequestType_UnsubscribeQuote, // 取消订阅询价

	RequestType_ReqOrderInsert,
	RequestType_ReqQuoteInsert,
	RequestType_ReqOrderAction,
	RequestType_ReqQuoteAction,
};

///查询
enum QueryType :char
{
	QueryType_ReqQryInstrument = 32,
	QueryType_ReqQryTradingAccount,
	QueryType_ReqQryInvestorPosition,

	QueryType_ReqQryOrder,
	QueryType_ReqQryTrade,
	QueryType_ReqQryQuote,

	QueryType_ReqQryInstrumentCommissionRate,
	QueryType_ReqQryInstrumentMarginRate,
	QueryType_ReqQrySettlementInfo,
	QueryType_ReqQryInvestor,

	QueryType_ReqQryHistoricalTicks,
	QueryType_ReqQryHistoricalBars,
};

///响应
enum ResponeType :char
{
	ResponeType_OnConnectionStatus = 64,
	ResponeType_OnRtnError,
	ResponeType_OnLog,

	ResponeType_OnRtnDepthMarketData,
	ResponeType_OnRspQryInstrument,
	ResponeType_OnRspQryTradingAccount,
	ResponeType_OnRspQryInvestorPosition,
	ResponeType_OnRspQrySettlementInfo,

	ResponeType_OnRspQryOrder,
	ResponeType_OnRspQryTrade,
	ResponeType_OnRspQryQuote,

	ResponeType_OnRtnOrder,
	ResponeType_OnRtnTrade,
	ResponeType_OnRtnQuote,

	ResponeType_OnRtnQuoteRequest,

	ResponeType_OnRspQryHistoricalTicks,
	ResponeType_OnRspQryHistoricalBars,
	ResponeType_OnRspQryInvestor,

	ResponeType_OnFilterSubscribe,
};

// >=100表示API自定义

#endif











#ifndef _QUEUE_HEADER_H_
#define _QUEUE_HEADER_H_

//#include "CrossPlatform.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef void* (__stdcall *fnOnRespone) (char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1,int size1, void* ptr2,int size2, void* ptr3,int size3);

	//DLL_PUBLIC void* __stdcall XRespone(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
#ifdef __cplusplus
}
#endif

#endif//end

#ifdef __cplusplus
extern "C" {
#endif

void* X_LoadLib(const char* libPath);
const char* X_GetLastError();
void* X_GetFunction(void* lib, const char* ProcName = "XRequest");
void X_FreeLib(void* lib);

ApiType X_GetApiType(void* pFun);
const char* X_GetApiVersion(void* pFun);
const char* X_GetApiName(void* pFun);

void* X_Create(void* pFun);
void X_Register(void* pFun, void* pApi, fnOnRespone pCallback, void* pClass);

void X_Connect(void* pFun, void* pApi, const char* szPath, ServerInfoField* pServerInfo, UserInfoField* pUserInfo, int count);
void X_Disconnect(void* pFun, void* pApi);

void X_Subscribe(void* pFun, void* pApi, const char* szInstrument, const char* szExchange);
void X_Unsubscribe(void* pFun, void* pApi, const char* szInstrument, const char* szExchange);
void X_SubscribeQuote(void* pFun, void* pApi, const char* szInstrument, const char* szExchange);
void X_UnsubscribeQuote(void* pFun, void* pApi, const char* szInstrument, const char* szExchange);

void X_ReqQuery(void* pFun, void* pApi, QueryType type, ReqQueryField* query);

const char* X_SendOrder(void* pFun, void* pApi, OrderField* pOrder, int count, char* pOut);
const char* X_CancelOrder(void* pFun, void* pApi, OrderIDType* pIn, int count, char* pOut);

const char* X_SendQuote(void* pFun, void* pApi, QuoteField* pQuote, int count, char* pOut);
const char* X_CancelQuote(void* pFun, void* pApi, OrderIDType* pIn, int count, char* pOut);

#ifdef __cplusplus
}
#endif
