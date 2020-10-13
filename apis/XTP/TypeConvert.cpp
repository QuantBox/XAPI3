#include "TypeConvert.h"
#include <stdint.h>
#include <string.h>

#include "../../include/ApiProcess.h"
#include "../../include/XTP/xtp_api_data_type.h"
// #include "Include.h"


const char* XTPExchangeType_2_Str(int xtp_exchange_type)
{
	switch (xtp_exchange_type)
	{
	case XTP_EXCHANGE_SH: return ExchangeType_2_String(ExchangeType_SSE);
	case XTP_EXCHANGE_SZ: return ExchangeType_2_String(ExchangeType_SZSE);
	}
	return ExchangeType_2_String(ExchangeType_Undefined);
}
const char* XTPMarketType_2_Str(int xtp_market_type)
{
	switch (xtp_market_type)
	{
	case XTP_MKT_SZ_A: return ExchangeType_2_String(ExchangeType_SSE);
	case XTP_MKT_SH_A: return ExchangeType_2_String(ExchangeType_SZSE);
	}
	return ExchangeType_2_String(ExchangeType_Undefined);
}
int Str_2_XTPExchangeType(const char *exchange_str)
{
	if (strcmp(exchange_str, ExchangeType_2_String(ExchangeType_SSE)) == 0)
	{
		return XTP_EXCHANGE_SH;
	}
	if (strcmp(exchange_str, ExchangeType_2_String(ExchangeType_SZSE)) == 0)
	{
		return XTP_EXCHANGE_SZ;
	}
	return XTP_EXCHANGE_UNKNOWN;
}
int Str_2_XTPMarketType(const char *market_str)
{
	if (strcmp(market_str, ExchangeType_2_String(ExchangeType_SSE)) == 0)
	{
		return XTP_MKT_SH_A;
	}
	if (strcmp(market_str, ExchangeType_2_String(ExchangeType_SZSE)) == 0)
	{
		return XTP_MKT_SZ_A;
	}
	return XTP_MKT_UNKNOWN;
}

int OrderDir_XTP_2_XAPI(int xtp_side)
{
	switch (xtp_side)
	{
	case XTP_SIDE_BUY: return OrderSide_Buy;
	case XTP_SIDE_SELL: return OrderSide_Sell;
	}
	return OrderSide_Unknown;
}
int OrderDir_XAPI_2_XTP(int side)
{
	switch (side)
	{
	case OrderSide_Buy: return XTP_SIDE_BUY;
	case OrderSide_Sell: return XTP_SIDE_SELL;
	}
	return XTP_SIDE_UNKNOWN;
}

int PositionDir_XTP_2_XAPI(int flag)
{
	switch (flag)
	{
	case XTP_POSITION_DIRECTION_LONG: return PositionSide::PositionSide_Long;
	case XTP_POSITION_DIRECTION_SHORT: return PositionSide::PositionSide_Short;
	}

	return PositionSide::PositionSide_Long;
}
int PositionDir_XAPI_2_XTP(int flag)
{
	switch (flag)
	{
	case PositionSide::PositionSide_Long: return XTP_POSITION_DIRECTION_LONG;
	case PositionSide::PositionSide_Short: return XTP_POSITION_DIRECTION_SHORT;
	}
	return XTP_POSITION_DIRECTION_NET;
}

int OrderType_XTP_2_XAPI(int flag)
{
	switch (flag)
	{
	case XTP_PRICE_LIMIT: return OrderType_Limit;
	case XTP_PRICE_ALL_OR_CANCEL: return OrderType_Market;
	}
	return OrderType_Market;
}
int OrderType_XAPI_2_XTP(int flag)
{
	switch (flag)
	{
	case OrderType_Limit: return XTP_PRICE_LIMIT;
	case OrderType_Market: return XTP_PRICE_ALL_OR_CANCEL;
	}
	return XTP_PRICE_ALL_OR_CANCEL;
}

int OpenClose_XTP_2_XAPI(int flag)
{
	switch (flag)
	{
	case XTP_POSITION_EFFECT_OPEN: return OpenCloseType_Open;
	case XTP_POSITION_EFFECT_CLOSE: return OpenCloseType_Close;
	case XTP_POSITION_EFFECT_CLOSETODAY: return  OpenCloseType_CloseToday;
	}
	return XTP_POSITION_EFFECT_UNKNOWN;
}
int OpenClose_XAPI_2_XTP(int flag)
{
	switch (flag)
	{
	case OpenCloseType_Open: return XTP_POSITION_EFFECT_OPEN;
	case OpenCloseType_Close: return XTP_POSITION_EFFECT_CLOSE;
	case OpenCloseType_CloseToday: return XTP_POSITION_EFFECT_CLOSETODAY;
	}
	return XTP_POSITION_EFFECT_INIT;
}

int OrderStatus_XTP_2_XAPI(int flag)
{
	switch (flag)
	{
	case XTP_ORDER_STATUS_INIT: return OrderStatus_New;
	case XTP_ORDER_STATUS_ALLTRADED: return OrderStatus_Filled;
	case XTP_ORDER_STATUS_PARTTRADEDQUEUEING: return OrderStatus_PartiallyFilled;
	case XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING: return OrderStatus_PendingCancel;
	case XTP_ORDER_STATUS_CANCELED: return OrderStatus_Cancelled;
	case XTP_ORDER_STATUS_REJECTED: return OrderStatus_Rejected;
	}
	return OrderStatus_NotSent;
}
int OrderStatus_XAPI_2_XTP(int flag)
{
	switch (flag)
	{
	case OrderStatus_New: return XTP_ORDER_STATUS_INIT;
	case OrderStatus_Filled: return XTP_ORDER_STATUS_ALLTRADED;
	case OrderStatus_PartiallyFilled: return XTP_ORDER_STATUS_PARTTRADEDQUEUEING;
	case OrderStatus_PendingCancel: return XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING;
	case OrderStatus_Cancelled: return XTP_ORDER_STATUS_CANCELED;
	case OrderStatus_Rejected: return XTP_ORDER_STATUS_REJECTED;
	}
	return XTP_ORDER_STATUS_UNKNOWN;
}

int TradeField_2_PositionSide(TradeField* pIn)
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