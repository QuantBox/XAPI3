#include "stdafx.h"
#include "../../include/ApiHeader.h"
#include "../../include/QueueEnum.h"
#include "MdUserApi.h"

inline CMdUserApi* GetApi(void* pApi)
{
    return static_cast<CMdUserApi*>(pApi);
}

void* __stdcall XRequest(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
{
    RequestType requestType = (RequestType)type;
    switch (requestType)
    {
    case RequestType_GetApiTypes:
        return (void*)(ApiType::ApiType_MarketData | ApiType::ApiType_Instrument);
    case RequestType_GetApiVersion:
        return (void*)"0.1.0.20160120";
    case RequestType_GetApiName:
        return (void*)"Wind";
    case RequestType_Create:
        return new CMdUserApi();
    default:
        break;
    }

    if (pApi1 == nullptr)
    {
        return nullptr;
    }

    CMdUserApi* api = GetApi(pApi1);

    switch (requestType)
    {
    case RequestType_Release:
        delete api;
        return nullptr;
    case RequestType_Register:
        api->Register(ptr1, ptr2);
        break;
    case RequestType_Connect:
        api->Connect();
        break;
    case RequestType_Disconnect:
        api->Disconnect();
        break;
    case RequestType_Subscribe:
        api->Subscribe((const char*)ptr1, (const char*)ptr2);
        break;
    case RequestType_Unsubscribe:
        api->Unsubscribe((const char*)ptr1, (const char*)ptr2);
        break;
    default:
        break;
    }

    return pApi1;
}
