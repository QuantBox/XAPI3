#pragma once
#include <mutex>
#include "OrderMap.h"

#include "../../include/REM/EesTraderDefine.h"

#include "../../include/ApiStruct.h"


using namespace std;

class CProcessor
{
public:
	OrderField* OnOrderAccept(EES_OrderAcceptField* pAccept, OrderField* pOrder);
	OrderField* OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept, OrderField* pOrder);
	OrderField* OnOrderReject(EES_OrderRejectField* pReject, OrderField* pOrder);
	OrderField* OnOrderMarketReject(EES_OrderMarketRejectField* pReject, OrderField* pOrder);
	TradeField* OnOrderExecution(EES_OrderExecutionField* pExec, OrderField* pOrder, TradeField* pTrade);
	OrderField* OnOrderCxled(EES_OrderCxled* pCxled, OrderField* pOrder);
	OrderField* OnCxlOrderReject(EES_CxlOrderRej* pReject, OrderField* pOrder);

public:
	// 外部传入，不要动它
	COrderMap<long long>* m_pOrderMap = nullptr;
	mutex	m_csOrderMap;
};

