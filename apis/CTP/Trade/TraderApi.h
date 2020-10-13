#pragma once

#include "../../include/ApiStruct.h"
#include "../../include/QueueEnum.h"
// 由于这个Include.h需要使用每个项目自己的文件，所以需要在VC++ Directories->Include Directories中添加一个"./"
#include "Include.h"




#include <set>
#include <list>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>

using namespace std;

class CMsgQueue;

#ifdef ENABLE_LICENSE
class CLicense;
#endif


class CTraderApi :
	public CThostFtdcTraderSpi
{
	//请求数据包类型
	enum RequestType
	{
		E_Init = 100,
		E_Disconnect,
		E_ReqAuthenticateField,
		E_ReqUserLoginField,
		E_UserLogoutField,
		E_SettlementInfoConfirmField,

		E_QryInvestorField,

		E_QryInstrumentField,

		E_InputOrderField,
		E_InputOrderActionField,
		E_InputQuoteField,
		E_InputQuoteActionField,
		E_ParkedOrderField,
		E_QryTradingAccountField,
		E_QryInvestorPositionField,
		E_QryInvestorPositionDetailField,
		E_QryInstrumentCommissionRateField,
		E_QryInstrumentMarginRateField,
		E_QryDepthMarketDataField,
		E_QrySettlementInfoField,
		E_QryOrderField,
		E_QryTradeField,
		E_QryQuoteField,
		E_ReqUserPasswordUpdateField,
	};

public:
	CTraderApi(void);
	virtual ~CTraderApi(void);

	void Register(void* pCallback,void* pClass);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo,
		int count);
	void Disconnect();

	char* ReqOrderInsert(
		OrderField* pOrder,
		int count,
		char* pszLocalIDBuf);

	char* ReqParkedOrderInsert(int OrderRef,
		OrderField* pOrder1,
		OrderField* pOrder2);

	char* ReqOrderAction(OrderIDType* szIds, int count, char* pzsRtn);
	int ReqOrderAction(OrderField *pOrder, int count, OrderIDType* pOutput);
	char* ReqOrderAction(CThostFtdcOrderField *pOrder, int count, char* pzsRtn);

#ifdef HAS_Quote
	char* ReqQuoteInsert(
		QuoteField* pQuote,
		OrderIDType* pAskRef,
		OrderIDType* pBidRef);

	int ReqQuoteAction(CThostFtdcQuoteField *pQuote, OrderIDType* pOutput);
	int ReqQuoteAction(const string& szId, OrderIDType* pOutput);
#endif // HAS_Quote

	//void ReqQryTradingAccount();
	//void ReqQryInvestorPosition(const string& szInstrumentId, const string& szExchange);
	////void ReqQryInvestorPositionDetail(const string& szInstrumentId);
	//void ReqQryInstrument(ReqQueryField* pQuery);
	void ReqQuery(QueryType type,ReqQueryField* pQuery);
	//void ReqQryInstrumentCommissionRate(const string& szInstrumentId);
	//void ReqQryInstrumentMarginRate(const string& szInstrumentId,TThostFtdcHedgeFlagType HedgeFlag = THOST_FTDC_HF_Speculation);
	//void ReqQryDepthMarketData(const string& szInstrumentId);
	//void ReqQrySettlementInfo(const string& szTradingDay);

	//void ReqQryOrder();
	//void ReqQryTrade();
	//void ReqQryQuote();

	//void ReqQryInvestor();

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	void Clear();

	int _Init();
	void _Disconnect(bool IsInQueue);
	void _DisconnectInThread();

	void ReqAuthenticate();
	int _ReqAuthenticate(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	void ReqUserLogout();
	int _ReqUserLogout(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	void ReqUserPasswordUpdate(char* szOldPassword, char* szNewPassword);
	int _ReqUserPasswordUpdate(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	


#ifdef HAS_Settlement
	void ReqSettlementInfoConfirm();
	int _ReqSettlementInfoConfirm(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQrySettlementInfo(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
#endif // HAS_Settlement

	
	int _ReqQryInstrument(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryTradingAccount(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryInvestorPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryInvestor(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	int _ReqQryOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryTrade(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	void OnOrder(CThostFtdcOrderField *pOrder, int nRequestID, bool bIsLast);
	void OnTrade(CThostFtdcTradeField *pTrade, int nRequestID, bool bIsLast);
	

	void OnTrade(TradeField *pTrade);

	//检查是否出错
	bool IsErrorRspInfo(const char* szSource, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//向消息队列输出信息
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);//不输出信息

	//连接
	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);

	//认证
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//下单
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

	//撤单
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	//报单回报
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	//成交回报
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

#ifdef HAS_Quote
	int _ReqQryQuote(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void OnQuote(CThostFtdcQuoteField *pQuote, int nRequestID, bool bIsLast);

	//报价录入
	virtual void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo);
	virtual void OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnQuote(CThostFtdcQuoteField *pQuote);

	//报价撤单
	virtual void OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction, CThostFtdcRspInfoField *pRspInfo);

#endif // HAS_Quote

#ifdef HAS_Settlement
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//请求查询投资者结算结果响应
	virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

#endif // HAS_Settlement

	
	//仓位
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//资金
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//合约、手续费
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//查询行情响应
	//virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	
	//其它
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
#ifdef HAS_InstrumentStatus
	virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);
#endif // HAS_InstrumentStatus

	

	// 今昨分开
	void GetPositionID(CThostFtdcInvestorPositionField *pInvestorPosition, PositionIDType positionId);
	// 今昨一起
	void GetPositionID2(CThostFtdcInvestorPositionField *pInvestorPosition, PositionIDType positionId);

	void QueryOrderTrade(bool bForceQuery);

private:
	//bool						m_delete;
	atomic<int>					m_lRequestID;			//请求ID,得保持自增

	CThostFtdcRspUserLoginField m_RspUserLogin;			//返回的登录成功响应，目前利用此内成员进行报单所属区分
	CThostFtdcInvestorField		m_Investor;

	OrderIDType					m_orderInsert_Id;
	OrderIDType					m_orderAction_Id;

	mutex						m_csOrderRef;
	int							m_nMaxOrderRef;			//报单引用，用于区分报单，保持自增

	CThostFtdcTraderApi*		m_pApi;					//交易API
	
	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;
	
	unordered_map<string, OrderField*>				m_id_platform_order;
	unordered_map<string, CThostFtdcOrderField*>		m_id_api_order;
	unordered_map<string, string>					m_sysId_orderId;

#ifdef HAS_Quote
	unordered_map<string, QuoteField*>				m_id_platform_quote;
	unordered_map<string, CThostFtdcQuoteField*>		m_id_api_quote;
	unordered_map<string, string>					m_sysId_quoteId;
#endif // HAS_Quote

	unordered_map<string, PositionField*>			m_id_platform_position;
	unordered_map<string, CThostFtdcInvestorPositionField*>			m_id_api_position;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;

#ifdef ENABLE_LICENSE
	CLicense*					m_pLicense;
#endif					
};

