#pragma once
#include "stdafx.h"
#include "TypeConvert.h"

#include <string>
#include <sstream>

using namespace std;

OrderSide EES_SideType_2_OrderSide(EES_SideType m_Side)
{
	switch (m_Side)
	{
	case EES_SideType_open_long:
	case EES_SideType_close_today_short:
	case EES_SideType_close_ovn_short:
	case EES_SideType_close_short:
		return OrderSide_Buy;
	case EES_SideType_open_short:
	case EES_SideType_close_today_long:
	case EES_SideType_close_ovn_long:
	case EES_SideType_close_long:
		return OrderSide_Sell;
	}
	return OrderSide_Buy;
}

OpenCloseType EES_SideType_2_OpenCloseType(EES_SideType m_Side)
{
	switch (m_Side)
	{
	case EES_SideType_open_long:
	case EES_SideType_open_short:
		return OpenCloseType_Open;
	case EES_SideType_close_long:
	case EES_SideType_close_short:
	case EES_SideType_close_ovn_long:
	case EES_SideType_close_ovn_short:
		return OpenCloseType_Close;
	case EES_SideType_close_today_short:
	case EES_SideType_close_today_long:
		return OpenCloseType_CloseToday;
	}
	return OpenCloseType_Close;
}

EES_SideType OrderSide_OpenCloseType_2_EES_SideType(OrderSide side, OpenCloseType openclose)
{
	switch (side)
	{
	case OrderSide_Buy:
		switch (openclose)
		{
		case OpenCloseType_Open:
			return EES_SideType_open_long;
		case OpenCloseType_Close:
			return EES_SideType_close_short;
		case OpenCloseType_CloseToday:
			return EES_SideType_close_today_short;
		}
		break;
	case OrderSide_Sell:
		switch (openclose)
		{
		case OpenCloseType_Open:
			return EES_SideType_open_short;
		case OpenCloseType_Close:
			return EES_SideType_close_long;
		case OpenCloseType_CloseToday:
			return EES_SideType_close_today_long;
		}
		break;
	}
	return EES_SideType_close_short;
}

EES_ExchangeID ExchangeID_2_EES_ExchangeID(const char* exchange)
{
	switch (exchange[1])
	{
	case 'H'://ExchangeType::ExchangeType_SHFE:
		return EES_ExchangeID_shfe;
	case 'C'://ExchangeType::ExchangeType_DCE:
		return EES_ExchangeID_dce;
	case 'Z'://ExchangeType::ExchangeType_CZCE:
		return EES_ExchangeID_zcze;
	case 'F'://ExchangeType::ExchangeType_CFFEX:
		return EES_ExchangeID_cffex;
	case 'N'://ExchangeType::ExchangeType_INE:
		return EES_ExchangeID_ine;
	case 'S'://ExchangeType::ExchangeType_SSE:
		return EES_ExchangeID_sh_cs;
		//case ExchangeType::ExchangeType_SZSE:
		//	return "SZSE";
		//case ExchangeType::ExchangeType_SGE:
		//	return "SGE";
		//case ExchangeType::ExchangeType_NEEQ:
		//	return "NEEQ";
		//case ExchangeType::ExchangeType_HKEx:
		//	return "HKEx";
		//case ExchangeType::ExchangeType_Undefined:
		//default:
		//	return "Undefined";
	}
	return EES_ExchangeID_shfe;
}

const char* EES_ExchangeID_2_ExchangeID(EES_ExchangeID exchange)
{
	switch (exchange)
	{
	case EES_ExchangeID_cffex:
		return "CFFEX";
	case EES_ExchangeID_shfe:
		return "SHFE";
	case EES_ExchangeID_dce:
		return "DCE";
	case EES_ExchangeID_zcze:
		return "CZCE";
	default:
		return "Undefined";
	}
}

EES_HedgeFlag HedgeFlagType_2_EES_HedgeFlag(HedgeFlagType In)
{
	switch (In)
	{
	case HedgeFlagType_Speculation:
		return EES_HedgeFlag_Speculation;
	case HedgeFlagType_Arbitrage:
		return EES_HedgeFlag_Arbitrage;
	case HedgeFlagType_Hedge:
		return EES_HedgeFlag_Hedge;
	}
	return EES_HedgeFlag_Speculation;
}

HedgeFlagType EES_HedgeFlag_2_HedgeFlagType(EES_HedgeFlag In)
{
	switch (In)
	{
	case EES_HedgeFlag_Speculation:
		return HedgeFlagType_Speculation;
	case EES_HedgeFlag_Arbitrage:
		return HedgeFlagType_Arbitrage;
	case EES_HedgeFlag_Hedge:
		return HedgeFlagType_Hedge;
	}
	return HedgeFlagType_Speculation;
}

PositionSide EES_PosiDirection_2_PositionSide(EES_PosiDirection In)
{
	switch (In)
	{
	case EES_PosiDirection_long:
		return PositionSide_Long;
	case EES_PosiDirection_short:
		return PositionSide_Short;
	}
	return PositionSide_Long;
}

TradingPhaseType EES_InstrumentStatus_2_InstrumentStatus(unsigned char In)
{
	///< 交易状态： '0':开盘前; '1':非交易; '2':连续交易; '3':集合竞价报单; '4'集合竞价价格平衡; '5':集合竞价撮合; '6': 收盘;
	switch (In)
	{
	case '0':
		return TradingPhaseType::TradingPhaseType_BeforeTrading;
	case '1':
		return TradingPhaseType::TradingPhaseType_NoTrading;
	case '2':
		return TradingPhaseType::TradingPhaseType_Continuous;
	case '3':
		return TradingPhaseType::TradingPhaseType_AuctionOrdering;
	case '4':
		return TradingPhaseType::TradingPhaseType_AuctionBalance;
	case '5':
		return TradingPhaseType::TradingPhaseType_AuctionMatch;
	case '6':
		return TradingPhaseType::TradingPhaseType_Closed;
	default:
		return TradingPhaseType::TradingPhaseType_NoTrading;
	}
}

const char* EES_CxlReasonCode_2_str(EES_CxlReasonCode In)
{
	switch (In)
	{
	case EES_CxlReasonCode_by_account:
		return "用户撤单";
	case EES_CxlReasonCode_timeout:
		return "系统timeout, 单子到期被交易所系统取消";
	case EES_CxlReasonCode_supervisory:
		return "Supervisory, 被盛立系统管理者取消";
	case EES_CxlReasonCode_by_market:
		return "被市场拒绝";
	case EES_CxlReasonCode_another:
		return "其他";
	case 0:
		return "已撤单";
	default:
		return "未知 ";
	}
}

