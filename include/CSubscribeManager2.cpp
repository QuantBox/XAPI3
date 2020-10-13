#include "stdafx.h"
#include "CSubscribeManager2.h"


CSubscribeManager2::CSubscribeManager2()
{
}


CSubscribeManager2::~CSubscribeManager2()
{
}

// 订阅，返回需要API订阅的合约
set<string> CSubscribeManager2::Subscribe(string sym, set<string> ss)
{
	auto it = m_maps.find(sym);
	if (it == m_maps.end())
	{
		m_maps.insert(pair<string, set<string>>(sym, ss));
	}
	else
	{
		m_maps[sym] = ss;
	}
	return ss;
}

// 取消订阅，返回需要API取消的合约
set<string> CSubscribeManager2::Unsubscribe(string sym)
{
	m_maps.erase(sym);
	return GetChildenInstruments();
}

set<string> CSubscribeManager2::GetChildenInstruments()
{
	lock_guard<mutex> cl(m_cs);

	set<string> ss;
	for (auto i = m_maps.begin(); i != m_maps.end(); ++i)
	{
		for (auto j = i->second.begin(); j != i->second.end(); ++j)
		{
			ss.insert(j->data());
		}
	}
	return ss;
}

set<string> CSubscribeManager2::GetParentInstruments()
{
	lock_guard<mutex> cl(m_cs);

	set<string> ss;
	for (auto i = m_maps.begin(); i != m_maps.end(); ++i)
	{
		ss.insert(i->first);
	}
	return ss;
}

void CSubscribeManager2::Clear()
{
	lock_guard<mutex> cl(m_cs);
	m_maps.clear();
}
