#include "stdafx.h"
#include "TypeConvert.h"

#include <stdlib.h>

#include <string.h>

#include "../../include/ChinaStock.h"
/// 类似于OpenQuant FIX一样的效果，插件层简单，基本不要做怎么计算或处理
/// 对于一个单子的某个状态可能是这样的，新单，部分成交，完全成交
/// EmitAccept,EmitFill
/// OnRtnOrder,OnRtnTrade,如何转成Emit
/// EmitAccept是什么
///
/// 接口向外提供的回报可以分两种方案，ExecutionReport或委托回报与成交回报
/// OpenQuant中使用ExecutionReport问题是因为OQ自己有OrderManager，如果其它软件要看到委托和成交列表是没法得到的
/// 所以接口应当返回委托与成交回报


HedgeFlagType TSecurityFtdcHedgeFlagType_2_HedgeFlagType(TSecurityFtdcHedgeFlagType In)
{
	switch (In)
	{
	//case SECURITY_FTDC_HF_Arbitrage:
	//	return HedgeFlagType::Arbitrage;
	case SECURITY_FTDC_HF_Hedge:
		return HedgeFlagType::HedgeFlagType_Hedge;
	case SECURITY_FTDC_HF_Speculation:
	default:
		return HedgeFlagType::HedgeFlagType_Speculation;
	}
}

TSecurityFtdcHedgeFlagType HedgeFlagType_2_TSecurityFtdcHedgeFlagType(HedgeFlagType In)
{
	switch (In)
	{
	//case HedgeFlagType::Arbitrage:
	//	return SECURITY_FTDC_HF_Arbitrage;
	case HedgeFlagType::HedgeFlagType_Hedge:
		return SECURITY_FTDC_HF_Hedge;
	case HedgeFlagType::HedgeFlagType_Speculation:
	default:
		return SECURITY_FTDC_HF_Speculation;
	}
}

OpenCloseType TSecurityFtdcOffsetFlagType_2_OpenCloseType(TSecurityFtdcOffsetFlagType In)
{
	switch (In)
	{
	case SECURITY_FTDC_OF_CloseToday:
		return OpenCloseType::OpenCloseType_CloseToday;
	case SECURITY_FTDC_OF_Close:
		return OpenCloseType::OpenCloseType_Close;
	case SECURITY_FTDC_OF_Open:
	default:
		return OpenCloseType::OpenCloseType_Open;
	}
}

TSecurityFtdcOffsetFlagType OpenCloseType_2_TSecurityFtdcOffsetFlagType(OpenCloseType In)
{
	switch (In)
	{
	case OpenCloseType::OpenCloseType_CloseToday:
		return SECURITY_FTDC_OF_CloseToday;
	case OpenCloseType::OpenCloseType_Close:
		return SECURITY_FTDC_OF_Close;
	case OpenCloseType::OpenCloseType_Open:
	default:
		return SECURITY_FTDC_OF_Open;
	}
}

TSecurityFtdcDirectionType OrderSide_2_TSecurityFtdcDirectionType(OrderSide In)
{
	if (In == OrderSide::OrderSide_Sell)
		return SECURITY_FTDC_D_Sell;
	return SECURITY_FTDC_D_Buy;
}

OrderSide TSecurityFtdcDirectionType_2_OrderSide(TSecurityFtdcDirectionType In)
{
	if (In == SECURITY_FTDC_D_Sell)
		return OrderSide::OrderSide_Sell;
	return OrderSide::OrderSide_Buy;
}

PositionSide TSecurityFtdcPosiDirectionType_2_PositionSide(TSecurityFtdcPosiDirectionType In)
{
	if (In == SECURITY_FTDC_PD_Short)
		return PositionSide::PositionSide_Short;
	return PositionSide::PositionSide_Long;
}

PositionSide TradeField_2_PositionSide(TradeField* pIn)
{
	//if (pIn->OpenClose == OpenCloseType::Open)
	//{
	//	if (pIn->Side == OrderSide::Buy)
	//		return PositionSide::Long;
	//	return PositionSide::Short;
	//}
	//else
	//{
	//	if (pIn->Side == OrderSide::Buy)
	//		return PositionSide::Short;
	//	return PositionSide::Long;
	//}
	return PositionSide::PositionSide_Long;
}

