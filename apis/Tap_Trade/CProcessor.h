#pragma once
#include <mutex>
#include "OrderMap.h"

#include "../../include/Tap/TapTradeAPIDataType.h"

#include "../../include/ApiStruct.h"


using namespace std;

class CProcessor
{
public:
	OrderField* OnOrderAccept(const TapAPIOrderInfoNotice* pAccept, OrderField* pOrder);
	OrderField* OnOrderMarketAccept(const TapAPIOrderInfoNotice* pAccept, OrderField* pOrder);
	OrderField* OnOrderReject(const TapAPIOrderInfoNotice* pReject, OrderField* pOrder);
	//OrderField* OnOrderMarketReject(const TapAPIOrderInfoNotice* pReject, OrderField* pOrder);
	TradeField* OnOrderExecution(const TapAPIFillInfo* pExec, OrderField* pOrder, TradeField* pTrade);
	OrderField* OnOrderCxled(const TapAPIOrderInfoNotice* pCxled, OrderField* pOrder);
	//OrderField* OnCxlOrderReject(EES_CxlOrderRej* pReject, OrderField* pOrder);

public:
	// 外部传入，不要动它
	COrderMap<string>* m_pOrderMap = nullptr;
	mutex	m_csOrderMap;
};

