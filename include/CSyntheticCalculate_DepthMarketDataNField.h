#pragma once

#include "synthetic/CSyntheticMarketData.h"

class CSyntheticCalculateFactory_XAPI:public CSyntheticCalculateFactory
{
public:
	virtual CSyntheticCalculate* Create(string method);
};


class CSyntheticCalculate_DepthMarketDataNField_OpenInterest :public CSyntheticCalculate
{
public:
	virtual void Begin(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut);
	virtual void Sum(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut);
	virtual void ForEach(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut);
	virtual void End(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut);
};

class CSyntheticCalculate_DepthMarketDataNField_Weight :public CSyntheticCalculate
{
public:
	virtual void Begin(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut);
	virtual void Sum(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut);
	virtual void ForEach(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut);
	virtual void End(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut);
};

class CSyntheticCalculate_DepthMarketDataNField_Copy :public CSyntheticCalculate
{
public:
	virtual void Begin(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut);
	virtual void Sum(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut);
	virtual void ForEach(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut);
	virtual void End(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut);
};

class CSyntheticCalculate_DepthMarketDataNField_Spread :public CSyntheticCalculate
{
public:
	virtual void Begin(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut);
	virtual void Sum(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut);
	virtual void ForEach(CSyntheticMarketData* pCall, LegData* pIn, LegData* pOut);
	virtual void End(CSyntheticMarketData* pCall, char* pBuf, LegData* pOut);
};

