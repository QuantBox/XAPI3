#pragma once
#include "../../../include/CrossPlatform.h"
#include "../../../include/XApiCpp.h"


class CXApiImpl :
	public CXApi
{
public:
	CXApiImpl(const char* libPath);
	~CXApiImpl();

	virtual ApiType GetApiTypes();
	virtual const char* GetApiVersion();
	virtual const char* GetApiName();

	virtual bool Init();
	virtual const char* GetLastError();
	virtual void RegisterSpi(CXSpi *pSpi);

	virtual void Connect(const char* szPath, ServerInfoField* pServerInfo, UserInfoField* pUserInfo, int count);
	virtual void Disconnect();

	virtual void Subscribe(const char* szInstrument, const char* szExchange);
	virtual void Unsubscribe(const char* szInstrument, const char* szExchange);

	virtual void ReqQuery(QueryType type, ReqQueryField* query);

	virtual const char* SendOrder(OrderField* pOrder, int count, char* pOut);
	virtual const char* CancelOrder(OrderIDType* pIn, int count, char* pOut);

	virtual const char* SendQuote(QuoteField* pQuote, int count, char* pOut);
	virtual const char* CancelQuote(OrderIDType* pIn, int count, char* pOut);
public:
	static  void* __stdcall OnResponse(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
private:
	void* _OnResponse(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);

	void* m_pLib;
	void* m_pFun;
	void* m_pApi;

	CXSpi *m_pSpi;
	char m_LibPath[1024];
};

