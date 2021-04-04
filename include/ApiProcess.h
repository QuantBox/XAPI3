#ifndef _API_PROCESS_H_
#define _API_PROCESS_H_

#include "../include/ApiStruct.h"

// 先对行情数据进行初始化，然后才能进行
void InitBidAsk(DepthMarketDataNField* pField)
{
	pField->Size = sizeof(DepthMarketDataNField);
	pField->BidCount = 0;
}
// 必须先AddBid才能AddAsk，先添加的是买一，然后才是买二
int AddBid(DepthMarketDataNField* pField, PriceType price, VolumeType volume, VolumeType Count)
{
	char* p = ((char*)pField) + pField->Size;
	DepthField* pDF = (DepthField*)p;
	pDF->Price = price;
	pDF->Size = volume;
	pDF->Count = Count;
	pField->Size += sizeof(DepthField);
	++pField->BidCount;

	return 0;
}
// 先添加的是卖一，然后才是卖二
int AddAsk(DepthMarketDataNField* pField, PriceType price, VolumeType volume, VolumeType Count)
{
	char* p = ((char*)pField) + pField->Size;
	DepthField* pDF = (DepthField*)p;
	pDF->Price = price;
	pDF->Size = volume;
	pDF->Count = Count;
	pField->Size += sizeof(DepthField);

	return 0;
}
// 从结构体中读出买档，Pos=1是买一
DepthField* GetBid(DepthMarketDataNField* pField, int Pos)
{
	if (Pos <= 0 || Pos > pField->BidCount)
		return nullptr;

	char* p = ((char*)pField) + sizeof(DepthMarketDataNField);
	DepthField* pDF = (DepthField*)p;
	return pDF + Pos - 1;
}

// 从结构体中读出卖档，Pos=1是卖一
DepthField* GetAsk(DepthMarketDataNField* pField, int Pos)
{
	int count = (pField->Size - sizeof(DepthMarketDataNField)) / sizeof(DepthField);
	int AskCount = count - pField->BidCount;

	if (Pos <= 0 || Pos > AskCount)
		return nullptr;

	char* p = ((char*)pField) + sizeof(DepthMarketDataNField);
	DepthField* pDF = (DepthField*)p;
	return pDF + pField->BidCount + (Pos - 1);
}

char* ExchangeType_2_String(ExchangeType exchange)
{
	switch (exchange)
	{
	case ExchangeType::ExchangeType_SHFE:
		return "SHFE";
	case ExchangeType::ExchangeType_DCE:
		return "DCE";
	case ExchangeType::ExchangeType_CZCE:
		return "CZCE";
	case ExchangeType::ExchangeType_CFFEX:
		return "CFFEX";
	case ExchangeType::ExchangeType_INE:
		return "INE";
	case ExchangeType::ExchangeType_SSE:
		return "SSE";
	case ExchangeType::ExchangeType_SZSE:
		return "SZSE";
	case ExchangeType::ExchangeType_SGE:
		return "SGE";
	case ExchangeType::ExchangeType_NEEQ:
		return "NEEQ";
	case ExchangeType::ExchangeType_HKEx:
		return "HKEx";
	case ExchangeType::ExchangeType_Undefined:
	default:
		return "Undefined";
	}
}

#endif