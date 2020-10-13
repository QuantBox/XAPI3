#if !defined(XAPI_CPP_H)
#define XAPI_CPP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CrossPlatform.h"

#include "ApiStruct.h"
#include "QueueEnum.h"

class CXApi;

class CXSpi
{
public:
	virtual void OnConnectionStatus(CXApi* pApi, ConnectionStatus status, RspUserLoginField* pUserLogin, int size1){};
	virtual void OnRtnError(CXApi* pApi, ErrorField* pError){};
	virtual void OnLog(CXApi* pApi, LogField* pLog) {};

	virtual void OnRtnDepthMarketDataN(CXApi* pApi, DepthMarketDataNField* pMarketData){};
	virtual void OnRspQryInstrument(CXApi* pApi, InstrumentField* pInstrument, int size1, bool bIsLast) {};
	virtual void OnRspQryTradingAccount(CXApi* pApi, AccountField* pAccount, int size1, bool bIsLast) {};
	virtual void OnRspQryInvestorPosition(CXApi* pApi, PositionField* pPosition, int size1, bool bIsLast) {};
	virtual void OnRspQrySettlementInfo(CXApi* pApi, SettlementInfoField* pSettlementInfo, int size1, bool bIsLast) {};

	virtual void OnRspQryOrder(CXApi* pApi, OrderField* pOrder, int size1, bool bIsLast) {};
	virtual void OnRspQryTrade(CXApi* pApi, TradeField* pTrade, int size1, bool bIsLast) {};
	virtual void OnRspQryQuote(CXApi* pApi, QuoteField* pQuote, int size1, bool bIsLast) {};

	virtual void OnRtnOrder(CXApi* pApi, OrderField* pOrder) {};
	virtual void OnRtnTrade(CXApi* pApi, TradeField* pTrade) {};
	virtual void OnRtnQuote(CXApi* pApi, QuoteField* pQuote) {};

	virtual void OnRtnQuoteRequest(CXApi* pApi, QuoteRequestField* pQuoteRequest){};
	virtual void OnRspQryHistoricalTicks(CXApi* pApi, TickField* pTicks, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast) {};
	virtual void OnRspQryHistoricalBars(CXApi* pApi, BarField* pBars, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast) {};
	virtual void OnRspQryInvestor(CXApi* pApi, InvestorField* pInvestor, int size1, bool bIsLast){};
	virtual void OnRtnInstrumentStatus(CXApi* pApi, InstrumentStatusField* pInstrumentStatus) {};

	virtual bool OnFilterSubscribe(CXApi* pApi, ExchangeType exchange, int instrument_part1, int instrument_part2, int instrument_part3, char* pInstrument){ return true; };
};

class DLL_PUBLIC CXApi
{
public:
	static CXApi* CreateApi(const char* libPath);
	virtual bool Init() = 0;
	virtual const char* GetLastError() = 0;

	virtual ApiType GetApiTypes() = 0;
	virtual const char* GetApiVersion() = 0;
	virtual const char* GetApiName() = 0;

	virtual void RegisterSpi(CXSpi *pSpi) = 0;

	virtual void Connect(const char* szPath, ServerInfoField* pServerInfo, UserInfoField* pUserInfo, int count) = 0;
	virtual void Disconnect() = 0;

	virtual void Subscribe(const char* szInstrument, const char* szExchange) = 0;
	virtual void Unsubscribe(const char* szInstrument, const char* szExchange) = 0;

	virtual void ReqQuery(QueryType type, ReqQueryField* query) = 0;

	virtual const char* SendOrder(OrderField* pOrder, int count, char* pOut) = 0;
	virtual const char* CancelOrder(OrderIDType* pIn, int count, char* pOut) = 0;

	virtual const char* SendQuote(QuoteField* pQuote, int count, char* pOut) = 0;
	virtual const char* CancelQuote(OrderIDType* pIn, int count, char* pOut) = 0;
protected:
	CXApi();
	~CXApi();
};

#endif
