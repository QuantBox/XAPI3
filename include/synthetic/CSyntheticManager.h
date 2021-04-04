#pragma once

#include <map>
#include <set>
#include <list>
#include <mutex>

#include "CSyntheticMarketData.h"

class CSyntheticManager
{
public:
	~CSyntheticManager()
	{
		for (auto iter = m_map_by_product.begin(); iter != m_map_by_product.end(); iter++)
		{
			delete iter->second;
		}
		m_map_by_product.clear();
		for (auto iter = m_map_by_instrument.begin(); iter != m_map_by_instrument.end(); iter++)
		{
			delete[] iter->second;
		}
		m_map_by_instrument.clear();
	}

	// 由于原设想的在Upate中IF1802同时创建IF000和IF888不可行，所以决定提前创建
	CSyntheticMarketData* Register(const char* product, set<string> emits, map<string, double> legs, CSyntheticCalculate* pCal)
	{
		// 用于注册多少个合成合约
		CSyntheticMarketData* pSMD = nullptr;
		auto it = m_map_by_product.find(product);
		if (it == m_map_by_product.end())
		{
			pSMD = new CSyntheticMarketData(product, emits, pCal);
			m_map_by_product.insert(pair<string, CSyntheticMarketData*>(product, pSMD));
		}
		else
		{
			pSMD = it->second;
		}

		// 登记需要用到多少实际合约
		for (auto iter = legs.begin(); iter != legs.end(); iter++)
		{
			char* pBuf = nullptr;
			auto it = m_map_by_instrument.find(iter->first);
			if (it == m_map_by_instrument.end())
			{
				// 创建
				pBuf = new char[LEG_DATA_BUF_LEN]();
				memset(pBuf, 0, LEG_DATA_BUF_LEN);
				m_map_by_instrument.insert(pair<string, char*>(iter->first, pBuf));
			}
			else
			{
				pBuf = it->second;
			}

			pSMD->Create(iter->first.c_str(), iter->second, pBuf);

			// 关联实盘合约与所能合成的合约 
			auto it2 = m_map_set_by_instrument.find(iter->first);
			if (it2 == m_map_set_by_instrument.end())
			{
				auto p = set<CSyntheticMarketData*>();
				p.insert(pSMD);
				m_map_set_by_instrument.insert(pair<string, set<CSyntheticMarketData*>>(iter->first, p));
			}
			else
			{
				it2->second.insert(pSMD);
			}
		}
		// 不管有没有用到权重，更新一下
		//pSMD->UpdateWeight();
		return pSMD;
	}
	set<string> UnRegister(const char* product)
	{
		auto it = m_map_by_product.find(product);
		if (it == m_map_by_product.end())
		{
			// 没找到，这是开玩笑的吧
			set<string> ss;
			return ss;
		}
		else
		{
			// 找到合成器指针
			auto p = it->second;
			// 找到了，需要得到用到了哪些合约
			auto ss = p->GetInstruments();
			for (auto s1 = ss.begin(); s1 != ss.end(); s1++)
			{
				auto s2 = m_map_set_by_instrument.find(s1->data());
				if (s2 == m_map_set_by_instrument.end())
				{
					// 没找到，这是开玩笑的吧
				}
				else
				{
					// 将此实际合约可以组合成的合成器删了
					s2->second.erase(p);
					if (s2->second.size() == 0)
					{
						// 没有要订的，需要删除
						m_map_by_instrument.erase(s1->data());
					}
				}
			}

			m_map_by_product.erase(product);

			return ss;
		}
	}

	// 先根据合约名更新合约内存区
	// 注意，返回的内存块0号位是标记位
	char* Update(const char* instrument, void* pData, int size)
	{
		auto it = m_map_by_instrument.find(instrument);
		if (it == m_map_by_instrument.end())
		{
			// 没找到
			return nullptr;
		}
		else
		{
			//// 用第一个字节标记数据已经收到
			//*it->second = 1;
			//memcpy(it->second+1, pData, size);
			memcpy(it->second, pData, size);
			return it->second;
		}
	}
	
	set<CSyntheticMarketData*> GetByInstrument(const char* instrument)
	{
		auto it2 = m_map_set_by_instrument.find(instrument);
		if (it2 == m_map_set_by_instrument.end())
		{
			auto p = set<CSyntheticMarketData*>();
			return p;
		}
		else
		{
			return it2->second;
		}
	}

private:
	// 记录的多少个合成合约计算对象
	map<string, CSyntheticMarketData*>	m_map_by_product;
	// 只记录的原始的行情buf，每个行情块会有多个指针指向它
	map<string, char*>	m_map_by_instrument;
	// 行情过来后，只想计算它所对应的序列
	map<string, set<CSyntheticMarketData*>>	m_map_set_by_instrument;

	mutex							m_cs;
};


