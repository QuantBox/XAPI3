#pragma once

#include <string>
#include <list>
#include <map>

using namespace std;

struct UserItem
{
	char loginId[32];
	char password[32];
};

class CUserConfig
{
public:
	void WriteDefault(const char* filename);
	void Write(const char* filename, UserItem* pItem);
	UserItem Read(const char* filename);
};

