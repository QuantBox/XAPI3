#pragma once

#include "../../include/ApiStruct.h"
#include "../../include/REM/EesTraderDefine.h"

OrderSide EES_SideType_2_OrderSide(EES_SideType m_Side);
OpenCloseType EES_SideType_2_OpenCloseType(EES_SideType m_Side);
EES_SideType OrderSide_OpenCloseType_2_EES_SideType(OrderSide side, OpenCloseType openclose);
EES_ExchangeID ExchangeID_2_EES_ExchangeID(const char* exchange);
const char* EES_ExchangeID_2_ExchangeID(EES_ExchangeID exchange);
EES_HedgeFlag HedgeFlagType_2_EES_HedgeFlag(HedgeFlagType In);
HedgeFlagType EES_HedgeFlag_2_HedgeFlagType(EES_HedgeFlag In);
PositionSide EES_PosiDirection_2_PositionSide(EES_PosiDirection In);

TradingPhaseType EES_InstrumentStatus_2_InstrumentStatus(unsigned char In);
const char* EES_CxlReasonCode_2_str(EES_CxlReasonCode In);


