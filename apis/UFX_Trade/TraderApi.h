#pragma once

#include "../../include/ApiStruct.h"
#include "../../include/QueueEnum.h"

#include "../../include/UFX/t2sdk_interface.h"

//#include "ufx_struct.h"

#include <set>
#include <list>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>


#ifdef _WIN64
	#pragma comment(lib, "../../include/UFX/win64/t2sdk.lib")
#else
	#pragma comment(lib, "../../include/UFX/win32/t2sdk.lib")
#endif

using namespace std;

#include "O32/ufx_struct_o32.h"
#include "OrderMap.h"

class CMsgQueue;
class CO32Codec;
class CO32Processor;
class CIDGenerator;

#ifdef ENABLE_LICENSE
class CLicense;
#endif


class CTraderApi :
	public CCallbackInterface,
	public CSubCallbackInterface
{
	//请求数据包类型
	enum RequestType
	{
		E_Init = 100,
		E_ReqAuthenticateField,
		E_ReqUserLoginField,
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

		E_Heartbeat,
		E_ReqSub,

		E_OnReceivedBizMsg, // 交易中心
		E_OnReceived, // 消息中心
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

	//char* ReqParkedOrderInsert(int OrderRef,
	//	OrderField* pOrder1,
	//	OrderField* pOrder2);

	char* ReqOrderAction(OrderIDType* szIds, int count, char* pzsRtn);
	int ReqOrderAction(OrderField *pOrder, int count, OrderIDType* pOutput);
	//char* ReqOrderAction(CThostFtdcOrderField *pOrder, int count, char* pzsRtn);

	//char* ReqQuoteInsert(
	//	QuoteField* pQuote,
	//	OrderIDType* pAskRef,
	//	OrderIDType* pBidRef);

	//int ReqQuoteAction(CThostFtdcQuoteField *pQuote, OrderIDType* pOutput);
	//int ReqQuoteAction(const string& szId, OrderIDType* pOutput);

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

	void Subscribe(const string& szInstrumentIDs, const string& szExchangeID);

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	friend void* __stdcall Recv(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void RecvInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	virtual void TestInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _OnReceivedBizMsg(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _OnReceived(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	void Clear();

	int _Init();
	int _Init2();

	void ReqSub();
	int _Sub();
	int _UnSub();

	/*void ReqAuthenticate();
	int _ReqAuthenticate(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);*/

	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	/*
	1.与TDX一样，要对协议进行解析
	2.与TDX一样，接收到的信息可能不全，没办法只通过包它还原数据包
	3.为了撤单，必须要一定量的信息才能操作
	*/
	void CheckThenHeartbeat(time_t _now);
	void OnHeartbeat(IBizMessage* lpMsg);
	int ReqHeartbeat();

	void ReqSettlementInfoConfirm();
	int _ReqSettlementInfoConfirm(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _ReqQryInstrument(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryTradingAccount(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryInvestorPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryInvestor(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQrySettlementInfo(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _ReqQryOrder(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryTrade(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _ReqQryQuote(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	virtual unsigned long  FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv);

	virtual unsigned long  FUNCTION_CALL_MODE AddRef();

	virtual unsigned long  FUNCTION_CALL_MODE Release();

	virtual void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection);

	virtual void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection);

	virtual void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection);

	virtual void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection);

	virtual void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData);

	virtual void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d);

	virtual void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d);

	virtual int  FUNCTION_CALL_MODE Reserved3();

	virtual void FUNCTION_CALL_MODE Reserved4();

	virtual void FUNCTION_CALL_MODE Reserved5();

	virtual void FUNCTION_CALL_MODE Reserved6();

	virtual void FUNCTION_CALL_MODE Reserved7();

	virtual void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult);

	virtual void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult);

	virtual void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg);

	// ========================================
	virtual void FUNCTION_CALL_MODE OnReceived(CSubscribeInterface *lpSub, int subscribeIndex, const void *lpData, int nLength, LPSUBSCRIBE_RECVDATA lpRecvData);
	virtual void FUNCTION_CALL_MODE OnRecvTickMsg(CSubscribeInterface *lpSub, int subscribeIndex, const char* TickMsgInfo);

private:
	atomic<int>					m_lRequestID;			//请求ID,得保持自增

	//REQ_DL_331100				m_ReqUserLogin_331100;
	//RSP_DL_331100				m_RspUserLogin_331100;			//返回的登录成功响应，目前利用此内成员进行报单所属区分

	REQ_DL_10001				m_ReqUserLogin_10001;
	RSP_DL_10001				m_RspUserLogin_10001;

	CO32Codec					*m_pO32Codec;
	CO32Processor				*m_pO32Processor;

	mutex						m_csOrderRef;
	mutex						m_csRecv;
	int							m_nMaxOrderRef;			//报单引用，用于区分报单，保持自增

	//CThostFtdcTraderApi*		m_pApi;					//交易API
	CConfigInterface*			m_lpConfig;
	CConnectionInterface*		m_lpConnection;

	CConfigInterface*			m_lpMCConfig;
	CConnectionInterface*		m_lpMCConnection;

	CSubscribeInterface*		m_lpSub;
	int							m_subscribeIndex;
	
	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	CMsgQueue*					m_msgQueue_Recv;
	void*						m_pClass;


	COrderMap<int>*				m_pOrderMap;			// 消息回报时正常处理
	CIDGenerator*				m_pIDGenerator;

	time_t						m_HeartbeatTime;
	//ConnectionStatus			m_ConnectionStatus;
#ifdef ENABLE_LICENSE
	CLicense*					m_pLicense;
#endif					
};

