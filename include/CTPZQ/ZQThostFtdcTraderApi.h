/////////////////////////////////////////////////////////////////////////
///@system 新一代交易所系统
///@company 上海期货信息技术有限公司
///@file ZQThostFtdcTraderApi.h
///@brief 定义了客户端接口
///@history 
///20060106	赵鸿昊		创建该文件
/////////////////////////////////////////////////////////////////////////

#if !defined(ZQTHOST_FTDCTRADERAPI_H)
#define ZQTHOST_FTDCTRADERAPI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZQThostFtdcUserApiStruct.h"

#if defined(ISLIB) && defined(WIN32)
#ifdef LIB_TRADER_API_EXPORT
#define TRADER_API_EXPORT __declspec(dllexport)
#else
#define TRADER_API_EXPORT __declspec(dllimport)
#endif
#else
#define TRADER_API_EXPORT 
#endif

class CZQThostFtdcTraderSpi
{
public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected(){};
	
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason){};
		
	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse){};
	
	///客户端认证响应
	virtual void OnRspAuthenticate(CZQThostFtdcRspAuthenticateField *pRspAuthenticateField, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};


	///错误应答
	virtual void OnRspError(CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///登录请求响应
	virtual void OnRspUserLogin(CZQThostFtdcRspUserLoginField *pRspUserLogin, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///登出请求响应
	virtual void OnRspUserLogout(CZQThostFtdcUserLogoutField *pUserLogout, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///用户口令更新响应
	virtual void OnRspUserPasswordUpdate(CZQThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///报单录入请求响应
	virtual void OnRspOrderInsert(CZQThostFtdcInputOrderField *pInputOrder, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///报单操作录入请求响应
	virtual void OnRspOrderAction(CZQThostFtdcInputOrderActionField *pInputOrderAction, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///报单通知
	virtual void OnRtnOrder(CZQThostFtdcOrderField *pOrder) {};

	///成交通知
	virtual void OnRtnTrade(CZQThostFtdcTradeField *pTrade) {};

	///报单录入错误回报
	virtual void OnErrRtnOrderInsert(CZQThostFtdcInputOrderField *pInputOrder, CZQThostFtdcRspInfoField *pRspInfo) {};

	///撤单操作错误回报
	virtual void OnErrRtnOrderAction(CZQThostFtdcOrderActionField *pOrderAction, CZQThostFtdcRspInfoField *pRspInfo) {};

	///资金转移回报
	virtual void OnRtnTransferFund(CZQThostFtdcTransferFundField *pTransferFund) {};

	///仓位转移回报
	virtual void OnRtnTransferPosition(CZQThostFtdcTransferPositionField *pTransferPosition) {};

	///请求查询交易所响应
	virtual void OnRspQryExchange(CZQThostFtdcExchangeField *pExchange, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询行情响应
	virtual void OnRspQryDepthMarketData(CZQThostFtdcDepthMarketDataField *pDepthMarketData, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询证券响应
	virtual void OnRspQrySecurity(CZQThostFtdcSecurityField *pSecurity, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询ETF清单响应
	virtual void OnRspQryETFFile(CZQThostFtdcETFFileField *pETFFile, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询ETF成分证券响应
	virtual void OnRspQryETFBasket(CZQThostFtdcETFBasketField *pETFBasket, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询新股信息响应
	virtual void OnRspQrySubscribingShares(CZQThostFtdcSubscribingSharesField *pSubscribingShares, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询质押申报代码
	virtual void OnRspQryPledgeOrderInfo(CZQThostFtdcPledgeOrderInfoField *pPledgeOrderInfo, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询市场响应
	virtual void OnRspQryMarket(CZQThostFtdcMarketField *pMarket, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询证券品种响应
	virtual void OnRspQrySecurityProduct(CZQThostFtdcSecurityProductField *pSecurityProduct, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询证券类别响应
	virtual void OnRspQrySecurityClass(CZQThostFtdcSecurityClassField *pSecurityClass, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询报单响应
	virtual void OnRspQryOrder(CZQThostFtdcOrderField *pOrder, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询成交响应
	virtual void OnRspQryTrade(CZQThostFtdcTradeField *pTrade, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CZQThostFtdcTradingAccountField *pTradingAccount, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资者证券持仓响应
	virtual void OnRspQryInvestorSecurityPosition(CZQThostFtdcInvestorSecurityPositionField *pInvestorSecurityPosition, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资者质押持仓响应
	virtual void OnRspQryInvestorPledgePosition(CZQThostFtdcInvestorPledgePositionField *pInvestorPledgePosition, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资者质押回购响应
	virtual void OnRspQryInvestorPledgeInfo(CZQThostFtdcInvestorPledgeInfoField *pInvestorPledgeInfo, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询报单资金明细响应
	virtual void OnRspQryOrderFundDetail(CZQThostFtdcOrderFundDetailField *pOrderFundDetail, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资者响应
	virtual void OnRspQryInvestor(CZQThostFtdcInvestorField *pInvestor, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询交易账户响应
	virtual void OnRspQryTradingCode(CZQThostFtdcTradingCodeField *pTradingCode, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资单元响应
	virtual void OnRspQryInvestUnit(CZQThostFtdcInvestUnitField *pInvestUnit, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资单元与交易资金账户关系响应
	virtual void OnRspQryInvestUnitAndTradingAcct(CZQThostFtdcInvestUnitAndTradingAcctField *pInvestUnitAndTradingAcct, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资单元与交易用户响应
	virtual void OnRspQryInvestUnitAndUser(CZQThostFtdcInvestUnitAndUserField *pInvestUnitAndUser, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资者佣金响应
	virtual void OnRspQryBrokerage(CZQThostFtdcBrokerageField *pBrokerage, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询新股申购额度响应
	virtual void OnRspQrySubscribingSharesQuota(CZQThostFtdcSubscribingSharesQuotaField *pSubscribingSharesQuota, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询标准券使用率响应
	virtual void OnRspQryRepoRatio(CZQThostFtdcRepoRatioField *pRepoRatio, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询回购放大倍数响应
	virtual void OnRspQryRepurchaseMaxTimes(CZQThostFtdcRepurchaseMaxTimesField *pRepurchaseMaxTimes, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询资金转移响应
	virtual void OnRspQryTransferFund(CZQThostFtdcTransferFundField *pTransferFund, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询仓位转移响应
	virtual void OnRspQryTransferPosition(CZQThostFtdcTransferPositionField *pTransferPosition, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///行权录入响应
	virtual void OnRspExecOrderInsert(CZQThostFtdcInputExecOrderField *pInputExecOrder, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///行权通知
	virtual void OnRtnExecOrder(CZQThostFtdcExecOrderField *pExecOrder) {};

	///行权录入错误通知
	virtual void OnErrRtnExecOrderInsert(CZQThostFtdcInputExecOrderField *pInputExecOrder, CZQThostFtdcRspInfoField *pRspInfo) {};

	///行权操作录入响应
	virtual void OnRspExecOrderAction(CZQThostFtdcInputExecOrderActionField *pInputExecOrderAction, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///行权操作录入错误通知
	virtual void OnErrRtnExecOrderAction(CZQThostFtdcExecOrderActionField *pExecOrderAction, CZQThostFtdcRspInfoField *pRspInfo) {};

	///锁定录入响应
	virtual void OnRspLockInsert(CZQThostFtdcInputLockField *pInputLock, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///锁定通知
	virtual void OnRtnLock(CZQThostFtdcLockField *pLock) {};

	///锁定录入错误通知
	virtual void OnErrRtnLockInsert(CZQThostFtdcInputLockField *pInputLock, CZQThostFtdcRspInfoField *pRspInfo) {};

	///现券转移响应
	virtual void OnRspSecurityTransfer(CZQThostFtdcInputSecurityTransferField *pInputSecurityTransfer, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///现券转移通知
	virtual void OnRtnSecurityTransfer(CZQThostFtdcSecurityTransferField *pSecurityTransfer) {};

	///现券转移错误通知
	virtual void OnErrRtnSecurityTransfer(CZQThostFtdcInputSecurityTransferField *pInputSecurityTransfer, CZQThostFtdcRspInfoField *pRspInfo) {};

	///个股期权合约查询响应
	virtual void OnRspQryInstrument(CZQThostFtdcInstrumentField *pInstrument, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///锁定查询响应
	virtual void OnRspQryLock(CZQThostFtdcLockField *pLock, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///执行宣告查询响应
	virtual void OnRspQryExecOrder(CZQThostFtdcExecOrderField *pExecOrder, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///锁定持仓查询响应
	virtual void OnRspQryLockPosition(CZQThostFtdcLockPositionField *pLockPosition, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///投资者保证金率查询响应
	virtual void OnRspQryMarginRate(CZQThostFtdcMarginRateField *pMarginRate, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///投资者买入额度查询响应
	virtual void OnRspQryLimitAmount(CZQThostFtdcLimitAmountField *pLimitAmount, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///投资者持仓限制查询响应
	virtual void OnRspQryLimitPosition(CZQThostFtdcLimitPositionField *pLimitPosition, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///行权指派查询响应
	virtual void OnRspQryExerciseAssignment(CZQThostFtdcExerciseAssignmentField *pExerciseAssignment, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///投资者合约持仓查询响应
	virtual void OnRspQryInvestorPosition(CZQThostFtdcInvestorPositionField *pInvestorPosition, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///现券转移查询响应
	virtual void OnRspQrySecurityTransfer(CZQThostFtdcSecurityTransferField *pSecurityTransfer, CZQThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
};

class TRADER_API_EXPORT CZQThostFtdcTraderApi
{
public:
	///创建TraderApi
	///@param pszFlowPath 存贮订阅信息文件的目录，默认为当前目录
	///@return 创建出的UserApi
	//modify for udp marketdata
	static CZQThostFtdcTraderApi *CreateFtdcTraderApi(const char *pszFlowPath = "", const bool bIsUsingUdp=false);
	
	///删除接口对象本身
	///@remark 不再使用本接口对象时,调用该函数删除接口对象
	virtual void Release() = 0;
	
	///初始化
	///@remark 初始化运行环境,只有调用后,接口才开始工作
	virtual void Init() = 0;
	
	///等待接口线程结束运行
	///@return 线程退出代码
	virtual int Join() = 0;
	
	///注册前置机网络地址
	///@param pszFrontAddress：前置机网络地址。
	///@remark 网络地址的格式为：“protocol://ipaddress:port”，如：”tcp://127.0.0.1:17001”。 
	///@remark “tcp”代表传输协议，“127.0.0.1”代表服务器地址。”17001”代表服务器端口号。
	virtual void RegisterFront(char *pszFrontAddress) = 0;
	
	///注册回调接口
	///@param pSpi 派生自回调接口类的实例
	virtual void RegisterSpi(CZQThostFtdcTraderSpi *pSpi) = 0;
	
	///订阅私有流。
	///@param nResumeType 私有流重传方式  
	///        THOST_TERT_RESTART:从本交易日开始重传
	///        THOST_TERT_RESUME:从上次收到的续传
	///        THOST_TERT_QUICK:只传送登录后私有流的内容
	///@remark 该方法要在Init方法前调用。若不调用则不会收到私有流的数据。
	virtual void SubscribePrivateTopic(ZQTHOST_TE_RESUME_TYPE nResumeType) = 0;
	
	///订阅公共流。
	///@param nResumeType 公共流重传方式  
	///        THOST_TERT_RESTART:从本交易日开始重传
	///        THOST_TERT_RESUME:从上次收到的续传
	///        THOST_TERT_QUICK:只传送登录后公共流的内容
	///@remark 该方法要在Init方法前调用。若不调用则不会收到公共流的数据。
	virtual void SubscribePublicTopic(ZQTHOST_TE_RESUME_TYPE nResumeType) = 0;

	///客户端认证请求
	virtual int ReqAuthenticate(CZQThostFtdcReqAuthenticateField *pReqAuthenticateField, int nRequestID) = 0;

	///用户登录请求
	virtual int ReqUserLogin(CZQThostFtdcReqUserLoginField *pReqUserLoginField, int nRequestID) = 0;
	

	///登出请求
	virtual int ReqUserLogout(CZQThostFtdcUserLogoutField *pUserLogout, int nRequestID) = 0;

	///用户口令更新请求
	virtual int ReqUserPasswordUpdate(CZQThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID) = 0;

	///报单录入请求
	virtual int ReqOrderInsert(CZQThostFtdcInputOrderField *pInputOrder, int nRequestID) = 0;

	///报单操作录入请求
	virtual int ReqOrderAction(CZQThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID) = 0;

	///请求查询交易所
	virtual int ReqQryExchange(CZQThostFtdcQryExchangeField *pQryExchange, int nRequestID) = 0;

	///请求查询行情
	virtual int ReqQryDepthMarketData(CZQThostFtdcQryDepthMarketDataField *pQryDepthMarketData, int nRequestID) = 0;

	///请求查询证券
	virtual int ReqQrySecurity(CZQThostFtdcQrySecurityField *pQrySecurity, int nRequestID) = 0;

	///请求查询ETF清单
	virtual int ReqQryETFFile(CZQThostFtdcQryETFFileField *pQryETFFile, int nRequestID) = 0;

	///请求查询ETF成分证券
	virtual int ReqQryETFBasket(CZQThostFtdcQryETFBasketField *pQryETFBasket, int nRequestID) = 0;

	///请求查询新股信息
	virtual int ReqQrySubscribingShares(CZQThostFtdcQrySubscribingSharesField *pQrySubscribingShares, int nRequestID) = 0;

	///请求查询质押申报代码
	virtual int ReqQryPledgeOrderInfo(CZQThostFtdcQryPledgeOrderInfoField *pQryPledgeOrderInfo, int nRequestID) = 0;

	///请求市场
	virtual int ReqQryMarket(CZQThostFtdcQryMarketField *pQryMarket, int nRequestID) = 0;

	///请求查询证券品种
	virtual int ReqQrySecurityProduct(CZQThostFtdcQrySecurityProductField *pQrySecurityProduct, int nRequestID) = 0;

	///请求查询证券类别
	virtual int ReqQrySecurityClass(CZQThostFtdcQrySecurityClassField *pQrySecurityClass, int nRequestID) = 0;

	///请求查询报单
	virtual int ReqQryOrder(CZQThostFtdcQryOrderField *pQryOrder, int nRequestID) = 0;

	///请求查询成交
	virtual int ReqQryTrade(CZQThostFtdcQryTradeField *pQryTrade, int nRequestID) = 0;

	///请求查询资金账户
	virtual int ReqQryTradingAccount(CZQThostFtdcQryTradingAccountField *pQryTradingAccount, int nRequestID) = 0;

	///请求查询投资者证券持仓
	virtual int ReqQryInvestorSecurityPosition(CZQThostFtdcQryInvestorSecurityPositionField *pQryInvestorSecurityPosition, int nRequestID) = 0;

	///请求查询投资者质押持仓
	virtual int ReqQryInvestorPledgePosition(CZQThostFtdcQryInvestorPledgePositionField *pQryInvestorPledgePosition, int nRequestID) = 0;

	///请求查询投资者质押回购
	virtual int ReqQryInvestorPledgeInfo(CZQThostFtdcQryInvestorPledgeInfoField *pQryInvestorPledgeInfo, int nRequestID) = 0;

	///请求查询报单资金明细
	virtual int ReqQryOrderFundDetail(CZQThostFtdcQryOrderFundDetailField *pQryOrderFundDetail, int nRequestID) = 0;

	///请求查询投资者
	virtual int ReqQryInvestor(CZQThostFtdcQryInvestorField *pQryInvestor, int nRequestID) = 0;

	///请求查询交易账户
	virtual int ReqQryTradingCode(CZQThostFtdcQryTradingCodeField *pQryTradingCode, int nRequestID) = 0;

	///请求查询投资单元
	virtual int ReqQryInvestUnit(CZQThostFtdcQryInvestUnitField *pQryInvestUnit, int nRequestID) = 0;

	///请求查询投资单元与交易资金账户关系
	virtual int ReqQryInvestUnitAndTradingAcct(CZQThostFtdcQryInvestUnitAndTradingAcctField *pQryInvestUnitAndTradingAcct, int nRequestID) = 0;

	///请求查询投资单元与交易用户关系
	virtual int ReqQryInvestUnitAndUser(CZQThostFtdcQryInvestUnitAndUserField *pQryInvestUnitAndUser, int nRequestID) = 0;

	///请求查询投资者佣金
	virtual int ReqQryBrokerage(CZQThostFtdcQryBrokerageField *pQryBrokerage, int nRequestID) = 0;

	///请求查询新股申购额度
	virtual int ReqQrySubscribingSharesQuota(CZQThostFtdcQrySubscribingSharesQuotaField *pQrySubscribingSharesQuota, int nRequestID) = 0;

	///请求查询标准券使用率
	virtual int ReqQryRepoRatio(CZQThostFtdcQryRepoRatioField *pQryRepoRatio, int nRequestID) = 0;

	///请求查询回购放大倍数
	virtual int ReqQryRepurchaseMaxTimes(CZQThostFtdcQryRepurchaseMaxTimesField *pQryRepurchaseMaxTimes, int nRequestID) = 0;

	///请求查询资金转移
	virtual int ReqQryTransferFund(CZQThostFtdcQryTransferFundField *pQryTransferFund, int nRequestID) = 0;

	///请求查询仓位转移
	virtual int ReqQryTransferPosition(CZQThostFtdcQryTransferPositionField *pQryTransferPosition, int nRequestID) = 0;

	///请求行权录入
	virtual int ReqExecOrderInsert(CZQThostFtdcInputExecOrderField *pInputExecOrder, int nRequestID) = 0;

	///请求行权操作录入
	virtual int ReqExecOrderAction(CZQThostFtdcInputExecOrderActionField *pInputExecOrderAction, int nRequestID) = 0;

	///请求锁定录入
	virtual int ReqLockInsert(CZQThostFtdcInputLockField *pInputLock, int nRequestID) = 0;

	///请求现券转移录入
	virtual int ReqSecurityTransfer(CZQThostFtdcInputSecurityTransferField *pInputSecurityTransfer, int nRequestID) = 0;

	///个股期权合约查询
	virtual int ReqQryInstrument(CZQThostFtdcQryInstrumentField *pQryInstrument, int nRequestID) = 0;

	///锁定查询
	virtual int ReqQryLock(CZQThostFtdcQryLockField *pQryLock, int nRequestID) = 0;

	///执行宣告查询
	virtual int ReqQryExecOrder(CZQThostFtdcQryExecOrderField *pQryExecOrder, int nRequestID) = 0;

	///锁定持仓查询
	virtual int ReqQryLockPosition(CZQThostFtdcQryLockPositionField *pQryLockPosition, int nRequestID) = 0;

	///投资者保证金率查询
	virtual int ReqQryMarginRate(CZQThostFtdcQryMarginRateField *pQryMarginRate, int nRequestID) = 0;

	///投资者买入额度查询
	virtual int ReqQryLimitAmount(CZQThostFtdcQryLimitAmountField *pQryLimitAmount, int nRequestID) = 0;

	///投资者持仓限制查询
	virtual int ReqQryLimitPosition(CZQThostFtdcQryLimitPositionField *pQryLimitPosition, int nRequestID) = 0;

	///行权指派查询
	virtual int ReqQryExerciseAssignment(CZQThostFtdcQryExerciseAssignmentField *pQryExerciseAssignment, int nRequestID) = 0;

	///投资者合约持仓查询
	virtual int ReqQryInvestorPosition(CZQThostFtdcQryInvestorPositionField *pQryInvestorPosition, int nRequestID) = 0;

	///现券转移查询
	virtual int ReqQrySecurityTransfer(CZQThostFtdcQrySecurityTransferField *pQrySecurityTransfer, int nRequestID) = 0;
protected:
	~CZQThostFtdcTraderApi(){};
};

#endif
