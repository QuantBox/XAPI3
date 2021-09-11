#pragma once
#include "stdafx.h"
#include "TypeConvert.h"

#include <string>
#include <sstream>

using namespace std;

OrderSide TAPISideType_2_OrderSide(TAPISideType In)
{
	switch (In)
	{
	case TAPI_SIDE_BUY:
		return OrderSide_Buy;
	case TAPI_SIDE_SELL:
		return OrderSide_Sell;
	}
	return OrderSide_Buy;
}

TAPISideType OrderSide_2_TAPISideType(OrderSide In)
{
	switch (In)
	{
	case OrderSide_Buy:
		return TAPI_SIDE_BUY;
	case OrderSide_Sell:
		return TAPI_SIDE_SELL;
	}
	return TAPI_SIDE_BUY;
}

OpenCloseType TAPIPositionEffectType_2_OpenCloseType(TAPIPositionEffectType In)
{
	switch (In)
	{
	case TAPI_PositionEffect_OPEN:
		return OpenCloseType_Open;
	case TAPI_PositionEffect_COVER:
		return OpenCloseType_Close;
	case TAPI_PositionEffect_COVER_TODAY:
		return OpenCloseType_CloseToday;
	}
	return OpenCloseType_Open;
}

TAPIPositionEffectType OpenCloseType_2_TAPIPositionEffectType(OpenCloseType In)
{
	switch (In)
	{
	case OpenCloseType_Open:
		return TAPI_PositionEffect_OPEN;
	case OpenCloseType_Close:
		return TAPI_PositionEffect_COVER;
	case OpenCloseType_CloseToday:
		return TAPI_PositionEffect_COVER_TODAY;
	}
	return TAPI_PositionEffect_OPEN;
}

OrderStatus TAPIOrderStateType_2_OrderStatus(TAPIOrderStateType In)
{
	switch (In)
	{
	case TAPI_ORDER_STATE_ACCEPT:
		return OrderStatus_New;
	case TAPI_ORDER_STATE_QUEUED:
		return OrderStatus_New;
	case TAPI_ORDER_STATE_PARTFINISHED:
		return OrderStatus_PartiallyFilled;
	case TAPI_ORDER_STATE_FINISHED:
		return OrderStatus_Filled;
	case TAPI_ORDER_STATE_CANCELING:
		return OrderStatus_PendingCancel;
	case TAPI_ORDER_STATE_CANCELED:
		return OrderStatus_Cancelled;
	case TAPI_ORDER_STATE_FAIL:
		return OrderStatus_Rejected;
	}
	return OrderStatus_New;
}


TradingPhaseType TAPITradingStateType_2_InstrumentStatus(TAPITradingStateType In)
{
	switch (In)
	{
	case TAPI_TRADE_STATE_CONTINUOUS:
		return TradingPhaseType::TradingPhaseType_Continuous;
	case TAPI_TRADE_STATE_BID:
		return TradingPhaseType::TradingPhaseType_AuctionOrdering;
	case TAPI_TRADE_STATE_MATCH:
		return TradingPhaseType::TradingPhaseType_AuctionMatch;
	case TAPI_TRADE_STATE_CLOSE:
		return TradingPhaseType::TradingPhaseType_Closed;
	default:
		return TradingPhaseType::TradingPhaseType_NoTrading;
	}
}

const char* ExchangeNo_2_ExchangeID(const char *In)
{
	if (In[0]=='Z')
	{
		return "CZCE";
	}
	return In;
}

const char* ExchangeID_2_ExchangeNo(const char* In)
{
	if (In[1] == 'Z')
	{
		return "ZCE";
	}
	return In;
}

TAPIHedgeFlagType HedgeFlagType_2_TAPIHedgeFlagType(HedgeFlagType In)
{
	switch (In)
	{
	case HedgeFlagType_Speculation:
		return TAPI_HEDGEFLAG_T;
	case HedgeFlagType_Arbitrage:
		return TAPI_HEDGEFLAG_L;
	case HedgeFlagType_Hedge:
		return TAPI_HEDGEFLAG_B;
	}
	return TAPI_HEDGEFLAG_T;
}

HedgeFlagType TAPIHedgeFlagType_2_HedgeFlagType(TAPIHedgeFlagType In)
{
	switch (In)
	{
	case TAPI_HEDGEFLAG_T:
		return HedgeFlagType_Speculation;
	case TAPI_HEDGEFLAG_L:
		return HedgeFlagType_Arbitrage;
	case TAPI_HEDGEFLAG_B:
		return HedgeFlagType_Hedge;
	}
	return HedgeFlagType_Speculation;
}

PositionSide TAPISideType_2_PositionSide(TAPISideType In)
{
	switch (In)
	{
	case TAPI_SIDE_BUY:
		return PositionSide_Long;
	case TAPI_SIDE_SELL:
		return PositionSide_Short;
	}
	return PositionSide_Long;
}
//
//TradingPhaseType EES_InstrumentStatus_2_InstrumentStatus(unsigned char In)
//{
//	///< 交易状态： '0':开盘前; '1':非交易; '2':连续交易; '3':集合竞价报单; '4'集合竞价价格平衡; '5':集合竞价撮合; '6': 收盘;
//	switch (In)
//	{
//	case '0':
//		return TradingPhaseType::TradingPhaseType_BeforeTrading;
//	case '1':
//		return TradingPhaseType::TradingPhaseType_NoTrading;
//	case '2':
//		return TradingPhaseType::TradingPhaseType_Continuous;
//	case '3':
//		return TradingPhaseType::TradingPhaseType_AuctionOrdering;
//	case '4':
//		return TradingPhaseType::TradingPhaseType_AuctionBalance;
//	case '5':
//		return TradingPhaseType::TradingPhaseType_AuctionMatch;
//	case '6':
//		return TradingPhaseType::TradingPhaseType_Closed;
//	default:
//		return TradingPhaseType::TradingPhaseType_NoTrading;
//	}
//}
//
//const char* EES_CxlReasonCode_2_str(EES_CxlReasonCode In)
//{
//	switch (In)
//	{
//	case EES_CxlReasonCode_by_account:
//		return "用户撤单";
//	case EES_CxlReasonCode_timeout:
//		return "系统timeout, 单子到期被交易所系统取消";
//	case EES_CxlReasonCode_supervisory:
//		return "Supervisory, 被盛立系统管理者取消";
//	case EES_CxlReasonCode_by_market:
//		return "被市场拒绝";
//	case EES_CxlReasonCode_another:
//		return "其他";
//	case 0:
//		return "已撤单";
//	default:
//		return "未知 ";
//	}
//}

