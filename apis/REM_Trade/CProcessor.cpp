#include "stdafx.h"
#include "CProcessor.h"

#include "../../include/ApiStruct.h"
#include "../../include/QueueEnum.h"

#include "../../include/queue/MsgQueue.h"
#include "TypeConvert.h"

int CProcessor::OnOrderAccept(EES_OrderAcceptField* pAccept)
{
	// 表示柜台接受了，会提供一个委托ID
	OrderField* pField = (OrderField*)m_pOrderMap->findOrderXAPI(pAccept->m_ClientOrderToken);
	if (pField == nullptr)
		return 0;
	pField->ExecType = ExecType::ExecType_New;
	pField->Status = OrderStatus::OrderStatus_New;
	pField->LeavesQty = pField->Qty;
	sprintf(pField->ID, "%lld", pAccept->m_MarketOrderToken);

	// 检验席位切换是否可用
	//EES_EnterOrderField* pEnter = (EES_EnterOrderField*)pField->pUserData1;
	//sprintf(pField->Text, "MarketSessionId: %d -> %d", pEnter->m_MarketSessionId, pAccept->m_MarketSessionId);

	// 换成用柜台的ID
	m_pOrderMap->replace(pAccept->m_ClientOrderToken, pAccept->m_MarketOrderToken, pField, pField->pUserData1);

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);

	return 0;
}

int CProcessor::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)
{
	OrderField* pField = (OrderField*)m_pOrderMap->findOrderXAPI(pAccept->m_MarketOrderToken);
	if (pField == nullptr)
		return 0;

	//pField->ExecType = ExecType::ExecType_New;
	//pField->Status = OrderStatus::OrderStatus_New;
	strcpy(pField->OrderID, pAccept->m_MarketOrderId);

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, 0, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);

	return 0;
}
int CProcessor::OnOrderReject(EES_OrderRejectField* pReject)
{
	OrderField* pField = (OrderField*)m_pOrderMap->findOrderXAPI(pReject->m_ClientOrderToken);
	if (pField == nullptr)
		return 0;

	pField->ExecType = ExecType::ExecType_Rejected;
	pField->Status = OrderStatus::OrderStatus_Rejected;
	pField->LeavesQty = 0;
	sprintf(pField->ID, "%d:%d", pReject->m_ClientOrderToken, pReject->m_Userid);
	pField->RawErrorID = pReject->m_ReasonCode;
	// 前面是语法检查，后面是风险检查
	sprintf(pField->Text, "%s%s", pReject->m_GrammerText, pReject->m_RiskText);

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);

	return 0;
}
int CProcessor::OnOrderMarketReject(EES_OrderMarketRejectField* pReject)
{
	OrderField* pField = (OrderField*)m_pOrderMap->findOrderXAPI(pReject->m_MarketOrderToken);
	if (pField == nullptr)
		return 0;
	pField->ExecType = ExecType::ExecType_Rejected;
	pField->Status = OrderStatus::OrderStatus_Rejected;
	sprintf(pField->ID, "%lld", pReject->m_MarketOrderToken);
	pField->RawErrorID = pReject->m_ExchangeErrorId;
	strcpy(pField->Text, pReject->m_ReasonText);

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);

	return 0;
}
int CProcessor::OnOrderExecution(EES_OrderExecutionField* pExec)
{
	// 这里成交数量要修改
	OrderField* pOrder = (OrderField*)m_pOrderMap->findOrderXAPI(pExec->m_MarketOrderToken);
	if (pOrder == nullptr)
		return 0;
	pOrder->ExecType = ExecType::ExecType_Trade;
	pOrder->CumQty += pExec->m_Quantity;
	pOrder->LeavesQty -= pExec->m_Quantity;
	if (pOrder->LeavesQty == 0)
	{
		pOrder->Status = OrderStatus::OrderStatus_Filled;
	}
	else
	{
		pOrder->Status = OrderStatus::OrderStatus_PartiallyFilled;
	}

	TradeField* pField = (TradeField*)m_msgQueue->new_block(sizeof(TradeField));
	strcpy(pField->InstrumentID, pOrder->InstrumentID);
	strcpy(pField->ExchangeID, pOrder->ExchangeID);
	strcpy(pField->AccountID, pOrder->AccountID);
	pField->Side = pOrder->Side;
	pField->Qty = pExec->m_Quantity;
	pField->Price = pExec->m_Price;
	pField->OpenClose = pOrder->OpenClose;
	pField->HedgeFlag = pOrder->HedgeFlag;
	pField->Commission = 0;
	//pTrade->Time = str_to_HHmmss(pTrade->TradeTime);
	strcpy(pField->TradeID, pExec->m_MarketExecID);
	sprintf(pField->ID, "%lld", pExec->m_MarketOrderToken);

	m_msgQueue->Input_NoCopy(ResponseType::ResponseType_OnRtnTrade, m_msgQueue, m_pClass, true, 0, pField, sizeof(TradeField), nullptr, 0, nullptr, 0);
	return 0;
}
int CProcessor::OnOrderCxled(EES_OrderCxled* pCxled)
{
	OrderField* pField = (OrderField*)m_pOrderMap->findCancelAPI(pCxled->m_MarketOrderToken);
	if (pField == nullptr)
		return 0;
	pField->ExecType = ExecType::ExecType_Cancelled;
	pField->Status = OrderStatus::OrderStatus_Cancelled;
	pField->LeavesQty = 0;
	pField->RawErrorID = pCxled->m_Reason;
	strcpy(pField->Text, EES_CxlReasonCode_2_str(pCxled->m_Reason));

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	return 0;
}

int CProcessor::OnCxlOrderReject(EES_CxlOrderRej* pReject)
{
	if (pReject->m_MarketOrderToken == 0)
	{
		printf("OnCxlOrderReject: 0 __del__\n");
		return 0;
	}
	OrderField* pField = (OrderField*)m_pOrderMap->findCancelAPI(pReject->m_MarketOrderToken);
	if (pField == nullptr)
		return 0;
	pField->ExecType = ExecType::ExecType_CancelReject;
	pField->RawErrorID = pReject->m_ReasonCode;
	strcpy(pField->Text, EES_CxlReasonCode_2_str(pReject->m_ReasonCode));

	m_msgQueue->Input_Copy(ResponseType::ResponseType_OnRtnOrder, m_msgQueue, m_pClass, true, 0, pField, sizeof(OrderField), nullptr, 0, nullptr, 0);
	return 0;
}