#pragma once

#include <string>
#include <list>
#include <map>

using namespace std;

struct ServerItem
{
	bool IsMulticast = false;
	// TCP行情
	string			eqsIp;
	unsigned short	eqsPort;
	// 组播行情
	string			mcIp;
	unsigned short	mcPort;			///< 组播端口1-65535
	string			mcLoacalIp;		///< 本机地址
	unsigned short	mcLocalPort;	///< 本机端口1-65535, 本机未被使用的端口号
	string			exchangeId;		///< 交易所代码，参考EESQuoteExchangeIDType取值范围
};

class CServerConfig
{
public:
	void WriteMd(const char* filename, ServerItem* pItem);
	ServerItem ReadMd(const char* filename);
	void WriteMdDefault(const char* filename);
};