TSecurityFtdcOrderPriceTypeType OrderType_2_TSecurityFtdcOrderPriceTypeType(OrderType In)
{
	switch (In)
	{
	case OrderType_Market:
		return SECURITY_FTDC_OPT_AnyPrice;
	case OrderType_Stop:
		return SECURITY_FTDC_OPT_AnyPrice;
	case OrderType_Limit:
		return SECURITY_FTDC_OPT_LimitPrice;
	case OrderType_StopLimit:
		return SECURITY_FTDC_OPT_LimitPrice;
	case OrderType_MarketOnClose:
		return SECURITY_FTDC_OPT_AnyPrice;
	case OrderType_TrailingStop:
		return SECURITY_FTDC_OPT_AnyPrice;
	case OrderType_TrailingStopLimit:
		return SECURITY_FTDC_OPT_LimitPrice;
	default:
		return SECURITY_FTDC_OPT_LimitPrice;
	}
}

OrderStatus CSecurityFtdcOrderField_2_OrderStatus(CSecurityFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case SECURITY_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == SECURITY_FTDC_OSS_InsertRejected)
			return OrderStatus::OrderStatus_Rejected;
		return OrderStatus::OrderStatus_Cancelled;
	case SECURITY_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == SECURITY_FTDC_OSS_InsertSubmitted)
			return OrderStatus::OrderStatus_New;
	default:
		if (0 == pIn->VolumeTotal)
			return OrderStatus::OrderStatus_Filled;
		else if (pIn->VolumeTotal == pIn->VolumeTotalOriginal)
			return OrderStatus::OrderStatus_New;
		else
			return OrderStatus::OrderStatus_PartiallyFilled;
	}
}

OrderType CSecurityFtdcOrderField_2_OrderType(CSecurityFtdcOrderField* pIn)
{
	switch (pIn->OrderPriceType)
	{
	case SECURITY_FTDC_OPT_AnyPrice:
		return OrderType::OrderType_Market;
	case SECURITY_FTDC_OPT_LimitPrice:
		return OrderType::OrderType_Limit;
	default:
		return OrderType::OrderType_Limit;
	}
}

TimeInForce CSecurityFtdcOrderField_2_TimeInForce(CSecurityFtdcOrderField* pIn)
{
	switch (pIn->TimeCondition)
	{
	case SECURITY_FTDC_TC_GFD:
		return TimeInForce::TimeInForce_Day;
	case SECURITY_FTDC_TC_IOC:
		switch (pIn->VolumeCondition)
		{
		case SECURITY_FTDC_VC_AV:
			return TimeInForce::TimeInForce_IOC;
		case SECURITY_FTDC_VC_CV:
			return TimeInForce::TimeInForce_FOK;
		default:
			return TimeInForce::TimeInForce_IOC;
		}
	default:
		return TimeInForce::TimeInForce_Day;
	}
}

ExecType CSecurityFtdcOrderField_2_ExecType(CSecurityFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case SECURITY_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == SECURITY_FTDC_OSS_InsertRejected)
			return ExecType::ExecType_Rejected;
		return ExecType::ExecType_Cancelled;
	case SECURITY_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == SECURITY_FTDC_OSS_InsertSubmitted)
			return ExecType::ExecType_New;
	case SECURITY_FTDC_OST_AllTraded:
	case SECURITY_FTDC_OST_PartTradedQueueing:
		return ExecType::ExecType_Trade;
	default:
		return ExecType::ExecType_New;
	}
}

PutCall CSecurityFtdcInstrumentField_2_PutCall(CSecurityFtdcInstrumentField* pIn)
{
	if (strlen(pIn->InstrumentID) == 8)
	{
		if (pIn->ExchangeInstID[6] == 'C')
		{
			return PutCall::PutCall_Call;
		}
	}
	return PutCall::PutCall_Put;
}

