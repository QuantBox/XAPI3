#pragma once

#include "../../include/ApiStruct.h"
#include "../../include/Tap/TapTradeAPIDataType.h"

OrderSide TAPISideType_2_OrderSide(TAPISideType In);
TAPISideType OrderSide_2_TAPISideType(OrderSide In);

OpenCloseType TAPIPositionEffectType_2_OpenCloseType(TAPIPositionEffectType In);
TAPIPositionEffectType OpenCloseType_2_TAPIPositionEffectType(OpenCloseType In);

//TAPIOrderStateType OrderStatus_2_TAPIOrderStateType(OrderStatus In);
OrderStatus TAPIOrderStateType_2_OrderStatus(TAPIOrderStateType In);

TradingPhaseType TAPITradingStateType_2_InstrumentStatus(TAPITradingStateType In);

const char* ExchangeNo_2_ExchangeID(const char* In);
const char* ExchangeID_2_ExchangeNo(const char* In);

//OpenCloseType EES_SideType_2_OpenCloseType(EES_SideType m_Side);
//EES_SideType OrderSide_OpenCloseType_2_EES_SideType(OrderSide side, OpenCloseType openclose);
//EES_ExchangeID ExchangeID_2_EES_ExchangeID(const char* exchange);
//const char* EES_ExchangeID_2_ExchangeID(EES_ExchangeID exchange);
TAPIHedgeFlagType HedgeFlagType_2_TAPIHedgeFlagType(HedgeFlagType In);
HedgeFlagType TAPIHedgeFlagType_2_HedgeFlagType(TAPIHedgeFlagType In);
PositionSide TAPISideType_2_PositionSide(TAPISideType In);
//
//TradingPhaseType EES_InstrumentStatus_2_InstrumentStatus(unsigned char In);
//const char* EES_CxlReasonCode_2_str(EES_CxlReasonCode In);


