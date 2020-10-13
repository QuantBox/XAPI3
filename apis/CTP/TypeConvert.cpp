#include "stdafx.h"
#include "TypeConvert.h"
#include <cstring>
#include <stdio.h>


/// 类似于OpenQuant FIX一样的效果，插件层简单，基本不要做怎么计算或处理
/// 对于一个单子的某个状态可能是这样的，新单，部分成交，完全成交
/// EmitAccept,EmitFill
/// OnRtnOrder,OnRtnTrade,如何转成Emit
/// EmitAccept是什么
///
/// 接口向外提供的回报可以分两种方案，ExecutionReport或委托回报与成交回报
/// OpenQuant中使用ExecutionReport问题是因为OQ自己有OrderManager，如果其它软件要看到委托和成交列表是没法得到的
/// 所以接口应当返回委托与成交回报

PutCall TThostFtdcOptionsTypeType_2_PutCall(TThostFtdcOptionsTypeType In)
{
	if (In == THOST_FTDC_CP_CallOptions)
		return PutCall::PutCall_Call;
	return PutCall::PutCall_Put;
}

HedgeFlagType TThostFtdcHedgeFlagType_2_HedgeFlagType(TThostFtdcHedgeFlagType In)
{
	switch (In)
	{
	case THOST_FTDC_HF_Arbitrage:
		return HedgeFlagType::HedgeFlagType_Arbitrage;
	case THOST_FTDC_HF_Hedge:
		return HedgeFlagType::HedgeFlagType_Hedge;
	case THOST_FTDC_HF_Speculation:
	default:
		return HedgeFlagType::HedgeFlagType_Speculation;
	}
}

TThostFtdcHedgeFlagType HedgeFlagType_2_TThostFtdcHedgeFlagType(HedgeFlagType In)
{
	switch (In)
	{
	case HedgeFlagType::HedgeFlagType_Arbitrage:
		return THOST_FTDC_HF_Arbitrage;
	case HedgeFlagType::HedgeFlagType_Hedge:
		return THOST_FTDC_HF_Hedge;
	case HedgeFlagType::HedgeFlagType_Speculation:
	default:
		return THOST_FTDC_HF_Speculation;
	}
}

OpenCloseType TThostFtdcOffsetFlagType_2_OpenCloseType(TThostFtdcOffsetFlagType In)
{
	switch (In)
	{
	case THOST_FTDC_OF_CloseToday:
		return OpenCloseType::OpenCloseType_CloseToday;
	case THOST_FTDC_OF_Close:
	case THOST_FTDC_OF_CloseYesterday:
		return OpenCloseType::OpenCloseType_Close;
	case THOST_FTDC_OF_Open:
	default:
		return OpenCloseType::OpenCloseType_Open;
	}
}

TThostFtdcOffsetFlagType OpenCloseType_2_TThostFtdcOffsetFlagType(OpenCloseType In)
{
	switch (In)
	{
	case OpenCloseType::OpenCloseType_CloseToday:
		return THOST_FTDC_OF_CloseToday;
	case OpenCloseType::OpenCloseType_Close:
		return THOST_FTDC_OF_Close;
	case OpenCloseType::OpenCloseType_Open:
	default:
		return THOST_FTDC_OF_Open;
	}
}

TThostFtdcDirectionType OrderSide_2_TThostFtdcDirectionType(OrderSide In)
{
	if (In == OrderSide::OrderSide_Sell)
		return THOST_FTDC_D_Sell;
	return THOST_FTDC_D_Buy;
}

OrderSide TThostFtdcDirectionType_2_OrderSide(TThostFtdcDirectionType In)
{
	if (In == THOST_FTDC_D_Sell)
		return OrderSide::OrderSide_Sell;
	return OrderSide::OrderSide_Buy;
}

PositionSide TThostFtdcPosiDirectionType_2_PositionSide(TThostFtdcPosiDirectionType In)
{
	if (In == THOST_FTDC_PD_Short)
		return PositionSide::PositionSide_Short;
	return PositionSide::PositionSide_Long;
}

PositionSide TradeField_2_PositionSide(TradeField* pIn)
{
	if (pIn->OpenClose == OpenCloseType::OpenCloseType_Open)
	{
		if (pIn->Side == OrderSide::OrderSide_Buy)
			return PositionSide::PositionSide_Long;
		return PositionSide::PositionSide_Short;
	}
	else
	{
		if (pIn->Side == OrderSide::OrderSide_Buy)
			return PositionSide::PositionSide_Short;
		return PositionSide::PositionSide_Long;
	}
}

