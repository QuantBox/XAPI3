#pragma once

#include <string>
#include <list>
#include <map>

using namespace std;

struct ServerItem
{
	string			AuthCode;

	string			IP;
	unsigned short	Port;

};

class CServerConfig
{
public:
	void WriteMd(const char* filename, ServerItem* pItem);
	ServerItem ReadMd(const char* filename);
	void WriteMdDefault(const char* filename);
};

