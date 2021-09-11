#pragma once

#include "../../include/CrossPlatform.h"
#include "../../include/ApiStruct.h"
#include "../../include/QueueEnum.h"

#include <set>
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <map>


#ifdef _WIN64
	#pragma comment(lib, "../../include/Tap/win64/TapQuoteAPI.lib)
#else
	#pragma comment(lib, "../../include/Tap/win32/TapQuoteAPI.lib")
#endif

#include <string.h>
#include "../../include/Tap/TapAPIError.h"
#include "../../include/Tap/TapQuoteAPI.h"


#include "CUserConfig.h"
#include "CServerConfig.h"

using namespace std;

class CMsgQueue;

class CMdUserApi :
	public ITapQuoteAPINotify
{
	//请求数据包类型
	enum RequestType
	{
		E_Init = 100,
		E_ReqUserLoginField,

	};

public:
	CMdUserApi(void);
	virtual ~CMdUserApi(void);

	void Register(void* pCallback,void* pClass);

	void Connect(
		const char* szServerPath,
		const char* szUserPath,
		const char* szPath);
	void Disconnect();
	ConnectionStatus GetStatus();

	//订阅行情
	void Subscribe(const string& szInstrumentIDs, const string& szExchangeID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID);
	void Subscribe(const set<string>& instrumentIDs, const string& szExchangeID);
	void Unsubscribe(const set<string>& instrumentIDs, const string& szExchangeID);

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	void Clear();

	int _Init();
	
	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
private:
	virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo* info);
	virtual void TAP_CDECL OnAPIReady();
	virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);
	virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo* info) {}
	virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo* info);
	virtual void TAP_CDECL OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole* info);
	virtual void TAP_CDECL OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract* info);
	virtual void TAP_CDECL OnRtnQuote(const TapAPIQuoteWhole* info);

private:
	// 交易日
	int							m_TradingDay;
	int							m_PreTradingDay;

	ITapQuoteAPI*				m_pApi;					//交易API
	
	string						m_szPath;				//输出文件路径
	string						m_szServerPath;			//服务器配置路径
	string						m_szUserPath;			//账号配置路径

	ServerItem					m_ServerItem;
	UserItem					m_UserItem;
	int							m_nSleep;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;

	ConnectionStatus			m_Status;

	TAPIUINT32					m_uiSessionID;

	map<string, TapAPIContract> m_Contracts;
};

