#pragma once

#include "../../include/CrossPlatform.h"
#include "../../include/ApiStruct.h"
#include "../../include/QueueEnum.h"

#include <set>
#include <string>
#include <mutex>
#include <atomic>
#include <thread>


#ifdef _WIN64
	#pragma comment(lib, "../../include/REM/win64/EESQuoteApi.lib")
#else
	#pragma comment(lib, "../../include/REM/win32/EESQuoteApi.lib")
#endif

#include <string.h>
#include "../../include/REM/EESQuoteDefine.h"
#include "../../include/REM/EESQuoteApi.h"

#ifdef WIN32
#include <Windows.h>
typedef HMODULE		T_DLL_HANDLE;
#else
typedef void* T_DLL_HANDLE;
#endif

#include "CUserConfig.h"
#include "CServerConfig.h"

using namespace std;

class CMsgQueue;

class CMdUserApi :
	public EESQuoteEvent
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
	/// \brief 当服务器连接成功，登录前调用, 如果是组播模式不会发生, 只需判断InitMulticast返回值即可
	virtual void OnEqsConnected();

	/// \brief 当服务器曾经连接成功，被断开时调用，组播模式不会发生该事件
	virtual void OnEqsDisconnected();

	/// \brief 当登录成功或者失败时调用，组播模式不会发生
	/// \param bSuccess 登陆是否成功标志  
	/// \param pReason  登陆失败原因  
	virtual void OnLoginResponse(bool bSuccess, const char* pReason);

	/// \brief 收到行情时调用,具体格式根据instrument_type不同而不同
	/// \param chInstrumentType  EES行情类型
	/// \param pDepthQuoteData   EES统一行情指针  
	virtual void OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData);

	/// \brief 日志接口，让使用者帮助写日志。
	/// \param nlevel    日志级别
	/// \param pLogText  日志内容
	/// \param nLogLen   日志长度
	virtual void OnWriteTextLog(EesEqsLogLevel nlevel, const char* pLogText, int nLogLen) {}

	/// \brief 注册symbol响应消息来时调用，组播模式不支持行情注册
	/// \param chInstrumentType  EES行情类型
	/// \param pSymbol           合约名称
	/// \param bSuccess          注册是否成功标志
	virtual void OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess) {}

	/// \brief  注销symbol响应消息来时调用，组播模式不支持行情注册
	/// \param chInstrumentType  EES行情类型
	/// \param pSymbol           合约名称
	/// \param bSuccess          注册是否成功标志
	virtual void OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess) {}

	/// \brief 查询symbol列表响应消息来时调用，组播模式不支持合约列表查询
	/// \param chInstrumentType  EES行情类型
	/// \param pSymbol           合约名称
	/// \param bLast             最后一条查询合约列表消息的标识
	/// \remark 查询合约列表响应, last = true时，本条数据是无效数据。
	virtual void OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast) {}

	/// \brief 加载EES行情API的动态库
	bool LoadEESQuote();
	/// \brief 释放EES行情API的动态库
	void UnloadEESQuote();

	/// \brief Windows版加载行情API的动态库
	bool Windows_LoadEESQuote();
	/// \brief Windows版释放行情API的动态库
	void Windows_UnloadEESQuote();

	/// \brief Linux版本加载行情API的动态库
	bool Linux_LoadEESQuote();
	/// \brief Linux版本释放行情API的动态库
	void Linux_UnloadEESQuote();

private:
	// 交易日
	int							m_TradingDay;
	int							m_PreTradingDay;

	EESQuoteApi*				m_pApi;					//交易API
	
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

	EESQuoteApi*				m_eesApi;				///< EES行情API接口
	T_DLL_HANDLE				m_handle;				///< EES行情API句柄
	funcDestroyEESQuoteApi		m_distoryFun;			///< EES行情API对象销毁函数
};

