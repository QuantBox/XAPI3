#include "stdafx.h"
#include "../../include/ApiHeader.h"
#include "../../include/QueueEnum.h"
#include "TraderApi.h"

inline CTraderApi* GetApi(void* pApi)
{
	return static_cast<CTraderApi*>(pApi);
}

void* __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestType rt = (RequestType)type;
	switch (rt)
	{
	case RequestType_GetApiTypes:
		return (void*)(ApiType::ApiType_Trade | ApiType::ApiType_Instrument | ApiType::ApiType_Query);
	case RequestType_GetApiVersion:
		return (void*)API_VERSION;
	case RequestType_GetApiName:
		return (void*)API_NAME;
	case RequestType_Create:
		return new CTraderApi();
	default:
		break;
	}

	if (pApi1 == nullptr)
	{
		return nullptr;
	}

	CTraderApi* pApi = GetApi(pApi1);

	switch (rt)
	{
	case RequestType_Release:
		delete pApi;
		return nullptr;
	case RequestType_Register:
		pApi->Register(ptr1, ptr2);
		break;
	case RequestType_Connect:
		pApi->Connect((const char*)ptr3, (ServerInfoField*)ptr1, (UserInfoField*)ptr2,size2);
		break;
	case RequestType_Disconnect:
		pApi->Disconnect();
		break;
	case QueryType_ReqQryInstrument:
	case QueryType_ReqQryInvestorPosition:
	case QueryType_ReqQryTradingAccount:
	case QueryType_ReqQrySettlementInfo:
	case QueryType_ReqQryTrade:
	case QueryType_ReqQryOrder:
		pApi->ReqQuery((QueryType)type, (ReqQueryField*)ptr1);
		break;
	case RequestType_ReqOrderInsert:
		return (void*)pApi->ReqOrderInsert((OrderField*)ptr1, size1, (char*)ptr2);
	case RequestType_ReqOrderAction:
		return (void*)pApi->ReqOrderAction((OrderIDType*)ptr1, size1, (char*)ptr2);
#ifdef HAS_Quote
	case RequestType_ReqQuoteInsert:
		return pApi->ReqQuoteInsert((QuoteField*)ptr1, (OrderIDType*)ptr2, (OrderIDType*)ptr3);
	case RequestType_ReqQuoteAction:
		return (void*)pApi->ReqQuoteAction((const char*)ptr1, (OrderIDType*)ptr2);
#endif // HAS_Quote

	default:
		break;
	}

	return pApi1;
}
