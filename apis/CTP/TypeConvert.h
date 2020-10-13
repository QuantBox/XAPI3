#pragma once

#include "../../include/ApiStruct.h"

// 由于这个Include.h需要使用每个项目自己的文件，所以需要在VC++ Directories->Include Directories中添加一个"./"
#include "Include.h"

PutCall TThostFtdcOptionsTypeType_2_PutCall(TThostFtdcOptionsTypeType In);

HedgeFlagType TThostFtdcHedgeFlagType_2_HedgeFlagType(TThostFtdcHedgeFlagType In);
TThostFtdcHedgeFlagType HedgeFlagType_2_TThostFtdcHedgeFlagType(HedgeFlagType In);

TThostFtdcOffsetFlagType OpenCloseType_2_TThostFtdcOffsetFlagType(OpenCloseType In);
OpenCloseType TThostFtdcOffsetFlagType_2_OpenCloseType(TThostFtdcOffsetFlagType In);

TThostFtdcDirectionType OrderSide_2_TThostFtdcDirectionType(OrderSide In);
OrderSide TThostFtdcDirectionType_2_OrderSide(TThostFtdcDirectionType In);
PositionSide TThostFtdcPosiDirectionType_2_PositionSide(TThostFtdcPosiDirectionType In);
PositionSide TradeField_2_PositionSide(TradeField* pIn);

TThostFtdcOrderPriceTypeType OrderType_2_TThostFtdcOrderPriceTypeType(OrderType In);

OrderType CThostFtdcOrderField_2_OrderType(CThostFtdcOrderField* pIn);
TimeInForce CThostFtdcOrderField_2_TimeInForce(CThostFtdcOrderField* pIn);

OrderStatus CThostFtdcOrderField_2_OrderStatus(CThostFtdcOrderField* pIn);
ExecType CThostFtdcOrderField_2_ExecType(CThostFtdcOrderField* pIn);



InstrumentType CThostFtdcInstrumentField_2_InstrumentType(CThostFtdcInstrumentField* pIn);

IdCardType TThostFtdcIdCardTypeType_2_IdCardType(TThostFtdcIdCardTypeType In);

ExchangeType TThostFtdcExchangeIDType_2_ExchangeType(TThostFtdcExchangeIDType In);

void CThostFtdcOrderField_2_OrderField_0(OrderIDType OrderID, CThostFtdcOrderField* pIn, OrderField* pOut);

TradingPhaseType TThostFtdcInstrumentStatusType_2_TradingPhaseType(TThostFtdcInstrumentStatusType In);


#ifdef HAS_Quote
OrderStatus CThostFtdcQuoteField_2_OrderStatus(CThostFtdcQuoteField* pIn);
ExecType CThostFtdcQuoteField_2_ExecType(CThostFtdcQuoteField* pIn);
#endif // HAS_Quote

