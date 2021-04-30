#pragma once
#include <mutex>
#include "OrderMap.h"

#include "../../include/REM/EesTraderDefine.h"

using namespace std;

class CMsgQueue;

class CProcessor
{
public:
	int OnOrderAccept(EES_OrderAcceptField* pAccept);
	int OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept);
	int OnOrderReject(EES_OrderRejectField* pReject);
	int OnOrderMarketReject(EES_OrderMarketRejectField* pReject);
	int OnOrderExecution(EES_OrderExecutionField* pExec);
	int OnOrderCxled(EES_OrderCxled* pCxled);
	int OnCxlOrderReject(EES_CxlOrderRej* pReject);

public:
	CMsgQueue* m_msgQueue = nullptr;
	void* m_pClass = nullptr;

public:
	// 外部传入，不要动它
	COrderMap<long long>* m_pOrderMap = nullptr;
	mutex	m_csOrderMap;
};

