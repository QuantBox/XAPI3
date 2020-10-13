#pragma once

#include <map>
#include <set>
#include <mutex>

using namespace std;

class CSubscribeManager2
{
public:
	CSubscribeManager2();
	~CSubscribeManager2();

	// 订阅，返回需要API订阅的合约
	set<string> Subscribe(string sym, set<string> ss);
	// 取消订阅，返回需要API取消的合约
	set<string> Unsubscribe(string sym);
	// 得到订阅中的合约
	set<string> GetChildenInstruments();
	// 得到合成的合约
	set<string> GetParentInstruments();
	// 重置
	void Clear();

private:
	// 需要记录订阅次数
	//unordered_map<string, int> m_map;
	mutex					   m_cs;
	map<string, set<string>> m_maps;
};

