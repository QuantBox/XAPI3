#include "stdafx.h"
#include "CSyntheticInfos.h"


CSyntheticInfos::CSyntheticInfos()
{
}


CSyntheticInfos::~CSyntheticInfos()
{
}

void CSyntheticInfos::Add_Product(const char* product, const char* instrument, double weight)
{
	auto it = m_map_by_product.find(product);
	if (it == m_map_by_product.end())
	{
		// 没找到
		unordered_map<string, double> sd;
		sd.insert(pair<string, double>(instrument, weight));
		
		m_map_by_product.insert(pair<string, unordered_map<string, double>>(product, sd));

		m_list_product.push_back(product);
	}
	else
	{
		// 找到了
		it->second.insert(pair<string, double>(instrument, weight));
	}
}

void CSyntheticInfos::Add_Instrument(const char* instrument, const char* product, double weight)
{
	auto it = m_map_by_instrument.find(instrument);
	if (it == m_map_by_instrument.end())
	{
		// 没找到
		unordered_map<string, double> sd;
		sd.insert(pair<string, double>(product, weight));

		m_map_by_instrument.insert(pair<string, unordered_map<string, double>>(instrument, sd));
	}
	else
	{
		// 找到了
		it->second.insert(pair<string, double>(product, weight));
	}
}

void CSyntheticInfos::Add(const char* product, const char* instrument, double weight)
{
	Add_Product(product, instrument, weight);
	Add_Instrument(instrument, product, weight);
}

unordered_map<string, double> CSyntheticInfos::GetByProduct(const char* product)
{
	auto it = m_map_by_product.find(product);
	if (it == m_map_by_product.end())
	{
		// 没找到
		unordered_map<string, double> sd;
		return sd;
	}
	else
	{
		return it->second;
	}
}

unordered_map<string, double> CSyntheticInfos::GetByInstrument(const char* product)
{
	auto it = m_map_by_instrument.find(product);
	if (it == m_map_by_instrument.end())
	{
		// 没找到
		unordered_map<string, double> sd;
		return sd;
	}
	else
	{
		return it->second;
	}
}

list<string> CSyntheticInfos::GetProducts()
{
	return m_list_product;
}
