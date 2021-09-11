#pragma once

#include <string>
#include <list>
#include <map>

using namespace std;

struct UserItem
{
	char UserNo[21];
	char Password[21];
};

class CUserConfig
{
public:
	void WriteDefault(const char* filename);
	void Write(const char* filename, UserItem* pItem);
	UserItem Read(const char* filename);
};

