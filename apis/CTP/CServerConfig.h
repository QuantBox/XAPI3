#pragma once

#include <string>
#include <list>
#include <map>

using namespace std;

struct ServerItem
{
	bool IsUsingUdp = false;
	bool IsMulticast = false;
	///经纪商ID
	string BrokerID;
	///产品信息
	string UserProductInfo;
	///授权码
	string AuthCode;
	///App代码
	string AppID;
	///地址信息
	list<string> Address;

	map<string, string> ResumeType;
};

class CServerConfig
{
public:
	void WriteMd(const char* filename, ServerItem* pItem);
	void WriteTd(const char* filename, ServerItem* pItem);
	ServerItem ReadMd(const char* filename);
	ServerItem ReadTd(const char* filename);

	void WriteMdDefault(const char* filename);
	void WriteTdDefault(const char* filename);
};

