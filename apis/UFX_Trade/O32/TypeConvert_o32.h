#pragma once


#include "../../../include/ApiStruct.h"

#include "ufx_enum_o32.h"
#include "ufx_struct_o32.h"

p_entrust_direction OrderSide_2_entrust_direction(OrderSide In);
p_entrust_direction OrderSide_2_entrust_direction_HG(OrderSide In);
OrderSide entrust_direction_2_OrderSide(_entrust_direction In);
OpenCloseType entrust_direction_2_OpenCloseType(_entrust_direction In);

_futures_direction OpenCloseType_2_futures_direction(OpenCloseType In);
OpenCloseType futures_direction_2_OpenCloseType(_futures_direction In);
_close_direction OpenCloseType_2_close_direction(OpenCloseType In);

OrderStatus entrust_state_2_OrderStatus(_entrust_state In);
ExecType entrust_state_2_ExecType(_entrust_state In);

ExchangeType market_no_2_ExchangeType(_market_no In);
BusinessType market_no_2_BusinessType(_market_no In);

PositionSide position_flag_2_PositionSide(_position_flag In);

void OrderField_2_REQ_QHWT_91004(RSP_DL_10001* pRspUserLogin, OrderField* pIn, REQ_QHWT_91004* pOut);
void OrderField_2_REQ_PTMMWT_91001(RSP_DL_10001* pRspUserLogin, OrderField* pIn, REQ_PTMMWT_91001* pOut);



const char* risk_operation_2_String(_risk_operation In);
