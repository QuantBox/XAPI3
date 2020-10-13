#pragma once

#include <map>
#include <set>
#include <mutex>
#include <vector>

using namespace std;

class CSubscribeManager1
{
public:
	CSubscribeManager1();
	~CSubscribeManager1();

	// 订阅，返回需要API订阅的合约
	set<string> Subscribe(set<string> ss);
	// 取消订阅，返回需要API取消的合约
	set<string> Unsubscribe(set<string> ss);

	// 得到订阅中的合约
	set<string> GetInstruments();
	// 重置
	void Clear();


private:
	// 需要记录订阅次数
	map<string, int> m_map;
	mutex					   m_cs;
};

