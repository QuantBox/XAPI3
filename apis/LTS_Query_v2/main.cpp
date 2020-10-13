#include "stdafx.h"
#include "../../include/ApiHeader.h"
#include "../../include/QueueEnum.h"
#include "QueryApi.h"

inline CQueryApi* GetApi(void* pApi)
{
	return static_cast<CQueryApi*>(pApi);
}

void* __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
	RequestType rt = (RequestType)type;
	switch (rt)
	{
	case RequestType_GetApiTypes:
		return (void*)(ApiType::ApiType_Query | ApiType::ApiType_Instrument);
	case RequestType_GetApiVersion:
		return (void*)"0.1.0.20151116";
	case RequestType_GetApiName:
		return (void*)"LTS2";
	case RequestType_Create:
		return new CQueryApi();
	default:
		break;
	}

	if (pApi1 == nullptr)
	{
		return nullptr;
	}

	CQueryApi* pApi = GetApi(pApi1);

	switch (rt)
	{
	case RequestType_Release:
		delete pApi;
		return nullptr;
	case RequestType_Register:
		pApi->Register(ptr1, ptr2);
		break;
	case RequestType_Connect:
		pApi->Connect((const char*)ptr3, (ServerInfoField*)ptr1, (UserInfoField*)ptr2);
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
	default:
		break;
	}

	return pApi1;
}
