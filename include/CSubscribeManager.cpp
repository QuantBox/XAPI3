#include "stdafx.h"
#include "CSubscribeManager.h"

#include "CSubscribeManager1.h"
#include "CSubscribeManager2.h"

#include "synthetic/CSyntheticConfig.h"
#include "synthetic/CSyntheticManager.h"

#include <algorithm>

CSubscribeManager::CSubscribeManager(CSyntheticConfig* pConfig, CSyntheticManager* pManager, CSyntheticCalculateFactory* pCalculateFactory)
{
	m_pConfig = pConfig;
	m_pManager = pManager;
	m_pCalculateFactory = pCalculateFactory;
}


CSubscribeManager::~CSubscribeManager()
{
	for (auto it = m_map_pSubscribeManager1.begin(); it != m_map_pSubscribeManager1.end(); it++)
	{
		delete it->second;
	}
	for (auto it = m_map_pSubscribeManager2.begin(); it != m_map_pSubscribeManager2.end(); it++)
	{
		delete it->second;
	}
}

CSubscribeManager1* CSubscribeManager::GetManager1(string exchange)
{
	auto it = m_map_pSubscribeManager1.find(exchange);
	if (it == m_map_pSubscribeManager1.end())
	{
		auto pSM = new CSubscribeManager1();
		m_map_pSubscribeManager1.insert(pair<string, CSubscribeManager1*>(exchange, pSM));
		return pSM;
	}
	else
	{
		return it->second;
	}
}

CSubscribeManager2* CSubscribeManager::GetManager2(string exchange)
{
	auto it = m_map_pSubscribeManager2.find(exchange);
	if (it == m_map_pSubscribeManager2.end())
	{
		auto pSM = new CSubscribeManager2();
		m_map_pSubscribeManager2.insert(pair<string, CSubscribeManager2*>(exchange, pSM));
		return pSM;
	}
	else
	{
		return it->second;
	}
}

set<string> CSubscribeManager::GetExchanges()
{
	set<string> ss;
	for (auto it = m_map_pSubscribeManager1.begin(); it != m_map_pSubscribeManager1.end(); it++)
	{
		ss.insert(it->first);
	}
	return ss;
}


// 订阅，返回需要API订阅的合约
set<string> CSubscribeManager::Subscribe(set<string> instruments, string exchange)
{
	if (instruments.size() == 0)
		return instruments;

	CSubscribeManager1* pSubscribeManager1 = GetManager1(exchange);
	CSubscribeManager2* pSubscribeManager2 = GetManager2(exchange);

	// 订阅前的合约情况
	set<string> s_syn_old = pSubscribeManager2->GetChildenInstruments();
	set<string> s_com_old = pSubscribeManager1->GetInstruments();

	// 分成两类
	set<string> s_syn;
	set<string> s_com;
	m_pConfig->Classify(instruments, s_syn, s_com);
	pSubscribeManager1->Subscribe(s_com);

	auto cfgs = m_pConfig->GetMap();
	for (auto it = cfgs.begin(); it != cfgs.end(); it++)
	{
		auto x = s_syn.find(it->second.symbol);
		if (x != s_syn.end())
		{
			auto pSMD = m_pManager->Register(
				it->second.symbol.c_str(),
				it->second.emits,
				it->second.legs,
				m_pCalculateFactory->Create(it->second.method));

			pSubscribeManager2->Subscribe(it->second.symbol.c_str(), pSMD->GetInstruments());
		}
	}

	// 订阅后的合约情况
	set<string> s_syn_new = pSubscribeManager2->GetChildenInstruments();
	set<string> s_com_new = pSubscribeManager1->GetInstruments();

	set<string> s_old;
	set<string> s_new;
	set<string> s_dif;
	set_union(s_syn_old.begin(), s_syn_old.end(), s_com_old.begin(), s_com_old.end(), inserter(s_old, s_old.begin()));
	set_union(s_syn_new.begin(), s_syn_new.end(), s_com_new.begin(), s_com_new.end(), inserter(s_new, s_new.begin()));
	set_difference(s_new.begin(), s_new.end(), s_old.begin(), s_old.end(), inserter(s_dif, s_dif.begin()));

	return s_dif;
}

