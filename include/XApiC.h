#pragma once

#include "CrossPlatform.h"
#include "ApiStruct.h"
#include "QueueEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef void* (__stdcall *fnOnResponse) (char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* X_LoadLib(const char* libPath);
const char* X_GetLastError();
void* X_GetFunction(void* lib, const char* ProcName = "XRequest");
void X_FreeLib(void* lib);

ApiType X_GetApiTypes(void* pFun);
const char* X_GetApiVersion(void* pFun);
const char* X_GetApiName(void* pFun);

void* X_Create(void* pFun);
void X_Register(void* pFun, void* pApi, fnOnResponse pCallback, void* pClass);

void X_Connect(void* pFun, void* pApi, const char* szPath, ServerInfoField* pServerInfo, UserInfoField* pUserInfo, int count);
void X_Disconnect(void* pFun, void* pApi);

void X_Subscribe(void* pFun, void* pApi, const char* szInstrument, const char* szExchange);
void X_Unsubscribe(void* pFun, void* pApi, const char* szInstrument, const char* szExchange);
void X_SubscribeQuote(void* pFun, void* pApi, const char* szInstrument, const char* szExchange);
void X_UnsubscribeQuote(void* pFun, void* pApi, const char* szInstrument, const char* szExchange);

void X_ReqQuery(void* pFun, void* pApi, QueryType type, ReqQueryField* query);

const char* X_SendOrder(void* pFun, void* pApi, OrderField* pOrder, int count, char* pOut);
const char* X_CancelOrder(void* pFun, void* pApi, OrderIDType* pIn, int count, char* pOut);

const char* X_SendQuote(void* pFun, void* pApi, QuoteField* pQuote, int count, char* pOut);
const char* X_CancelQuote(void* pFun, void* pApi, OrderIDType* pIn, int count, char* pOut);

#ifdef __cplusplus
}
#endif
