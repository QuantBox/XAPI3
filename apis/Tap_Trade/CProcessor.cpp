#include "stdafx.h"
#include "CProcessor.h"


#include "../../include/QueueEnum.h"
#include "../../include/queue/MsgQueue.h"
#include "TypeConvert.h"

OrderField* CProcessor::OnOrderAccept(const TapAPIOrderInfoNotice* pAccept, OrderField* pOrder)
{
	pOrder->ExecType = ExecType::ExecType_New;
	pOrder->Status = OrderStatus::OrderStatus_New;
	pOrder->LeavesQty = pOrder->Qty;
	strcpy(pOrder->ID, pAccept->OrderInfo->OrderNo);

	// 检验席位切换是否可用
	TapAPINewOrder* pEnter = (TapAPINewOrder*)pOrder->pUserData1;
	sprintf(pOrder->Text, "%s->%s", pEnter->UpperChannelNo, pAccept->OrderInfo->UpperChannelNo);

	return pOrder;
}

OrderField* CProcessor::OnOrderMarketAccept(const TapAPIOrderInfoNotice* pAccept, OrderField* pOrder)
{
	//pField->ExecType = ExecType::ExecType_New;
	//pField->Status = OrderStatus::OrderStatus_New;
	// 
	strcpy(pOrder->OrderID, pAccept->OrderInfo->OrderSystemNo);

	return pOrder;
}

OrderField* CProcessor::OnOrderReject(const TapAPIOrderInfoNotice* pReject, OrderField* pOrder)
{
	pOrder->ExecType = ExecType::ExecType_Rejected;
	pOrder->Status = OrderStatus::OrderStatus_Rejected;
	pOrder->LeavesQty = 0;
	//sprintf(pOrder->ID, "%d:%d", pReject->m_ClientOrderToken, pReject->m_Userid);
	pOrder->RawErrorID = pReject->OrderInfo->ErrorCode;
	//strcpy(pOrder->Text, pReject->OrderInfo->ErrorText);
	sprintf(pOrder->Text, "%s|%s", pOrder->Text, pReject->OrderInfo->ErrorText);

	return pOrder;
}
//OrderField* CProcessor::OnOrderMarketReject(EES_OrderMarketRejectField* pReject, OrderField* pOrder)
//{
//	pOrder->ExecType = ExecType::ExecType_Rejected;
//	pOrder->Status = OrderStatus::OrderStatus_Rejected;
//	sprintf(pOrder->ID, "%lld", pReject->m_MarketOrderToken);
//	pOrder->RawErrorID = pReject->m_ExchangeErrorId;
//	sprintf(pOrder->Text, "%s|%s", pOrder->Text, pReject->m_ReasonText);
//
//	return pOrder;
//}
TradeField* CProcessor::OnOrderExecution(const TapAPIFillInfo* pExec, OrderField* pOrder, TradeField* pTrade)
{
	// 这里成交数量要修改
	pOrder->ExecType = ExecType::ExecType_Trade;
	pOrder->CumQty += pExec->MatchQty;
	pOrder->LeavesQty -= pExec->MatchQty;
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
	pTrade->Qty = pExec->MatchQty;
	pTrade->Price = pExec->MatchPrice;
	pTrade->OpenClose = pOrder->OpenClose;
	pTrade->HedgeFlag = pOrder->HedgeFlag;
	pTrade->Commission = 0;
	//pTrade->Time = str_to_HHmmss(pTrade->TradeTime);
	strcpy(pTrade->TradeID, pExec->ExchangeMatchNo);
	strcpy(pTrade->ID, pExec->OrderNo);

	return pTrade;
}

OrderField* CProcessor::OnOrderCxled(const TapAPIOrderInfoNotice* pCxled, OrderField* pOrder)
{
	pOrder->ExecType = ExecType::ExecType_Cancelled;
	pOrder->Status = OrderStatus::OrderStatus_Cancelled;
	pOrder->LeavesQty = 0;
	pOrder->RawErrorID = pCxled->OrderInfo->ErrorCode;

	sprintf(pOrder->Text, "%s|%s", pOrder->Text, pCxled->OrderInfo->ErrorText);

	return pOrder;
}
//
//OrderField* CProcessor::OnCxlOrderReject(EES_CxlOrderRej* pReject, OrderField* pOrder)
//{
//	pOrder->ExecType = ExecType::ExecType_CancelReject;
//	pOrder->RawErrorID = pReject->m_ReasonCode;
//	sprintf(pOrder->Text, "%s|%s", pOrder->Text, EES_CxlReasonCode_2_str(pReject->m_ReasonCode));
//
//	return pOrder;
//}