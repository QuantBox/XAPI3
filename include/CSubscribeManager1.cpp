#include "stdafx.h"
#include "CSubscribeManager1.h"


CSubscribeManager1::CSubscribeManager1()
{
}


CSubscribeManager1::~CSubscribeManager1()
{
}

set<string> CSubscribeManager1::Subscribe(set<string> ss)
{
	lock_guard<mutex> cl(m_cs);

	set<string> sss;
	for (auto it = ss.begin(); it != ss.end(); ++it)
	{
		auto s = m_map.find(it->data());
		if (s == m_map.end())
		{
			m_map.insert(pair<string, int>(it->data(), 1));
			sss.insert(it->data());
		}
		else
		{
			int t = s->second + 1;
			m_map[it->data()] = t;
		}
	}
	return sss;
}

set<string> CSubscribeManager1::Unsubscribe(set<string> ss)
{
	lock_guard<mutex> cl(m_cs);

	set<string> sss;
	for (auto it = ss.begin(); it != ss.end(); ++it)
	{
		auto s = m_map.find(it->data());
		if (s == m_map.end())
		{
			// m_map.insert(pair<string, int>(it->data(), 1));
		}
		else
		{
			int t = s->second - 1;
			if (t <= 0)
			{
				m_map.erase(it->data());
				sss.insert(it->data());
			}
			else
			{
				m_map[it->data()] = t;
			}
		}
	}
	return sss;
}


set<string> CSubscribeManager1::GetInstruments()
{
	lock_guard<mutex> cl(m_cs);

	set<string> ss;
	for (auto it = m_map.begin(); it != m_map.end(); ++it)
	{
		if (it->second>0)
		{
			ss.insert(it->first);
		}
	}
	return ss;
}

void CSubscribeManager1::Clear()
{
	lock_guard<mutex> cl(m_cs);
	m_map.clear();
}
