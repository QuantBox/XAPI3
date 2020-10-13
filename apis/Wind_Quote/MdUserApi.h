#pragma once

#include "../../include/CrossPlatform.h"
#include "../../include/ApiStruct.h"

#ifdef _WIN64
	#pragma comment(lib, "../../include/Wind/win64/WAPIWrapperCpp.lib")
#else
	#pragma comment(lib, "../../include/Wind/win32/WAPIWrapperCpp.lib")
#endif

#include <set>
#include <string>
#include <atomic>
#include <mutex>
#include <map>
#include <future> 

#include "../../include/Wind/WAPIWrapperCpp.h"

using namespace std;

class CMsgQueue;

class CMdUserApi
{
    enum RequestType
    {
        Start = 100,
        Stop,
        QueryInstrument,
    };

public:
    CMdUserApi(void);
    virtual ~CMdUserApi(void);

    void Register(void* pCallback, void* pClass);

    void Connect();
    void Disconnect();

    void Subscribe(const string& szInstrumentIDs, const string& szExchangeID);
    void Unsubscribe(const string& szInstrumentIDs, const string& szExchangeID);

private:
    friend void* __stdcall Query(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
    void QueryInThread(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	friend LONG WINAPI WindCallback(ULONGLONG reqId, const WindData &wd);
	LONG _WindCallback(ULONGLONG reqId, const WindData &wd);

    void WindStart();
    void WindStop();
    bool WindConnected();
    void WindQryInstrument();

    //订阅行情
    void Subscribe(const set<string>& instrumentIDs, const string& szExchangeID);

private:
    mutex m_csMapInstrumentIDs;
    mutex m_csMapQuoteInstrumentIDs;

    //正在订阅的合约
    map<string, set<string> > m_mapInstrumentIDs;
    //正在订阅的合约
    map<string, set<string> > m_mapQuoteInstrumentIDs;

	map<string, ULONGLONG> m_map_code_reqId;

    //消息队列指针
    CMsgQueue* m_msgQueue;
    CMsgQueue* m_msgQueue_Query;
    void* m_pClass;
    bool m_CoInitialized;
    HANDLE m_ExitEvent;
public:
	static CMdUserApi * pThis;
};

