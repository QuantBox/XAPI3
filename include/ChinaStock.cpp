#include "stdafx.h"
#include "ChinaStock.h"

#include <stdlib.h>

//http://www.jisilu.cn/question/37830
//提问：如何识别哪些分级是上海分级基金？
//回答：上海分级母基、A和B份额的代码是以50开头的，深交所的分级A和B类的代码以15开头，母基的代码以16开头。

//http://www.sse.com.cn/assortment/fund/list/
//http://lefu.szse.cn/
InstrumentType InstrumentID_2_InstrumentType_SSE(int In)
{
	// 只有6位，8位的期权已经提前过滤
	int prefix1 = In / 100000;
	int prefix2 = In / 10000;
	int prefix3 = In / 1000;
	switch (prefix1)
	{
	case 0:
		switch (prefix3)
		{
		case 0:
			return InstrumentType::InstrumentType_Index;
		case 9:
		case 10:
		case 18:
		case 19:
		case 90:
		case 99:
			return InstrumentType::InstrumentType_Bond;
		default:
			return InstrumentType::InstrumentType_Index;
		}
	case 1:
		return InstrumentType::InstrumentType_Bond;
	case 2:
		return InstrumentType::InstrumentType_Bond;
	case 3:
		return InstrumentType::InstrumentType_Future;
	case 5:
		switch (prefix2)
		{
		case 50:
			return InstrumentType::InstrumentType_LOF; //
		case 51:
			return InstrumentType::InstrumentType_ETF; //
		}
		switch (prefix3)
		{
		case 500:
		case 510:
		case 519:
		case 521:
		case 522:
		case 523:
		case 524:
			return InstrumentType::InstrumentType_ETF;
		case 580:
		case 582:
			return InstrumentType::InstrumentType_Option;
		default:
			return InstrumentType::InstrumentType_ETF;
		}
	case 6:
		return InstrumentType::InstrumentType_Stock;
	case 9:
		return InstrumentType::InstrumentType_Stock;
	default:
		return InstrumentType::InstrumentType_Synthetic;
	}
}

InstrumentType InstrumentID_2_InstrumentType_SZSE(int In)
{
	// 只有6位，取前2
	int prefix1 = In / 100000;
	int prefix2 = In / 10000;
	int prefix3 = In / 1000;
	switch (prefix2)
	{
	case 0:
	case 7:
	case 9:
		return InstrumentType::InstrumentType_Stock;
	case 3:
	case 8:
		return InstrumentType::InstrumentType_Option;
	case 10:
	case 11:
	case 12:
	case 13:
		return InstrumentType::InstrumentType_Bond;
	case 15:// AB
		switch (prefix3)
		{
		case 159:
			return InstrumentType::InstrumentType_ETF;
		}
		return InstrumentType::InstrumentType_LOF;
	case 16:// 母基
		return InstrumentType::InstrumentType_LOF;
	case 17:
	case 18:
		return InstrumentType::InstrumentType_ETF;
	case 20:
	case 30:
		return InstrumentType::InstrumentType_Stock;
	case 27:
	case 37:
		return InstrumentType::InstrumentType_Stock;
	case 28:
	case 38:
		return InstrumentType::InstrumentType_Option;
	case 39:
		return InstrumentType::InstrumentType_Index;
	default:
		return InstrumentType::InstrumentType_Synthetic;
	}
}

InstrumentType InstrumentID_2_InstrumentType_NEEQ(int In)
{
	return InstrumentType::InstrumentType_Stock;
}

PriceType InstrumentID_2_PriceTick_SSE(int In)
{
	// 只有6位，8位的期权已经提前过滤
	int prefix1 = In / 100000;
	int prefix2 = In / 10000;
	int prefix3 = In / 1000;
	switch (prefix1)
	{
	case 0:
		return 0.01;
	case 1:
		return 0.01;
	case 2:
		return 0.001;
	case 3:
		return 0.01;
	case 5:
		return 0.001;
	case 6:
		return 0.01;
	case 9:
		return 0.001;
	case 100:
		return 0.001;
	case 900:
		return 0.001;
	default:
		return 0.01;
	}

	return 0.01;
}

PriceType InstrumentID_2_PriceTick_SZSE(int In)
{
	// 只有6位，取前2
	int prefix1 = In / 100000;
	int prefix2 = In / 10000;
	int prefix3 = In / 1000;
	switch (prefix2)
	{
	case 0:
	case 7:
	case 9:
		return 0.01;
	case 3:
	case 8:
		return 0.001;
	case 10:
	case 11:
	case 12:
	case 13:
		return 0.001;
	case 15:
	case 16:
		return 0.001;
	case 17:
	case 18:
		return 0.001;
	case 20:
	case 30:
		return 0.01;
	case 27:
	case 37:
		return 0.01;
	case 28:
	case 38:
		return 0.001;
	case 39:
		return 0.01;
	default:
		return 0.01;
	}
}

PriceType InstrumentID_2_PriceTick_NEEQ(int In)
{
	return 0.01;
}