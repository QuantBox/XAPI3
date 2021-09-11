#pragma once

#include "../../include/CrossPlatform.h"
#include "../../include/ApiStruct.h"
#include "../../include/QueueEnum.h"

#include <set>
#include <list>
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>


#ifdef _WIN64
#pragma comment(lib, "../../include/Tap/win64/TapTradeAPI.lib")
#else
#pragma comment(lib, "../../include/Tap/win32/TapTradeAPI.lib")
#endif

#include "../../include/Tap/TapAPIError.h"
#include "../../include/Tap/TapTradeAPI.h"

#include "CUserConfig.h"
#include "CServerConfig.h"

using namespace std;

#include "OrderMap.h"

class CMsgQueue;
class CProcessor;
class PositionManager;

class CTraderApi :
	public ITapTradeAPINotify
{
	//请求数据包类型
	enum RequestType
	{
		E_Init = 100,
		E_ReqUserLoginField,

		E_QuerySymbolStatus,
		E_CancelOrder,
		E_QueryMarketSession,

		E_Heartbeat,
	};

public:
	CTraderApi(void);
	virtual ~CTraderApi(void);

	void Register(void* pCallback, void* pClass);

	void Connect(
		const char* szServerPath,
		const char* szUserPath,
		const char* szPath);
	void Disconnect();
	ConnectionStatus GetStatus();

	char* ReqOrderInsert(
		OrderField* pOrder,
		int count,
		char* pszLocalIDBuf);

	char* ReqOrderAction(OrderIDType* szIds, int count, char* pzsRtn);
	void ReqQuery(QueryType type, ReqQueryField* pQuery);

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	//void TestInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	//void CheckThenHeartbeat(time_t _now);

	void Clear();

	int _Init();

	//void ReqUserLogin();
	//int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	//void QuerySymbolStatus();
	//int _QuerySymbolStatus(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);


	int _QryFund(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	int _QryPosition(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	
	PositionField* OnPosition(const TapAPIPositionInfo* info);


	virtual void TAP_CDECL OnConnect();
	virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo* loginRspInfo);
	virtual void TAP_CDECL OnAPIReady();
	virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);
	virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode) {}
	virtual void TAP_CDECL OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info) {}
	virtual void TAP_CDECL OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo* info) {}
	virtual void TAP_CDECL OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData* info);
	virtual void TAP_CDECL OnRtnFund(const TapAPIFundData* info);
	virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo* info) {}
	virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICommodityInfo* info) {}
	virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeContractInfo* info);
	virtual void TAP_CDECL OnRtnContract(const TapAPITradeContractInfo* info) {}
	virtual void TAP_CDECL OnRtnOrder(const TapAPIOrderInfoNotice* info);
	virtual void TAP_CDECL OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp* info);
	virtual void TAP_CDECL OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo* info) {}
	virtual void TAP_CDECL OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo* info) {}
	virtual void TAP_CDECL OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo* info) {}
	virtual void TAP_CDECL OnRtnFill(const TapAPIFillInfo* info);
	virtual void TAP_CDECL OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo* info);
	virtual void TAP_CDECL OnRtnPosition(const TapAPIPositionInfo* info);
	virtual void TAP_CDECL OnRspQryClose(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo* info) {}
	virtual void TAP_CDECL OnRtnClose(const TapAPICloseInfo* info) {}
	virtual void TAP_CDECL OnRtnPositionProfit(const TapAPIPositionProfitNotice* info) {}
	virtual void TAP_CDECL OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIDeepQuoteQryRsp* info) {}
	virtual void TAP_CDECL OnRspQryExchangeStateInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeStateInfo* info);
	virtual void TAP_CDECL OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice* info);
	virtual void TAP_CDECL OnRtnReqQuoteNotice(const TapAPIReqQuoteNotice* info) {}//V9.0.2.0 20150520
	virtual void TAP_CDECL OnRspUpperChannelInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo* info);
	virtual void TAP_CDECL OnRspAccountRentInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountRentInfo* info) {}
	virtual void TAP_CDECL OnRspSubmitUserLoginInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPISubmitUserLoginRspInfo* info) {}
	virtual void TAP_CDECL OnRspQryBill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIBillQryRsp* info) {}
	virtual void TAP_CDECL OnRspQryAccountStorage(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountStorageInfo* info) {}
	virtual void TAP_CDECL OnRtnAccountStorage(const TapAPIAccountStorageInfo* info) {}


private:
	time_t						m_HeartbeatTime;
	//EES_LogonResponse			m_RspUserLogin;

	mutex						m_csOrderRef;
	mutex						m_csRecv;
	unsigned int				m_nMaxOrderRef;			//报单引用，用于区分报单，保持自增

	ITapTradeAPI*				m_pApi;					//交易API

	string						m_szPath;				//输出文件路径
	string						m_szServerPath;			//服务器配置路径
	string						m_szUserPath;			//账号配置路径

	ServerItem					m_ServerItem;
	UserItem					m_UserItem;
	int							m_nSleep;

	CMsgQueue* m_msgQueue;				//消息队列指针
	CMsgQueue* m_msgQueue_Query;
	void* m_pClass;


	COrderMap<string>* m_pOrderMap;			// 消息回报时正常处理
	CProcessor* m_pProcessor;
	PositionManager* m_pPositionManager;

	ConnectionStatus			m_Status;

	TAPIUINT32					m_uiSessionID;

	map<string, TapAPITradeContractInfo> m_Contracts;


	// 由于回报太多，用这种方法减少数量
	int							m_HHmmss;
	int							m_HHmmss_OnSymbolStatusReport;

	//// 交易所席位
	//unsigned char				m_SessionCount = 0;
	//EES_MarketSessionId			m_SessionId[255] = { 0 };
	//unsigned char				m_curr_session = 0;

	int m_nReqId = 1;
};