InstrumentType CSecurityFtdcInstrumentField_2_InstrumentType(CSecurityFtdcInstrumentField* pIn)
{
	switch (pIn->ProductClass)
	{
	case SECURITY_FTDC_PC_Futures:
		return InstrumentType::InstrumentType_Future;
	case SECURITY_FTDC_PC_Options:
		return InstrumentType::InstrumentType_Option;
	case SECURITY_FTDC_PC_Combination:
		return InstrumentType::InstrumentType_MultiLeg;
	case SECURITY_FTDC_PC_EFP:
		return InstrumentType::InstrumentType_Future;
	case SECURITY_FTDC_PC_ETF:
		return InstrumentType::InstrumentType_ETF;
	case SECURITY_FTDC_PC_ETFPurRed:
		return InstrumentType::InstrumentType_ETF;
	default:
		if (strlen(pIn->InstrumentID) == 8)
		{
			return InstrumentType::InstrumentType_Option;
		}
		if (pIn->ExchangeID[1] == 'Z')
		{
			return InstrumentID_2_InstrumentType_SZSE(atoi(pIn->InstrumentID));
		}
		else
		{
			return InstrumentID_2_InstrumentType_SSE(atoi(pIn->InstrumentID));
		}
	}
}

PriceType CSecurityFtdcInstrumentField_2_PriceTick(CSecurityFtdcInstrumentField* pIn)
{
	if (pIn->PriceTick != 0)
		return pIn->PriceTick;

	// 期权为0.001
	if (strlen(pIn->InstrumentID) == 8)
	{
		return 0.001;
	}

	if (pIn->ExchangeID[1] == 'Z')
	{
		return InstrumentID_2_PriceTick_SZSE(atoi(pIn->InstrumentID));
	}
	else
	{
		return InstrumentID_2_PriceTick_SSE(atoi(pIn->InstrumentID));
	}
}

IdCardType TSecurityFtdcIdCardTypeType_2_IdCardType(TSecurityFtdcIdCardTypeType In)
{
	switch (In)
	{
	case SECURITY_FTDC_ICT_EID:
		return IdCardType::IdCardType_EID;
	case SECURITY_FTDC_ICT_IDCard:
		return IdCardType::IdCardType_IDCard;
	case SECURITY_FTDC_ICT_Passport:
		return IdCardType::IdCardType_Passport;
	case SECURITY_FTDC_ICT_LicenseNo:
		return IdCardType::IdCardType_LicenseNo;
	case SECURITY_FTDC_ICT_TaxNo:
		return IdCardType::IdCardType_TaxNo;
	case SECURITY_FTDC_ICT_OtherCard:
	default:
		return IdCardType::IdCardType_OtherCard;
	}
}

ExchangeType TSecurityFtdcExchangeIDType_2_ExchangeType(TSecurityFtdcExchangeIDType In)
{
	switch (In[1])
	{
	case 'S':
		return ExchangeType::ExchangeType_SSE;
	case 'Z':
		return ExchangeType::ExchangeType_SZSE;
	default:
		return ExchangeType::ExchangeType_Undefined;
	}
}

void CSecurityFtdcOrderField_2_OrderField_0(OrderIDType OrderID, CSecurityFtdcOrderField* pIn, OrderField* pOut)
{
	strcpy(pOut->ID, OrderID);
	strcpy(pOut->LocalID, pOut->ID);
	strcpy(pOut->InstrumentID, pIn->InstrumentID);
	strcpy(pOut->ExchangeID, pIn->ExchangeID);
	pOut->HedgeFlag = TSecurityFtdcHedgeFlagType_2_HedgeFlagType(pIn->CombHedgeFlag[0]);
	pOut->Side = TSecurityFtdcDirectionType_2_OrderSide(pIn->Direction);
	pOut->Price = atof(pIn->LimitPrice);
	pOut->StopPx = pIn->StopPrice;
	strncpy(pOut->Text, pIn->StatusMsg, sizeof(Char256Type));
	pOut->OpenClose = TSecurityFtdcOffsetFlagType_2_OpenCloseType(pIn->CombOffsetFlag[0]);
	pOut->Status = CSecurityFtdcOrderField_2_OrderStatus(pIn);
	pOut->Qty = pIn->VolumeTotalOriginal;
	pOut->CumQty = pIn->VolumeTraded;
	pOut->LeavesQty = pIn->VolumeTotal;
	pOut->Type = CSecurityFtdcOrderField_2_OrderType(pIn);
	pOut->TimeInForce = CSecurityFtdcOrderField_2_TimeInForce(pIn);
	pOut->ExecType = CSecurityFtdcOrderField_2_ExecType(pIn);
	strcpy(pOut->OrderID, pIn->OrderSysID);
	strncpy(pOut->Text, pIn->StatusMsg, sizeof(Char256Type));
}