// 取消订阅，返回需要API取消的合约
set<string> CSubscribeManager::Unsubscribe(set<string> instruments, string exchange)
{
	if (instruments.size() == 0)
		return instruments;

	CSubscribeManager1* pSubscribeManager1 = GetManager1(exchange);
	CSubscribeManager2* pSubscribeManager2 = GetManager2(exchange);

	// 订阅前的合约情况
	set<string> s_syn_old = pSubscribeManager2->GetChildenInstruments();
	set<string> s_com_old = pSubscribeManager1->GetInstruments();

	// 分成两类
	set<string> s_syn;
	set<string> s_com;
	m_pConfig->Classify(instruments, s_syn, s_com);
	pSubscribeManager1->Unsubscribe(s_com);

	auto cfgs = m_pConfig->GetMap();
	for (auto it = cfgs.begin(); it != cfgs.end(); it++)
	{
		auto x = s_syn.find(it->second.symbol);
		if (x != s_syn.end())
		{
			auto pSMD = m_pManager->UnRegister(
				it->second.symbol.c_str());

			pSubscribeManager2->Unsubscribe(it->second.symbol.c_str());
		}
	}

	// 订阅后的合约情况
	set<string> s_syn_new = pSubscribeManager2->GetChildenInstruments();
	set<string> s_com_new = pSubscribeManager1->GetInstruments();

	set<string> s_old;
	set<string> s_new;
	set<string> s_dif;
	set_union(s_syn_old.begin(), s_syn_old.end(), s_com_old.begin(), s_com_old.end(), inserter(s_old, s_old.begin()));
	set_union(s_syn_new.begin(), s_syn_new.end(), s_com_new.begin(), s_com_new.end(), inserter(s_new, s_new.begin()));
	set_difference(s_old.begin(), s_old.end(), s_new.begin(), s_new.end(), inserter(s_dif, s_dif.begin()));

	return s_dif;
}

string CSubscribeManager::Set2String(set<string> instruments, const char* seps)
{
	string str;
	for (auto it = instruments.begin(); it != instruments.end(); ++it)
	{
		str.append(*it);
		str.append(seps);
	}
	return str;
}

char* CSubscribeManager::String2Buf(const char* str, vector<char*>& vct, const char* seps)
{
	vct.clear();

	if (strlen(str) == 0)
		return nullptr;

	//这里不知道要添加的字符有多长，很悲剧
	size_t len = (size_t)(strlen(str)*1.5 + 1);
	char* buf = new char[len];
	strncpy(buf, str, len);

	char* token = strtok(buf, seps);
	while (token)
	{
		if (strlen(token)>0)
		{
			vct.push_back(token);
		}
		token = strtok(nullptr, seps);
	}

	return buf;
}

set<string> CSubscribeManager::String2Set(const char* szString, const char* seps, int before, const char* prefix)
{
	size_t len = (size_t)(strlen(szString)*1.5 + 1);
	char* buf = new char[len];
	strncpy(buf, szString, len);

	set<string> ss;

	char* token = strtok(buf, seps);
	while (token)
	{
		if (strlen(token) > 0)
		{
			char temp[64] = { 0 };
			if (prefix)
			{
				if (before > 0)
				{
					sprintf(temp, "%s%s", prefix, token);
				}
				else
				{
					sprintf(temp, "%s%s", token, prefix);
				}
			}
			else
			{
				sprintf(temp, "%s", token);
			}

			ss.insert(temp);
		}
		token = strtok(nullptr, seps);
	}

	delete[] buf;
	return ss;
}

set<string> CSubscribeManager::GetParentInstruments(string exchange)
{
	CSubscribeManager1* pSubscribeManager1 = GetManager1(exchange);
	CSubscribeManager2* pSubscribeManager2 = GetManager2(exchange);

	set<string> s_syn_old = pSubscribeManager2->GetParentInstruments();
	set<string> s_com_old = pSubscribeManager1->GetInstruments();

	set<string> s_old;
	set_union(s_syn_old.begin(), s_syn_old.end(), s_com_old.begin(), s_com_old.end(), inserter(s_old, s_old.begin()));
	return s_old;
}

void CSubscribeManager::Clear()
{
	for (auto it = m_map_pSubscribeManager1.begin(); it != m_map_pSubscribeManager1.end(); it++)
	{
		it->second->Clear();
	}
	for (auto it = m_map_pSubscribeManager2.begin(); it != m_map_pSubscribeManager2.end(); it++)
	{
		it->second->Clear();
	}
}