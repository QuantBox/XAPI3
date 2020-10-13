#ifndef TYPE_CONVERT_H_
#define TYPE_CONVERT_H_

struct TradeField;

const char* XTPExchangeType_2_Str(int xtp_exchange_type);
const char* XTPMarketType_2_Str(int xtp_market_type);

int Str_2_XTPExchangeType(const char *exchange_str);
int Str_2_XTPMarketType(const char *market_str);

int OrderDir_XTP_2_XAPI(int xtp_side);
int OrderDir_XAPI_2_XTP(int side);

int PositionDir_XTP_2_XAPI(int flag);
int PositionDir_XAPI_2_XTP(int flag);

int OrderType_XTP_2_XAPI(int flag);
int OrderType_XAPI_2_XTP(int flag);

int OpenClose_XTP_2_XAPI(int flag);
int OpenClose_XAPI_2_XTP(int flag);

int OrderStatus_XTP_2_XAPI(int flag);
int OrderStatus_XAPI_2_XTP(int flag);

int TradeField_2_PositionSide(TradeField* pIn);

#endif