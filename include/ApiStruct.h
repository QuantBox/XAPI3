#ifndef _API_STRUCT_H_
#define _API_STRUCT_H_

#include "ApiDataType.h"
#include "ApiEnum.h"

#pragma pack(push,1)

//////////////////////////////////////////////////////////////////////////
///字段分为两大类
/// 1.由API直接提供有效值
/// 2.计算出来的值，字段正好预留了此字段，可以直接填充，这样使用者就不需要定义自己的字段了

/////////////////////////////////////////////////////////////////////////
///
/// 账号体系分成三层
/// 客户端XAPI层，主要是多账号功能，如TDX可以登录多个账号
/// 分账户层，如O32中下单要指定资产单元或投资组合
/// 交易所层，比如股东代码，有可能无法区分需要



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



	/// 基金
	IDChar32Type	PortfolioID1;
	/// 资产单元
	IDChar32Type	PortfolioID2;
	/// 投资组合
	IDChar32Type	PortfolioID3;

	BusinessType	Business;
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

	///同一账号下的唯一ID，本地拼接成的，方便其它程序存到dict中
	PositionIDType	ID;

	/// 基金
	IDChar32Type	PortfolioID1;
	/// 资产单元
	IDChar32Type	PortfolioID2;
	/// 投资组合
	IDChar32Type	PortfolioID3;

	BusinessType	Business;
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

	/// 基金
	IDChar32Type	PortfolioID1;
	/// 资产单元
	IDChar32Type	PortfolioID2;
	/// 投资组合
	IDChar32Type	PortfolioID3;

	BusinessType	Business;

	// 本处代码只内部使用，用于方便在封装API时快速的进行查找，减少代码
	void*			pUserData1;
	void*			pUserData2;
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

	/// 基金
	IDChar32Type	PortfolioID1;
	/// 资产单元
	IDChar32Type	PortfolioID2;
	/// 投资组合
	IDChar32Type	PortfolioID3;

	BusinessType	Business;
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
	///App代码
	AppIDType			AppID;
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

	Char32Type		Version;

	Char32Type		Lang;
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


/// 合约状态信息，CTP中其实是交易所信息
struct InstrumentStatusField
{
	///唯一符号
	SymbolType			Symbol;
	///合约代码
	InstrumentIDType	InstrumentID;
	///交易所代码
	ExchangeIDType		ExchangeID;
	///合约交易状态
	TradingPhaseType	InstrumentStatus;
	///进入本状态时间
	TimeIntType			EnterTime;
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
