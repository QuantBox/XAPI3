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
	void Write(const char* filename, ServerItem* pItem);
	ServerItem Read(const char* filename);
	void WriteDefault(const char* filename);
};

