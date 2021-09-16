#pragma once
#pragma once

#include <string>
#include <map>
#include <list>

#include "../../include/ApiStruct.h"

using namespace std;


template <typename T1, typename T2>
class InstrumentManager
{
public:
	virtual ~InstrumentManager(void)
	{
		Clear();
	}

	InstrumentField* OnInstrument(const char* key, InstrumentField* pInstrument, const T1* pT1)
	{
		// 根据持仓合计更新内存中的持仓
		InstrumentField* pField = nullptr;
		auto it = instruments.find(key);
		if (it == instruments.end())
		{
			pField = new InstrumentField();
			memcpy(pField, pInstrument, sizeof(InstrumentField));

			instruments.insert(pair<string, InstrumentField*>(key, pField));
		}
		else
		{
			pField = it->second;
			memcpy(pField, pInstrument, sizeof(InstrumentField));
		}

		{
			T1* pField2 = nullptr;
			auto it = instruments2.find(key);
			if (it == instruments2.end())
			{
				pField2 = new T1();
				memcpy(pField2, pT1, sizeof(T1));

				instruments2.insert(pair<string, T1*>(key, pField2));
			}
			else
			{
				pField2 = it->second;
				memcpy(pField2, pT1, sizeof(T1));
			}
		}

		return pField;
	}

	InstrumentField* OnProduct(const char* key, InstrumentField* pInstrument, const T2* pT2)
	{
		// 根据持仓合计更新内存中的持仓
		InstrumentField* pField = nullptr;
		auto it = products.find(key);
		if (it == products.end())
		{
			pField = new InstrumentField();
			memcpy(pField, pInstrument, sizeof(InstrumentField));

			products.insert(pair<string, InstrumentField*>(key, pField));
		}
		else
		{
			pField = it->second;
			memcpy(pField, pInstrument, sizeof(InstrumentField));
		}

		{
			T2* pField2 = nullptr;
			auto it = products2.find(key);
			if (it == products2.end())
			{
				pField2 = new T2();
				memcpy(pField2, pT2, sizeof(T2));

				products2.insert(pair<string, T2*>(key, pField2));
			}
			else
			{
				pField2 = it->second;
				memcpy(pField2, pT2, sizeof(T2));
			}
		}

		return pField;
	}

	virtual void Merge()
	{
		for (auto it = instruments.begin(); it != instruments.end(); ++it)
		{
			InstrumentField* pDst = it->second;
			auto it2 = products.find(pDst->InstrumentID);
			if (it2 == products.end())
				continue;
			InstrumentField* pSrc = it2->second;

			strcpy(pDst->InstrumentName, pSrc->InstrumentName);
			pDst->VolumeMultiple = pSrc->VolumeMultiple;
			pDst->PriceTick = pSrc->PriceTick;
		}
	}

	T1* GetT1(const char* key)
	{
		auto it = instruments2.find(key);
		if (it == instruments2.end())
			return nullptr;
		return it->second;
	}

	list<InstrumentField*> All()
	{
		list<InstrumentField*> ll;
		for (auto it = instruments.begin(); it != instruments.end(); ++it)
			ll.push_back(it->second);

		return ll;
	}

	void Clear()
	{
		for (auto it = instruments.begin(); it != instruments.end(); ++it)
			delete it->second;
		instruments.clear();
		for (auto it = products.begin(); it != products.end(); ++it)
			delete it->second;
		products.clear();

		for (auto it = instruments2.begin(); it != instruments2.end(); ++it)
			delete it->second;
		instruments2.clear();
		for (auto it = products2.begin(); it != products2.end(); ++it)
			delete it->second;
		products2.clear();
	}
private:
	map<string, InstrumentField*> instruments;
	map<string, InstrumentField*> products;
	map<string, T1*> instruments2;
	map<string, T2*> products2;
};