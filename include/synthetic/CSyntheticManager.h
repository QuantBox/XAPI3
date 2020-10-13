#pragma once

#include <map>
#include <set>
#include <list>
#include <mutex>

#include "CSyntheticMarketData.h"

class CSyntheticManager
{
public:
	CSyntheticManager();
	~CSyntheticManager();

	// 由于原设想的在Upate中IF1802同时创建IF000和IF888不可行，所以决定提前创建
	CSyntheticMarketData* Register(const char* product, set<string> emits, map<string,double> legs, CSyntheticCalculate* pCal);
	set<string> UnRegister(const char* product);

	// 先根据合约名更新合约内存区
	// 注意，返回的内存块0号位是标记位
	char* Update(const char* instrument, void* pData, int size);
	
	set<CSyntheticMarketData*> GetByInstrument(const char* instrument);

private:
	// 记录的多少个合成合约计算对象
	map<string, CSyntheticMarketData*>	m_map_by_product;
	// 只记录的原始的行情buf，每个行情块会有多个指针指向它
	map<string, char*>	m_map_by_instrument;
	// 行情过来后，只想计算它所对应的序列
	map<string, set<CSyntheticMarketData*>>	m_map_set_by_instrument;

	mutex							m_cs;
};

