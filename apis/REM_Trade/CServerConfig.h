#pragma once

#include <string>
#include <list>
#include <map>

using namespace std;

struct ServerItem
{
	string			RemoteTradeIp;
	unsigned short	RemoteTradeTCPPort;
	unsigned short	RemoteTradeUDPPort;

	string			RemoteQueryIp;
	unsigned short  RemoteQueryTCPPort;

	string			LocalTradeIp;
	unsigned short  LocalTradeUDPPort;
};

class CServerConfig
{
public:
	void WriteTd(const char* filename, ServerItem* pItem);
	ServerItem ReadTd(const char* filename);
	void WriteTdDefault(const char* filename);
};

