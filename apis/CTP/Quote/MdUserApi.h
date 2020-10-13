#pragma once

#include "../../include/ApiStruct.h"

// 由于这个Include.h需要使用每个项目自己的文件，所以需要在VC++ Directories->Include Directories中添加一个"./"
#include "Include.h"

#include <string>
#include <atomic>
#include <mutex>
#include <set>


using namespace std;

class CMsgQueue;

class CSubscribeManager;
class CSyntheticConfig;
class CSyntheticManager;
class CSyntheticCalculateFactory;

class CMdUserApi :
	public CThostFtdcMdSpi
{
	enum RequestType
	{
		E_Init,
		E_Disconnect,
		E_ReqUserLoginField,
	};

public:
	CMdUserApi(void);
	virtual ~CMdUserApi(void);

	void Register(void* pCallback, void* pClass);
	ConfigInfoField* Config(ConfigInfoField* pConfigInfo);

	void Connect(const string& szPath,
		ServerInfoField* pServerInfo,
		UserInfoField* pUserInfo,
		int count);
	void Disconnect();

	void Subscribe(const string& szInstrumentIDs, const string& szExchangeID);
	void Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID);

#ifdef HAS_Quote
	void SubscribeQuote(const string& szInstrumentIDs, const string& szExchangeID);
	void UnsubscribeQuote(const string& szInstrumentIDs, const string& szExchangeID);
#endif // HAS_Quote

private:
	friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
	
	int _Init();
	void _Disconnect(bool IsInQueue);
	void _DisconnectInThread();

	//登录请求
	void ReqUserLogin();
	int _ReqUserLogin(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	//订阅行情
	void Subscribe(const set<string>& instrumentIDs, const string& szExchangeID);
	void Unsubscribe(const set<string>& instrumentIDs, const string& szExchangeID);
	void SubscribeQuote(const set<string>& instrumentIDs, const string& szExchangeID);
	void UnsubscribeQuote(const set<string>& instrumentIDs, const string& szExchangeID);

	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	//检查是否出错
	bool IsErrorRspInfo(const char* szSource, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//将出错消息送到消息队列
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);//不送出错消息

#ifdef HAS_Quote
	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);
#endif // HAS_Quote

private:
	//bool						m_delete;

	atomic<int>					m_lRequestID;			//请求ID，每次请求前自增




	CThostFtdcMdApi*			m_pApi;					//行情API

	string						m_szPath;				//生成配置文件的路径
	ServerInfoField				m_ServerInfo;
	UserInfoField				m_UserInfo;
	int							m_nSleep;

	CMsgQueue*					m_msgQueue;				//消息队列指针
	CMsgQueue*					m_msgQueue_Query;
	void*						m_pClass;

	// 交易日
	int							m_TradingDay;
	int							m_PreTradingDay;

	CSubscribeManager*			m_pSubscribeManager;
	CSubscribeManager*			m_pQuoteSubscribeManager;
	CSyntheticConfig*			m_pSyntheticConfig;
	CSyntheticManager*			m_pSyntheticManager;
	CSyntheticCalculateFactory*	m_pCalculateFactory;
};