TThostFtdcOrderPriceTypeType OrderType_2_TThostFtdcOrderPriceTypeType(OrderType In)
{
	switch (In)
	{
	case OrderType_Market:
		return THOST_FTDC_OPT_AnyPrice;
	case OrderType_Stop:
		return THOST_FTDC_OPT_AnyPrice;
	case OrderType_Limit:
		return THOST_FTDC_OPT_LimitPrice;
	case OrderType_StopLimit:
		return THOST_FTDC_OPT_LimitPrice;
	case OrderType_MarketOnClose:
		return THOST_FTDC_OPT_AnyPrice;
	case OrderType_TrailingStop:
		return THOST_FTDC_OPT_AnyPrice;
	case OrderType_TrailingStopLimit:
		return THOST_FTDC_OPT_LimitPrice;
	default:
		return THOST_FTDC_OPT_LimitPrice;
	}
}

OrderStatus CThostFtdcOrderField_2_OrderStatus(CThostFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case THOST_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return OrderStatus::OrderStatus_Rejected;
		return OrderStatus::OrderStatus_Cancelled;
	case THOST_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			return OrderStatus::OrderStatus_New;
	default:
		if (pIn->VolumeTotal == 0)
			return OrderStatus::OrderStatus_Filled;
		else if (pIn->VolumeTotal == pIn->VolumeTotalOriginal)
			return OrderStatus::OrderStatus_New;
		else
			return OrderStatus::OrderStatus_PartiallyFilled;
	}
}

ExecType CThostFtdcOrderField_2_ExecType(CThostFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case THOST_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return ExecType::ExecType_Rejected;
		return ExecType::ExecType_Cancelled;
	case THOST_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			return ExecType::ExecType_New;
	case THOST_FTDC_OST_AllTraded:
	case THOST_FTDC_OST_PartTradedQueueing:
		return ExecType::ExecType_Trade;
	default:
		return ExecType::ExecType_New;
	}
}

OrderType CThostFtdcOrderField_2_OrderType(CThostFtdcOrderField* pIn)
{
	switch (pIn->OrderPriceType)
	{
	case THOST_FTDC_OPT_AnyPrice:
		return OrderType::OrderType_Market;
	case THOST_FTDC_OPT_LimitPrice:
		return OrderType::OrderType_Limit;
	default:
		return OrderType::OrderType_Limit;
	}
}

TimeInForce CThostFtdcOrderField_2_TimeInForce(CThostFtdcOrderField* pIn)
{
	switch (pIn->TimeCondition)
	{
	case THOST_FTDC_TC_GFD:
		return TimeInForce::TimeInForce_Day;
	case THOST_FTDC_TC_IOC:
		switch (pIn->VolumeCondition)
		{
		case THOST_FTDC_VC_AV:
			return TimeInForce::TimeInForce_IOC;
		case THOST_FTDC_VC_CV:
			return TimeInForce::TimeInForce_FOK;
		default:
			return TimeInForce::TimeInForce_IOC;
		}
	default:
		return TimeInForce::TimeInForce_Day;
	}
}


InstrumentType CThostFtdcInstrumentField_2_InstrumentType(CThostFtdcInstrumentField* pIn)
{
	switch (pIn->ProductClass)
	{
	case THOST_FTDC_PC_Futures:
		return InstrumentType::InstrumentType_Future;
	case THOST_FTDC_PC_Options:
		return InstrumentType::InstrumentType_Option;
	case THOST_FTDC_PC_Combination:
		return InstrumentType::InstrumentType_MultiLeg;
	case THOST_FTDC_PC_EFP:
		return InstrumentType::InstrumentType_Future;
	case THOST_FTDC_PC_SpotOption:
		return InstrumentType::InstrumentType_Option;
	default:
		return InstrumentType::InstrumentType_Stock;
	}
}

IdCardType TThostFtdcIdCardTypeType_2_IdCardType(TThostFtdcIdCardTypeType In)
{
	switch (In)
	{
	case THOST_FTDC_ICT_EID:
		return IdCardType::IdCardType_EID;
	case THOST_FTDC_ICT_IDCard:
		return IdCardType::IdCardType_IDCard;
	case THOST_FTDC_ICT_Passport:
		return IdCardType::IdCardType_Passport;
	case THOST_FTDC_ICT_LicenseNo:
		return IdCardType::IdCardType_LicenseNo;
	case THOST_FTDC_ICT_TaxNo:
		return IdCardType::IdCardType_TaxNo;
	case THOST_FTDC_ICT_DrivingLicense:
		return IdCardType::IdCardType_DrivingLicense;
	case THOST_FTDC_ICT_SocialID:
		return IdCardType::IdCardType_SocialID;
	case THOST_FTDC_ICT_LocalID:
		return IdCardType::IdCardType_LocalID;
	case THOST_FTDC_ICT_OtherCard:
	default:
		return IdCardType::IdCardType_OtherCard;
	}
}

