#pragma once

#include <set>
#include <string>
#include <vector>
#include <map>

class CSubscribeManager1;
class CSubscribeManager2;
class CSyntheticConfig;
class CSyntheticManager;
class CSyntheticCalculateFactory;

using namespace std;

class CSubscribeManager
{
public:
	CSubscribeManager(CSyntheticConfig* pConfig, CSyntheticManager* pManager, CSyntheticCalculateFactory* pCalculateFactory);
	~CSubscribeManager();

	// 订阅，返回需要API订阅的合约
	set<string> Subscribe(set<string> instruments, string exchange);
	// 取消订阅，返回需要API取消的合约
	set<string> Unsubscribe(set<string> instruments, string exchange);

	// set转成字符串，用于发送请求
	string Set2String(set<string> instruments, const char* seps = ";");

	// 将字符串转成api可用的格式
	char* String2Buf(const char* str, vector<char*>& vct, const char* seps = ";");
	// 字符串转成set，用于后面的订阅计数
	set<string> String2Set(const char* str, const char* seps = ";", int before = 1, const char* prefix = "");

	set<string> GetParentInstruments(string exchange);

	void Clear();

	CSubscribeManager1* GetManager1(string exchange);
	CSubscribeManager2* GetManager2(string exchange);

	set<string> GetExchanges();

private:
	map<string, CSubscribeManager1*> m_map_pSubscribeManager1;
	map<string, CSubscribeManager2*> m_map_pSubscribeManager2;

	CSyntheticConfig*			m_pConfig;
	CSyntheticManager*			m_pManager;

	CSyntheticCalculateFactory* m_pCalculateFactory;
};

