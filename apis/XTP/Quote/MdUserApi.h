#ifndef XTP_MD_USER_API_H_
#define XTP_MD_USER_API_H_

#include <stdint.h>
#include <float.h>
#include <set>
#include <string>
#include <atomic>
#include <mutex>
#include <map>

#include "../../include/ApiStruct.h"
#include "Include.h"

using namespace std;

class CMsgQueue;

class CSubscribeManager;
class CSyntheticConfig;
class CSyntheticManager;
class CSyntheticCalculateFactory;

class CMdUserApi : public XTP::API::QuoteSpi
{
	enum RequestType
	{
		E_Init,
		E_Disconnect,
		E_ReqUserLoginField,
	};

public:
	CMdUserApi();
	~CMdUserApi();

	void Register(void* pCallback, void* pClass);
	ConfigInfoField* Config(ConfigInfoField* pConfigInfo);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo);
	void Disconnect();

	void Subscribe(const string& szInstrumentIDs, const string& szExchangeID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID);

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	int _Init();
	void _DisconnectInThread();

	//登录请求
	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	//订阅行情
	void Subscribe(const set<string>& instrumentIDs, const string& szExchangeID);
	void Unsubscribe(const set<string>& instrumentIDs, const string& szExchangeID);

public:
	virtual void OnDisconnected(int reason) override;

	virtual void OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last) override;
	virtual void OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last) override;
	virtual void OnDepthMarketData(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) override;

private:
	XTP::API::QuoteApi*			m_pApi;					//行情API
	atomic<int>					m_lRequestID;			//请求ID，每次请求前自增

	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;

	CSubscribeManager*			m_pSubscribeManager;
	CSubscribeManager*			m_pQuoteSubscribeManager;
	CSyntheticConfig*			m_pSyntheticConfig;
	CSyntheticManager*			m_pSyntheticManager;
	CSyntheticCalculateFactory*	m_pCalculateFactory;
};

#endif