TradingPhaseType TThostFtdcInstrumentStatusType_2_TradingPhaseType(TThostFtdcInstrumentStatusType In)
{
	switch (In)
	{
	case THOST_FTDC_IS_BeforeTrading:
		return TradingPhaseType::TradingPhaseType_BeforeTrading;
	case THOST_FTDC_IS_NoTrading:
		return TradingPhaseType::TradingPhaseType_NoTrading;
	case THOST_FTDC_IS_Continous:
		return TradingPhaseType::TradingPhaseType_Continuous;
	case THOST_FTDC_IS_AuctionOrdering:
		return TradingPhaseType::TradingPhaseType_AuctionOrdering;
	case THOST_FTDC_IS_AuctionBalance:
		return TradingPhaseType::TradingPhaseType_AuctionBalance;
	case THOST_FTDC_IS_AuctionMatch:
		return TradingPhaseType::TradingPhaseType_AuctionMatch;
	case THOST_FTDC_IS_Closed:
		return TradingPhaseType::TradingPhaseType_Closed;
	default:
		return TradingPhaseType::TradingPhaseType_NoTrading;
	}
}

ExchangeType TThostFtdcExchangeIDType_2_ExchangeType(TThostFtdcExchangeIDType In)
{
	switch (In[1])
	{
	case 'H':
		return ExchangeType::ExchangeType_SHFE;
	case 'C':
		return ExchangeType::ExchangeType_DCE;
	case 'Z':
		return ExchangeType::ExchangeType_CZCE;
	case 'F':
		return ExchangeType::ExchangeType_CFFEX;
	case 'N':
		return ExchangeType::ExchangeType_INE;
	default:
		return ExchangeType::ExchangeType_Undefined;
	}
}

void CThostFtdcOrderField_2_OrderField_0(OrderIDType OrderID,CThostFtdcOrderField* pIn, OrderField* pOut)
{
	strcpy(pOut->ID, OrderID);
	strcpy(pOut->LocalID, pOut->ID);
	strcpy(pOut->InstrumentID, pIn->InstrumentID);
	strcpy(pOut->ExchangeID, pIn->ExchangeID);
	strcpy(pOut->AccountID, pIn->InvestorID);
	pOut->HedgeFlag = TThostFtdcHedgeFlagType_2_HedgeFlagType(pIn->CombHedgeFlag[0]);
	pOut->Side = TThostFtdcDirectionType_2_OrderSide(pIn->Direction);
	pOut->Price = pIn->LimitPrice;
	pOut->StopPx = pIn->StopPrice;
	strncpy(pOut->Text, pIn->StatusMsg, sizeof(Char256Type));
	pOut->OpenClose = TThostFtdcOffsetFlagType_2_OpenCloseType(pIn->CombOffsetFlag[0]);
	pOut->Status = CThostFtdcOrderField_2_OrderStatus(pIn);
	pOut->Qty = pIn->VolumeTotalOriginal;
	pOut->CumQty = pIn->VolumeTraded;
	pOut->LeavesQty = pIn->VolumeTotal;
	pOut->Type = CThostFtdcOrderField_2_OrderType(pIn);
	pOut->TimeInForce = CThostFtdcOrderField_2_TimeInForce(pIn);
	pOut->ExecType = CThostFtdcOrderField_2_ExecType(pIn);
	strcpy(pOut->OrderID, pIn->OrderSysID);
	strncpy(pOut->Text, pIn->StatusMsg, sizeof(Char256Type));
}

#ifdef HAS_Quote
OrderStatus CThostFtdcQuoteField_2_OrderStatus(CThostFtdcQuoteField* pIn)
{
	switch (pIn->QuoteStatus)
	{
	case THOST_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return OrderStatus::OrderStatus_Rejected;
		return OrderStatus::OrderStatus_Cancelled;
	case THOST_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			return OrderStatus::OrderStatus_New;
	case THOST_FTDC_OST_Touched:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return OrderStatus::OrderStatus_Rejected;
	default:
		// 这个地方要改

		//if (pIn->VolumeTotal == 0)
		//	return OrderStatus::OrderStatus_Filled;
		//else if (pIn->VolumeTotal == pIn->VolumeTotalOriginal)
		return OrderStatus::OrderStatus_New;
		//else
		//	return OrderStatus::OrderStatus_PartiallyFilled;
	}
}

ExecType CThostFtdcQuoteField_2_ExecType(CThostFtdcQuoteField* pIn)
{
	switch (pIn->QuoteStatus)
	{
	case THOST_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return ExecType::ExecType_Rejected;
		return ExecType::ExecType_Cancelled;
	case THOST_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			return ExecType::ExecType_New;
	case THOST_FTDC_OST_AllTraded:
	case THOST_FTDC_OST_PartTradedQueueing:
		return ExecType::ExecType_Trade;
	case THOST_FTDC_OST_Touched:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return ExecType::ExecType_Rejected;
	default:
		return ExecType::ExecType_New;
	}
}
#endif // HAS_Quote

