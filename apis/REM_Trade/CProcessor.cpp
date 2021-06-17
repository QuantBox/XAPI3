#include "stdafx.h"
#include "CProcessor.h"


#include "../../include/QueueEnum.h"
#include "../../include/queue/MsgQueue.h"
#include "TypeConvert.h"

OrderField* CProcessor::OnOrderAccept(EES_OrderAcceptField* pAccept, OrderField* pOrder)
{
	pOrder->ExecType = ExecType::ExecType_New;
	pOrder->Status = OrderStatus::OrderStatus_New;
	pOrder->LeavesQty = pOrder->Qty;
	sprintf(pOrder->ID, "%lld", pAccept->m_MarketOrderToken);

	// 检验席位切换是否可用
	//EES_EnterOrderField* pEnter = (EES_EnterOrderField*)pField->pUserData1;
	//sprintf(pField->Text, "MarketSessionId: %d -> %d", pEnter->m_MarketSessionId, pAccept->m_MarketSessionId);

	return pOrder;
}

OrderField* CProcessor::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept, OrderField* pOrder)
{
	//pField->ExecType = ExecType::ExecType_New;
	//pField->Status = OrderStatus::OrderStatus_New;
	strcpy(pOrder->OrderID, pAccept->m_MarketOrderId);

	return pOrder;
}
OrderField* CProcessor::OnOrderReject(EES_OrderRejectField* pReject, OrderField* pOrder)
{
	pOrder->ExecType = ExecType::ExecType_Rejected;
	pOrder->Status = OrderStatus::OrderStatus_Rejected;
	pOrder->LeavesQty = 0;
	sprintf(pOrder->ID, "%d:%d", pReject->m_ClientOrderToken, pReject->m_Userid);
	pOrder->RawErrorID = pReject->m_ReasonCode;
	// 前面是语法检查，后面是风险检查
	sprintf(pOrder->Text, "%s%s", pReject->m_GrammerText, pReject->m_RiskText);

	return pOrder;
}
OrderField* CProcessor::OnOrderMarketReject(EES_OrderMarketRejectField* pReject, OrderField* pOrder)
{
	pOrder->ExecType = ExecType::ExecType_Rejected;
	pOrder->Status = OrderStatus::OrderStatus_Rejected;
	sprintf(pOrder->ID, "%lld", pReject->m_MarketOrderToken);
	pOrder->RawErrorID = pReject->m_ExchangeErrorId;
	strcpy(pOrder->Text, pReject->m_ReasonText);

	return pOrder;
}
TradeField* CProcessor::OnOrderExecution(EES_OrderExecutionField* pExec, OrderField* pOrder, TradeField* pTrade)
{
	// 这里成交数量要修改
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
	
	strcpy(pTrade->InstrumentID, pOrder->InstrumentID);
	strcpy(pTrade->ExchangeID, pOrder->ExchangeID);
	strcpy(pTrade->AccountID, pOrder->AccountID);
	pTrade->Side = pOrder->Side;
	pTrade->Qty = pExec->m_Quantity;
	pTrade->Price = pExec->m_Price;
	pTrade->OpenClose = pOrder->OpenClose;
	pTrade->HedgeFlag = pOrder->HedgeFlag;
	pTrade->Commission = 0;
	//pTrade->Time = str_to_HHmmss(pTrade->TradeTime);
	strcpy(pTrade->TradeID, pExec->m_MarketExecID);
	sprintf(pTrade->ID, "%lld", pExec->m_MarketOrderToken);

	return pTrade;
}

OrderField* CProcessor::OnOrderCxled(EES_OrderCxled* pCxled, OrderField* pOrder)
{
	pOrder->ExecType = ExecType::ExecType_Cancelled;
	pOrder->Status = OrderStatus::OrderStatus_Cancelled;
	pOrder->LeavesQty = 0;
	pOrder->RawErrorID = pCxled->m_Reason;
	strcpy(pOrder->Text, EES_CxlReasonCode_2_str(pCxled->m_Reason));

	return pOrder;
}

OrderField* CProcessor::OnCxlOrderReject(EES_CxlOrderRej* pReject, OrderField* pOrder)
{
	pOrder->ExecType = ExecType::ExecType_CancelReject;
	pOrder->RawErrorID = pReject->m_ReasonCode;
	strcpy(pOrder->Text, EES_CxlReasonCode_2_str(pReject->m_ReasonCode));

	return pOrder;
}